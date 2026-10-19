"""Pinned original firmware callback/menu evidence and isolated instruction tests.

Read-only extracted files. No camera, USB, mounted media, target process or I/O.
All external calls in Unicorn are synthetic, recorded stubs.
"""
import hashlib
import json
import struct
from pathlib import Path
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import *
from map_ap_interfaces import Image, LIB

ROOT=Path(__file__).resolve().parents[1]
PINS={'appFw.so':'2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b',
      'libmpr.so':'4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9',
      'libInfraFsys.so':'33451a884b65f946ac8aca44b620cd3cec677d05752c52f2ff0e212a01824bfa'}
REGIONS={
 'libmpr.so':[(0xe81ac,0xe8284,'filesystem callback queues owned context/status/size metric'),
              (0xf1494,0xf183c,'queued write finished worker; frees context'),
              (0x2c8aa8,0x2c8ab0,'SetCallbackFunc no-op')],
 'libInfraFsys.so':[(0x32eac,0x32fd4,'async V2 request enqueue'),
              (0x2f7e4,0x2f998,'V2 async worker and IO queue callback registration'),
              (0x2d1d8,0x2d26c,'IO completion aggregates status and invokes original callback'),
              (0x2f5ac,0x2f7e4,'V2 request translation'),
              (0x2ec58,0x2f5ac,'write core; explicit close/flush error aggregation'),
              (0x2e964,0x2ea0c,'close or fsync branch'),
              (0x2c9e0,0x2ca64,'actual fclose dispatch')],
 'appFw.so':[(0x356bd70,0x356bf2c,'singleton registration and identity checked unregistration'),
              (0x3573c2c,0x3573d78,'0x58-byte callback conversion and dispatch'),
              (0x3554d94,0x3554e00,'raw error conversion'),
              (0x34d628c,0x34d62f8,'delegate registration instrumentation wrappers'),
              (0x34c569c,0x34c57b0,'bounded delegate neighborhood; not asserted complete'),
              (0x1794860,0x17949ec,'send_key builds 32-byte message to 0x5503e0'),
              (0x1795208,0x1795250,'key-code remapping helper'),
              (0x22aea04,0x22aeb08,'histogram controller enter-key state machine')]
}
REG=[UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X3,UC_ARM64_REG_X4,UC_ARM64_REG_X5,UC_ARM64_REG_X6]
BASE=0x10000000; STOP=BASE+0x1f000

def machine(im,ranges):
    u=Uc(UC_ARCH_ARM64,UC_MODE_ARM);mapped=set()
    def map_(a,n):
        for p in range(a&-4096,(a+n+4095)&-4096,4096):
            if p not in mapped:u.mem_map(p,4096);mapped.add(p)
    for a,b,_ in ranges:
        map_(a,b-a);u.mem_write(a,im.bytes_at(a,b-a))
    map_(BASE,0x20000);u.reg_write(UC_ARM64_REG_SP,BASE+0x1e000);u.reg_write(UC_ARM64_REG_LR,STOP)
    return u,map_

def execute(u,start,stubs):
    calls=[]
    map_stubs(u,stubs)
    def hook(uc,at,size,user):
        if at in stubs:
            args=[uc.reg_read(r) for r in REG];calls.append({'address':hex(at),'arguments':[hex(v) for v in args]})
            value=stubs[at](uc,args)
            if value is not None:uc.reg_write(UC_ARM64_REG_X0,value&0xffffffffffffffff)
            uc.reg_write(UC_ARM64_REG_PC,uc.reg_read(UC_ARM64_REG_LR))
    h=u.hook_add(UC_HOOK_CODE,hook)
    u.emu_start(start,STOP,count=10000);u.hook_del(h)
    assert u.reg_read(UC_ARM64_REG_PC)==STOP
    return calls

