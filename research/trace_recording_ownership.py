"""Original AP still-record buffer share/free routines with DMM intercepted."""
import hashlib
import json
from pathlib import Path
import struct
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X3,UC_ARM64_REG_X4,UC_ARM64_REG_X5,UC_ARM64_REG_X6,UC_ARM64_REG_X7,UC_ARM64_REG_X30,UC_ARM64_REG_PC,UC_ARM64_REG_SP
from unicorn.arm64_const import UC_ARM64_REG_X19,UC_ARM64_REG_X27,UC_ARM64_REG_X28,UC_ARM64_REG_X29

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked/lib/libmpr.so'
REGS=[UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X3,UC_ARM64_REG_X4,UC_ARM64_REG_X5,UC_ARM64_REG_X6,UC_ARM64_REG_X7]


def main():
    blob=SOURCE.read_bytes();digest=hashlib.sha256(blob).hexdigest();assert digest=='4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9'
    u=Uc(UC_ARCH_ARM64,UC_MODE_ARM);segments=[]
    with SOURCE.open('rb')as f:
        e=ELFFile(f)
        for s in e.iter_segments():
            if s.header.p_type!='PT_LOAD':continue
            a=s.header.p_vaddr&~4095;b=(s.header.p_vaddr+s.header.p_memsz+4095)&~4095;u.mem_map(a,b-a);u.mem_write(s.header.p_vaddr,s.data());segments.append((s.header.p_vaddr,s.header.p_filesz,s.header.p_offset))
        sy=e.get_section_by_name('.dynsym');symbols={s.name:s.entry.st_value for s in sy.iter_symbols()if isinstance(s.entry.st_shndx,int)}
        pl=e.get_section_by_name('.plt');imports={pl.header.sh_addr+32+16*n:sy.get_symbol(r.entry.r_info_sym).name for n,r in enumerate(e.get_section_by_name('.rela.plt').iter_relocations())}
    arena,stack,end=0x10000000,0x20000000,0x30000000
    for a,n in [(arena,0x20000),(stack,0x10000),(end,4096)]:u.mem_map(a,n)
    events=[]
    def ret(value=0):u.reg_write(UC_ARM64_REG_X0,value);u.reg_write(UC_ARM64_REG_PC,u.reg_read(UC_ARM64_REG_X30))
    def hook(cpu,address,size,_):
        a=[cpu.reg_read(r)for r in REGS]
        if address==0x2b2270:
            events.append({'boundary':'DMM command service intercepted','handle':hex(a[0]),'command':hex(a[1]),'mode':a[2]});ret(1)
        elif address==0x2b1d78:
            events.append({'boundary':'share_and_open service intercepted','unit_type':hex(a[0]),'handle':hex(a[1]),'command':hex(a[2]),'mode':a[3],'open_flag':a[5]});ret(1)
        elif address in imports:
            name=imports[address]
            if name=='memset':cpu.mem_write(a[0],bytes([a[1]&255])*a[2]);ret(a[0])
            elif name in symbols:cpu.reg_write(UC_ARM64_REG_PC,symbols[name])
            else:raise RuntimeError('Unstubbed external '+name)
    u.hook_add(UC_HOOK_CODE,hook);initial=u.context_save()
    def run(va,args,extra=None,stop=None):
        u.context_restore(initial);u.mem_write(stack,b'\0'*0x10000);events.clear();u.reg_write(UC_ARM64_REG_SP,stack+0x8000);u.reg_write(UC_ARM64_REG_X30,end)
        for r,v in zip(REGS,args):u.reg_write(r,v)
        for r,v in (extra or {}).items():u.reg_write(r,v)
        u.emu_start(va,stop or end,count=10000);assert u.reg_read(UC_ARM64_REG_PC)==(stop or end)
        return u.reg_read(UC_ARM64_REG_X0)
    free_sequences=[]
    for handles in [[0],[7],[7,7],[7,8,7],[1,0,2,1,3],[0xffffffff,0xffffffff],[9,9,0,10]]:
        u.mem_write(arena,b'\0'*0x1000);count=0;all_events=[];expected=[]
        for h in handles:
            u.mem_write(arena+0x800,struct.pack('<4I',h,0,99,0x4d4));count=run(0xe3bb4,[arena,count,arena+0x800]);all_events.extend(events)
            if h and h not in expected:expected.append(h)
        assert count==len(expected)
        assert [int(x['handle'],16)for x in all_events]==expected
        assert all(x['command']=='0x23'and x['mode']==2 for x in all_events)
        free_sequences.append({'area_handles':list(map(hex,handles)),'distinct_nonzero_count':count,'intercepted_free_requests':all_events})
    hif_cases=[]
    for mode in [0,2]:
        for handles in [[101,102,103,104,105,106],[111,111,0,112,112,113]]:
            u.mem_write(arena,b'\0'*0x10000)
            incoming,record,jpgs,raws,hifs,common,info=[arena+x for x in [0x1000,0x2000,0x3000,0x3100,0x3200,0x4000,0x6000]]
            u.mem_write(record+0xc,struct.pack('<I',mode));u.mem_write(hifs,struct.pack('<2Q',info,info+0x200))
            offsets=[0,0xa0,0xb0,0x90,0xc0,0xd0]
            for i,h in enumerate(handles):u.mem_write(info+offsets[i],struct.pack('<4I',h,0,0x1000,0x4d4))
            for off in offsets:u.mem_write(info+0x200+off,bytes(u.mem_read(info+off,16)))
            run(0xe3c14,[incoming,record,jpgs,raws,hifs,common,0]);expected=list(dict.fromkeys(h for h in handles if h))
            assert [int(x['handle'],16)for x in events]==expected
            hif_cases.append({'record_mode':mode,'two_HIF_descriptors_same_handles':True,'area_handles':list(map(hex,handles)),'requests':events.copy()})
    shares=[]
    for mode in [0,2]:
        for flag in [0,1]:
            u.mem_write(arena,b'\0'*0x10000)
            incoming,record,jpgs,raws,hifs,common,info,munmaps=[arena+x for x in [0x1000,0x2000,0x3000,0x3100,0x3200,0x4000,0x6000,0x8000]]
            u.mem_write(record+0xc,struct.pack('<I',mode));u.mem_write(record+0x1f4,struct.pack('<I',4));u.mem_write(record+0x23c,b'\x01');u.mem_write(record+0x794,struct.pack('<I',0xffffffff))
            u.mem_write(hifs,struct.pack('<Q',info))
            for i,off in enumerate([0,0x80,0x90,0xa0,0xb0,0xc0,0xd0]):u.mem_write(info+off,struct.pack('<4I',0x1200+i,0,0x1000,0x4d4))
            run(0xe274c,[incoming,record,jpgs,raws,hifs,common,flag,munmaps])
            assert events and all(x['command']=='0x21'and x['mode']==1 for x in events)
            shares.append({'record_mode':mode,'flag':flag,'requests':events.copy()})
    ordering=[]
    for mode in [0,2,3]:
        for flag in [0,1]:
            u.mem_write(arena,b'\0'*0x10000);incoming,record,common,info,munmaps,frame=[arena+x for x in [0x1000,0x2000,0x4000,0x6000,0x8000,0xa000]]
            u.mem_write(record+0xc,struct.pack('<I',mode));u.mem_write(record+0x1f4,struct.pack('<I',4));u.mem_write(record+0x23c,b'\x01');u.mem_write(record+0x794,struct.pack('<I',0xffffffff))
            u.mem_write(frame+0x110,struct.pack('<Q',common));u.mem_write(frame+0x100,bytes([flag]));u.mem_write(frame+0x1b0,struct.pack('<Q',info))
            for i,off in enumerate([0,0x80,0x90,0xa0,0xb0,0xc0,0xd0]):u.mem_write(info+off,struct.pack('<4I',0x1200+i,0,0x1000,0x4d4))
            run(0xe93bc,[],{UC_ARM64_REG_X19:incoming,UC_ARM64_REG_X27:record,UC_ARM64_REG_X28:munmaps,UC_ARM64_REG_X29:frame},0xe9418)
            commands=[x['command']for x in events]
            if mode==3:assert not commands
            else:
                assert '0x21'in commands and '0x23'in commands
                assert commands==sorted(commands)
                for handle in ['0x1202','0x1203','0x1204']: # HEIF +90/+a0/+b0 areas.
                    assert any(x['command']=='0x21'and x['handle']==handle for x in events)
                    assert any(x['command']=='0x23'and x['handle']==handle for x in events)
            ordering.append({'record_mode':mode,'flag':flag,'requests':events.copy(),'share_precedes_original_reference_free':mode!=3})
    md=Cs(CS_ARCH_ARM64,CS_MODE_LITTLE_ENDIAN);regions=[]
    for lo,hi,role in [(0xe3bb4,0xe3c14,'Distinct nonzero area-handle free helper'),(0xe3c14,0xe3edc,'Still-record descriptor cleanup'),(0xe274c,0xe2a30,'Still-record share/open enumeration'),(0x2b2258,0x2b2270,'share_and_open command wrapper'),(0x2b24d8,0x2b24fc,'DMM release/free command wrappers'),(0xe93bc,0xe9418,'RecordStill caller share-before-free branch')]:
        v,_,o=next(s for s in segments if s[0]<=lo<hi<=s[0]+s[1]);start=lo-v+o;ins=list(md.disasm(blob[start:start+hi-lo],lo));assert sum(i.size for i in ins)==hi-lo
        regions.append({'file_offset':start,'start_va':hex(lo),'end_va_exclusive':hex(hi),'role':role,'instructions':[{'va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in ins]})
    out={'source':{'path':str(SOURCE.relative_to(ROOT)),'sha256':digest,'bytes':len(blob)},'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},'camera_accessed':False,'network_access':False,'DMM_service_executed':False,'codec_executed':False,'method':'Original ELF instruction bodies in host Unicorn; local PLT methods bind to original definitions. DMM services intercepted before transport, libc memset substituted.','regions':regions,'free_sequence_cases':free_sequences,'HIF_descriptor_cleanup_cases':hif_cases,'HIF_share_cases':shares,'caller_ordering_cases':ordering,'counts':{'regions':len(regions),'instructions':sum(len(r['instructions'])for r in regions),'host_cases':len(free_sequences)+len(hif_cases)+len(shares)+len(ordering)},'limits':['Shared handles and service success are synthetic; actual resource reference counts and lifetime are not observed.','Existence of these cleanup routines does not prove their scheduling relative to every asynchronous write, error or cancellation.','Area cleanup deduplicates nonzero numeric handles in its local array; aliases under distinct handles are an external DMM contract.','No camera, filesystem writer, real IPC or saved image is exercised.']}
    (ROOT/'research/evidence/recording-ownership.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out['counts'],indent=2))


if __name__=='__main__':main()
