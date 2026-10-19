"""Offline pinned capture/record/open/close evidence; never accesses camera/media.

Unicorn executes original instruction slices. External calls are synthetic
recorded stubs. Research coordinates are ELF VAs or raw AV file offsets, never
runtime/ASLR addresses. This does not install hooks or read live Sony memory.
"""
import hashlib
import json
import struct
from pathlib import Path
from unicorn.arm64_const import *
from map_ap_interfaces import Image, LIB
from trace_sony_integration import machine, execute, execute_slice, BASE, REG, PINS
from trace_still_info_producer import AV
ROOT=Path(__file__).resolve().parents[1]
REGIONS={
 'appFw.so':[(0x352d794,0x352d7e8,'locked capture ID allocator, including wrap'),
             (0x34a0360,0x34a03a0,'ID assigned before release-key spinal command'),
             (0x3539910,0x3539a40,'ID-bearing 0x4040 request serialization'),
             (0x3539f44,0x3539ff8,'explicit set_capture_start 0x4038 command serialization'),
             (0x3533200,0x3533218,'allocate 0x68c-byte AP payload'),
             (0x35332e8,0x3533300,'wire capture ID to AP payload'),
             (0x34acd48,0x34acf10,'AP retains same payload across mpr_RecordStill'),
             (0x34acff0,0x34ad00c,'AP reads retained ID after RecordStill'),
             (0x34aca5c,0x34acb74,'Sony pending capture deque explicitly matches state, ID and flag'),
             (0x357cbd0,0x357cbfc,'RecordStill adapter calls imported native MPR'),
             (0x34a6198,0x34a6258,'MPR Capture happens at data entry, too late to freeze presets')],
 'libmpr.so':[(0xeb2a4,0xeb2e8,'under mutex, select actual FIFO record, then validate'),
              (0xeb600,0xeb654,'RecordStill normal processing dispatch neighborhood'),
              (0xf1fdc,0xf20f4,'cookie allocation, suffix initialization and actual V2 submit'),
              (0xf227c,0xf2298,'non-last-writer suffix path; DB prefix not necessarily initialized')],
 'libInfraFsys.so':[(0x2e880,0x2e8dc,'full generated handle lookup in active list'),
              (0x2d17c,0x2d1a0,'open handle generation with 27-bit serial'),
              (0x2cac4,0x2caec,'open builds backend path info before calling backend'),
              (0x3f210,0x3f240,'PrFILE stores per-file backing handle'),
              (0x3f658,0x3f674,'PrFILE stores supplied backend path, not independently external Sony path'),
              (0x2eeb0,0x2eee0,'write opens and saves logical handle'),
              (0x2f144,0x2f180,'same handle close and error precedence'),
              (0x2f5ac,0x2f7e4,'V2-to-core translator; no cookie in core completion fields'),
              (0x2f7e4,0x2f8bc,'V2 worker owns original callback/cookie scope'),
              (0x2e964,0x2ea0c,'close versus fsync and close-request flag'),
              (0x30450,0x304e8,'temporary lock count and deferred real close'),
              (0x53df8,0x53e64,'TNtfs stores returned handle in global, not established PrFILE lease'),
              (0x2c9e0,0x2ca64,'actual close, clears/unlinks slot even on failure')]
}
def main():
    images={n:Image(LIB/n) for n in PINS}
    for n,im in images.items():assert hashlib.sha256(im.blob).hexdigest()==PINS[n]
    assert hashlib.sha256(AV.read_bytes()).hexdigest()=='90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096'
    rows=[];ranges=[]
    for name,rs in REGIONS.items():
        for a,b,why in rs:
            im=images[name];ranges.append({'source':name,'ELF_VA':hex(a),'end_exclusive':hex(b),'scope':why,
              'sha256':hashlib.sha256(im.bytes_at(a,b-a)).hexdigest(),'instructions':im.disasm(a,b)})
    im=images['appFw.so']
    for before in [0,0x80000001,0xfffffffd,0xfffffffe]:
        u,m=machine(im,[REGIONS['appFw.so'][0]]);m(0x5ef3000,4096)
        u.mem_write(0x5ef3fe0,struct.pack('<I',before))
        cs=execute(u,0x352d794,{0x1794224:lambda *_:0,0x1794280:lambda *_:0})
        expected=0x80000001 if before==0xfffffffe else before+1
        assert u.reg_read(REG[0])==expected and struct.unpack('<I',u.mem_read(0x5ef3fe0,4))[0]==expected
        rows.append({'case':'locked-ID-allocation-and-wrap','before':hex(before),'after':hex(expected),'calls':cs})
    for frame in [0,73,0x80000001]:
        # Original AP wire -> payload copy, not a reimplemented decoder.
        u,m=machine(im,[(0x35332f0,0x35332f8,'copy ID')]);src=BASE;dst=BASE+0x2000
        # Pin register operands too; fail if this version's interpretation changes.
        ins=im.disasm(0x35332f0,0x35332f8)
        assert [r['operands'] for r in ins]==['w0, [x19, #0xc]','w0, [x25, #0x688]']
        u.reg_write(UC_ARM64_REG_X19,src);u.reg_write(UC_ARM64_REG_X25,dst);u.mem_write(src+12,struct.pack('<I',frame))
        execute_slice(u,0x35332f0,0x35332f8,{})
        assert struct.unpack('<I',u.mem_read(dst+0x688,4))[0]==frame
        rows.append({'case':'wire-ID-to-AP-payload','frame':frame})
    for frame in [0,73,0x80000001]:
        u,m=machine(im,[(0x3539f44,0x3539f84,'capture-start command prefix')]);inp=BASE
        u.mem_write(inp,struct.pack('<III',frame,0,0));u.reg_write(REG[0],inp);u.reg_write(REG[1],BASE+0x2000)
        execute_slice(u,0x3539f44,0x3539f84,{})
        command=bytes(u.mem_read(u.reg_read(UC_ARM64_REG_X29)+0x40,16))
        assert command[:8]==struct.pack('<HHI',5,0x4038,frame)
        rows.append({'case':'original-explicit-capture-start-ID-serialization','ID':frame,'prefix_hex':command[:8].hex()})
    for state in [0,1]:
      for same in [False,True]:
       for flag in [0,1]:
        u,m=machine(im,[(0x34acac0,0x34acae8,'pending capture identity filter')]);fp=BASE+0x10000;entry=BASE
        u.reg_write(UC_ARM64_REG_X29,fp);u.reg_write(UC_ARM64_REG_X23,73);u.reg_write(UC_ARM64_REG_X24,1)
        u.mem_write(fp+0x50,struct.pack('<Q',entry));u.mem_write(entry,struct.pack('<IIB',state,73 if same else 74,flag))
        match=state==1 and same and flag==1
        execute_slice(u,0x34acac0,0x34acae8 if match else 0x34acb50,{})
        rows.append({'case':'Sony-pending-capture-filter-matches-ID-not-FIFO-order','state':state,'same_ID':same,'flag':flag,'match':match})
    im=images['libmpr.so']
    for record in [0,BASE+0x4000]:
        u,m=machine(im,[(0xeb2ac,0xeb2e4,'FIFO selection')]);fifo=BASE;slot=BASE+0x1000;fp=BASE+0x10000
        m(0x2fd000,4096);u.mem_write(0x2fd9e0,struct.pack('<Q',fifo));u.mem_write(slot,struct.pack('<Q',record))
        u.reg_write(UC_ARM64_REG_X25,0x2fd000);u.reg_write(UC_ARM64_REG_X29,fp);u.reg_write(UC_ARM64_REG_X22,fp+0xb8)
        u.reg_write(UC_ARM64_REG_X19,BASE+0x6000);seen=[]
        def iterator(uc,args):uc.mem_write(args[0],struct.pack('<Q',slot));return 0
        def check(uc,args):seen.append(args[:2]);return 3 if not record else 0
        cs=execute_slice(u,0xeb2ac,0xeb2e4,{0x7c9e0:lambda *_:1,0x7cc60:iterator,0x7ec90:check})
        assert seen==[[record,BASE+0x6000]] and u.reg_read(UC_ARM64_REG_X20)==record
        rows.append({'case':'MPR-selected-record-not-external-FIFO-order','record':hex(record),'NULL_is_not_a_binding':not record,'calls':cs})
    for group in [0,1]:
      for media in range(4):
        u,m=machine(im,[(0xf20a0,0xf20e0,'write cookie suffix')]);r=BASE;c=BASE+0x4000;path=BASE+0x8000;fp=BASE+0x10000
        u.mem_write(path,b'A:/DCIM/100MSDCF/DSC00001.HIF\0');u.mem_write(r+0x15c+4*media,struct.pack('<I',200+media));u.mem_write(r+0x1ec+4*group,struct.pack('<I',300+group))
        u.mem_write(fp+0xd8,struct.pack('<Q',r+4*media));u.mem_write(fp+0xc8,struct.pack('<I',media))
        for reg,val in [(UC_ARM64_REG_X19,r),(UC_ARM64_REG_X22,c),(UC_ARM64_REG_X23,path),(UC_ARM64_REG_X24,group),(UC_ARM64_REG_X25,group),(UC_ARM64_REG_X29,fp)]:u.reg_write(reg,val)
        def copy(uc,args):
            raw=bytes(uc.mem_read(args[1],args[2]));raw=raw.split(b'\0',1)[0];uc.mem_write(args[0],raw+bytes(args[2]-len(raw)));return args[0]
        execute_slice(u,0xf20a0,0xf20e0,{0x7f270:copy})
        values=(struct.unpack('<I',u.mem_read(c+0x15e0,4))[0],struct.unpack('<Q',u.mem_read(c+0x1670,8))[0],struct.unpack('<I',u.mem_read(c+0x1678,4))[0])
        assert values==(media,r,group)
        assert struct.unpack('<II',u.mem_read(c+0x15e8,8))==(200+media,300+group)
        rows.append({'case':'original-cookie-suffix-materialization','media':media,'format_group':group,'parent_record':hex(r)})
    im=images['libInfraFsys.so']
    for requested in [0x81,0xa1,0,0x9f]:
        u,m=machine(im,[(0x2e880,0x2e8dc,'generated handle lookup')]);m(0xb4000,4096)
        table=BASE;drive=BASE+0x1000;file=BASE+0x4000;sentinel=drive+0x1c10
        u.mem_write(0xb4fa8,struct.pack('<Q',table));u.mem_write(table,struct.pack('<Q',drive));u.mem_write(drive+0x1c18,struct.pack('<Q',file))
        u.mem_write(file+8,struct.pack('<Q',sentinel));u.mem_write(file+0x20,struct.pack('<Q',0x81));u.reg_write(REG[0],requested)
        execute(u,0x2e880,{})
        assert u.reg_read(REG[0])==(file if requested==0x81 else 0)
        rows.append({'case':'handle-full-generation-not-just-drive-or-slot','requested':hex(requested),'matched':requested==0x81})
    # Translator does not carry MPR's cookie into the core close parameters.
    u,m=machine(im,[(0x2f658,0x2f6b0,'V2 translation prefix')]);fp=BASE+0x10000;q=BASE
    u.reg_write(UC_ARM64_REG_X29,fp);u.reg_write(UC_ARM64_REG_X19,q)
    u.mem_write(q,bytes(0x508));u.mem_write(q+0x500,struct.pack('<Q',0x12345678))
    def zero(uc,args):uc.mem_write(args[0],bytes(args[2]));return args[0]
    def copy(uc,args):uc.mem_write(args[0],bytes(uc.mem_read(args[1],args[2])));return args[0]
    execute_slice(u,0x2f658,0x2f6b0,{0x21fa0:zero,0x20f50:copy})
    assert bytes(u.mem_read(fp+0x58+0x430,16))==bytes(16)
    rows.append({'case':'V2-core-completion-fields-are-zero-not-original-cookie','request_cookie':hex(0x12345678),'core_fields_hex':bytes(u.mem_read(fp+0x58+0x430,16)).hex()})
    for active in [0,1]:
      for fs in [0,0x10]:
       for status in [0,0x80000030]:
        u,m=machine(im,[(0x2c9e0,0x2ca64,'real close dispatcher')]);f=BASE;drive=BASE+0x2000;link=BASE+0x5000
        u.mem_write(f,struct.pack('<QQI',link,link,active));u.mem_write(f+0x18,struct.pack('<QQ',drive,0x81));u.mem_write(f+0x40,struct.pack('<Q',0x1234))
        u.mem_write(drive+0x17c0,struct.pack('<H',fs));u.mem_write(drive+0x1c20,struct.pack('<Q',link));u.reg_write(REG[0],f)
        cs=execute(u,0x2c9e0,{0x24490:lambda *_:status,0x23800:lambda *_:status})
        assert len(cs)==active and u.reg_read(REG[0])==(status if active else 0)
        if active:assert cs[0]['address']==hex(0x24490 if fs==0x10 else 0x23800)
        assert bytes(u.mem_read(f+0x18,16))==bytes(16) and bytes(u.mem_read(f+0x40,8))==bytes(8)
        rows.append({'case':'underlying-close-not-common-zero-return','active':active,'filesystem':fs,'close_status':hex(status),'underlying_call_count':len(cs),'slot_cleared_even_on_error':True,'calls':cs})
    for retained in [1,2]:
      for requested in [0,1]:
        # Exact lock-return/deferred-close instructions; execute only the chosen
        # branch slice, stop before unrelated epilogue by using boundary hooks.
        u,m=machine(im,[(0x30494,0x304d4,'deferred-close tail')]);f=BASE
        u.mem_write(f+0x470,struct.pack('<I',retained));u.mem_write(f+0x10,struct.pack('<I',1|(0x200 if requested else 0)))
        u.reg_write(UC_ARM64_REG_X19,f);u.reg_write(UC_ARM64_REG_X20,0);u.reg_write(UC_ARM64_REG_X24,1)
        # Branch to 0x303cc means "no close requested". Stop there without
        # pretending that the rest of the function or external I/O was modeled.
        from unicorn import UC_HOOK_CODE
        def stop(uc,a,s,data):
            if a in (0x303cc,0x303b8):uc.emu_stop()
        m(0x30300,256);h=u.hook_add(UC_HOOK_CODE,stop)
        closed=[]
        def real(uc,args):closed.append(args[0]);return 0
        try:cs=execute(u,0x30494,{0x2c9e0:real})
        except AssertionError:assert u.reg_read(UC_ARM64_REG_PC) in (0x303cc,0x303b8)
        u.hook_del(h)
        assert bool(closed)==bool(requested and retained==1)
        assert struct.unpack('<I',u.mem_read(f+0x470,4))[0]==retained-1
        rows.append({'case':'deferred-close-after-lock-drains','lock_count_before':retained,'close_requested':bool(requested),'underlying_close':bool(closed)})
    # AV snippets retain raw file-offset coordinates, rather than an ELF VA.
    from capstone import Cs,CS_ARCH_ARM64,CS_MODE_ARM
    md=Cs(CS_ARCH_ARM64,CS_MODE_ARM);av=AV.read_bytes()
    for a,b,why in [(0x1fb2ec,0x1fb308,'Stage input copies ID from request+8 to stage+1c8'),(0x1f3438,0x1f3440,'Stage context ID copy'),(0x2035b0,0x2035d0,'still-info wire ID neighborhood')]:
        ranges.append({'source':'av-cam.bin','file_offset':hex(a),'end_exclusive':hex(b),'scope':why,
                      'sha256':hashlib.sha256(av[a:b]).hexdigest(),'instructions':[{'address':hex(i.address),'mnemonic':i.mnemonic,'operands':i.op_str} for i in md.disasm(av[a:b],a)]})
    output={'schema':1,'camera_accessed':False,'network_accessed':False,'installed':False,
      'source_sha256':{**PINS,'av-cam.bin':hashlib.sha256(av).hexdigest()},'ranges':ranges,'original_instruction_cases':rows,
      'conclusions':{'AP_RecordStill_dynamic_scope_to_record_to_cookie':'source-backed join; no installed trampoline',
       'actual_close':'underlying PrFILE/TNtfs close only; fsync and inactive-helper success excluded; deferred close required',
       'shutter_request':'earliest ID assignment before ID-bearing imaging command is an arming candidate; physical shutter edge and AV command ingress not fully joined',
       'safe_worker_enqueue':False,'remaining':['Verify AP imaging command ID through AV ingress before treating pre-request preset freeze as proven for every drive mode.',
       'Implement validated process-local scoped hooks with FIFO/worker lock and reentrancy/lifetime handling.',
       'Capture owned encoded bytes from same FSYS open, or prove another stable storage identity; late pathname hash is insufficient.',
       'Map Sony filesystem to worker storage, and validate resource/idle scheduling.']}}
    (ROOT/'research/evidence/capture-identity.json').write_text(json.dumps(output,indent=2)+'\n')
    print(f'{len(rows)} original instruction cases; {len(ranges)} pinned ranges; no camera access')
if __name__=='__main__':main()
