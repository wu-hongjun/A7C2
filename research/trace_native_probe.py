"""Offline original stream-cursor/position instructions and ABI probe replay.

No live process, camera, G HUB, USB, mounted media, device node or network.
All Sony external calls are synthetic stubs; only owned generated memory runs.
"""
import hashlib
import json
import struct
import sys
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.arm64_const import *
from map_ap_interfaces import Image,LIB
from trace_sony_integration import machine,execute,execute_slice,BASE,REG,PINS
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from camera_worker.native_probe_veneer import build,SITE,DISPLACED

ROOT=Path(__file__).resolve().parents[1]
MC_SHA='d7cd602e9348e51703a6d3da396cfda949ea306856df7fa0deb62df1ff4ca974'
RANGES={'libInfraFsys.so':[(0x50b74,0x50b8c,'actual drive stream allocator/releaser installation'),
    (0x4dc34,0x4df80,'stream descriptor allocation: cursor 1 and vector data, not CPU buffer'),
    (0x4d728,0x4d770,'stream release validates cursor then clears descriptor'),
    (0x7abcc,0x7ac1c,'PFFILE writer enters volume critical section'),
    (0x7ae78,0x7af18,'append handling, current position, proposed instruction probe'),
    (0x7a6a8,0x7a6c4,'fgetpos exports same 64-bit backing+0x38 field'),
    (0x40054,0x40084,'PrFILE getter passes file-info backing to pf2_fgetpos'),
    (0x559f4,0x55a18,'pf2_fwrite forwards backing as PFFILE argument x3'),
    (0x50fd0,0x51020,'native stream cursor resolves to vector extents')],
    'libInfraMediaCommon.so':[(0x18e80,0x18ed0,'first CPU mapping and padding window'),
    (0x18f74,0x18fb8,'second CPU mapping and its physical base/length'),
    (0x19158,0x191b0,'mapping fields cleared and existing CPU windows unmapped at destruction'),
    (0x193c8,0x19440,'range-checked CPU to physical mapping used for mode-2 padding'),
    (0x4c71c,0x4c744,'native mapping requests PROT_READ|PROT_WRITE and MAP_SHARED')]}
