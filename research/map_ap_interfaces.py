"""Offline AP ELF dependencies, resolved call sites and bounded LUT/still traces.

No target execution or USB API. Outputs ordinary static evidence, not REA Evidence.
"""
from collections import Counter, defaultdict
from pathlib import Path
from hashlib import sha256
import io
import json
import re
import struct
import sys
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[1]
DEV = ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev'
LIB = DEV/'nflasha15_unpacked/lib'
OUT = ROOT/'research/evidence/ap-interfaces.json'
TOPICS = ['Lut', 'lut', 'RecordStill', 'ReadStillImage', 'libstill', 'avbbc', 'Avbbc', 'Dmm', 'dmm', 'Interchip']

class Image:
    def __init__(self, path):
        self.path = path
        self.blob = path.read_bytes()
        self.elf = ELFFile(io.BytesIO(self.blob))
        self.symbols = list(self.elf.get_section_by_name('.dynsym').iter_symbols()) if self.elf.get_section_by_name('.dynsym') else []
        self.labels = defaultdict(list)
        for s in self.symbols:
            if s['st_shndx'] != 'SHN_UNDEF': self.labels[s['st_value']].append(s.name)
        plt, rel = self.elf.get_section_by_name('.plt'), self.elf.get_section_by_name('.rela.plt')
        if plt and rel:
            for i, r in enumerate(rel.iter_relocations()):
                self.labels[plt['sh_addr'] + 32 + i * 16].append(self.symbols[r['r_info_sym']].name+'@plt')
        self.relocations = []
        for sec in self.elf.iter_sections():
            if sec['sh_type'] not in ['SHT_RELA', 'SHT_REL']: continue
            syms = self.elf.get_section(sec['sh_link']) if sec['sh_link'] else None
            for r in sec.iter_relocations():
                sym = syms.get_symbol(r['r_info_sym']).name if syms and r['r_info_sym'] else None
                self.relocations.append({'offset':r['r_offset'], 'type':r['r_info_type'],
                                         'addend':r.entry.get('r_addend'), 'symbol':sym})
    def bytes_at(self, addr, size):
        for seg in self.elf.iter_segments():
            if seg['p_type']=='PT_LOAD' and seg['p_vaddr']<=addr and addr+size<=seg['p_vaddr']+seg['p_filesz']:
                at=seg['p_offset']+addr-seg['p_vaddr']; return self.blob[at:at+size]
        raise ValueError(hex(addr))
    def read_string(self, addr, limit=300):
        for seg in self.elf.iter_segments():
            if seg['p_type']=='PT_LOAD' and seg['p_vaddr']<=addr<seg['p_vaddr']+seg['p_filesz']:
                at=seg['p_offset']+addr-seg['p_vaddr']; raw=self.blob[at:at+limit].split(b'\0')[0]
                if raw and all(c in [9,10,13] or 32<=c<127 for c in raw): return raw.decode('ascii')
        return None
    def symbols_containing(self, addr):
        return [s.name for s in self.symbols if s['st_shndx']!='SHN_UNDEF' and s['st_value']<=addr<s['st_value']+s['st_size']]
    def disasm(self, start, end):
        result=[]; regs={}
        for i in Cs(CS_ARCH_ARM64, CS_MODE_ARM).disasm(self.bytes_at(start,end-start),start):
            row={'address':hex(i.address), 'bytes':i.bytes.hex(), 'mnemonic':i.mnemonic,'operands':i.op_str}
            if i.mnemonic in ['bl','b'] and i.op_str.startswith('#0x'):
                target=int(i.op_str[1:],16); row['target']=hex(target)
                if target in self.labels: row['symbols']=self.labels[target]
            parts=i.op_str.split(', ')
            if i.mnemonic in ['adrp','adr'] and len(parts)==2 and parts[1].startswith('#0x'):
                regs[parts[0]]=int(parts[1][1:],16)
            elif i.mnemonic=='add' and len(parts)==3 and parts[1] in regs and parts[2].startswith('#0x'):
                regs[parts[0]]=regs[parts[1]]+int(parts[2][1:],16)
                value=regs[parts[0]]; st=self.read_string(value)
                if st: row['linear_literal_candidate']={'address':hex(value),'text':st}
            elif parts and parts[0] in regs and i.mnemonic not in ['cmp','str','strb','strh','stp','stur','sturh','tst','bl','b']:
                regs.pop(parts[0],None)
            if i.mnemonic=='bl':
                regs={k:v for k,v in regs.items() if k.startswith('x') and int(k[1:])>=19}
            result.append(row)
        assert len(result)*4==end-start
        return result
    def direct_sites(self, targets):
        calls=[]
        for sec in self.elf.iter_sections():
            if sec['sh_type']!='SHT_PROGBITS' or not(sec['sh_flags']&4): continue
            data=sec.data()
            for n,(w,) in enumerate(struct.iter_unpack('<I',data[:len(data)//4*4])):
                if w&0x7c000000!=0x14000000: continue
                imm=w&0x3ffffff
                if imm&0x2000000:imm-=0x4000000
                addr=sec['sh_addr']+n*4; target=addr+imm*4
                if target in targets:
                    calls.append({'address':hex(addr),'destination':hex(target),'kind':'bl' if w&0x80000000 else 'b',
                                  'target_symbols':self.labels.get(target,[]),'containing_exported_functions':self.symbols_containing(addr)})
        return calls
    def marker_references(self, markers):
        """Exact ADRP+ADD candidates, retaining instruction bytes for checking."""
        targets={}
        for marker in markers:
            match=self.blob.find(marker.encode())
            if match<0: continue
            at=self.blob.rfind(b'\0',max(0,match-400),match)+1
            for sec in self.elf.iter_sections():
                if sec['sh_offset']<=at<sec['sh_offset']+sec['sh_size']:
                    addr=sec['sh_addr']+at-sec['sh_offset']
                    targets[addr]=self.read_string(addr,500)
        result=[]; sec=self.elf.get_section_by_name('.text')
        words=struct.unpack('<'+'I'*(sec['sh_size']//4),sec.data())
        pages={t&~4095 for t in targets}
        for n,w in enumerate(words):
            if w&0x9f000000!=0x90000000: continue
            imm=((w>>29)&3)|(((w>>5)&0x7ffff)<<2)
            if imm&0x100000: imm-=0x200000
            addr=sec['sh_addr']+n*4; page=(addr&~4095)+(imm<<12)
            if page not in pages:continue
            reg=w&31
            for m in range(n+1,min(n+7,len(words))):
                q=words[m]
                if q&0xffc00000==0x91000000 and (q>>5)&31==reg:
                    value=page+((q>>10)&4095)
                    if value in targets:
                        result.append({'adrp_address':hex(addr),'add_address':hex(sec['sh_addr']+m*4),
                                       'literal_address':hex(value),'literal':targets[value],
                                       'instruction_neighborhood':self.disasm(addr,sec['sh_addr']+(m+1)*4)})
        return result
    def enum_name_records(self):
        """Recover only records corroborated by a relocation and exact length."""
        targets={}
        pattern=rb'(?:SHOOTING_MODE_BASE_SETTING_|TARGET_DISPLAY_|SELECT_LUT_)[A-Z0-9_]+\0'
        for match in re.finditer(pattern,self.blob):
            at=match.start()
            for sec in self.elf.iter_sections():
                if sec['sh_offset']<=at<sec['sh_offset']+sec['sh_size']:
                    targets[sec['sh_addr']+at-sec['sh_offset']]=match.group()[:-1].decode()
        result=[]
        for rel in self.relocations:
            if rel['addend'] not in targets:continue
            pointer=rel['offset']; raw=self.bytes_at(pointer-8,16)
            value,length,address=struct.unpack('<IIQ',raw); label=targets[rel['addend']]
            if length!=len(label)+1 or address!=rel['addend']:continue
            result.append({'record_address':hex(pointer-8),'value':value,'string_bytes_including_NUL':length,
                           'literal_address':hex(address),'name':label,'raw_record_hex':raw.hex(),
                           'pointer_relocation':{'address':hex(pointer),'type':rel['type'],'addend':hex(rel['addend'])}})
        return result

def inventory():
    rows=[]; definitions=defaultdict(list)
    for path in sorted(DEV.rglob('*')):
        if not path.is_file():continue
        with path.open('rb') as f:
            if f.read(4)!=b'\x7fELF':continue
        try:
            im=Image(path); elf=im.elf; dyn=elf.get_section_by_name('.dynamic'); tags=list(dyn.iter_tags()) if dyn else []
            needed=[t.needed for t in tags if t.entry.d_tag=='DT_NEEDED']
            soname=next((t.soname for t in tags if t.entry.d_tag=='DT_SONAME'),None)
            exports=[s for s in im.symbols if s['st_shndx']!='SHN_UNDEF' and s.name]
            imports=[s for s in im.symbols if s['st_shndx']=='SHN_UNDEF' and s.name]
            rel=str(path.relative_to(ROOT))
            for s in exports: definitions[s.name].append({'path':rel,'address':hex(s['st_value']),'bytes':s['st_size']})
            rows.append({'path':rel,'bytes':len(im.blob),'sha256':sha256(im.blob).hexdigest(),'machine':elf['e_machine'],
                         'elf_class':elf.elfclass,'type':elf['e_type'],'soname':soname,'needed':needed,
                         'runpaths':[getattr(t,'runpath',getattr(t,'rpath',None)) for t in tags if t.entry.d_tag in ['DT_RPATH','DT_RUNPATH']],
                         'import_count':len(imports),'export_count':len(exports),
                         'topic_symbols':[{'name':s.name,'address':hex(s['st_value']),'bytes':s['st_size'],'defined':s['st_shndx']!='SHN_UNDEF'} for s in im.symbols if any(t in s.name for t in TOPICS)]})
        except Exception as exc: rows.append({'path':str(path.relative_to(ROOT)),'error':str(exc)})
    providers=defaultdict(list)
    for row in rows:
        if 'error' in row:continue
        for key in {Path(row['path']).name,row['soname']} - {None}:providers[key].append(row['path'])
    edges=[]
    for row in rows:
        for name in row.get('needed',[]): edges.append({'from':row['path'],'needed':name,'candidate_paths':providers[name]})
    return rows,edges,definitions

def main():
    expected={'appFw.so':'2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b',
              'libmpr.so':'4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9',
              'libosal_uipc.so':'09ebf5980237fc473077dc844763dcb00cba423f431646e3be7c0ca49ea70ff3'}
    for name,digest in expected.items():
        if sha256((LIB/name).read_bytes()).hexdigest()!=digest:
            raise ValueError('Require pinned official A7C II 2.01 input: '+name)
    rows,edges,definitions=inventory()
    report={'mode':'ordinary_offline_static_analysis_not_REA_Evidence','camera_accessed':False,'target_executed':False,
            'writes_to_extracted_firmware':False,'inventory_scope':str(DEV.relative_to(ROOT)),
            'elf_inventory':rows,'DT_NEEDED_graph':edges,
            'counts':{'ELF_files':len(rows),'DT_NEEDED_edges':len(edges),'unresolved_needed_edges':sum(not e['candidate_paths'] for e in edges)},
            'limitations':['DT_NEEDED candidates are filename/SONAME matches within extracted files, not observed loader bindings.',
                           'Direct branch scan excludes indirect calls, callbacks, dynamically resolved symbols and runtime paths.',
                           'All addresses are exact ELF virtual addresses, not runtime addresses.',
                           'No camera session, execution, upload or installation.','Linear literal comments are candidates; disassembly is authoritative.']}
    ranges={
      'appFw.so':[(0x3560f18,0x356103c,'SetLutData command adapter'),(0x3559528,0x35595f0,'64-byte string copy'),(0x3554e00,0x3554f00,'return conversion bounded neighborhood'),
                   (0x34d5548,0x34d5588,'SetLutData category16 operation116 wrapper'),
                   (0x3428510,0x342863c,'sequence_select_lut send_lut'),(0x342863c,0x342888c,'sequence_select_lut send_lut_procam'),
                   (0x342888c,0x34289a8,'LUT selection to CP request input'),(0x34289a8,0x34289f0,'selection request wrapper'),
                   (0x34cfac8,0x34cfb08,'CP LUT category10 operation276 wrapper'),
                   (0x3537eac,0x3537f58,'CP LUT request command0x4155 construction'),(0x352c85c,0x352c8a0,'CP LUT command payload packing'),
                   (0x253b114,0x253b1e0,'CP command0x4155 registration/response neighborhood'),
                   (0x35358b8,0x3535b5c,'execute_infra_cmd_i complete request submission'),
                   (0x3599b68,0x3599bf8,'create_msg allocation and envelope copy'),
                   (0x34f4f4c,0x34f4fec,'optional response future retrieval'),
                   (0x31018f0,0x310197c,'future state wait and futex loop'),
                   (0x1794224,0x1794298,'mutex lock and unlock wrapper neighborhood')],
      'libmpr.so':[(0xd5dcc,0xd60e8,'Reset/Execute/SetLutData'),(0x119de4,0x119e34,'receiver set/reset LUT state'),
                   (0x17dc40,0x17dce0,'LUT utility entry points bounded'),(0x17dd3c,0x17dfc8,'CUBE header field parsing'),(0x17dfc8,0x17e174,'CUBE parser'),
                   (0x17e174,0x17e3ac,'binary metadata/header packing'),(0x17e3ac,0x17e880,'table conversion'),
                   (0x17e880,0x17eb54,'file loader'),(0x17daec,0x17db18,'binary byte order helpers'),
                   (0x19e638,0x19e708,'KlvConverter GetSerialize_SonyLookControl'),
                   (0x1211c8,0x12120c,'writer consumer of stored LUT payload'),(0xc6448,0xc6520,'stillplay GetLutNrtInfo'),
                   (0x250da8,0x250df0,'NRT metadata wrapper forwarding'),(0x2374b0,0x2374c0,'NRT metadata inner forwarding'),
                   (0x240080,0x2400bc,'NRT metadata virtual dispatch'),
                   (0xab640,0xab830,'mpr GetLutNrtInfo'),(0xeb260,0xeb654,'RecordStill'),(0xc72c8,0xc7674,'ReadStillImage')],
      'libosal_uipc.so':[(0x5e60,0x610c,'osal_snd_msg complete exported function'),
                         (0x67b0,0x6970,'osal_valloc_msg_wait complete exported function'),
                         (0x7e20,0x7e28,'osal_set_msg_src header assignment'),
                         (0x3a10,0x3a98,'u_osal_snd_msg ioctl forwarding'),
                         (0x3e98,0x3f28,'u_osal_valloc_msg_wait ioctl forwarding')]}
    report['images']={}
    for name,rangeset in ranges.items():
        im=Image(LIB/name)
        names=['SetLutData','RecordStill','ReadStillImage','GetLutNrtInfo']
        targets={a for a,labels in im.labels.items() if any(any(n in label for n in names) for label in labels)}
        targets.update([a for a,_,_ in rangeset])
        image={'sha256':sha256(im.blob).hexdigest(),'file_bytes':len(im.blob),
               'direct_reference_scan':im.direct_sites(targets),
               'ranges':[{'start':hex(a),'end_exclusive':hex(b),'label':label,'entry_symbols':im.labels.get(a,[]),
                          'instructions':im.disasm(a,b)} for a,b,label in rangeset],
               'selected_symbol_candidates':[],'relocations_to_selected_functions':[]}
        if name=='appFw.so':
            image['assertion_literal_references']=im.marker_references([
                'sequence_select_lut::send_lut_procam(', 'sequence_select_lut::send_lut(', 'SELECT_LUT_SDR_STILL'])
            image['enum_value_name_records']=im.enum_name_records()
        starts={a for a,_,_ in rangeset}
        image['relocations_to_selected_functions']=[{**r,'offset':hex(r['offset']),'addend':hex(r['addend']) if r['addend'] is not None else None} for r in im.relocations if r['addend'] in starts]
        for s in im.symbols:
            if s['st_shndx']=='SHN_UNDEF' and any(t in s.name for t in TOPICS):
                image['selected_symbol_candidates'].append({'name':s.name,'candidate_definitions':definitions.get(s.name,[])})
        report['images'][name]=image
    report['recovered_contracts']={
        'mpr_SetLutData_input':{
            'evidence':'appFw.so 0x3560f18–0x356103c; libmpr.so 0xd5e60–0xd60e8',
            'minimum_accessed_extent_bytes':95,'caller_stack_allocation_bytes':96,
            'fields':[{'offset':0,'bytes':4,'observed':'caller writes 0x6e; role unresolved'},
                      {'offset':4,'bytes':1,'observed':'zero resets; nonzero executes under mutex'},
                      {'offset':5,'bytes':26,'observed':'filename copied with strncpy(26); last byte checked for NUL; passed to fopen'},
                      {'offset':31,'bytes':64,'observed':'name/identifier copied with strncpy(64); last byte checked for NUL; empty first byte rejected by Execute'}],
            'unknowns':['Not a complete safe runtime ABI.','Thread/lifetime/setup requirements unknown.','No proof this API grades pixels.']},
        'CP_0x4155_AP_request':{
            'evidence':'appFw.so 0x3537eac–0x3537f58 and helper 0x352c85c–0x352c8a0',
            'message_bytes':96,'event_id':'0x90030001','command_id':'0x4155',
            'fields':[{'offset':0,'bytes':1,'observed':'5'}, {'offset':2,'bytes':2,'observed':'0x4155'},
                      {'offset':4,'bytes':4,'observed':'input+0'}, {'offset':8,'bytes':4,'observed':'input+4'},
                      {'offset':12,'bytes':4,'observed':'input+8 when nonzero, otherwise 0xffffffff'},
                      {'offset':16,'bytes':4,'observed':'input+12 if input byte+92 nonzero, otherwise zero'},
                      {'offset':20,'bytes':4,'observed':'input+16'},
                      {'offset':24,'bytes':4,'observed':'input+20 if input byte+92 nonzero, otherwise zero'},
                      {'offset':28,'bytes':4,'observed':'input+24 if input byte+92 nonzero, otherwise zero'},
                      {'offset':32,'bytes':63,'observed':'memcpy from input+28; final message byte stays zero'}],
            'unknowns':['Field meanings require CP corroboration.','No execution/acknowledgement observed.']},
        'AP_request_lifetime':{
            'evidence':'appFw.so 0x35358b8–0x3535b5c, 0x3599b68–0x3599bf8, 0x3537eac–0x3537f58; libosal_uipc.so functions retained',
            'observations':['create_msg allocates a message using osal_valloc_msg_wait and memcpy copies the entire caller envelope.',
                            'For 0x4155, envelope size is 96, event ID0x90030001, source ID0x80020002.',
                            'osal_set_msg_src stores source ID at allocated payload pointer-12.',
                            'osal_snd_msg event bit15 clear selects u_osal_snd_msg, which forwards event and pointer through ioctl.',
                            'Only non-NULL output buffer registers promise lookup and calls future retrieval after send.',
                            '0x4155 passes NULL output buffer and skips both promise association and future retrieval.',
                            'Generic future retrieval reaches a libstdc++ futex wait loop, but this branch is not taken for0x4155.',
                            'No request-message free appears on the normal NULL-output branch of execute_infra_cmd_i.'],
            'inferences':['Caller stack envelope need not survive successful copy; referenced payload resources still need separate lifetime management.',
                          'Normal return from this 0x4155 adapter is submission-path completion, not CP processing or frame completion.'],
            'unknowns':['Kernel-side queue ownership and receiver/free path not traced.',
                        'Allocation and mutex operations may block; not a nonblocking guarantee.',
                        'No runtime success, CP acknowledgement, DMA completion or frame scheduling observed.']},
        'mpr_embedded_LUT_payload':{
            'evidence':'libmpr.so 0x17e174–0x17eb54',
            'size_formula':'45 + 0xd00 + 12 * grid_size ** 3',
            'outer_container':'12-byte ELUT header, then SonyLookControl KLV; inner SONYBCUB starts at outer+45',
            'SonyLookControl_UL_key':{'symbol':'_ZN3mpr12KlvConverter27UDAMSetSonyLookControlValueE',
                                     'address':'0x2d3e38','hex':'060e2b34021301010e060c0201020242'},
            'allocation_bytes':'0x6a1b9','allocation_formula_match':'33**3*12+0xd00+45',
            'relative_to_SONYBCUB_start':[{'offset':0,'bytes':8,'observed':'SONYBCUB'},
                                       {'offset':64,'bytes':64,'observed':'zeroed'},
                                       {'offset':128,'bytes':64,'observed':'configuration structure including domain floats'},
                                       {'offset':192,'bytes':64,'observed':'zeroed'},
                                       {'offset':256,'bytes':768,'observed':'zeroed, then name/identifier copied'},
                                       {'offset':1024,'bytes':256,'observed':'zeroed'},
                                       {'offset':1280,'bytes':2048,'observed':'zeroed'},
                                       {'offset':3328,'observed':'RGB table: sscanf doubles -> float32 -> byte-swap32 -> copy; 12 bytes per entry'}],
            'unknowns':['Not asserted to be CP hardware LUT format.','Float sample input order follows file order; channel and cube-axis conventions not independently validated.','Metadata presence does not prove saved still pixel processing.']}}
    OUT.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({'output':str(OUT.relative_to(ROOT)),**report['counts'],'camera_accessed':False}))
if __name__=='__main__':main()