def tests(images):
    rows=[];im=images['libInfraFsys.so']
    # Execute the original IO-queue completion, with no stream areas to release.
    for own,io in [(0,0),(0,0x80000010),(0x80000020,0),(0x80000020,0x80000010)]:
        u,m=machine(im,REGIONS['libInfraFsys.so']);ctx=BASE;cb=BASE+0x4000;cookie=0x12345678
        u.mem_write(ctx,bytes(0x5c0));u.mem_write(ctx+0xd0,struct.pack('<QQI',cb,cookie,own));u.mem_write(ctx+0xe4,b'A:/DCIM/100MSDCF/DSC00001.HIF\0')
        captured=[]
        def callback(uc,args):
            captured.append(args[:3]);return 0
        u.reg_write(REG[0],ctx);u.reg_write(REG[1],io)
        cs=execute(u,0x2d1d8,{0x21fd0:lambda *_:0,0x23280:lambda *_:0,cb:callback})
        assert captured==[[cookie,own or io,ctx+0xe4]]
        rows.append({'case':'actual-FSYS-callback-status-precedence','request_status':hex(own),'queue_status':hex(io),'callback_arguments':captured[0],'calls':cs})
    # Execute close/flush dispatcher. A success callback alone cannot select it.
    for retained in [0,1]:
      for fs in [0,0x10]:
       for err in [0,0x80000030]:
        u,m=machine(im,REGIONS['libInfraFsys.so']);file=BASE;drive=BASE+0x4000
        u.mem_write(file,bytes(0x500));u.mem_write(file+0x470,struct.pack('<I',retained));u.mem_write(drive+0x17c0,struct.pack('<H',fs))
        u.reg_write(REG[0],0x81);u.reg_write(REG[1],drive)
        calls=execute(u,0x2e964,{0x23650:lambda *_:file,0x2c9e0:lambda *_:err,0x255d0:lambda *_:err,0x21cf0:lambda *_:err})
        expected=0x2c9e0 if not retained else (0x255d0 if fs==0x10 else 0x21cf0)
        assert any(r['address']==hex(expected) for r in calls);assert u.reg_read(REG[0])==err
        rows.append({'case':'actual-close-versus-fsync','retained_handle':retained,'filesystem_selector':fs,'error':hex(err),'calls':calls})
    # Execute post-close selection preserving earlier write/truncate errors.
    for before,closing in [(0,0),(0,0x80000030),(0x80000010,0),(0x80000010,0x80000030)]:
        u,m=machine(im,[(0x2f144,0x2f180,'post-close status selection')]);u.reg_write(UC_ARM64_REG_X29,BASE+0x10000)
        u.mem_write(BASE+0x10080,struct.pack('<Q',1));u.reg_write(UC_ARM64_REG_X21,before)
        u.reg_write(UC_ARM64_REG_LR,STOP)
        # Stop at slice boundary; only the exact original instruction range runs.
        def stop(uc,a,s,data):
            if a==0x2f180:uc.emu_stop()
        h=u.hook_add(UC_HOOK_CODE,stop)
        calls=execute_slice(u,0x2f144,0x2f180,{0x24890:lambda *_:0,0x2e964:lambda *_:closing,0x25560:lambda *_:0})
        u.hook_del(h);assert u.reg_read(UC_ARM64_REG_X21)==(before or closing)
        rows.append({'case':'original-post-close-error-aggregation','prior':hex(before),'close':hex(closing),'result':hex(u.reg_read(UC_ARM64_REG_X21)),'calls':calls})
    im=images['appFw.so']
    for register,occupied,matching in [(True,False,False),(True,True,False),(False,True,True),(False,True,False)]:
        u,m=machine(im,REGIONS['appFw.so']);m(0x9117000,4096)
        owner=BASE+0x1000;caller=owner if matching else BASE+0x2000
        u.mem_write(0x9117898,struct.pack('<Q',owner if occupied else 0));u.reg_write(REG[0],caller)
        asserted=[]
        def fatal(uc,args):asserted.append(True);uc.emu_stop();return 0
        stubs={0x3513fe0:lambda *_:0,0x352ddd0:lambda *_:0,0x36e9310:lambda *_:0,
               0x36804c8:lambda *_:0,0x165e250:lambda *_:0,0x165ea70:lambda *_:0,0x35aa4b4:fatal}
        # A deliberate Sony assertion is intercepted, never executed or resumed.
        try:cs=execute(u,0x356bd70 if register else 0x356be4c,stubs)
        except AssertionError:
            assert asserted;cs=[]
        expected=(caller if register else 0) if (not occupied if register else matching) else owner
        assert struct.unpack('<Q',u.mem_read(0x9117898,8))[0]==expected
        assert bool(asserted)==(occupied if register else not matching)
        rows.append({'case':'actual-singleton-registration-ownership','register':register,'occupied':occupied,'same_owner':matching,
                     'assertion_intercepted':bool(asserted),'result_slot':hex(expected),'calls':cs})
    for state,armed in [(0,0),(1,0),(1,1),(2,0),(2,1)]:
        u,m=machine(im,REGIONS['appFw.so']);obj=BASE;u.mem_write(obj+0x302,bytes([armed]));u.reg_write(REG[0],obj);u.reg_write(REG[1],state)
        cs=execute(u,0x22aea04,{0x22ae904:lambda *_:0,0x22ae7c8:lambda *_:0})
        flag=u.mem_read(obj+0x302,1)[0];assert flag==(1 if state==0 else 0)
        if state==0:assert struct.unpack('<I',u.mem_read(obj+0x304,4))[0]==50
        else:assert len(cs)==armed
        rows.append({'case':'actual-Sony-enter-key-state-machine','input_state':state,'prior_armed':armed,'new_armed':flag,'calls':cs,
                     'limit':'States 0/1/2 observed; physical edge semantics and scheduler units remain unproven.'})
    # Original callback copies only media/count/flags/status, never a filename.
    for count in [0,1,2]:
      for err in [0,11,12]:
        u,m=machine(im,REGIONS['appFw.so']);m(0x9117000,4096);m(0x4c8e000,4096)
        u.mem_write(0x4c8e470,im.bytes_at(0x4c8e470,12));src=BASE;dest=BASE+0x1000;delegate=BASE+0x3000;vt=BASE+0x4000;cb=BASE+0x5000
        raw=bytearray(0x58);struct.pack_into('<IIII',raw,4,1,3,71,count);struct.pack_into('<I',raw,0x54,err);raw[0x30]=7;raw[0x50]=9;u.mem_write(src,bytes(raw))
        u.mem_write(0x9117898,struct.pack('<Q',delegate));u.mem_write(delegate,struct.pack('<Q',vt));u.mem_write(vt,struct.pack('<Q',cb));u.reg_write(REG[0],src)
        seen=[]
        def receive(uc,args):seen.append(bytes(uc.mem_read(args[1],20)));return 0
        calls=execute(u,0x3573c2c,{0x2dafe04:lambda *_:dest,0x3513fe0:lambda *_:0,0x352ddd0:lambda *_:0,
                                 0x36804c8:lambda *_:0,cb:receive,0x2db0570:lambda *_:0})
        assert len(seen)==1;out=seen[0];assert struct.unpack_from('<III',out)==(3,71,count)
        if count:assert out[12]==7
        if count>1:assert out[13]==9
        expected=im.bytes_at(0x4c8e470,12)[err] if err<=11 else 2
        assert struct.unpack_from('<I',out,16)[0]==expected
        rows.append({'case':'actual-AP-still-finished-dispatch','count':count,'raw_error':err,'mapped_error':expected,'normalized_payload_hex':out.hex(),'calls':calls})
    return rows