def main():
    images={n:Image(LIB/n) for n in RANGES}
    for name,sha in [('libInfraFsys.so',PINS['libInfraFsys.so']),('libInfraMediaCommon.so',MC_SHA)]:
        assert hashlib.sha256(images[name].blob).hexdigest()==sha
    im=images['libInfraFsys.so'];rows=[]
    assert im.bytes_at(SITE,4)==DISPLACED
    u,m=machine(im,RANGES['libInfraFsys.so']);drive=BASE;u.reg_write(UC_ARM64_REG_X20,drive)
    execute_slice(u,0x50b74,0x50b8c,{})
    assert struct.unpack('<QQ',u.mem_read(drive+0x6638,16))==(0x4dc34,0x4d728)
    rows.append({'case':'installed-stream-callbacks','allocator':'0x4dc34','release':'0x4d728'})
    for mode in [1,2,3]:
      for allocation_ok in [True,False]:
        u,m=machine(im,RANGES['libInfraFsys.so']);drive=BASE;source=BASE+0x8000;descriptor=BASE+0xa000;out=BASE+0xe000
        u.mem_write(source,struct.pack('<QQ',0x30000000,4096));u.mem_write(drive+0x6680,struct.pack('<Q',BASE+0x7000));u.mem_write(BASE+0x7060,struct.pack('<I',1))
        for reg,value in zip(REG,[drive,8192,mode,1,source,out]):u.reg_write(reg,value)
        def padding(uc,args):uc.mem_write(args[0],struct.pack('<Q',BASE+0x9000));uc.mem_write(args[1],struct.pack('<Q',4096));return 0
        def copy(uc,args):uc.mem_write(args[0],bytes(uc.mem_read(args[1],args[2])));return args[0]
        calls=execute(u,0x4dc34,{0x216f0:padding,0x22020:lambda *_:descriptor if allocation_ok else 0,
                               0x250e0:lambda *_:0x40000000,0x20f50:copy})
        ok=mode in (1,2) and allocation_ok
        assert (u.reg_read(REG[0])==0)==ok
        if ok:
            assert struct.unpack('<Q',u.mem_read(out,8))[0]==1
            assert struct.unpack('<Q',u.mem_read(drive+0x66a0,8))[0]==descriptor
            header=bytes(u.mem_read(descriptor,0x38));assert struct.unpack_from('<II',header)==(mode,2)
            assert struct.unpack_from('<QQ',header,0x18)==(1,8193)
            padding_address=struct.unpack('<Q',u.mem_read(descriptor+0x48,8))[0]
            assert padding_address==(BASE+0x9000 if mode==1 else 0x40000000)
        rows.append({'case':'original-allocator-returns-cursor-not-pointer','mode':mode,'allocation_ok':allocation_ok,
                     'accepted':ok,'returned_cursor':1 if ok else None,'calls':calls})
    for supplied in [1,2]:
        u,m=machine(im,RANGES['libInfraFsys.so']);drive=BASE;descriptor=BASE+0xa000
        u.mem_write(drive+0x66a0,struct.pack('<Q',descriptor));u.mem_write(descriptor+0x18,struct.pack('<Q',1))
        u.reg_write(REG[0],drive);u.reg_write(REG[1],supplied)
        calls=execute(u,0x4d728,{0x20fe0:lambda *_:0})
        assert bool(calls)==(supplied==1)
        assert struct.unpack('<Q',u.mem_read(drive+0x66a0,8))[0]==(0 if supplied==1 else descriptor)
        rows.append({'case':'release-is-stream-lease-not-byte-buffer','cursor':supplied,'freed':bool(calls)})
    for position in [0,1,0x100000001,0x123456789abcdef0]:
        u,m=machine(im,RANGES['libInfraFsys.so']);pf=BASE;out=BASE+0x4000
        u.mem_write(pf+0x38,struct.pack('<Q',position));u.reg_write(UC_ARM64_REG_X20,pf);u.reg_write(UC_ARM64_REG_X24,out)
        execute_slice(u,0x7a6a8,0x7a6b0,{})
        assert struct.unpack('<Q',u.mem_read(out,8))[0]==position
        rows.append({'case':'original-fgetpos-exports-full-64-bit-current-position','position':hex(position)})
    # Native append adjustment precedes the proposed point. The append helper
    # itself is a stub, so its filesystem semantics are not claimed as tested.
    for flags in [0,4,0x8004]:
        u,m=machine(im,RANGES['libInfraFsys.so']);pf=BASE;u.reg_write(REG[0],flags);u.reg_write(UC_ARM64_REG_X25,pf)
        u.mem_write(pf+0x38,struct.pack('<Q',0))
        def append(uc,args):uc.mem_write(args[0]+0x38,struct.pack('<Q',77));return 0
        calls=execute_slice(u,0x7ae78,0x7ae90,{0x23d50:append})
        assert bool(calls)==(flags==4)
        rows.append({'case':'append-branch-runs-before-position-probe','flags':hex(flags),'append_called':bool(calls)})
    mc=images['libInfraMediaCommon.so']
    for physical,length in [(0x40000000,4096),(0x60000000,8192),(0x140000000,0x20000)]:
        u,m=machine(mc,RANGES['libInfraMediaCommon.so']);u.reg_write(UC_ARM64_REG_X20,17)
        u.reg_write(UC_ARM64_REG_X23,physical);u.reg_write(UC_ARM64_REG_X21,length);seen=[]
        def mapping(uc,args):seen.append(args[:6]);return 0x30000000
        calls=execute_slice(u,0x4c71c,0x4c738,{0x17130:mapping})
        assert seen==[[0,length,3,1,17,physical]]
        rows.append({'case':'original-mmap-declares-readable-shared-CPU-window','physical':hex(physical),'bytes':length,'calls':calls,
                     'limit':'mmap/ioctl/driver are NOT executed; cache visibility and live mapping lifetime remain unproved'})
    for value in [0,0x30000000-1,0x30000000,0x30001fff,0x30002000,0x50000000,0x50000fff,0x50001000]:
        u,m=machine(mc,RANGES['libInfraMediaCommon.so']);m(0x79000,4096)
        for address,n in [(0x79f28,0x30000000),(0x79ec0,8192),(0x79eb8,0x40000000),
                          (0x79f48,0x50000000),(0x79f10,4096),(0x79f08,0x60000000)]:u.mem_write(address,struct.pack('<Q',n))
        u.reg_write(REG[0],value);execute(u,0x193c8,{})
        expected=0x40000000+value-0x30000000 if 0x30000000<=value<0x30002000 else 0x60000000+value-0x50000000 if 0x50000000<=value<0x50001000 else 0xffffffffffffffff
        assert u.reg_read(REG[0])==expected
        rows.append({'case':'original-two-window-physical-mapper-bounds','CPU':hex(value),'physical':hex(expected)})
    veneers=[]
    for bias in [0,0x100000000]:
      for position in [0,0x100000001]:
        site=SITE+bias;veneer=site+0x100000;callback=0x14000000;plan=build(site,veneer,callback,DISPLACED)
        u,m=machine(im,[(SITE,SITE+8,'source instruction pair')]);m(site,8);m(veneer,len(plan['veneer']));m(callback,4)
        u.mem_write(site,plan['patch']+im.bytes_at(SITE+4,4));u.mem_write(veneer,plan['veneer'])
        original=[0x1000+i*0x100 for i in range(31)];original[25]=BASE+0x4000;original[29]=BASE+0x10000
        for i,value in enumerate(original):u.reg_write(globals()[f'UC_ARM64_REG_X{i}'],value)
        sp=BASE+0x1d000;u.reg_write(UC_ARM64_REG_SP,sp);u.mem_write(original[25]+0x38,struct.pack('<Q',position))
        u.reg_write(UC_ARM64_REG_CPACR_EL1,3<<20);u.reg_write(UC_ARM64_REG_NZCV,0xa0000000)
        u.reg_write(UC_ARM64_REG_FPCR,0x400000);u.reg_write(UC_ARM64_REG_FPSR,0x10)
        for i in range(32):u.reg_write(globals()[f'UC_ARM64_REG_Q{i}'],int.from_bytes(bytes([i+1])*16,'little'))
        observed=[]
        def probe(uc,address,size,data):
            if address!=callback:return
            saved=bytes(uc.mem_read(uc.reg_read(REG[0]),800));assert uc.reg_read(UC_ARM64_REG_SP)%16==0
            assert list(struct.unpack_from('<31Q',saved))==original and struct.unpack_from('<Q',saved,248)[0]==sp
            assert struct.unpack_from('<Q',saved,792)[0]==site
            ret=uc.reg_read(UC_ARM64_REG_LR);observed.append(True)
            for i in range(31):uc.reg_write(globals()[f'UC_ARM64_REG_X{i}'],0xf000+i)
            for i in range(32):uc.reg_write(globals()[f'UC_ARM64_REG_Q{i}'],0xdeadbeef)
            uc.reg_write(UC_ARM64_REG_NZCV,0x50000000);uc.reg_write(UC_ARM64_REG_FPCR,0x800000);uc.reg_write(UC_ARM64_REG_FPSR,8)
            uc.reg_write(UC_ARM64_REG_PC,ret)
        hook=u.hook_add(UC_HOOK_CODE,probe);u.emu_start(site,site+8,count=1000);u.hook_del(hook)
        assert observed==[True] and u.reg_read(UC_ARM64_REG_PC)==site+8
        for i,value in enumerate(original):assert u.reg_read(globals()[f'UC_ARM64_REG_X{i}'])==(position if i==0 else value)
        for i in range(32):assert u.reg_read(globals()[f'UC_ARM64_REG_Q{i}'])==int.from_bytes(bytes([i+1])*16,'little')
        assert u.reg_read(UC_ARM64_REG_SP)==sp and u.reg_read(UC_ARM64_REG_NZCV)==0xa0000000
        assert u.reg_read(UC_ARM64_REG_FPCR)==0x400000 and u.reg_read(UC_ARM64_REG_FPSR)==0x10
        assert struct.unpack('<Q',u.mem_read(original[29]+0x88,8))[0]==position
        veneers.append({'case':'ABI-veneer-restores-all-registers-and-replays-original-load','load_bias':hex(bias),
            'position':hex(position),'veneer_bytes':len(plan['veneer']),'sha256':hashlib.sha256(plan['veneer']).hexdigest(),'stack_bytes':800})
    for values in [(SITE,SITE+(1<<28),callback,DISPLACED),(SITE,0xc0000,callback,bytes(4)),(SITE,0xc0001,callback,DISPLACED)]:
        try:build(*values)
        except ValueError:pass
        else:raise AssertionError('Bad veneer accepted')
    ranges=[{'source':name,'ELF_VA':hex(a),'end_exclusive':hex(b),'scope':why,'sha256':hashlib.sha256(image.bytes_at(a,b-a)).hexdigest(),'instructions':image.disasm(a,b)}
        for name,image in images.items() for a,b,why in RANGES[name]]
    doc={'schema':1,'camera_accessed':False,'ghub_accessed':False,'network_accessed':False,'installed':False,
        'source_sha256':{'libInfraFsys.so':PINS['libInfraFsys.so'],'libInfraMediaCommon.so':MC_SHA},
        'ranges':ranges,'original_instruction_cases':rows,'veneer_cases':veneers,'rejected_veneer_plans':3,
        'correction':'FSYS 0x2f088 x1 can be cursor 1+offset. It is not a readable scratch-buffer proof, including when forwarded as pf2_fwrite data.',
        'position_proof':'Source-bound PFFILE backing+0x38 at 0x7af08, after append branch and within writer volume critical section; not a live lock/attach test.',
        'remaining':['No camera trampoline installed or native entry/exit hooks attached.','Physical windows and allocation lifetime/cache visibility need actual source-bound runtime registration.','TLS nesting, safe readers and veneer are offline-tested candidates; target kernel read permissions, stack headroom, lock duration and teardown remain unvalidated.']}
    (ROOT/'research/evidence/native-probe-trace.json').write_text(json.dumps(doc,indent=2)+'\n')
    print(f'{len(rows)} original instruction cases; {len(veneers)} ABI veneer cases; no device or live-process access')
if __name__=='__main__':main()
