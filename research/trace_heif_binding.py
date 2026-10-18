#!/usr/bin/env python3
"""Offline source-bound HEIF table binding, initialization and activation checks."""
import json
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from elftools.elf.elffile import ELFFile
from unicorn import UC_HOOK_MEM_WRITE
from unicorn.arm64_const import UC_ARM64_REG_PC, UC_ARM64_REG_X0
from trace_heif_encoder import Machine, SOURCE, SOURCE_SHA, ROOT, REGS, sha

TABLE_NAME = '_ZN16ObjAvbbcCmnTable16midConvTableToMcE'


class BindingMachine(Machine):
    def __init__(self):
        super().__init__()
        self.init_mode=False;self.activation_mode=False
        self.service_stubs=[];self.ctors={};self.selected=[];self.started=[]
        self.bindings=[];self.allocation_cursor=self.ARENA+0xc000
        self.observed_writes=[];self.init_source_write_count=0;self.init_local_calls=set()
        with SOURCE.open('rb') as f:
            e=ELFFile(f);sy=e.get_section_by_name('.dynsym')
            for r in e.get_section_by_name('.rela.dyn').iter_relocations():
                if r.entry.r_info_type not in (257,1025) or not r.entry.r_info_sym:continue
                s=sy.get_symbol(r.entry.r_info_sym)
                if not isinstance(s.entry.st_shndx,int):continue
                self.u.mem_write(r.entry.r_offset,struct.pack('<Q',s.entry.st_value+r.entry.r_addend))
                self.bindings.append({'offset':hex(r.entry.r_offset),'type':r.entry.r_info_type,'symbol':s.name,'value':hex(s.entry.st_value+r.entry.r_addend)})
        self.u.hook_add(UC_HOOK_MEM_WRITE,self.write_watch)

    def write_watch(self,u,access,address,size,value,_):
        if not self.init_mode:return
        if address<0x200000:self.init_source_write_count+=1
        if 0x16bd80<=address<0x16be30 or 0x1b3668<=address<0x1b3670:
            self.observed_writes.append({'pc':hex(u.reg_read(UC_ARM64_REG_PC)),'address':hex(address),'bytes':size,'value':hex(value)})

    def external(self,u,addr,size,user):
        name=self.imports.get(addr);a=[u.reg_read(r) for r in REGS]
        if self.activation_mode and name=='_Znwm':
            ptr=self.allocation_cursor;self.allocation_cursor+=(a[0]+15)&~15
            assert self.allocation_cursor<self.ARENA+0xf000
            self.ctors[ptr]={'bytes':a[0]};self.ret(ptr)
        elif self.activation_mode and 'C1E' in name and 'Seq' in name:
            self.ctors[a[0]].update({'constructor':name,'task_argument':hex(a[3])});self.ret()
        elif self.activation_mode and 'SetSeqInstance' in name:
            self.selected.append(self.ctors.get(a[1],{'pointer':hex(a[1])}));self.ret()
        elif self.activation_mode and 'StartSequence' in name:
            self.started.append({'message':hex(a[1])});self.ret()
        elif 'ConvertCid' in name or 'ConvertCmdMid' in name:
            assert name in self.symbols
            u.reg_write(UC_ARM64_REG_PC,self.symbols[name][0])
        elif self.init_mode and name=='_ZN3MWF7ObjLockC1Ev':
            self.service_stubs.append({'name':name,'object':hex(a[0]),'operation':'No external lock service executes'});self.ret()
        elif self.init_mode and name=='__cxa_atexit':
            self.service_stubs.append({'name':name,'arguments':[hex(v)for v in a[:3]]});self.ret()
        elif self.init_mode and name in ('__gmon_start__','_Jv_RegisterClasses','_ITM_registerTMCloneTable'):
            self.service_stubs.append({'name':name});self.ret()
        elif self.init_mode and name in self.symbols:
            self.init_local_calls.add(name);u.reg_write(UC_ARM64_REG_PC,self.symbols[name][0])
        else:
            super().external(u,addr,size,user)


