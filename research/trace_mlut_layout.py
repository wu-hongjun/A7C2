"""Recover MLUT native bank layout from actual CP tables and narrow code paths.

All work is offline. Host Unicorn executes only the isolated three-register
metadata builder, never a camera, driver, queue, DMA or image processor.
"""
from pathlib import Path
from hashlib import sha256
import itertools
import json
import re
import struct
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha8_unpacked/cpapp-b.bin'
EXPECTED='f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428'
BASE=0x108000
OUT=ROOT/'research/evidence/mlut-layout.json'

class CP:
    def __init__(self):
        self.data=SOURCE.read_bytes()
        assert sha256(self.data).hexdigest()==EXPECTED,'Require official pinned CP'
    def read(self,va,size):
        off=va-BASE
        if not 0<=off<=off+size<=len(self.data):raise ValueError('Not file backed: '+hex(va))
        return self.data[off:off+size]
    def words(self,va,n):return struct.unpack('<'+'I'*n,self.read(va,n*4))
    def code(self,start,end,label):
        instructions=list(Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(self.read(start,end-start),start))
        assert sum(i.size for i in instructions)==end-start,(hex(start),hex(end))
        return {'label':label,'runtime_start':hex(start),'runtime_end_exclusive':hex(end),
                'file_start':hex(start-BASE),'file_end_exclusive':hex(end-BASE),'isa':'Thumb-2',
                'instructions':[{'runtime_address':hex(i.address),'file_offset':hex(i.address-BASE),
                                 'bytes':i.bytes.hex(),'operation':i.mnemonic,'operands':i.op_str} for i in instructions]}

def descriptors(cp):
    records=[]
    pattern=rb'MLUT_L3D_(?:SRAM|REG|SEL)_[A-Z0-9_]+\0'
    for match in re.finditer(pattern,cp.data):
        start=match.start()-12
        first,second,count=struct.unpack_from('<III',cp.data,start)
        name=match.group()[:-1].decode()
        record={'name':name,'record_file_offset':hex(start),'record_runtime_address':hex(start+BASE),
                'header_words':[hex(first),hex(second),hex(count)],'name_runtime_address':hex(match.start()+BASE),
                'header_bytes':cp.data[start:start+12].hex()}
        if name.startswith('MLUT_L3D_SRAM_') and second==0x3000 and count==500:
            data=cp.read(first,count*4)
            record.update({'kind':'SRAM_payload_descriptor','source_runtime_address':hex(first),'source_file_offset':hex(first-BASE),
                           'register_relative_destination':hex(second),'word_count':count,'payload_bytes':len(data),
                           'payload_sha256':sha256(data).hexdigest(),
                           'first_8_words':[hex(x) for x in struct.unpack('<8I',data[:32])],
                           'last_8_words':[hex(x) for x in struct.unpack('<8I',data[-32:])]})
        elif name.startswith('MLUT_L3D_REG_') and second==0x400 and count==31:
            data=cp.read(first,count*4); words=struct.unpack('<31I',data)
            record.update({'kind':'register_payload_descriptor','source_runtime_address':hex(first),'source_file_offset':hex(first-BASE),
                           'register_relative_destination':hex(second),'word_count':count,'payload_bytes':len(data),
                           'payload_sha256':sha256(data).hexdigest(),'words':[hex(x) for x in words],
                           'node_or_threshold_words_11_through_30':[hex(x) for x in words[11:]]})
        elif name.startswith('MLUT_L3D_SEL_') and second==0x408 and count==1:
            record.update({'kind':'bank_control_descriptor','source_runtime_address':hex(first),
                           'register_relative_destination':hex(second),'word_count':1})
            if BASE<=first<BASE+len(cp.data):record['file_backed_word']=hex(cp.words(first,1)[0])
            else:record['qualification']='Source is outside file-backed image; actual runtime value not observed.'
        else:
            record['kind']='other_named_descriptor_candidate'
        records.append(record)
    assert records
    for r in records:
        if r['kind']=='register_payload_descriptor':
            assert all(int(r['words'][i],16)==0 for i in [3,6,7])
    return records

