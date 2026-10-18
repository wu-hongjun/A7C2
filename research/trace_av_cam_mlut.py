"""Bounded av-cam MLUT software boundary; no register or pixel access."""
import hashlib
import itertools
import json
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X30,UC_ARM64_REG_PC,UC_ARM64_REG_SP

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha3_unpacked/av-cam.bin'
SHA='90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096'
LINK=0xffffffc001000000


def main():
    blob=SOURCE.read_bytes();assert hashlib.sha256(blob).hexdigest()==SHA and blob[8:12]==b'ORIL'
    u=Uc(UC_ARCH_ARM64,UC_MODE_ARM);u.mem_map(0,(len(blob)+4095)&~4095);u.mem_write(0,blob)
    arena,stack,end=0x10000000,0x20000000,0x30000000
    for a,n in [(arena,0x10000),(stack,0x10000),(end,4096)]:u.mem_map(a,n)
    events=[]
    def hook(cpu,address,size,_):
        if address==0x2bfc24:
            p=cpu.reg_read(UC_ARM64_REG_X1)
            events.append({'boundary_file_offset':hex(address),'IP_argument':cpu.reg_read(UC_ARM64_REG_X0),'dimensions':list(struct.unpack('<2I',cpu.mem_read(p,8)))})
            cpu.reg_write(UC_ARM64_REG_X0,0);cpu.reg_write(UC_ARM64_REG_PC,cpu.reg_read(UC_ARM64_REG_X30))
        elif address==0x2bf7e8:
            events.append({'stub':'diagnostic logger'});cpu.reg_write(UC_ARM64_REG_X0,0);cpu.reg_write(UC_ARM64_REG_PC,cpu.reg_read(UC_ARM64_REG_X30))
    u.hook_add(UC_HOOK_CODE,hook)
    initial=u.context_save()
    def run(va,args):
        u.context_restore(initial);u.mem_write(stack,b'\0'*0x10000);events.clear()
        u.reg_write(UC_ARM64_REG_SP,stack+0x8000);u.reg_write(UC_ARM64_REG_X30,end)
        for r,v in zip([UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2],args):u.reg_write(r,v)
        u.emu_start(va,end,count=5000);assert u.reg_read(UC_ARM64_REG_PC)==end
        return u.reg_read(UC_ARM64_REG_X0)
    addresses=[]
    for ip in [0,1,2,3,6,0xffffffff]:
        actual=run(0x2b05c8,[ip]);assert actual==(0xfffffffff3019000 if ip==0 else 0)
        addresses.append({'IP':ip,'returned_address_scalar':hex(actual),'address_dereferenced':False})
    dimensions=[]
    for w,h in itertools.product([0,1,4672,7008,32767,32768,0xffffffff],repeat=2):
        u.mem_write(arena,struct.pack('<2I',w,h));ret=run(0x2b230c,[0,arena]);valid=w<32768 and h<32768
        assert ret==(0 if valid else 0xfffffff3)
        assert events==([{'boundary_file_offset':'0x2bfc24','IP_argument':0,'dimensions':[w,h]}]if valid else [])
        dimensions.append({'supplied_dimensions':[w,h],'return_u32':hex(ret),'intercepted_calls':events.copy()})
    wrappers=[]
    for flag,w,h in itertools.product([0,1,0xffffffff],[7008,32768],[4672,32768]):
        u.mem_write(arena+0x20,struct.pack('<2I',w,h));ret=run(0x2e3e34,[arena,flag]);valid=flag==0 and w<32768 and h<32768
        assert ret==int(not valid)
        assert bool(events)==valid
        wrappers.append({'flag':flag,'source_dimensions_at_20_24':[w,h],'return':ret,'intercepted_calls':events.copy()})
    def string(pointer):
        off=pointer-LINK;assert 0<=off<len(blob);return blob[off:blob.index(b'\0',off)].decode('ascii')
    registry=[]
    for n in range(64):
        at=0xbe2348+24*n;name,helptext,handler=struct.unpack_from('<3Q',blob,at)
        assert LINK<=handler<LINK+0x320000
        registry.append({'table_file_offset':hex(at),'raw_hex':blob[at:at+24].hex(),'name':string(name),'help':string(helptext),'handler_file_offset':hex(handler-LINK)})
    assert blob[0xbe2948:0xbe2960]==b'\0'*24
    targets={0x2b05c8,0x2b230c,0x2beb94,0x2e3e34};branches=[]
    for at in range(0,0x320000,4):
        word=struct.unpack_from('<I',blob,at)[0]
        if word&0x7c000000!=0x14000000:continue
        imm=word&0x3ffffff
        if imm&0x2000000:imm-=0x4000000
        target=at+4*imm
        if target in targets:branches.append({'file_offset':hex(at),'instruction_bytes':blob[at:at+4].hex(),'target_file_offset':hex(target),'kind':'BL'if word>>31 else'B'})
    md=Cs(CS_ARCH_ARM64,CS_MODE_LITTLE_ENDIAN);regions=[]
    for lo,hi,role in [(0x2b05c8,0x2b05dc,'IP-to-MMIO scalar lookup'),(0x2b230c,0x2b235c,'Native MLUT geometry dispatcher'),(0x2beb94,0x2bebc8,'Native MLUT geometry validation'),(0x2e3e34,0x2e3e74,'Geometry wrapper; provenance of its caller remains unknown')]:
        ins=list(md.disasm(blob[lo:hi],lo));assert sum(i.size for i in ins)==hi-lo
        regions.append({'source_path':str(SOURCE.relative_to(ROOT)),'file_offset':lo,'start_va':hex(lo),'end_va_exclusive':hex(hi),'role':role,'instructions':[{'va':hex(i.address),'file_offset':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in ins]})
    out={'source':{'path':str(SOURCE.relative_to(ROOT)),'bytes':len(blob),'sha256':SHA},'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},'camera_accessed':False,'network_access':False,'target_executed':False,'MMIO_accessed':False,'codec_executed':False,'method':'Original AArch64 instructions in bounded host Unicorn at file-offset alias0; lower register writer and diagnostics intercepted. No pixel data or physical memory.','regions':regions,'MMIO_address_cases':addresses,'geometry_cases':dimensions,'wrapper_cases':wrappers,'diagnostic_registry':registry,'bounded_direct_branch_inventory':{'file_start':'0x0','file_end_exclusive':'0x320000','step_bytes':4,'targets':[hex(x)for x in sorted(targets)],'matches':branches,'limitation':'Aligned direct B/BL encodings only; no indirect calls, pointer construction, relocations, overlays or runtime control flow are recovered.'},'counts':{'regions':len(regions),'instructions':sum(len(r['instructions'])for r in regions),'host_cases':len(addresses)+len(dimensions)+len(wrappers),'diagnostic_registry_entries':len(registry)},'limits':['A returned MMIO-like scalar is not dereferenced. The final register writer is intercepted before any register access.','Geometry admission proves 15-bit representation in these software instructions, not hardware dimensions, throughput or full-resolution still support.','The registry identifies diagnostic handlers; it does not establish use during capture.','No actual allocation, handle, pixel-port direction or MLUT-to-encoder identity is supplied by these helpers.','The av-cam embedded MMIO target and CP MLUT table use different address models; aliasing or physical identity has not been demonstrated.']}
    (ROOT/'research/evidence/av-cam-mlut.json').write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out['counts'],indent=2))


if __name__=='__main__':main()