class AvCamMachine:
    ARENA,STACK,END=0x10000000,0x20000000,0x30000000
    def __init__(self,blob):
        self.u=Uc(UC_ARCH_ARM64,UC_MODE_ARM)
        self.u.mem_map(0,(len(blob)+4095)&~4095);self.u.mem_write(0,blob)
        for address,size in [(self.ARENA,0x10000),(self.STACK,0x10000),(self.END,4096)]:self.u.mem_map(address,size)
        self.actions=[];self.sent=[]
        self.u.hook_add(UC_HOOK_CODE,self.service,begin=0x2798f0,end=0x2798f0)
        self.u.hook_add(UC_HOOK_CODE,self.service,begin=0x279930,end=0x279930)
        for address in [0x1aac8c,0x1aaed8,0x1ab124,0x1a94dc]:self.u.hook_add(UC_HOOK_CODE,self.service,begin=address,end=address)
        self.initial=self.u.context_save()
    def service(self,u,address,size,_):
        from unicorn.arm64_const import UC_ARM64_REG_X30
        a=[u.reg_read(r)for r in REGS]
        if address in [0x1aac8c,0x1aaed8,0x1ab124]:self.actions.append({'service_file_offset':hex(address),'context':hex(a[0]),'payload':hex(a[1])})
        elif address==0x1a94dc:self.sent.append({'boundary_file_offset':hex(address),'reply_destination':hex(a[1]),'bytes':a[3],'payload_hex':bytes(u.mem_read(a[2],a[3])).hex()})
        u.reg_write(UC_ARM64_REG_X0,0);u.reg_write(UC_ARM64_REG_PC,u.reg_read(UC_ARM64_REG_X30))
    def run(self,va,args):
        from unicorn.arm64_const import UC_ARM64_REG_SP, UC_ARM64_REG_X30
        self.u.context_restore(self.initial);self.u.mem_write(self.STACK,bytes(0x10000))
        self.u.reg_write(UC_ARM64_REG_SP,self.STACK+0x8000);self.u.reg_write(UC_ARM64_REG_X30,self.END)
        for reg,value in zip(REGS,args):self.u.reg_write(reg,value)
        self.u.emu_start(va,self.END,count=10000)
        assert self.u.reg_read(UC_ARM64_REG_PC)==self.END
    def setup(self):
        self.u.mem_write(self.ARENA,bytes(0x10000));self.obj,self.request,self.ctx=self.ARENA+0x1000,self.ARENA+0x5000,self.ARENA+0x8000
        self.u.mem_write(self.obj+0x2668,struct.pack('<Q',self.ctx));self.u.mem_write(self.ctx+8,struct.pack('<I',3))
        self.u.mem_write(self.obj+0x2678,struct.pack('<Q',self.ctx+0x100));self.u.mem_write(self.obj+0x2688,struct.pack('<2Q',7,8))
        self.actions.clear();self.sent.clear()


