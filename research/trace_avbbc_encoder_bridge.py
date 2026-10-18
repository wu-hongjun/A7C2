"""Byte-bound AP JPEG command/response tests; services are intercepted on host."""
import hashlib
import json
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile
from unicorn.arm64_const import UC_ARM64_REG_PC
from trace_heif_encoder import Machine, SOURCE, ROOT, REGS


class JpegMachine(Machine):
    def __init__(self):
        super().__init__()
        # Bind only locally defined GLOB_DAT symbols. This is a bounded local
        # binding experiment, not a claim about the camera's ELF loader scope.
        with SOURCE.open('rb') as f:
            e = ELFFile(f); sy = e.get_section_by_name('.dynsym')
            for r in e.get_section_by_name('.rela.dyn').iter_relocations():
                if r.entry.r_info_type == 1025:
                    s = sy.get_symbol(r.entry.r_info_sym)
                    if isinstance(s.entry.st_shndx, int):
                        self.u.mem_write(r.entry.r_offset, struct.pack('<Q', s.entry.st_value+r.entry.r_addend))

    def external(self, u, addr, size, user):
        name = self.imports.get(addr)
        a = [u.reg_read(r) for r in REGS]
        if name == 'memset':
            u.mem_write(a[0], bytes([a[1]&255])*a[2]); self.ret(a[0])
        elif name == 'memcpy':
            u.mem_write(a[0], bytes(u.mem_read(a[1], a[2]))); self.ret(a[0])
        elif 'ConvertCid' in name or 'ConvertCmdMid' in name:
            assert name in self.symbols, name
            u.reg_write(UC_ARM64_REG_PC, self.symbols[name][0])
        elif 'MakeInputYcStdData' in name:
            self.calls.append({'stub': 'MakeInputYcStdData metadata setup', 'payload': hex(a[1])}); self.ret()
        elif 'getAspect' in name:
            self.calls.append({'stub': 'getAspect', 'return': 1}); self.ret(1)
        else:
            super().external(u, addr, size, user)

    def jpeg_setup(self, mode=0):
        self.setup()
        self.u.mem_write(self.obj+0x20, bytes([mode]))
        self.values.update({0x1047: self.yc, 0x104a: self.payload, 0x1048: self.payload})
        self.u.mem_write(self.payload+8, struct.pack('<II', 0x11002200, 0x33004400))
        self.u.mem_write(self.payload+0x20, struct.pack('<Q', 0x1122334455667788))
        self.u.mem_write(self.yc+0x54, struct.pack('<III', 0x778899aa, 0x120, 0x340))