def execute_slice(u,start,end,stubs):
    calls=[]
    map_stubs(u,stubs)
    def hook(uc,a,s,data):
        if a in stubs:
            args=[uc.reg_read(r) for r in REG];calls.append({'address':hex(a),'arguments':[hex(v) for v in args]})
            uc.reg_write(REG[0],stubs[a](uc,args));uc.reg_write(UC_ARM64_REG_PC,uc.reg_read(UC_ARM64_REG_LR))
    h=u.hook_add(UC_HOOK_CODE,hook);u.emu_start(start,end,count=1000);u.hook_del(h)
    assert u.reg_read(UC_ARM64_REG_PC)==end
    return calls

def map_stubs(u,stubs):
    for addr in stubs:
        if not any(a<=addr<=b for a,b,_ in u.mem_regions()):
            u.mem_map(addr&-4096,4096)
            u.mem_write(addr,bytes.fromhex('1f2003d5'))

def main():
    images={name:Image(LIB/name) for name in PINS};ranges=[]
    for name,im in images.items():
        assert hashlib.sha256(im.blob).hexdigest()==PINS[name]
        for a,b,why in REGIONS[name]:
            ranges.append({'source':name,'ELF_VA':hex(a),'end_exclusive':hex(b),'scope':why,
                           'sha256':hashlib.sha256(im.bytes_at(a,b-a)).hexdigest(),'instructions':im.disasm(a,b)})
    # Exact enum/value/length/pointer records with relocation, no string-order guess.
    im=images['appFw.so'];enums=[]
    names=['UIBIZ_KEY_TACT_UP','UIBIZ_KEY_TACT_LEFT','UIBIZ_KEY_TACT_DOWN','UIBIZ_KEY_TACT_CUSTOM','UIBIZ_KEY_TACT_MENU','UIBIZ_KEY_TACT_ENTER','UIBIZ_KEY_TACT_RIGHT','UIBIZ_KEY_TACT_CUSTOM2']
    for name in names:
        offset=im.blob.index(name.encode()+b'\0')
        for rel in im.relocations:
            if rel['addend']!=offset:continue
            raw=im.bytes_at(rel['offset']-8,16);value,n,ptr=struct.unpack('<IIQ',raw)
            if n!=len(name)+1 or ptr!=offset:continue
            enums.append({'name':name,'value':value,'record_ELF_VA':hex(rel['offset']-8),'record_hex':raw.hex(),'relocation':rel})
    assert len(enums)==8
    cases=tests(images)
    report={'schema':1,'camera_accessed':False,'network_accessed':False,'firmware_modified':False,
      'sources':{n:{'path':str((LIB/n).relative_to(ROOT)),'sha256':v} for n,v in PINS.items()},
      'address_convention':'Original ELF virtual addresses; REA/Ghidra shared-library addresses add 0x100000; never runtime hook addresses',
      'regions':ranges,'menu_enum_records':enums,'isolated_original_instruction_cases':cases,
      'limits':['Synthetic external calls; no filesystem or complete camera process executed.',
                'UIBIZ names/values do not prove physical button mapping or ux::KeyEvent ABI.',
                'Close path is conditional; successful async callback does not prove fclose was selected.',
                'No full-original request/shutter association, mount mapping or hook installation established.'],
      'script_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    (ROOT/'research/evidence/sony-integration.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'{len(cases)} original-instruction cases; {len(enums)} enum records; {len(ranges)} regions')

if __name__=='__main__':main()
