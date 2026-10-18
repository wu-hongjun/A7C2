"""Offline CP segmented SDS backing-address and SSYC image-entry mapping.

Original Thumb instructions run only in bounded host Unicorn. Transport, locks,
address alias conversion and C memory primitives are explicit stand-ins. No USB,
network, camera, device process, DMA, codec or pixel operation is executed.
"""
import hashlib
import json
from pathlib import Path
import struct
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *

ROOT=Path(__file__).resolve().parents[1]
BASE=0x108000
DIGEST='f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428'
AP_FIXTURE_SHA256='a63009190294540b84f4be83977d065c35cdef4eabf862d1e322b3c48b2e1862'
AP_GENERATOR_SHA256='675134ea40ae2685ba519a1d0593772dade98c4e63e501ad077046a4ed6903a5'


def main():
    path=next((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob('cpapp-b.bin'))
    blob=path.read_bytes();assert hashlib.sha256(blob).hexdigest()==DIGEST
    cs=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
    def region(start,end):
        ins=list(cs.disasm(blob[start-BASE:end-BASE],start))
        assert sum(i.size for i in ins)==end-start,(hex(start),hex(end))
        return {'runtime_start':hex(start),'runtime_end_exclusive':hex(end),'file_start':hex(start-BASE),
                'ISA':'Thumb-2','instructions':[{'runtime_address':hex(i.address),'file_offset':hex(i.address-BASE),
                 'bytes':i.bytes.hex(),'operation':i.mnemonic,'operands':i.op_str}for i in ins]}
    cpu=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
    cpu.mem_map(BASE,(len(blob)+4095)&~4095);cpu.mem_write(BASE,blob)
    arena,stack,stop,metadata=0x5000000,0x6000000,0x7000000,0x80010000
    for a,n in [(arena,0x10000),(stack,0x10000),(stop,0x1000),(metadata,0x1000),(0x1ac5000,0x1000)]:cpu.mem_map(a,n)
    tables={};events=[];fault=None
    def hook(u,a,n,_):
        value=0
        if a==0x61e60c:
            p=u.reg_read(UC_ARM_REG_R0);q=u.reg_read(UC_ARM_REG_R1);n=u.reg_read(UC_ARM_REG_R2)
            u.mem_write(p,bytes(u.mem_read(q,n)));value=p
        elif a==0x61ed00:
            p=u.reg_read(UC_ARM_REG_R0);n=u.reg_read(UC_ARM_REG_R1);u.mem_write(p,b'\0'*n);value=p
        elif a==0x61f038:
            p=u.reg_read(UC_ARM_REG_R0);q=u.reg_read(UC_ARM_REG_R1);n=u.reg_read(UC_ARM_REG_R2)
            value=0 if bytes(u.mem_read(p,n))==bytes(u.mem_read(q,n)) else 1
        elif a in(0x56309c,0x563308,0xaecb3c,0xaecb9c,0x68e3a2,0x68af0c):pass
        elif a==0x5feb94:
            value=u.reg_read(UC_ARM_REG_R0)
            events.append({'operation':'address_alias_standin','input_and_output':hex(value)})
        elif a==0x68c968:
            envelope=u.reg_read(UC_ARM_REG_R2);destination=u.reg_read(UC_ARM_REG_R1)
            kind=struct.unpack('<I',u.mem_read(envelope+4,4))[0]
            length=struct.unpack('<H',u.mem_read(envelope+8,2))[0]
            body=bytes(u.mem_read(envelope+0x14,length))
            command=struct.unpack_from('<H',body)[0];handle=struct.unpack_from('<I',body,8)[0]
            sequence=struct.unpack_from('<H',body,0x14)[0]
            assert kind==2 and length==0x98 and command in(0x24,0x25) and destination==0x61003c
            assert struct.unpack('<I',u.mem_read(envelope+0xc,4))[0]==0x9056015c
            event={'operation':'synthetic_remote_exchange','service':'0x9056015c','destination_argument':hex(destination),
                   'request_command':hex(command),'handle':hex(handle),'sequence':sequence,'request_bytes':body.hex()}
            if command==0x24:event['flags_u16_16']=struct.unpack_from('<H',body,0x16)[0]
            events.append(event)
            if fault=='transport':value=1
            else:
                response=bytearray(0xd8)
                response_command=(command|0x8000) if fault!='wrong_response_command' else 0x8123
                seq=sequence if fault!='wrong_sequence' else (sequence+1)&0xffff
                status=-1 if fault=='negative_status' or (fault=='release_negative_status' and command==0x25) else 0
                struct.pack_into('<Hh',response,0,response_command,status)
                struct.pack_into('<H',response,6,seq)
                if command==0x24:
                    segments=tables[handle]
                    assert len(segments)<=8
                    struct.pack_into('<H',response,8,len(segments))
                    for k,(address,size)in enumerate(segments):struct.pack_into('<QI',response,0x18+16*k,address,size)
                struct.pack_into('<I',response,0x14,0)
                u.mem_write(envelope+0x14,bytes(response));u.mem_write(envelope+8,struct.pack('<H',len(response)))
                event['response_command']=hex(response_command);event['response_status']=status
        else:return
        u.reg_write(UC_ARM_REG_R0,value);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR))
    cpu.hook_add(UC_HOOK_CODE,hook)
    initial=cpu.context_save()
    def reset():
        cpu.context_restore(initial);cpu.mem_write(arena,b'\0'*0x10000);cpu.mem_write(stack,b'\0'*0x10000)
        cpu.mem_write(metadata,b'\0'*0x1000);events.clear();cpu.mem_write(0x1ac50d6,b'\0\0')
    def execute(address,args=(),budget=20000):
        cpu.reg_write(UC_ARM_REG_SP,stack+0x8000);cpu.reg_write(UC_ARM_REG_LR,stop|1)
        for r,v in zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3],args):cpu.reg_write(r,v)
        cpu.emu_start(address|1,stop,count=budget)
        assert cpu.reg_read(UC_ARM_REG_PC)==stop,hex(cpu.reg_read(UC_ARM_REG_PC))
        return cpu.reg_read(UC_ARM_REG_R0),cpu.reg_read(UC_ARM_REG_R1)
    def expected(segments,offset):
        for address,size in segments:
            if offset<size:return address+offset
            offset-=size
        return 0
    cases=[]
    fixtures=[([(0x500200000,100)], [0,99,100]),
              ([(0x500200000,100),(0x800300000,50),(0x500400000,25)], [0,99,100,149,150,174,175]),
              ([(0,0),(0x500200000,10),(0,0),(0x500300000,20)],[0,9,10,29,30]),
              ([(0x500200000+0x1000*k,10)for k in range(8)],[69,70,79,80])]
    for segments,offsets in fixtures:
        for offset in offsets:
            for descriptor_type in (0,1,0xffffffff):
                reset();fault=None;tables={0x222:segments}
                desc=arena+0x100;cpu.mem_write(desc,struct.pack('<3I',descriptor_type,0x222,offset))
                lo,hi=execute(0xaecc5c,[arena,desc]);address=lo+(hi<<32)
                assert address==expected(segments,offset)
                exchanges=[e for e in events if e['operation']=='synthetic_remote_exchange']
                assert [e['request_command']for e in exchanges]==['0x24','0x25']
                assert all(e['handle']=='0x222'for e in exchanges) and exchanges[0]['flags_u16_16']==0
                cases.append({'segments':[{'address':hex(a),'bytes':n}for a,n in segments],
                              'descriptor_type':hex(descriptor_type),'handle':'0x222','logical_offset':offset,
                              'resolved_address':hex(address),'service_events':exchanges})
    errors=[]
    for fault in('transport','wrong_response_command','wrong_sequence','negative_status','release_negative_status'):
        reset();tables={0x222:[(0x500200000,100)]}
        cpu.mem_write(arena+0x100,struct.pack('<3I',0,0x222,0))
        lo,hi=execute(0xaecc5c,[arena,arena+0x100]);assert(lo,hi)==(0,0)
        errors.append({'synthetic_fault':fault,'resolved_address':'0x0','events':list(events)})
    flag_cases=[];fault=None
    for flags in range(16):
        reset();tables={0x222:[(0x500200000,100)]}
        cpu.mem_write(arena+0x100,struct.pack('<2I',0x222,flags))
        ret,_=execute(0xaeccea,[arena,arena+0x100,arena+0x400]);assert ret==0
        assert len(events)==1 and events[0]['request_command']=='0x24'
        encoded=(flags&1)|((flags&2)<<1)|((flags&4)>>1)|((flags&8)<<1)
        assert events[0]['flags_u16_16']==encoded
        assert struct.unpack('<H',cpu.mem_read(arena+0x400,2))[0]==1
        assert struct.unpack('<QI',cpu.mem_read(arena+0x408,12))==(0x500200000,100)
        flag_cases.append({'query_flags_input':flags,'request_flags_encoded':encoded,'event':events[0],
                           'qualification':'Direct query helper alone does not release; combined resolver/release cases are separate.'})
    capacity=[];fault=None
    for segments in [[(0x500200000,100),(0x800300000,50)],[(0x500200000,0xffffffff),(0x800300000,1)]]:
        reset();tables={0x222:segments};cpu.mem_write(arena+0x100,struct.pack('<3I',0,0x222,0))
        lo,hi=execute(0xaeccc0,[arena,arena+0x100]);assert lo==sum(n for a,n in segments)&0xffffffff
        capacity.append({'segment_lengths':[n for a,n in segments],'capacity_u32':lo,'events':list(events),
                         'qualification':'Capacity sum is logical across segments; this does not establish physical contiguity.'})
    # Original outer SSYC lookup + entry walk + nested image-triple decoding,
    # then the original nested backing resolver. Metadata, not pixels, is read.
    nested=[]
    for offset in(0,100,0x200):
        reset();fault=None
        outer_handle,pixel_handle=0x111,0x222
        tables={outer_handle:[(0x500010000,0x1000)],pixel_handle:[(0x500200000,100),(0x800300000,0x1000)]}
        body=bytearray(0x200);body[:4]=b'SSYC'
        struct.pack_into('<II',body,0xc,1,0x20)
        body[0x20:0x24]=b'ETRY';struct.pack_into('<I',body,0x24,0)
        struct.pack_into('<I',body,0x2c,0x40)
        struct.pack_into('<II',body,0x40,0,pixel_handle);struct.pack_into('<I',body,0x58,offset)
        body[0x5c:0x66]=bytes([1,0,0,0,0,0,0,0,0,0])
        for k in range(12):struct.pack_into('<H',body,0x40+0x2a+2*k,100+k)
        struct.pack_into('<HH',body,0x40+0x2e,7008,4672)
        cpu.mem_write(metadata,bytes(body))
        outer,out=arena+0x100,arena+0x200
        cpu.mem_write(outer,struct.pack('<3I',0,outer_handle,0))
        ret,_=execute(0x45b62c,[arena,outer,out]);assert ret==0
        decoded=bytes(cpu.mem_read(out,0x38));triple=struct.unpack_from('<3I',decoded,4)
        assert triple==(0,pixel_handle,offset)
        assert struct.unpack_from('<HH',decoded,0x18)==(7008,4672)
        lo,hi=execute(0xaecc5c,[arena,out+4]);address=lo+(hi<<32)
        assert address==expected(tables[pixel_handle],offset)
        exchanges=[e for e in events if e['operation']=='synthetic_remote_exchange']
        assert[(e['request_command'],e['handle'])for e in exchanges]==[('0x24','0x111'),('0x25','0x111'),('0x24','0x222'),('0x25','0x222')]
        nested.append({'outer_descriptor':[0,outer_handle,0],'physical_metadata_base':'0x500010000','host_alias':hex(metadata),
                       'metadata_bytes':body.hex(),'decoded_record_bytes':decoded.hex(),
                       'nested_pixel_descriptor':[hex(v)for v in triple],'decoded_dimensions':[7008,4672],
                       'resolved_pixel_address':hex(address),'events':list(events),
                       'qualification':'All resource-service responses and metadata are synthetic; address alias helper is identity-stubbed. Original parsing/field transport and nested segmented resolution execute; no pixel is read.'})
    # Cross-decode the parent's original-AP-generated metadata bytes using
    # original CP parser instructions. Only backing-address services are fixtures.
    fixture_path=ROOT/'research/evidence/sdyc-metadata.json'
    fixture_raw=fixture_path.read_bytes();assert hashlib.sha256(fixture_raw).hexdigest()==AP_FIXTURE_SHA256
    fixture=json.loads(fixture_raw)
    assert fixture['generator']['sha256']==AP_GENERATOR_SHA256
    assert hashlib.sha256((ROOT/fixture['generator']['path']).read_bytes()).hexdigest()==AP_GENERATOR_SHA256
    for source in fixture['sources']:
        assert hashlib.sha256((ROOT/source['path']).read_bytes()).hexdigest()==source['sha256']
    cross=[]
    for case in fixture['cases']:
        reset();fault=None
        body=bytes.fromhex(case['metadata_1024_hex']);assert len(body)==1024 and case['guards_unchanged']
        supplied=case['supplied_pixel_descriptor'];triple=tuple(int(supplied[k],16)for k in('unit_type','handle','offset'))
        tables={0x111:[(0x500010000,0x1000)],triple[1]:[(0x500800000,0x100000)]}
        cpu.mem_write(metadata,body);outer,out=arena+0x100,arena+0x200
        cpu.mem_write(outer,struct.pack('<3I',0,0x111,0))
        ret,_=execute(0x45b62c,[arena,outer,out]);assert ret==0
        decoded=bytes(cpu.mem_read(out,0x38));actual=struct.unpack_from('<3I',decoded,4)
        dims=list(struct.unpack_from('<2H',decoded,0x18))
        assert actual==triple and dims==case['supplied_dimensions']
        lo,hi=execute(0xaecc5c,[arena,out+4]);physical=lo+(hi<<32)
        assert physical==0x500800000+triple[2]
        cross.append({'AP_case_index':case['index'],'metadata_sha256':hashlib.sha256(body).hexdigest(),
                      'decoded_nested_descriptor':[hex(v)for v in actual],'decoded_dimensions':dims,
                      'CP_decoded_record_bytes':decoded.hex(),'fixture_resolved_pixel_address':hex(physical),
                      'service_events':list(events),
                      'qualification':'AP-emitted bytes are unchanged; outer and pixel backing service mappings are synthetic CP fixtures, not the AP supplied physical addresses or a shared camera allocation.'})
    cross_source={'path':str(fixture_path.relative_to(ROOT)),'artifact_sha256_at_read':hashlib.sha256(fixture_raw).hexdigest(),
                  'AP_binary_sources_verified':fixture['sources'],'parent_generator_record':fixture['generator'],
                  'qualification':'Final parent artifact and generator SHA256 are pinned and asserted; each consumed metadata byte sequence also has its own SHA256 above.'}
    ranges={
      'backing_address_resolver':(0xaecc5c,0xaecc7e),
      'query_then_release_table':(0xaecc7e,0xaeccc0),
      'logical_capacity_sum':(0xaeccc0,0xaeccea),
      'address_query_command24':(0xaeccea,0xaed0d6),
      'release_command25':(0xaed0ee,0xaed41a),
      'response_extract':(0xaedcd8,0xaedd16),
      'status_converter':(0xaedd16,0xaedd58),
      'query_sequence_generator':(0xaedc90,0xaedcd8),
      'logical_offset_to_segment':(0x45c214,0x45c2a8),
      'metadata_outer_resolver':(0xaec230,0xaec276),
      'metadata_outer_release':(0xaec284,0xaec2aa),
      'SSYC_signature_match':(0xaec2b8,0xaec2f0),
      'entry_walk':(0xaec314,0xaec380),
      'entry_payload_location':(0xaec380,0xaec3ac),
      'JPEG_YC_metadata_reader':(0x45b62c,0x45b70c),
      'YC_payload_decode':(0xaec69e,0xaec734),
      'JPEG_input_wrapper_extent_check':(0xaefbbe,0xaefc68),
      'JPEG_base_initializer_nested_pixel_resolution':(0xaf1d20,0xaf1dbe),
    }
    windows=[struct.unpack_from('<4Q',blob,0xcee7b0-BASE+32*i)for i in range(4)]
    result={'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},'source':{'path':str(path.relative_to(ROOT)),'sha256':DIGEST,'bytes':len(blob)},
      'camera_accessed':False,'target_executed':False,'host_emulation_performed':True,
      'scope':'Offline original Thumb instructions in synthetic host memory; no service, resource allocation, target process, camera, pixel processing or codec executes.',
      'address_model':'Retained runtime addresses = CP file offsets +0x108000; odd pointers select Thumb.',
      'observed':{'resolver_protocol':'0xaecc5c ->0xaecc7e ->query0x24 service0x9056015c ->validated response0x8024 ->8-slot table ->release0x25/response0x8025 ->45c214 segmented-offset arithmetic.',
        'descriptor_fields':'Backing helper reads handle at+4 and logical offset at+8. Descriptor type+0 is not consulted by this helper; higher layers may use it.',
        'query_result_table':'Count u16 at+0; up to8 interpreted slots stride16: u64 base at+8+16*i, u32 length at+0x10+16*i. Resolver scans8 lengths, not count.',
        'response_table':'Response command/status at+0/+2, sequence u16+6, segment count u16+8; segment base+0x18+16*i,length+0x20+16*i.',
        'outer_vs_nested':'JPEG45b62c resolves outer metadata descriptor, checks SSYC, walks ETRY type0, decodes a second pixel unit/handle/offset triple from payload+0/+4/+0x18 into record+4/+8/+0xc. Pixel geometry sourcepayload+0x2e/+0x30 ->record+0x18/+0x1a.',
        'metadata_release':'45b62c releases outer handle after parsing. aecc5c releases query handle before returning physical scalar. Neither timing establishes live buffer ownership/lifetime.',
        'JPEG_pixel_callsite':'Initializer0xaf1d9e copies object+0x88/+0x8c/+0x90 (decoded image triple) totemp;0xaf1db4 calls aecc5c; stores physical result64bits at object+0xd8.',
        'read_alias_windows':[{'physical_base':hex(a),'alias_base':hex(b),'third_word':hex(c),'span':hex(d)}for a,b,c,d in windows]},
      'host_tests':{'AP_to_CP_crossdecode_source':cross_source,'AP_to_CP_crossdecode_cases':cross,'query_flag_cases':flag_cases,'segment_resolution_cases':cases,'query_error_cases':errors,'capacity_cases':capacity,'nested_metadata_cases':nested,
        'stubs':['0x68c968 synthetic service exchange mutates real envelope withfixture reply; no real transport','0x56309c/0x563308 lock/unlock return0; sequencegeneratoractualcounterexecutes',
                 '0x61e60c hostmemcpy;0x61ed00 hostzero;0x61f038 hostmemcmp','0x5feb94 identity address-alias standin after actualwindow arithmetic',
                 '0xaecb3c/0xaecb9c/0x68e3a2/0x68af0c diagnostics intercepted'],
        'qualification':'Synthetic handles, segment bases, lengths, metadata, sequence replies and aliases prove instruction transport/arithmetic only. No real allocation, cache behavior, mapping validity, frame ownership or image content is observed.'},
      'comparison_to_MLUT_ring':{'established':'Earlier trace_mlut_encoder_link.py follows resource-service pool token/base/current ->context+0x1e0 ->pixel port+0x160. This mapper establishes how an independently supplied nested pixel handle+offset resolves to a backing address.',
        'unresolved':'No source-bound equality yet between the live nested pixel handle/segments and the MLUT ring token/base/current, and no port direction or saved-frame ownership proof.'},
      'limits':['Resolving one logical offset returns one physical point; capacity summed across segments does not prove a contiguous extent.',
        'No caller-provided requested pixel extent is checked by45c214; separate wrapper checks may apply.',
        'Response segment count is trusted in query copy loops; this pass tests atmost8 and doesnotestablish remote-service guarantees.',
        'No CUBE grid/channel/precision or arbitrary still LUT support is established.'],
      'data':{'status_table_file_offset':hex(0xce7770-BASE),'status_table_words':[hex(v)for v in struct.unpack_from('<18I',blob,0xce7770-BASE)],
              'metadata_signature':{'runtime_address':'0xfb9176','bytes':blob[0xfb9176-BASE:0xfb9176-BASE+4].hex()},
              'entry_signature':{'runtime_address':'0xf4066c','bytes':blob[0xf4066c-BASE:0xf4066c-BASE+4].hex()}},
      'disassembled_regions':{n:region(*r)for n,r in ranges.items()},
      'verification':{'SHA256_pinned':True,'decoded_regions':len(ranges),'host_cases':len(cases)+len(errors)+len(capacity)+len(nested)+len(cross)+len(flag_cases)}}
    out=ROOT/'research/evidence/sdyc-mapping.json';out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'output':str(out.relative_to(ROOT)),**result['verification']}))


if __name__=='__main__':main()
