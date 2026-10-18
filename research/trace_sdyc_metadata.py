"""Original AP SDYC metadata construction in a bounded, linked host emulator."""
import hashlib
import json
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X6, UC_ARM64_REG_X7, UC_ARM64_REG_X30, UC_ARM64_REG_PC, UC_ARM64_REG_SP

ROOT=Path(__file__).resolve().parents[1]
LIB=ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked/lib'
REGS=[UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X3,UC_ARM64_REG_X4,UC_ARM64_REG_X5,UC_ARM64_REG_X6,UC_ARM64_REG_X7]


class MetadataMachine:
    ARENA,STACK,END=0x10000000,0x20000000,0x30000000
    def __init__(self):
        self.u=Uc(UC_ARCH_ARM64,UC_MODE_ARM);self.symbols={};self.imports={};self.modules=[];self.calls=[]
        for filename,bias in [('libInfraAvbbc.so',0),('dataflowInfraFramework.so',0x1000000)]:
            p=LIB/filename;blob=p.read_bytes()
            assert hashlib.sha256(blob).hexdigest()=={'libInfraAvbbc.so':'676197567dbbefa54e08f528968efc6ca2aaffe8bee3ee3eae8164382ed5a7e5','dataflowInfraFramework.so':'620e04a9cd51e65bd190ed1d5459b7235ff5a4109bc6498df342972621991d47'}[filename]
            with p.open('rb')as f:
                e=ELFFile(f);segments=[]
                for s in e.iter_segments():
                    if s.header.p_type!='PT_LOAD':continue
                    a=(s.header.p_vaddr+bias)&~4095;b=(s.header.p_vaddr+bias+s.header.p_memsz+4095)&~4095
                    self.u.mem_map(a,b-a);self.u.mem_write(s.header.p_vaddr+bias,s.data());segments.append((s.header.p_vaddr,s.header.p_filesz,s.header.p_offset))
                sy=e.get_section_by_name('.dynsym')
                for s in sy.iter_symbols():
                    if isinstance(s.entry.st_shndx,int):self.symbols[s.name]=bias+s.entry.st_value
                pl=e.get_section_by_name('.plt')
                for n,r in enumerate(e.get_section_by_name('.rela.plt').iter_relocations()):self.imports[pl.header.sh_addr+bias+32+16*n]=sy.get_symbol(r.entry.r_info_sym).name
                self.modules.append({'path':str(p.relative_to(ROOT)),'sha256':hashlib.sha256(blob).hexdigest(),'bytes':len(blob),'bias':bias,'segments':segments,'relocations':[(r.entry.r_offset,r.entry.r_info_type,r.entry.r_addend,sy.get_symbol(r.entry.r_info_sym).name if r.entry.r_info_sym else None)for r in e.get_section_by_name('.rela.dyn').iter_relocations()]})
                self.u.hook_add(UC_HOOK_CODE,self.external,begin=pl.header.sh_addr+bias,end=pl.header.sh_addr+bias+pl.header.sh_size-1)
        for a,n in [(self.ARENA,0x20000),(self.STACK,0x10000),(self.END,4096)]:self.u.mem_map(a,n)
        for m in self.modules:
            for off,kind,add,name in m['relocations']:
                if kind==1027:value=add+m['bias']
                elif kind in (257,1025) and name in self.symbols:value=self.symbols[name]+add
                elif name=='avbizPrintLevel':value=self.ARENA+0x1f000
                else:continue
                self.u.mem_write(off+m['bias'],struct.pack('<Q',value))
        self.initial=self.u.context_save()

    def ret(self,v=0):
        self.u.reg_write(UC_ARM64_REG_X0,v);self.u.reg_write(UC_ARM64_REG_PC,self.u.reg_read(UC_ARM64_REG_X30))

    def external(self,u,address,size,_):
        name=self.imports[address];a=[u.reg_read(r)for r in REGS]
        if name=='memcpy':u.mem_write(a[0],bytes(u.mem_read(a[1],a[2])));self.ret(a[0])
        elif name=='memset':u.mem_write(a[0],bytes([a[1]&255])*a[2]);self.ret(a[0])
        elif name=='strncmp':
            left=bytes(u.mem_read(a[0],a[2])).split(b'\0')[0];right=bytes(u.mem_read(a[1],a[2])).split(b'\0')[0]
            self.ret(((left>right)-(left<right))&0xffffffff)
        elif name in ('_ZN12AvbbcPhycMemC2EmjNS_10CACHE_MODEEj','_ZN12AvbbcPhycMemC1EmjNS_10CACHE_MODEEj'):
            self.calls.append({'stub':'AvbbcPhycMem constructor','requested_physical_address':hex(a[1]),'bytes':a[2],'cache_mode':a[3],'tag':hex(a[4])})
            u.mem_write(a[0]+0x10,struct.pack('<Q',self.ARENA+0x10000));u.mem_write(a[0]+0x20,struct.pack('<Q',a[1]));self.ret()
        elif name in ('_ZN12AvbbcPhycMemD2Ev','_ZN12AvbbcPhycMemD1Ev'):self.calls.append({'stub':'AvbbcPhycMem destructor'});self.ret()
        elif 'PrintYcStdData' in name:self.calls.append({'stub':'diagnostic metadata printer'});self.ret()
        elif name=='_ZN9DefCommon9getAspectEtt':self.calls.append({'stub':'getAspect','return':1});self.ret(1)
        elif name in self.symbols:u.reg_write(UC_ARM64_REG_PC,self.symbols[name])
        else:raise RuntimeError('Unresolved external '+name)

    def run(self,va,args):
        self.u.context_restore(self.initial);self.u.mem_write(self.STACK,b'\0'*0x10000)
        self.u.reg_write(UC_ARM64_REG_SP,self.STACK+0x8000);self.u.reg_write(UC_ARM64_REG_X30,self.END)
        for r,v in zip(REGS,args):self.u.reg_write(r,v)
        self.u.emu_start(va,self.END,count=50000)
        assert self.u.reg_read(UC_ARM64_REG_PC)==self.END
        return self.u.reg_read(UC_ARM64_REG_X0)