def identity_layout(cp,records):
    arrays={}
    for r in records:
        m=re.fullmatch(r'MLUT_L3D_SRAM_([0-7])_([RGB])_THRU',r['name'])
        if m and r['kind']=='SRAM_payload_descriptor':
            arrays[(m[2],int(m[1]))]=struct.unpack('<1000H',cp.read(int(r['source_runtime_address'],16),2000))
    assert len(arrays)==24
    register=next(r for r in records if r['name']=='MLUT_L3D_REG_THRU')
    nodes=[int(x,16) for x in register['node_or_threshold_words_11_through_30']]
    assert nodes==[0xccc*n for n in range(20)]
    cases=0; summaries=[]
    for c,channel in enumerate('RGB'):
        for bank in range(8):
            data=arrays[(channel,bank)]; samples=[]
            for local in range(1000):
                r,g,b=(2*(local//100)+((bank>>2)&1),2*((local//10)%10)+((bank>>1)&1),2*(local%10)+(bank&1))
                expected=nodes[[r,g,b][c]]
                assert data[local]==expected,(channel,bank,local,[r,g,b],data[local],expected)
                cases+=1
                if local in [0,1,9,10,99,100,999]:samples.append({'local_halfword_index':local,'RGB_coordinate':[r,g,b],'value_u16':data[local]})
            summaries.append({'channel':channel,'bank':bank,'matched_halfwords':1000,'representative_samples':samples})
    return {'method':'Independent exhaustive comparison of all named THRU SRAM halfwords against the twenty register threshold values.',
            'matched_halfwords':cases,'channels':list('RGB'),'banks_per_channel':8,'halfwords_per_bank':1000,
            'byte_order':'little-endian','samples_per_32bit_transfer_word':2,
            'THRU_thresholds':[hex(v) for v in nodes],
            'bank_formula':'((r & 1) << 2) | ((g & 1) << 1) | (b & 1)',
            'local_halfword_index_formula':'((r // 2) * 10 + (g // 2)) * 10 + (b // 2)',
            'flat_payload_byte_offset_formula':'channel_index * 16000 + bank * 2000 + local_halfword_index * 2',
            'qualification':'Exact identity-table data relationship; not a measured interpolation rule, pixel color encoding, or silicon validation.',
            'banks':summaries}

def controls(cp,records):
    pointers=cp.words(0xe953fc,24); rows=[]
    for i,pointer in enumerate(pointers):
        channel='RGB'[i//8]; bank=i%8
        wanted=f'MLUT_L3D_SEL_{channel}_RAM{bank}_THRU'
        record=next(r for r in records if r['name']==wanted)
        named_pointer=int(record['header_words'][0],16)
        assert named_pointer+4==pointer
        row={'index':i,'channel':channel,'bank':bank,'pointer_table_runtime_address':hex(0xe953fc+i*4),
             'source_word_pointer':hex(pointer),'matching_descriptor':record['record_runtime_address'],
             'named_descriptor_source_word_pointer':hex(named_pointer)}
        if 'file_backed_word' in record:
            value=int(record['file_backed_word'],16)
            assert value==(i//8)|(bank<<4)
            assert cp.words(pointer,1)[0]==value
            row['file_backed_word']=hex(value)
        else:row['runtime_word_unobserved']=True
        rows.append(row)
    return {'pointer_table_runtime_address':'0xe953fc','pointer_table_file_offset':hex(0xe953fc-BASE),
            'ordering':'R bank0..7, G bank0..7, B bank0..7',
            'observed_control_encoding':'channel index in low nibble, bank index in next nibble; 23 file-backed words checked.',
            'qualification':'R/bank0 pointer0x1a76524 is outside file-backed image; its runtime word is not inferred as a measured zero.',
            'entries':rows}

def diagnostic_pattern(cp):
    total=0
    for channel in range(3):
        for bank in range(8):
            values=cp.words(0x112b93c+(channel*8+bank)*2000,500)
            for index,value in enumerate(values):
                expected=0x3d000000|(channel<<20)|(bank<<16)|(0x3000+index*4)
                assert value==expected
                total+=1
    return {'source_runtime_address':'0x112b93c','source_file_offset':hex(0x112b93c-BASE),'bytes':48000,
            'matched_words':total,'formula':'0x3d000000 | (channel << 20) | (bank << 16) | (0x3000 + word_index * 4)',
            'meaning':'Address/bank-tagged diagnostic data copied by debug command31; not a numerical LUT or evidence of sample precision.'}

def switch_builder_checks(cp):
    cpu=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
    cpu.mem_map(BASE,(len(cp.data)+4095)&~4095);cpu.mem_write(BASE,cp.data)
    arena,stack,finish=0x5000000,0x6000000,0x7000000
    for a,n in [(arena,0x10000),(stack,0x10000),(finish,4096),(0x262d000,0x10000)]:cpu.mem_map(a,n)
    def boundary(engine,address,size,user):
        if not 0xb833f4<=address<0xb8346e and address!=finish:raise RuntimeError('Unexpected external execution '+hex(address))
    cpu.hook_add(UC_HOOK_CODE,boundary)
    initial=cpu.context_save(); cases=[]
    for ip in [0,6]:
        for bits in itertools.product([0,1],repeat=6):
            cpu.context_restore(initial)
            cpu.mem_write(arena,struct.pack('<6I',*bits))
            cpu.mem_write(0x262d000,b'\0'*0x10000)
            cpu.reg_write(UC_ARM_REG_R0,ip);cpu.reg_write(UC_ARM_REG_R1,arena);cpu.reg_write(UC_ARM_REG_R2,arena+0x100)
            cpu.reg_write(UC_ARM_REG_SP,stack+0x8000);cpu.reg_write(UC_ARM_REG_LR,finish|1)
            cpu.emu_start(0xb833f5,finish,count=1000)
            assert cpu.reg_read(UC_ARM_REG_PC)==finish
            expected=(bits[0],bits[5]|(bits[4]<<1),bits[1]|(bits[2]<<8)|(bits[3]<<16))
            actual=struct.unpack('<3I',cpu.mem_read(0x262ebbc,12));assert actual==expected
            summary=struct.unpack('<3I',cpu.mem_read(arena+0x100,12));assert summary==(ip,3,0x262ebc8)
            for n,(offset,value) in enumerate(zip([0x400,0x410,0x414],expected)):
                pointer,destination,count=struct.unpack('<3I',cpu.mem_read(0x262ebc8+n*0x8c,12))
                assert (pointer,destination,count)==(0x262ebbc+n*4,offset,1)
                assert struct.unpack('<I',cpu.mem_read(pointer,4))[0]==value
            cases.append({'IP_index':ip,'inputs_all_R_G_B_preblack_postblack':list(bits),'register_words_400_410_414':[hex(v) for v in actual]})
    return {'method':'Isolated Thumb Unicorn execution of0xb833f4–0xb8346e only. No external stubs or peripheral/queue accesses.',
            'cases':len(cases),'results':cases,'qualification':'Confirms descriptor construction, not hardware bit effects or transfer timing.'}

def main():
    cp=CP(); records=descriptors(cp)
    ranges=[(0xb7acac,0xb7af10,'debug SRAM copy and submit'),(0xb7b524,0xb7b594,'debug register copy and submit'),
            (0xb8336c,0xb833f4,'SRAM descriptor builder'),(0xb833f4,0xb8346e,'L3D switch descriptor builder'),
            (0xb7a936,0xb7a9f4,'named L3D switch diagnostic'),(0xb7ce6a,0xb7cec6,'synchronous switch parameter packing'),
            (0xb82e00,0xb82e5a,'register31-word worker'),(0xb7cf0c,0xb7cf54,'bank/channel selector packing'),
            (0xb7994a,0xb799de,'bank/channel selector validation'),(0xb82a0c,0xb82a42,'offset/value register-list write'),
            (0xb7d2c4,0xb7d30e,'native look descriptor lookup'),
            (0xb7a364,0xb7a3c4,'named CPU L3D RAM access diagnostic'),
            (0xb78ed8,0xb78f58,'L3D CPU RAM access wrapper'),
            (0xb82a66,0xb82a6a,'L3D CPU RAM access forwarding'),
            (0xb7832e,0xb78354,'L3D CPU RAM access register write')]
    debug=[]
    for index in [10,22,29,30,42]:
        address=0x1031cac+BASE+index*12
        key,help_pointer,handler=cp.words(address,3)
        def string(pointer):
            offset=pointer-BASE;return cp.data[offset:cp.data.index(b'\0',offset)].decode('ascii')
        debug.append({'record_runtime_address':hex(address),'record_file_offset':hex(address-BASE),
                      'key':string(key),'help':string(help_pointer),'help_runtime_address':hex(help_pointer),
                      'handler_runtime_pointer':hex(handler),'ISA':'Thumb' if handler&1 else 'ARM'})
    report={'date':'2026-10-17','method':'ordinary_offline_static_and_bounded_host_emulation_not_REA_Evidence',
            'camera_accessed':False,'on_camera_execution':False,'firmware_modified':False,
            'source':{'path':str(SOURCE.relative_to(ROOT)),'bytes':len(cp.data),'sha256':EXPECTED},
            'address_model':{'file_to_link_model_addend':hex(BASE),'ISA':'Thumb-2 for retained functions',
                             'qualification':'Addresses recovered offline; no runtime camera mapping measured.'},
            'named_descriptors':records,'counts':{'named_records':len(records),
                                                 'SRAM_records':sum(r['kind']=='SRAM_payload_descriptor' for r in records),
                                                 'register31_records':sum(r['kind']=='register_payload_descriptor' for r in records)},
            'identity_layout':identity_layout(cp,records),'bank_controls':controls(cp,records),
            'relevant_debug_interfaces':debug,
            'diagnostic_not_LUT_pattern':diagnostic_pattern(cp),'switch_builder_emulation':switch_builder_checks(cp),
            'disassembled_regions':[cp.code(a,b,label) for a,b,label in ranges],
            'register_fields':[
                {'offset':'0x400','established':'overall L3D enable word copied from all_on parameter'},
                {'offset':'0x404','established':'named CPU L3D RAM access parameter, forwarded to a full-word MMIO write'},
                {'offset':'0x408','established':'CPU bank/channel selector: channel low nibble, bank next nibble; validation channel<=3 andbank<8'},
                {'offset':'0x410','established':'pre_black parameter at bit1 and post_black at bit0'},
                {'offset':'0x414','established':'R/G/B enable parameters at bits0/8/16'},
                {'offset':'0x418–0x41c','established':'zero in the examined register31 templates; semantics unresolved'},
                {'offset':'0x420–0x428','established':'three words vary across templates; semantics unresolved'},
                {'offset':'0x42c–0x478','established':'twenty coordinate-threshold candidates corroborated by THRU identity data; USER_LUT_17 uses17active-scale values and3out-of-range padding values0x10001'}],
            'selector_validation_qualification':{'evidence_runtime_address':'0xb79982–0xb7998c',
                                                 'IP_index':'unsigned comparison rejects values above6',
                                                 'channel':'signed BGT comparison rejects values above3; no negative-value rejection in this wrapper',
                                                 'bank':'unsigned comparison rejects values >=8',
                                                 'qualification':'Acceptance by this wrapper is not hardware validity; no invalid selector was invoked on hardware.'},
            'limitations':['Hardware interpolation, input encoding and exact usable precision not established by CP table storage alone.',
                           'No numerical sample transformation is performed by the inspected data movers.',
                           'The three USER17 padding words0x10001 are observed; a dedicated hardware disable-bit interpretation is not established.',
                           'The namedTHRU tables demonstrate a20node capacity layout; they do not establish that arbitrary20-point CUBE files are accepted.',
                           'Hardware validity, cache coherency, resource lifetime and still routing remain unmeasured.']}
    OUT.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({'output':str(OUT.relative_to(ROOT)),**report['counts'],
                      'identity_halfword_matches':report['identity_layout']['matched_halfwords'],
                      'switch_builder_cases':report['switch_builder_emulation']['cases'],'camera_accessed':False}))
if __name__=='__main__':main()