def main():
    blob = SOURCE.read_bytes()
    digest = hashlib.sha256(blob).hexdigest()
    assert digest == '676197567dbbefa54e08f528968efc6ca2aaffe8bee3ee3eae8164382ed5a7e5'
    m = JpegMachine()
    conversions = []
    for comp, logical, expected_cid, expected_mid in [(3,0x304,0x2000,4),(5,0x534,0x2003,0xffff),(5,0x531,0x2003,0x109)]:
        cid = m.run(0x141a34,[0,comp])
        mid = m.run(0x14234c,[0,comp,logical])
        assert (cid,mid)==(expected_cid,expected_mid)
        conversions.append({'logical_component':comp,'logical_command':hex(logical),'category':hex(cid),'command':hex(mid),'binding':'local defined symbols only'})
    builders=[]
    for q in range(256):
        m.jpeg_setup(); m.u.mem_write(m.payload+0x70,bytes([q,3,4]))
        assert m.run(0x121ba0,[m.obj,m.out])==0
        b=bytes(m.u.mem_read(m.out,80))
        assert struct.unpack_from('<H',b,8)[0]==7
        assert struct.unpack_from('<4H',b,12)==(0x2000,0x8310,0x2000,0x8311)
        assert struct.unpack_from('<3I',b,0x18)==(0x11002200,0x33004400,0x55667788)
        assert struct.unpack_from('<3I',b,0x24)==(0,0x778899aa,0)
        assert struct.unpack_from('<3I',b,0x34)==(0x460,0x120,0x340)
        assert struct.unpack_from('<4H',b,0x40)==(6000,4000,6000,4000)
        assert list(b[0x48:0x4e])==[0,{0:0,1:1,3:6}.get(q,2),1,0,3,4]
        builders.append({'quality_input':q,'quality_output':b[0x49],'payload_hex':b.hex(),'stubs':m.calls.copy()})
    submissions=[]
    # Execute the actual main-image command wrapper with the main parameter
    # builder. Metadata writes and final OSAL send remain explicitly stubbed.
    for status in [0,1,0xffff]:
        m.jpeg_setup(); m.u.mem_write(m.unit+2,struct.pack('<H',status))
        ret=m.run(0x1226c4,[m.obj,m.unit])
        assert ret==(0 if status==0 else 1)
        if status==0:
            assert len(m.sent)==1 and m.sent[0]['destination']=='0x82830752' and m.sent[0]['bytes']==80
            b=bytes.fromhex(m.sent[0]['payload_hex'])
            assert struct.unpack_from('<4H',b)==(0x2000,4,0x50,0)
            assert struct.unpack_from('<3I',b,0x18)==(0x11002200,0x33004400,0x55667788)
        else: assert not m.sent
        submissions.append({'DMM_status_u16':status,'return':ret,'send':m.sent.copy(),'stubs':m.calls.copy()})
    responses=[]
    for mode in [0,1,2,3]:
        for status in [0,1,0x80000000]:
            m.jpeg_setup(mode);m.u.mem_write(m.payload+0x5c,b'\xa5'*32)
            m.u.mem_write(m.reply+8,struct.pack('<II',status,0x87654321))
            ret=m.run(0x122b44,[m.obj,m.reply,0])
            assert ret==(0x201 if status else 1 if mode==3 else 0)
            words=[]
            for target,off in [(0,0x5c),(1,0x68),(2,0x74)]:
                value=struct.unpack('<I',m.u.mem_read(m.payload+off,4))[0]
                assert value==(0xa5a5a5a5 if mode!=target else 0 if status else 0x87654321)
                words.append({'offset':hex(off),'value':hex(value)})
            responses.append({'mode':mode,'status':hex(status),'return':ret,'length_words':words,'published_parameters':m.errors.copy()})
    md=Cs(CS_ARCH_ARM64,CS_MODE_LITTLE_ENDIAN);regions=[]
    for lo,hi,role in [(0x1218a8,0x121ba0,'SDYC metadata constructor (static only)'),(0x121ba0,0x121f78,'Main JPEG parameter builder'),(0x1226c4,0x122b08,'JPEG command wrapper'),(0x122b44,0x122dfc,'JPEG completion'),(0x141a34,0x141a58,'category converter'),(0x141b18,0x141b58,'SDF converter'),(0x141b98,0x141bd8,'MC converter'),(0x14234c,0x1423b4,'component switch'),(0x139158,0x139abc,'OSAL sender (static only)')]:
        v,n,o=next(s for s in m.segments if s[0]<=lo<hi<=s[0]+s[1]);start=lo-v+o;raw=blob[start:start+hi-lo]
        ins=list(md.disasm(raw,lo));assert sum(i.size for i in ins)==hi-lo
        regions.append({'file_offset':start,'start_va':hex(lo),'end_va_exclusive':hex(hi),'role':role,'instructions':[{'va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in ins]})
    tables=[]
    for name in ['_ZN16ObjAvbbcCmnTable17midConvTableToSdfE','_ZN16ObjAvbbcCmnTable16midConvTableToMcE']:
        a,n=m.symbols[name];raw=bytes(m.u.mem_read(a,n));v,_,o=next(s for s in m.segments if s[0]<=a<a+n<=s[0]+s[1]);assert blob[a-v+o:a-v+o+n]==raw
        tables.append({'symbol':name,'va':hex(a),'file_offset':a-v+o,'raw_hex':raw.hex(),'pairs':[[hex(x)for x in struct.unpack_from('<HH',raw,i)]for i in range(0,n,4)]})
    df=SOURCE.with_name('dataflowInfraFramework.so');dfblob=df.read_bytes()
    with df.open('rb') as f:
        e=ELFFile(f)
        for lo,hi,role in [(0x32538,0x32564,'24-byte dataflow header constructor'),(0x32564,0x32698,'Dataflow command copy')]:
            s=next(s for s in e.iter_segments() if s.header.p_type=='PT_LOAD' and s.header.p_vaddr<=lo<hi<=s.header.p_vaddr+s.header.p_filesz)
            start=lo-s.header.p_vaddr+s.header.p_offset;raw=dfblob[start:start+hi-lo];ins=list(md.disasm(raw,lo));assert sum(i.size for i in ins)==hi-lo
            regions.append({'source_path':str(df.relative_to(ROOT)),'file_offset':start,'start_va':hex(lo),'end_va_exclusive':hex(hi),'role':role,'instructions':[{'va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in ins]})
    sought={'_ZN16ObjAvbbcCmnTable16midConvTableToMcE','_ZN20InfraAvbbcMsgManager13ConvertCmdMidEtt','_ZN11ObjAvbbcCmn13ConvertCmdMidEtt','_ZN11ObjAvbbcCmn17ConvertCmdMidToMcEt'}
    definitions=[];elf_count=0;dynsym_count=0
    for p in sorted((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob('*')):
        if not p.is_file():continue
        with p.open('rb') as f:
            if f.read(4)!=b'\x7fELF':continue
            elf_count+=1;f.seek(0);e=ELFFile(f);sy=e.get_section_by_name('.dynsym')
            if sy is None:continue
            dynsym_count+=1
            for s in sy.iter_symbols():
                if s.name in sought and isinstance(s.entry.st_shndx,int):
                    definitions.append({'path':str(p.relative_to(ROOT)),'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'symbol':s.name,'va':hex(s.entry.st_value),'size':s.entry.st_size})
    out={'source':{'path':str(SOURCE.relative_to(ROOT)),'sha256':digest,'bytes':len(blob)},'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},'supporting_harness':{'path':'research/trace_heif_encoder.py','sha256':hashlib.sha256((ROOT/'research/trace_heif_encoder.py').read_bytes()).hexdigest()},'camera_access':False,'network_access':False,'codec_execution':False,'regions':regions,'conversion_cases':conversions,'conversion_tables':tables,'JPEG_builder_cases':builders,'JPEG_submission_cases':submissions,'JPEG_response_cases':responses,'counts':{'instruction_regions':len(regions),'instructions':sum(len(r['instructions'])for r in regions),'conversion_cases':len(conversions),'builder_cases':len(builders),'submission_cases':len(submissions),'response_cases':len(responses)},'limits':['Original AArch64 functions execute only in host Unicorn with synthetic payloads.','SDYC metadata construction is intercepted, not executed; physical backing bytes and descriptor services are not proved.','getAspect returns synthetic1; no aspect mapping claim.','OsalSndMsg is intercepted; no message is sent and no codec runs.','Locally defined converter/table binding is explicit; runtime symbol resolution and mutation are not modeled.','AP JPEG wire fields and CP JPEG object fields remain separate layouts until transport transformation is proved.','No equality between selected MLUT resource/address, JPEG/HEIF input and saved file image areas has been established.']}
    out.update({'dataflow_source':{'path':str(df.relative_to(ROOT)),'sha256':hashlib.sha256(dfblob).hexdigest(),'bytes':len(dfblob)},'bounded_dynamic_definition_inventory':{'ELF_files_inspected':elf_count,'ELF_files_with_dynsym':dynsym_count,'symbols_sought':sorted(sought),'definitions':definitions,'limit':'Named dynamic definitions in this extraction only; no runtime binding, table mutation, dlopen target or alternate resolver is observed.'}})
    target=ROOT/'research/evidence/avbbc-encoder-bridge.json';target.write_text(json.dumps(out,indent=2)+'\n')
    print(json.dumps(out['counts'],indent=2))


if __name__=='__main__': main()
