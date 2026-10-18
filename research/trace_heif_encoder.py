#!/usr/bin/env python3
"""Offline HEIF MC encoder request/result evidence, without codec/hardware execution."""
import hashlib
import itertools
import json
from pathlib import Path
import re
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X6, UC_ARM64_REG_X7, UC_ARM64_REG_X30, UC_ARM64_REG_SP, UC_ARM64_REG_PC

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked/lib/libInfraAvbbc.so'
CP = next((ROOT / 'research/extracted/ILCE-7CM2-2.01').rglob('cpapp-b.bin'))
SOURCE_SHA = '676197567dbbefa54e08f528968efc6ca2aaffe8bee3ee3eae8164382ed5a7e5'
CP_SHA = 'f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428'
REGS = [UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X6, UC_ARM64_REG_X7]


def sha(data): return hashlib.sha256(data).hexdigest()


class Machine:
    ARENA, STACK, END, STUB = 0x10000000, 0x20000000, 0x30000000, 0x40000000
    def __init__(self):
        self.u = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
        self.calls, self.sent, self.errors, self.handle_writes = [], [], [], []
        self.values = {}
        self.sequence_mode = False
        self.allocated, self.command_slots, self.transitions = {}, [], []
        self.next_alloc = self.ARENA + 0xc000
        self.symbols, self.imports = {}, {}
        self.segments = []
        with SOURCE.open('rb') as f:
            e = ELFFile(f)
            for s in e.iter_segments():
                if s.header.p_type != 'PT_LOAD': continue
                a = s.header.p_vaddr & ~4095
                b = (s.header.p_vaddr+s.header.p_memsz+4095)&~4095
                self.u.mem_map(a,b-a); self.u.mem_write(s.header.p_vaddr,s.data())
                self.segments.append((s.header.p_vaddr,s.header.p_filesz,s.header.p_offset))
            sy = e.get_section_by_name('.dynsym')
            self.symbols = {s.name: (s.entry.st_value,s.entry.st_size) for s in sy.iter_symbols() if isinstance(s.entry.st_shndx,int)}
            pl = e.get_section_by_name('.plt')
            self.imports = {pl.header.sh_addr+32+16*n:sy.get_symbol(r.entry.r_info_sym).name for n,r in enumerate(e.get_section_by_name('.rela.plt').iter_relocations())}
            rel = e.get_section_by_name('.rela.dyn')
            for r in rel.iter_relocations():
                if r.entry.r_info_type==1027: self.u.mem_write(r.entry.r_offset,struct.pack('<Q',r.entry.r_addend))
            self.debug_got = next(r.entry.r_offset for r in rel.iter_relocations() if r.entry.r_info_sym and sy.get_symbol(r.entry.r_info_sym).name=='avbizPrintLevel')
        for a,n in [(self.ARENA,0x10000),(self.STACK,0x10000),(self.END,4096),(self.STUB,4096)]:self.u.mem_map(a,n)
        self.u.mem_write(self.debug_got,struct.pack('<Q',self.ARENA+0xf000))
        self.u.hook_add(UC_HOOK_CODE,self.external,begin=pl.header.sh_addr,end=pl.header.sh_addr+pl.header.sh_size-1)
        self.u.hook_add(UC_HOOK_CODE,self.virtual,begin=self.STUB,end=self.STUB+0xfff)
        self.initial = self.u.context_save()

    def ret(self,v=0):
        self.u.reg_write(UC_ARM64_REG_X0,v);self.u.reg_write(UC_ARM64_REG_PC,self.u.reg_read(UC_ARM64_REG_X30))

    def virtual(self,u,addr,size,_):
        a = [u.reg_read(r) for r in REGS]
        if addr in (self.STUB,self.STUB+4):
            self.calls.append({'stub':'ParameterControl virtual getter','slot':hex(0xb0 if addr==self.STUB else 0xa8),'key':hex(a[1])})
            assert a[1] in self.values,hex(a[1]);u.mem_write(a[2],struct.pack('<Q',self.values[a[1]]));self.ret()
        elif addr==self.STUB+12 and self.sequence_mode:
            self.calls.append({'stub':'ParameterControl copy/set','key':hex(a[1]),'bytes':a[3],'input_hex':bytes(u.mem_read(a[2],a[3])).hex()});self.ret()
        elif addr in (self.STUB+16,self.STUB+20):
            self.handle_writes.append({'slot':hex(0x50 if addr==self.STUB+16 else 0x40),'key':hex(a[1]),'value':hex(a[2])});self.ret()
        elif addr==self.STUB+8:
            self.errors.append({'parameter_id':a[1],'converted_error_stub_value':a[2]});self.ret()
        else: raise RuntimeError('Unknown synthetic virtual address')

    def external(self,u,addr,size,_):
        name = self.imports.get(addr);a=[u.reg_read(r) for r in REGS]
        if self.sequence_mode and name=='_Znwm':
            ptr=self.next_alloc;self.next_alloc+=(a[0]+15)&~15
            assert self.next_alloc<self.ARENA+0xf000
            self.allocated[ptr]={'bytes':a[0]};self.ret(ptr)
        elif self.sequence_mode and 'C1E' in name and 'RES_PAYLOAD' not in name:
            match=re.match(r'_ZN(\d+)',name);start=match.end();cls=name[start:start+int(match.group(1))]
            self.allocated[a[0]].update({'class':cls,'arguments_after_this':[hex(v) for v in a[1:]]});self.ret()
        elif self.sequence_mode and 'RES_PAYLOADC1E' in name:
            u.mem_write(a[0],bytes(44));self.ret()
        elif self.sequence_mode and 'SetExecCmdE' in name:
            self.command_slots.append({'state':a[1],'object':hex(a[2]),**self.allocated[a[2]]});self.ret()
        elif self.sequence_mode and 'SetNextStatusE' in name:
            self.transitions.append({'next':a[1],'previous':a[2]});self.ret()
        elif 'ConvertCid' in name:
            self.calls.append({'stub':'ConvertCid','logical_component':a[1]});self.ret(0x3344)
        elif 'ConvertCmdMid' in name:
            self.calls.append({'stub':'ConvertCmdMid','logical_component':a[1],'logical_command':hex(a[2])});self.ret(0x5566)
        elif 'OsalSndMsg' in name:
            self.sent.append({'destination':hex(a[1]),'bytes':a[3],'payload_hex':bytes(u.mem_read(a[2],a[3])).hex()});self.ret()
        elif 'InfraAvbbcErrConverter7Convert' in name:
            self.calls.append({'stub':'error converter','component':a[0],'status':hex(a[1])});self.ret(0x7abc)
        elif name.startswith('_ZN13InfraAvbbcLog') or 'BizPrintf' in name:
            self.ret()
        elif name in self.symbols and self.symbols[name][0]:
            u.reg_write(UC_ARM64_REG_PC,self.symbols[name][0])
        else:raise RuntimeError('Unstubbed external '+str(name))

    def run(self,va,args):
        self.u.context_restore(self.initial)
        self.u.mem_write(self.STACK,b'\0'*0x10000)
        self.u.reg_write(UC_ARM64_REG_SP,self.STACK+0x8000);self.u.reg_write(UC_ARM64_REG_X30,self.END)
        for r,v in zip(REGS,args):self.u.reg_write(r,v)
        self.u.emu_start(va,self.END,count=20000)
        assert self.u.reg_read(UC_ARM64_REG_PC)==self.END,hex(self.u.reg_read(UC_ARM64_REG_PC))
        return self.u.reg_read(UC_ARM64_REG_X0)

    def setup(self,mode=3,direct=0):
        a=self.ARENA
        self.u.mem_write(a,b'\0'*0xf000)
        self.obj,self.payload,self.unit,self.out,self.param,self.vtable,self.yc,self.reply = [a+x for x in [0x1000,0x2000,0x3000,0x4000,0x5000,0x6000,0x7000,0x8000]]
        self.u.mem_write(self.obj+0x10,struct.pack('<2Q',a+0x9000,self.param));self.u.mem_write(self.obj+0x20,struct.pack('<I',0x1099));self.u.mem_write(self.obj+0x24,bytes([mode]));self.u.mem_write(self.obj+0x28,struct.pack('<I',0x10aa));self.u.mem_write(self.obj+0x2c,bytes([direct]))
        self.u.mem_write(a+0x9008,struct.pack('<I',0x12345678))
        self.u.mem_write(self.param,struct.pack('<Q',self.vtable))
        for off,fn in [(0xb0,self.STUB),(0xa8,self.STUB+4),(0x48,self.STUB+8),(0x50,self.STUB+16),(0x40,self.STUB+20)]:self.u.mem_write(self.vtable+off,struct.pack('<Q',fn))
        self.values={0x1099:self.yc,0x1060:0x1122334455667788,0x104a:self.payload,0x105f:self.payload}
        self.u.mem_write(self.yc+4,struct.pack('<I',0x10203040));self.u.mem_write(self.yc+0x10,struct.pack('<Q',0x2233445566778899))
        self.u.mem_write(self.unit+0x18,struct.pack('<QI',0x33445566778899aa,0x13572468))
        self.u.mem_write(self.payload+0x34,struct.pack('<I',0x20304050));self.u.mem_write(self.payload+0x40,struct.pack('<Q',0x445566778899aabb))
        for off,v in [(0x54,7008),(0x56,4672),(0x5c,3),(0x5e,2),(0x68,6000),(0x6a,4000),(0x6c,1920),(0x6e,1080)]:self.u.mem_write(self.payload+off,struct.pack('<H',v))
        self.u.mem_write(self.payload+0x60,bytes([1,4,9,1]));self.u.mem_write(self.payload+0x70,bytes([0,0,0,0,0]))
        self.calls.clear();self.sent.clear();self.errors.clear();self.handle_writes.clear()
        self.allocated.clear();self.command_slots.clear();self.transitions.clear();self.next_alloc=self.ARENA+0xc000