def main():
    data=SOURCE.read_bytes();assert sha(data)==SOURCE_SHA
    m=BindingMachine();m.setup()
    with SOURCE.open('rb')as f:
        elf=ELFFile(f);sy=elf.get_section_by_name('.dynsym');relocs=list(elf.get_section_by_name('.rela.dyn').iter_relocations());rel={r.entry.r_offset:r for r in relocs}
        initsec=elf.get_section_by_name('.init_array');roots=[(elf.get_section_by_name('.init').header.sh_addr,'DT_INIT')]
        for offset in range(0,initsec.header.sh_size,8):
            r=rel.get(initsec.header.sh_addr+offset);value=r.entry.r_addend if r and r.entry.r_info_type==1027 else struct.unpack_from('<Q',initsec.data(),offset)[0]
            roots.append((value,f'DT_INIT_ARRAY[{offset//8}]'))
        table_symbol=next(s for s in sy.iter_symbols()if s.name==TABLE_NAME)
        table_sec=elf.get_section(table_symbol.entry.st_shndx)
        table_segment=next(s for s in elf.iter_segments()if s.header.p_type=='PT_LOAD'and s.header.p_vaddr<=table_symbol.entry.st_value<s.header.p_vaddr+s.header.p_filesz)
        table_relocs=[{'offset':hex(r.entry.r_offset),'type':r.entry.r_info_type,'symbol':sy.get_symbol(r.entry.r_info_sym).name}for r in relocs if r.entry.r_info_sym and sy.get_symbol(r.entry.r_info_sym).name==TABLE_NAME]
        imports=sorted(s.name for s in sy.iter_symbols()if not isinstance(s.entry.st_shndx,int))
        textsec=elf.get_section_by_name('.text');textdata=textsec.data();text_va=textsec.header.sh_addr
    table_va,table_size=m.symbols[TABLE_NAME];table_before=bytes(m.u.mem_read(table_va,table_size));got_before=bytes(m.u.mem_read(0x1b3668,8))
    conversion_cases=[]
    pairs=list(struct.iter_unpack('<HH',table_before));lookup={}
    for logical,native in pairs:
        if native==0xffff:break
        lookup.setdefault(logical,native)
    queries=sorted(set(list(range(0x500,0x539))+[0,0xffff,0x10534]))
    for logical in queries:
        actual=m.run(0x141b98,[0,logical]);expected=lookup.get(logical&0xffff,0xffff);assert actual==expected
        conversion_cases.append({'logical':hex(logical),'actual_native':hex(actual),'independent_table_expected':hex(expected)})
    # A synthetic table override demonstrates the GOT dependency, not a patch or
    # a discovered runtime override. The marker deliberately is not a wire ID.
    synthetic=m.ARENA+0xb000;m.u.mem_write(synthetic,struct.pack('<4H',0x534,0x7a11,0xffff,0xffff));m.u.mem_write(0x1b3668,struct.pack('<Q',synthetic))
    alternate=m.run(0x141b98,[0,0x534]);assert alternate==0x7a11
    m.u.mem_write(0x1b3668,got_before);assert m.run(0x141b98,[0,0x534])==0xffff
    initializers=[];m.init_mode=True
    for va,name in roots:
        writes=m.init_source_write_count;calls=len(m.service_stubs);m.run(va,[])
        initializers.append({'name':name,'entry_va':hex(va),'finished':True,'native_source_region_write_count':m.init_source_write_count-writes,'external_service_stubs':m.service_stubs[calls:]})
    m.init_mode=False
    assert bytes(m.u.mem_read(table_va,table_size))==table_before and bytes(m.u.mem_read(0x1b3668,8))==got_before and not m.observed_writes
    after_init=[]
    for logical in [0x531,0x532,0x533,0x534]:
        actual=m.run(0x141b98,[0,logical]);assert actual==lookup.get(logical,0xffff);after_init.append({'logical':hex(logical),'actual_native':hex(actual)})
    full_requests=[]
    for mode in [3,4,5]:
        m.setup(mode);assert m.run(0x1205b4,[m.obj,m.unit])==0 and len(m.sent)==1
        raw=bytes.fromhex(m.sent[0]['payload_hex']);assert struct.unpack_from('<4H',raw)==(0x2003,0xffff,96,1)
        full_requests.append({'kind':'encode','object_mode':mode,'header_halfwords':list(struct.unpack_from('<4H',raw)),'intercepted_send':m.sent.copy(),'sender_executed':False})
    for command,length in [(0x532,16),(0x533,24)]:
        m.setup();m.u.mem_write(m.obj+0x20,struct.pack('<H',command));assert m.run(0x1206a4,[m.obj,m.unit])==0 and len(m.sent)==1
        raw=bytes.fromhex(m.sent[0]['payload_hex']);assert struct.unpack_from('<4H',raw)==(0x2003,0xffff,length,1)
        full_requests.append({'kind':hex(command),'header_halfwords':list(struct.unpack_from('<4H',raw)),'intercepted_send':m.sent.copy(),'sender_executed':False})
    reverse=[]
    for native in [0x8107,0x8108,0x8109,0x810c,0x810d,0x810e,0xffff]:
        m.u.mem_write(m.out,b'\xa5'*8);m.run(0x141ed8,[0,native,m.out,m.out+4]);raw=bytes(m.u.mem_read(m.out,8))
        expected={0x8107:0x855f,0x8108:0x8560,0x8109:0x8561}.get(native)
        assert raw[:2]==(b'\xa5\xa5'if expected is None else struct.pack('<H',expected))
        reverse.append({'native':hex(native),'output_u16':hex(struct.unpack_from('<H',raw)[0]),'kind_byte':raw[4],'output_unchanged':raw==b'\xa5'*8})
    hook_cases=[]
    for args in [(0,0,0,0,0),(0,5,0x534,m.out,96),(0xffffffff,0xffff,0x8565,m.out,32)]:
        before=bytes(m.u.mem_read(m.out,128));actual=m.run(0x1423b8,list(args));assert actual==1 and bytes(m.u.mem_read(m.out,128))==before
        hook_cases.append({'arguments':[hex(v)for v in args],'return':actual,'buffer_unchanged':True})
    activation=[];m.activation_mode=True
    gate_inputs=[(0x1500,2,x)for x in range(256)]+[(f,s,2)for f in [0x1500,0x1630,0x149f]for s in [0,1,3,4,5,6,7,8,9]]
    for family,selector,fmt in gate_inputs:
        m.setup();m.ctors.clear();m.selected.clear();m.started.clear();m.allocation_cursor=m.ARENA+0xc000
        m.u.mem_write(m.obj+0x20,struct.pack('<3Q',m.ARENA+0xa000,m.ARENA+0x9000,m.param))
        m.u.mem_write(m.unit+0xa,struct.pack('<H',selector));m.u.mem_write(m.unit+0x2c,struct.pack('<I',family));m.u.mem_write(m.unit+0x24,bytes([fmt]))
        ret=m.run(0x11785c,[m.obj,m.unit,0x11223344]);assert ret==0 and len(m.selected)==len(m.started)==1
        selected=m.selected[0].get('constructor','none')
        expected_heif=family==0x1500 and selector==2 and fmt==2
        assert ('SeqResizeEncForHeifC1E'in selected)==expected_heif
        activation.append({'family':hex(family),'selector':selector,'format_byte':fmt,'selected_constructor':selected,'is_HEIF_sequence':expected_heif,'started_original_message':True})
    m.activation_mode=False
    # Narrow reference scan: exact ADRP+LDR slot pattern in .text. It does not
    # claim a complete indirect-pointer/dataflow or malicious mutation analysis.
    md=Cs(CS_ARCH_ARM64,CS_MODE_LITTLE_ENDIAN);instructions=list(md.disasm(textdata,text_va));got_refs=[]
    for index,i in enumerate(instructions):
        if i.mnemonic=='adrp'and i.op_str=='x2, #0x1b3000':
            for j in instructions[index+1:index+5]:
                if j.mnemonic=='ldr'and j.op_str=='x2, [x2, #0x668]':got_refs.append({'adrp_va':hex(i.address),'load_va':hex(j.address),'access':'read GLOB_DAT slot'})
    assert got_refs==[{'adrp_va':'0x141b98','load_va':'0x141ba8','access':'read GLOB_DAT slot'}]
    avpath=next((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob('av-cam.bin'));av=avpath.read_bytes();avsha='90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096';assert sha(av)==avsha and av[8:12]==b'ORIL'
    cam=AvCamMachine(av);receivers=[]
    for selector in [0x10c,0x10d,0x10e,0xffff,0x109,0]:
        cam.setup();cam.u.mem_write(cam.request,struct.pack('<4H',0x2003,selector,96,1)+b'\xa5'*88);cam.run(0x1a7e2c,[cam.obj,cam.request])
        expected={0x10c:0x1aac8c,0x10d:0x1aaed8,0x10e:0x1ab124}.get(selector)
        assert (len(cam.actions)==1)==(expected is not None)
        if expected:assert cam.actions[0]=={'service_file_offset':hex(expected),'context':hex(cam.ctx+0x100),'payload':hex(cam.request+8)}
        receivers.append({'native_selector':hex(selector),'original_dispatch_actions':cam.actions.copy(),'actual_encoding_stubbed':True})
    completions=[]
    for selector in [0x810c,0x810d,0x810e]:
        for status in [0,1,0x80000000]:
            for es_size in [0,0x100000001]:
                cam.setup();cam.run(0x1a8470,[cam.obj,selector,0x1122334455667788,0x2233445566778899,status,0x12345678,es_size]);assert len(cam.sent)==1
                sent=cam.sent[0];raw=bytes.fromhex(sent['payload_hex']);assert sent['reply_destination']=='0x12345678'
                assert struct.unpack_from('<4H',raw)==(0x2003,selector,32 if selector==0x810e else 24,0)
                assert struct.unpack_from('<I',raw,0x10)[0]==status
                if selector==0x810e:assert struct.unpack_from('<Q',raw,0x18)[0]==es_size and struct.unpack_from('<Q',raw,8)[0]==0x2233445566778899
                else:assert struct.unpack_from('<Q',raw,8)[0]==0x1122334455667788
                completions.append({'selector':hex(selector),'status':hex(status),'synthetic_u64_es_size':hex(es_size),'submission':sent})
    regions=[]
    for lo,hi,role in [(0x9c170,0x9c190,'ELF DT_INIT'),(0xa2460,0xa27a0,'All init-array roots and startup helpers'),(0x141b98,0x141bd8,'MC forward lookup'),(0x141ed8,0x141f28,'MC inverse lookup'),(0x1423b8,0x1423c0,'Local default message hook'),(0x11785c,0x117b18,'Still encoder class activation')]:
        v,n,o=next(seg for seg in m.segments if seg[0]<=lo<hi<=seg[0]+seg[1]);off=lo-v+o;raw=data[off:off+hi-lo];decoded=list(md.disasm(raw,lo));assert sum(i.size for i in decoded)==hi-lo
        regions.append({'source_path':str(SOURCE.relative_to(ROOT)),'start_va':hex(lo),'end_va_exclusive':hex(hi),'file_offset':off,'role':role,'sha256':sha(raw),'instructions':[{'va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in decoded]})
    for lo,hi,role in [(0x1a7e2c,0x1a7fa4,'Native still encoder host selector dispatch'),(0x1a8470,0x1a86c4,'Native still encoder completion builder')]:
        raw=av[lo:hi];decoded=list(md.disasm(raw,lo));assert sum(i.size for i in decoded)==hi-lo
        regions.append({'source_path':str(avpath.relative_to(ROOT)),'file_offset':lo,'start_file_alias':hex(lo),'candidate_linked_start':hex(0xffffffc001000000+lo),'role':role,'sha256':sha(raw),'instructions':[{'file_alias':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in decoded]})
    result={'schema':1,'method':'Original instructions and bounded host emulation under explicit local ELF/GOT binding; not REA Evidence','source':{'path':str(SOURCE.relative_to(ROOT)),'sha256':SOURCE_SHA},'supporting_source':{'path':str(avpath.relative_to(ROOT)),'sha256':avsha,'bytes':len(av),'header_hex':av[:0x58].hex(),'candidate_link_base':'0xffffffc001000000'},'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':sha(Path(__file__).read_bytes())},'supporting_harness':{'path':'research/trace_heif_encoder.py','sha256':sha((ROOT/'research/trace_heif_encoder.py').read_bytes())},'camera_access':False,'network_access':False,'syscall_forwarding':False,'codec_execution':False,'table_location':{'va':hex(table_va),'bytes':table_size,'sha256_before':sha(table_before),'section':table_sec.name,'section_flags':table_sec.header.sh_flags,'load_segment_flags':table_segment.header.p_flags,'binding':table_symbol.entry.st_info.bind,'visibility':table_symbol.entry.st_other.visibility,'relocations':table_relocs,'exact_pattern_GOT_read_references':got_refs},'dynamic_loader_import_inventory':{'symbols_checked':['mprotect','dlopen','dlsym','dlvsym','dlmopen'],'present':sorted(set(imports)&{'mprotect','dlopen','dlsym','dlvsym','dlmopen'}),'limit':'Imports of this ELF only; another module, kernel or debugger can act outside this boundary.'},'forward_conversion_cases':conversion_cases,'synthetic_GOT_dependency_test':{'logical':'0x534','synthetic_marker':'0x7a11','original_converter_result':hex(alternate),'original_table_restored':True,'is_discovered_runtime_override':False},'initializer_cases':initializers,'initializer_local_callees':sorted(m.init_local_calls),'initializer_table_validation':{'table_unchanged':True,'GOT_slot_unchanged':True,'observed_target_writes':m.observed_writes,'original_source_region_store_count':m.init_source_write_count,'service_stubs':m.service_stubs,'conversion_after_initialization':after_init},'locally_bound_full_request_cases':full_requests,'inverse_conversion_cases':reverse,'local_default_hook_cases':hook_cases,'activation_cases':activation,'native_receiver_cases':receivers,'native_completion_cases':completions,'regions':regions,'counts':{'original_instruction_regions':len(regions),'forward_cases':len(conversion_cases),'synthetic_GOT_case':1,'initializer_cases':len(initializers),'after_initializer_cases':len(after_init),'full_request_cases':len(full_requests),'inverse_cases':len(reverse),'hook_cases':len(hook_cases),'activation_cases':len(activation),'native_receiver_cases':len(receivers),'native_completion_cases':len(completions)},'stubs':['ELF initializer external MWF ObjLock constructor and __cxa_atexit do not create OS locks or register native destructors; reached local constructors execute original bodies.','Activation tests intercept new/sequence constructors/SetSeqInstance/StartSequence and prove factory selection only.','av-cam tests execute only PC-relative selected bodies at file-offset alias0; object pointers are synthetic, absolute linked-data pointers are not consumed.','av-cam logging, state-controller actions and completion send helper are explicit no-op/capture stubs; no codec or transport executes.'],'limits':['No live ELF loader scope, table mutation, command hook override or alternate symbol provider was observed.','The single exact ADRP/LDR reference scan is a bounded pattern inventory, not a complete indirect-store/cross-file dataflow proof.','Unchanged table under the recorded initializer service stubs does not prove no external startup service can mutate memory.','Native MC selectors are independently recovered; this does not prove that a particular AP request reaches this receiver.','The selected HEIF sequence factory branch is reachable with synthetic messages; real product call-site values and normal still-capture activation remain unobserved.','No patched firmware, camera-upload data or installable program is generated.']}
    target=ROOT/'research/evidence/heif-binding.json';target.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result['counts'],indent=2))


if __name__=='__main__':main()
