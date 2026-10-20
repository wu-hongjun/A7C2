"""Pinned original FSYS cursor arithmetic -> checked write -> truncate -> release.

CORRECTION: native-probe-trace establishes that the assigned callback returns
stream cursor 1. The synthetic base below exercises arithmetic, NOT readable
CPU memory. Use the native vector probe, never dereference this cursor.

All external calls are synthetic. No Sony execution, camera, USB, media or G HUB.
Run with the local research interpreter: python research/trace_encoded_write.py.
"""
import hashlib
import json
import struct
from pathlib import Path
from unicorn.arm64_const import *
from map_ap_interfaces import Image, LIB
from trace_sony_integration import machine, execute_slice, BASE, REG, PINS

ROOT=Path(__file__).resolve().parents[1]
REGIONS=[(0x2eef0,0x2ef34,'logical size, alignment and stream-lease acquisition'),
         (0x2ef54,0x2ef90,'stream lifetime and release after loop'),
         (0x2f054,0x2f0d8,'stream cursor offset, checked exact count and advance'),
         (0x2f108,0x2f144,'logical-size final truncation'),
         (0x2ebe0,0x2ec3c,'PrFILE versus TNtfs writer dispatch'),
         (0x44b84,0x44bdc,'PrFILE aligned write and restore logical length'),
         (0x44cd0,0x44cec,'PrFILE direct write branch'),
         (0x3cf6c,0x3cfb4,'pf2 byte buffer and element-size/count ABI')]

def main():
    im=Image(LIB/'libInfraFsys.so')
    assert hashlib.sha256(im.blob).hexdigest()==PINS['libInfraFsys.so']
    rows=[]
    symbols={hex(a):im.labels[a] for a in [0x220a0,0x22f00,0x23800,0x25510]}
    assert 'pf2_fwrite@plt' in symbols['0x22f00']
    assert 'PrFILE_ftruncate@plt' in symbols['0x25510']
    # Execute one actual loop iteration. Scratch pointers are synthetic virtual
    # addresses; the native write stub records them and does not perform I/O.
    for total,offset in [(4096,0),(8192,4096),(0x40000001,0)]:
      count=min(total-offset,0x40000000)
      for status in [count,max(0,count-1),0x80000030]:
        u,m=machine(im,REGIONS);fp=BASE+0x10000;scratch=BASE+0x4000;handle=0x81;drive=BASE+0x8000
        u.reg_write(UC_ARM64_REG_X29,fp);u.reg_write(UC_ARM64_REG_X22,drive);u.reg_write(UC_ARM64_REG_X24,total)
        u.reg_write(UC_ARM64_REG_X6,offset);u.reg_write(UC_ARM64_REG_X7,0x3fffffff);u.reg_write(UC_ARM64_REG_X8,0x40000000)
        u.mem_write(fp+0xa0,struct.pack('<Q',scratch));u.mem_write(fp+0x80,struct.pack('<Q',handle))
        end=0x2f580 if status&0x80000000 else 0x2f578 if status!=count else 0x2f0d4
        m(end,4)
        calls=execute_slice(u,0x2f054,end,{0x23940:lambda *_:0,0x224f0:lambda *_:0,0x2eaf4:lambda *_:status})
        write=[c for c in calls if c['address']=='0x2eaf4'][0]
        assert [int(v,16) for v in write['arguments'][:6]]==[handle,scratch+offset,count,0,1,drive]
        if status==count:assert u.reg_read(UC_ARM64_REG_X6)==offset+count
        rows.append({'case':'original-scratch-write-and-return-gate','total':total,'offset':offset,'requested':count,
                     'status':hex(status),'accepted':status==count,'calls':calls})
    for logical in [1,4095,4096,5000,64*1024*1024]:
        u,m=machine(im,REGIONS);request=BASE;drive=BASE+0x8000;slot=BASE+0x4000
        u.reg_write(UC_ARM64_REG_X19,request);u.reg_write(UC_ARM64_REG_X22,drive);u.reg_write(UC_ARM64_REG_X25,slot)
        u.mem_write(request+0x42c,struct.pack('<I',logical));observed=[]
        def trunc(uc,args):observed.append(args[:3]);return 0
        calls=execute_slice(u,0x2f11c,0x2f12c,{0x25510:trunc})
        assert observed==[[drive,slot,logical]]
        rows.append({'case':'original-final-truncation-uses-logical-size','logical_size':logical,'calls':calls})
    for count in [1,4096,65536]:
        u,m=machine(im,REGIONS);slot=BASE;data=BASE+0x8000;backing=0x12345678
        u.reg_write(UC_ARM64_REG_X21,slot);u.reg_write(UC_ARM64_REG_X24,data);u.reg_write(UC_ARM64_REG_X19,count)
        u.mem_write(slot+0x40,struct.pack('<Q',backing));seen=[]
        def pf2(uc,args):seen.append(args[:4]);return count
        calls=execute_slice(u,0x3cf6c,0x3cf84,{0x22f00:pf2})
        assert seen==[[data,1,count,backing]]
        rows.append({'case':'original-pf2-byte-pointer-and-size-one','count':count,'calls':calls})
    for scratch in [BASE+0x4000,BASE+0x8000]:
        u,m=machine(im,REGIONS);fp=BASE+0x10000;drive=BASE;release=BASE+0x19000
        u.reg_write(UC_ARM64_REG_X29,fp);u.reg_write(UC_ARM64_REG_X22,drive)
        u.mem_write(fp+0xa0,struct.pack('<Q',scratch));u.mem_write(drive+0x6640,struct.pack('<Q',release));seen=[]
        def free(uc,args):seen.append(args[:2]);return 0
        calls=execute_slice(u,0x2ef80,0x2ef90,{release:free})
        assert seen==[[drive,scratch]]
        rows.append({'case':'scratch-released-before-late-completion','calls':calls})
    ranges=[{'ELF_VA':hex(a),'end_exclusive':hex(b),'scope':why,
             'sha256':hashlib.sha256(im.bytes_at(a,b-a)).hexdigest(),'instructions':im.disasm(a,b)} for a,b,why in REGIONS]
    doc={'schema':1,'camera_accessed':False,'ghub_accessed':False,'network_during_tests':False,'installed':False,
         'source':'libInfraFsys.so','source_sha256':PINS['libInfraFsys.so'],'PLT_symbols':symbols,'ranges':ranges,
         'original_instruction_cases':rows,'conclusions':{
           'candidate_copy_point':'CORRECTED: 0x2f088 x1 is stream cursor+offset; not a CPU pointer. Resolve mode-2 vectors at the native locked probe before copying.',
           'acceptance':'exact successful count at 0x2f0bc..0x2f0c4, successful logical truncation, ledger acceptance/close/completion',
           'scratch_lifetime':'Stream descriptor released before async completion; cursor and vector leases cannot be retained without ownership.',
           'implemented':'owned-buffer reconstruction model and synthetic worker replay; no Sony hook',
           'remaining':['Runtime readable span, same-open initial zero position and reentrancy/lock-safe native adapter not established.',
                        'Pre-exposure selection association across all drive modes remains unproved.',
                        'Preallocated memory budget, copied-byte bandwidth, private spool storage and capture-safe scheduling need hardware validation.',
                        'TNtfs and write paths bypassing this scratch loop are excluded; sRGB provenance remains explicit.']}}
    (ROOT/'research/evidence/encoded-write.json').write_text(json.dumps(doc,indent=2)+'\n')
    print(f'{len(rows)} original instruction cases; {len(ranges)} pinned ranges; no camera or G HUB access')
if __name__=='__main__':main()