def main():
    data=SOURCE.read_bytes();cp=CP.read_bytes();assert sha(cp)==CP_SHA and sha(data)==SOURCE_SHA
    m=Machine();helpers=[]
    names=[('bit_depth',0x1200ec,1),('capture_gamma',0x1200fc,1),('colorimetry',0x120110,1),('y_offset',0x120120,2),('colorspace',0x120134,2)]
    for name,va,arity in names:
        outputs=[]
        for v in range(256):
            for flag in ([0] if arity==1 else [0,1,255]):
                actual=m.run(va,[0,v,flag])
                expected={'bit_depth':int(v==1),'capture_gamma':0x57 if v==4 else 0x58,'colorimetry':int(v==4),'y_offset':int(flag==0 and v==1),'colorspace':int(flag==0 and v in (1,2))}[name]
                assert actual==expected,(name,v,flag,actual,expected)
                outputs.append({'input':v,'flag':flag,'output':actual})
        helpers.append({'name':name,'va':hex(va),'cases_passed':len(outputs),'outputs':outputs})
    builders=[]
    for mode,direct,quality,gamma,flag in itertools.product([3,4,5],[0,1],[0,1,2,3,4,255],[0,1,2,4],[0,1]):
        m.setup(mode,direct);m.u.mem_write(m.payload+0x70,bytes([quality,flag,flag,0,flag]));m.u.mem_write(m.payload+0x61,bytes([gamma]))
        m.run(0x120150,[m.obj,m.payload,m.unit,m.out]);raw=bytes(m.u.mem_read(m.out,96))
        q=struct.unpack_from('<I',raw,0x20)[0];assert q=={0:0x100,1:0x101,2:0x102,3:0x103}.get(quality,0x102)
        assert raw[:8]==struct.pack('<4H',0x3344,0x5566,0x60,1)
        assert struct.unpack_from('<Q',raw,8)[0]==0x1122334455667788
        assert struct.unpack_from('<Q',raw,0x10)[0]==0x12345678 and struct.unpack_from('<I',raw,0x18)[0]==0x12345678
        assert struct.unpack_from('<QI',raw,0x50)==(0x33445566778899aa,0x13572468)
        if mode==3 and not direct:expected_unit,expected_addr=0x20304050,0x445566778899aabb
        else:expected_unit,expected_addr=0x10203040,0x2233445566778899
        assert struct.unpack_from('<I',raw,0x28)[0]==expected_unit and struct.unpack_from('<Q',raw,0x30)[0]==expected_addr
        assert raw[0x24]==(0 if mode==4 or flag else 2)
        assert raw[0x25]==int(flag==0) and raw[0x26]==int(flag==0 and gamma in (1,2))
        assert raw[0x38:0x3c]==bytes([9,1,0x57 if gamma==4 else 0x58,int(gamma==4)])
        geom=list(struct.unpack_from('<10H',raw,0x3c));expected_geom={3:[7008,4672,0,0,6000,4000,0,0,6000,4000],4:[640,480,0,0,320,212,0,0,320,212],5:[1920,1088,0,0,1920,1080,0,0,1920,1080]}[mode]
        assert geom==expected_geom and struct.unpack_from('<I',raw,0x1c)[0]=={3:0,4:2,5:1}[mode]
        builders.append({'mode':mode,'resolved_input_flag':direct,'quality_input':quality,'gamma_input':gamma,'override_flags':flag,'request_sha256':sha(raw),'quality_output':q,'geometry_halfwords_3c_to_4e':geom,'input_unit_u32':hex(expected_unit),'input_address_u64':hex(expected_addr),'stub_calls':m.calls.copy()})
    aspects=[]
    for w,h,expected in [(1,1,[320,320]),(4,3,[320,240]),(3,2,[320,212]),(16,9,[320,180]),(1,0,[320,320]),(1,2,[320,320]),(2,1,[320,320])]:
        m.setup(4);m.u.mem_write(m.payload+0x5c,struct.pack('<2H',w,h));m.run(0x120150,[m.obj,m.payload,m.unit,m.out]);actual=list(struct.unpack('<2H',m.u.mem_read(m.out+0x44,4)));assert actual==expected;aspects.append({'source_halfwords_5c_5e':[w,h],'output_halfwords_44_46':actual})
    submissions=[]
    for mode in [3,4,5]:
        m.setup(mode);ret=m.run(0x1205b4,[m.obj,m.unit]);assert ret==0 and len(m.sent)==1 and m.sent[0]['destination']=='0x8057060a' and m.sent[0]['bytes']==96
        submissions.append({'mode':mode,'return_code':ret,'submission':m.sent.copy(),'stubs':m.calls.copy()})
    results=[]
    for mode,selector,status in itertools.product([3,4,5,2],[0x8565,0x1234],[0,1,0x80000000]):
        m.setup(mode);m.u.mem_write(m.payload+0x5c,b'\xa5'*32);m.u.mem_write(m.reply+0x10,struct.pack('<I',status));m.u.mem_write(m.reply+0x18,struct.pack('<Q',0x1122334455667788));ret=m.run(0x11fdcc,[m.obj,m.reply,selector]);expected=1 if selector!=0x8565 else (0x201 if status else 0);assert ret==expected
        changes=[]
        for mo,off in [(3,0x5c),(4,0x68),(5,0x74)]:
            val=struct.unpack('<I',m.u.mem_read(m.payload+off,4))[0];assert val==(0xa5a5a5a5 if selector!=0x8565 or mode!=mo else (0 if status else 0x55667788));changes.append({'offset':hex(off),'value':hex(val)})
        assert bool(m.errors)==bool(selector==0x8565 and status)
        results.append({'mode':mode,'selector':hex(selector),'status':hex(status),'return_code':ret,'result_words':changes,'error_notifications':m.errors.copy(),'stub_calls':m.calls.copy()})
    open_close=[]
    for command,expected_len in [(0x532,16),(0x533,24)]:
        m.setup();m.u.mem_write(m.obj+0x20,struct.pack('<H',command));m.u.mem_write(m.obj+0x24,struct.pack('<I',0x89abcdef))
        ret=m.run(0x1206a4,[m.obj,m.unit]);assert ret==0 and len(m.sent)==1 and m.sent[0]['bytes']==expected_len
        raw=bytes.fromhex(m.sent[0]['payload_hex']);assert raw[:8]==struct.pack('<4H',0x3344,0x5566,expected_len,1)
        if command==0x532:assert struct.unpack_from('<2I',raw,8)==(0x89abcdef,0x12345678)
        else:assert struct.unpack_from('<QI',raw,8)==(0x1122334455667788,0x12345678)
        open_close.append({'logical_command':hex(command),'return_code':ret,'submission':m.sent.copy(),'stubs':m.calls.copy()})
    open_close_responses=[]
    for selector,status in itertools.product([0x8563,0x8564,0x1234],[0,1,0x80000000]):
        m.setup();m.u.mem_write(m.reply+8,struct.pack('<Q',0x1122334455667788));m.u.mem_write(m.reply+0x10,struct.pack('<I',status))
        ret=m.run(0x120864,[m.obj,m.reply,selector]);assert ret==(1 if selector==0x1234 else 0)
        expected=[] if selector==0x1234 or status else [{'slot':hex(0x50 if selector==0x8563 else 0x40),'key':'0x1060','value':hex(0x1122334455667788 if selector==0x8563 else 0)}]
        assert m.handle_writes==expected
        open_close_responses.append({'selector':hex(selector),'status':hex(status),'return_code':ret,'parameter_handle_operations':m.handle_writes.copy()})
    sequences=[]
    for main_on,thumb_on,screen_on,unit_bit15 in itertools.product([0,1],repeat=4):
        m.setup();m.sequence_mode=True
        m.u.mem_write(m.obj+8,struct.pack('<Q',m.ARENA+0x9000));m.u.mem_write(m.obj+0x1c0,struct.pack('<Q',m.param));m.u.mem_write(m.obj+0x1b8,struct.pack('<I',0x55667788))
        m.u.mem_write(m.vtable+0x60,struct.pack('<Q',m.STUB+12))
        m.u.mem_write(m.unit+0x30,struct.pack('<Q',m.payload));m.u.mem_write(m.payload+0x34,struct.pack('<I',unit_bit15<<15))
        flags=m.ARENA+0xa000;m.values[0x105e]=flags
        for off,on in [(0x54,main_on),(0x60,thumb_on),(0x6c,screen_on)]:m.u.mem_write(flags+off,struct.pack('<I',on))
        m.run(0x1241fc,[m.obj,m.unit])
        assert sorted(x['state'] for x in m.command_slots)==list(range(1,28)), [(x['state'],x['class']) for x in m.command_slots]
        encode=[x for x in m.command_slots if x['class']=='ExecCmd_MC_EncodeHeif']
        assert [x['state'] for x in encode]==[6,14,22]
        assert [int(x['arguments_after_this'][3],16) for x in encode]==[0x1058,0x105a,0x105c]
        assert [int(x['arguments_after_this'][4],16) for x in encode]==[3,4,5]
        assert [int(x['arguments_after_this'][6],16) for x in encode]==[unit_bit15,1,1]
        order=[1]+((list(range(2,10)) if unit_bit15 else [5,6,7]) if main_on else [])+(list(range(10,18)) if thumb_on else [])+(list(range(18,26)) if screen_on else [])+[26,27,0xffff]
        assert [x['next'] for x in m.transitions]==order
        assert [x['previous'] for x in m.transitions]==[0]+order[:-1]
        sequences.append({'enabled_main_thumb_screen':[main_on,thumb_on,screen_on],'source_unit_word_bit15':unit_bit15,'constructed_commands':m.command_slots.copy(),'transition_order':m.transitions.copy(),'initial_submission':m.sent.copy(),'parameter_copy_stubs':m.calls.copy()})
        m.sequence_mode=False
    md=Cs(CS_ARCH_ARM64,CS_MODE_LITTLE_ENDIAN)
    regions=[]
    for lo,hi,role in [(0x1200ec,0x120150,'Five original enum conversion helpers'),(0x120150,0x1205b4,'MC HEIF frame parameter builder'),(0x1205b4,0x120668,'MC HEIF asynchronous submission'),(0x11fdcc,0x120080,'MC HEIF response handler'),(0x1206a4,0x120968,'MC still encoder open/close request and response'),(0x1241fc,0x124d60,'HEIF resize/encode sequence parameter construction')]:
        v,size,off=next(x for x in m.segments if x[0]<=lo<hi<=x[0]+x[1]);a=lo-v+off;b=hi-v+off;ins=list(md.disasm(data[a:b],lo));assert sum(i.size for i in ins)==hi-lo
        regions.append({'start_va':hex(lo),'end_va_exclusive':hex(hi),'file_offset':a,'role':role,'sha256':sha(data[a:b]),'instructions':[{'va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str} for i in ins]})
    cp_strings=[]
    for match in re.finditer(rb'[\x20-\x7e]{4,}\x00',cp):
        text=match.group()[:-1].decode()
        if re.search('heif|hevc|h265',text,re.I):cp_strings.append({'file_offset':hex(match.start()),'link_address':hex(match.start()+0x108000),'text':text})
    defined=[{'name':name,'va':hex(v),'size':n} for name,(v,n) in m.symbols.items() if any(t in name for t in ['ExecCmd_MC_EncodeHeif','ExecCmd_MC_StillEncOpenClose','InfraStillEncoderSeqResizeEncForHeif'])]
    bindings=[];typed_tables=[]
    with SOURCE.open('rb') as f:
        elf=ELFFile(f);dyn=elf.get_section_by_name('.dynsym');relocations=list(elf.get_section_by_name('.rela.dyn').iter_relocations())
        selected=['_ZN20InfraAvbbcMsgManager10ConvertCidEt','_ZN20InfraAvbbcMsgManager13ConvertCmdMidEtt','_ZN20InfraAvbbcMsgManager10OsalSndMsgEjPKhj','_ZN21ExecCmd_MC_EncodeHeif22MakeMcEncodeFrameParamEPKN17StillEncoderParam23RESIZEENC_PARAM_PAYLOADEPK18dmm_comp_open_unitPN2MC29MCODEC_STILL_ENC_ENCODE_FRAMEE']
        for symbol in dyn.iter_symbols():
            if symbol.name in selected:bindings.append({'name':symbol.name,'value':hex(symbol.entry.st_value),'size':symbol.entry.st_size,'binding':symbol.entry.st_info.bind,'visibility':symbol.entry.st_other.visibility})
        for name in ['_ZTV21ExecCmd_MC_EncodeHeif','_ZTI21ExecCmd_MC_EncodeHeif','_ZTS21ExecCmd_MC_EncodeHeif']:
            va,length=m.symbols[name];v,n,off=next(x for x in m.segments if x[0]<=va<va+length<=x[0]+x[1]);file_offset=va-v+off;raw=data[file_offset:file_offset+length]
            rels=[]
            for relocation in relocations:
                if va<=relocation.entry.r_offset<va+length:
                    symbol=dyn.get_symbol(relocation.entry.r_info_sym)
                    rels.append({'slot_offset':hex(relocation.entry.r_offset-va),'type':relocation.entry.r_info_type,'symbol':symbol.name,'symbol_value':hex(symbol.entry.st_value),'addend':relocation.entry.r_addend})
            typed_tables.append({'name':name,'va':hex(va),'file_offset':file_offset,'bytes':raw.hex(),'sha256':sha(raw),'relocations':rels})
    out={'schema':1,'method':'Original AArch64 instructions plus bounded host Unicorn tests with explicit external service stubs; not REA Evidence','source':{'path':str(SOURCE.relative_to(ROOT)),'sha256':sha(data),'bytes':len(data)},'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':sha(Path(__file__).read_bytes())},'camera_access':False,'network_access':False,'syscall_forwarding':False,'codec_execution':False,'defined_symbols':defined,'selected_dynamic_bindings':bindings,'class_type_and_vtable_data':typed_tables,'static_regions':regions,'helper_cases':helpers,'builder_cases':builders,'aspect_cases':aspects,'submission_cases':submissions,'response_cases':results,'sequence_cases':sequences,'open_close_request_cases':open_close,'open_close_response_cases':open_close_responses,'cp_bounded_string_inventory':{'source':str(CP.relative_to(ROOT)),'sha256':CP_SHA,'case_insensitive_terms':['heif','hevc','h265'],'strings':cp_strings},'counts':{'original_instruction_regions':len(regions),'enum_helper_cases':sum(h['cases_passed'] for h in helpers),'parameter_builder_cases':len(builders),'aspect_cases':len(aspects),'submission_cases':len(submissions),'response_cases':len(results),'sequence_cases':len(sequences),'open_close_request_cases':len(open_close),'open_close_response_cases':len(open_close_responses)},'stubs':['ParameterControl virtual+0xb0/+0xa8 supplies synthetic payload, Yc descriptor and handle by key.','ParameterControl virtual+0x48 records error publication; +0x50/+0x40 record handle set/clear operations.','ConvertCid/ConvertCmdMid return synthetic sentinels0x3344/0x5566; logical inputs are retained.','OsalSndMsg records destination/count/payload and returns without sending.','Error converter returns synthetic0x7abc; no claim about actual vendor error mapping.','Logging calls return without logging.', 'Sequence-only tests stub allocation and command constructors, record SetExecCmd/SetNextStatus, and copy/set parameter calls; no constructed command executes.','Other locally defined PLT calls deliberately bind to this ELF original function bodies; unknown external calls abort. Runtime symbol interposition is not simulated.'],'limits':['No codec, MC receiver, firmware process, real resource manager, physical memory or camera executes.','Address-like and unit fields are synthetic transport markers, not a proven physical/virtual resource contract.','The MC request/result boundary has not been joined to saved HEIF image areas or the selected MLUT buffer.','Mode3/4/5 result diagnostics reuse MainJpg/ThmJpg/screenNail labels inside the HEIF class; these labels do not prove the actual encoded bitstream codec.', 'MC logical-to-wire command binding remains unresolved; stub header values must not be used as actual command IDs.','Numeric enum mappings are proven for byte inputs; meanings such as bitdepth/color standard are not inferred solely from helper names.','The finite CP string inventory is a bounded negative result, not proof that CP lacks HEIF functionality.']}
    target=ROOT/'research/evidence/heif-encoder.json';target.write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out['counts'],indent=2))


if __name__=='__main__':main()