def main():
    m=MetadataMachine();cases=[]
    for index,(w,h,offset,depth)in enumerate([(7008,4672,0x80,0),(7008,4672,0x1200,1),(1920,1080,0x300,0),(640,480,0,1)]):
        m.u.mem_write(m.ARENA,b'\0'*0x20000);meta=m.ARENA+0x10000;m.u.mem_write(meta-16,b'\xa5'*0x420)
        payload=m.ARENA+0x2000;unit=0x20000100+index;handle=0x12345000+index;phys=0x1122334400000000+index*0x10000;outeroffset=0x400+index*0x100
        m.u.mem_write(payload+0x18,struct.pack('<QQ',phys,outeroffset));m.u.mem_write(payload+0x30,struct.pack('<II',unit,handle));m.u.mem_write(payload+0x48,struct.pack('<Q',offset))
        for off,value in [(0x54,w),(0x56,h),(0x58,w),(0x5a,h),(0x5c,w),(0x5e,h)]:m.u.mem_write(payload+off,struct.pack('<H',value))
        m.u.mem_write(payload+0x60,bytes([1,4,9,depth]));m.calls.clear();m.run(0x1218a8,[m.ARENA+0x1000,payload])
        raw=bytes(m.u.mem_read(meta,0x400));assert raw[:4]==b'SSYC'
        assert bytes(m.u.mem_read(meta-16,16))==b'\xa5'*16 and bytes(m.u.mem_read(meta+0x400,16))==b'\xa5'*16
        assert next(c for c in m.calls if c['stub']=='AvbbcPhycMem constructor')['requested_physical_address']==hex(phys+outeroffset)
        assert struct.unpack_from('<III',raw,4)==(0,1,3)
        entries=[];at=struct.unpack_from('<I',raw,0x10)[0]
        for category,entry_at,data_at,extent,next_at in [(0,0x20,0x40,0x60,0xa0),(1,0xa0,0xc0,0x20,0xe0),(2,0xe0,0x100,0x40,0)]:
            assert at==entry_at and raw[at:at+4]==b'ETRY'
            fields=list(struct.unpack_from('<5I',raw,at+4));assert fields==[category,0,data_at,extent,next_at]
            entries.append({'entry_offset':hex(at),'category':category,'version':fields[1],'data_offset':hex(data_at),'allocated_data_extent':extent,'next_offset':hex(next_at)})
            at=next_at
        nested=[struct.unpack_from('<I',raw,0x40+off)[0]for off in [0,4,0x18]]
        assert nested==[unit,handle,offset]
        dims=list(struct.unpack_from('<HH',raw,0x40+0x2e));assert dims==[w,h]
        assert struct.unpack_from('<Q',raw,0x48)[0]==0 # Separate physical-address field is zero on this builder path.
        cases.append({'index':index,'supplied_pixel_descriptor':{'unit_type':hex(unit),'handle':hex(handle),'offset':hex(offset)},'supplied_dimensions':[w,h],'supplied_depth_flag':depth,'outer_metadata_physical_base':hex(phys),'outer_metadata_offset':hex(outeroffset),'metadata_1024_hex':raw.hex(),'independent_serialized_entries':entries,'independent_nested_pixel_triple':[hex(x)for x in nested],'independent_nested_dimensions':dims,'external_calls':m.calls.copy(),'guards_unchanged':True})
    md=Cs(CS_ARCH_ARM64,CS_MODE_LITTLE_ENDIAN);regions=[]
    for mod,ranges in [(m.modules[0],[(0x1218a8,0x121ba0),(0x13ff44,0x13ff54),(0x1401a8,0x140330)]),(m.modules[1],[(0x348d8,0x34c14),(0x3500c,0x35144),(0x35194,0x35364),(0x35840,0x3588c),(0x363b8,0x36430),(0x36444,0x364a8),(0x37bac,0x37c14),(0x3627c,0x362a4)])]:
        blob=(ROOT/mod['path']).read_bytes()
        for lo,hi in ranges:
            v,_,o=next(s for s in mod['segments']if s[0]<=lo<hi<=s[0]+s[1]);start=lo-v+o;ins=list(md.disasm(blob[start:start+hi-lo],lo));assert sum(i.size for i in ins)==hi-lo
            regions.append({'source_path':mod['path'],'file_offset':start,'start_va':hex(lo),'end_va_exclusive':hex(hi),'instructions':[{'va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in ins]})
    out={'sources':[{k:v for k,v in x.items()if k in ('path','sha256','bytes')}for x in m.modules],'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},'camera_accessed':False,'network_access':False,'target_codec_executed':False,'method':'Original AP MakeInputYcStdData plus original linked dataflow library code in bounded Unicorn, synthetic physical mapper and documented diagnostic/aspect/libc stubs.','regions':regions,'cases':cases,'counts':{'host_cases':len(cases),'regions':len(regions),'instructions':sum(len(r['instructions'])for r in regions)},'limits':['Physical mapper returns synthetic host memory; no physical memory is accessed.','getAspect returns1 and diagnostic printer is intercepted.','Only local defined symbol bindings are modeled; no target dynamic linker runs.','Generated SSYC metadata is a synthetic descriptor fixture, not pixels or a captured image.']}
    (ROOT/'research/evidence/sdyc-metadata.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out['counts'],indent=2))


if __name__=='__main__':main()
