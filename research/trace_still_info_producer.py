"""Offline source-bound av-cam still-info producer and selected descriptor writers.

Raw AArch64/Capstone and isolated Unicorn evidence, not REA decompilation.
No transport, camera, installation, native target execution or codec invocation.
"""
import hashlib
import itertools
import json
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import *
from map_ap_interfaces import Image, LIB

ROOT=Path(__file__).resolve().parents[1]
AV=ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha3_unpacked/av-cam.bin'
EXPECTED='90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096'
BASE=0xffffffc001000000
NORMAL=0x01000000
RANGES=[
 ('ORIL entry stubs',0x58,0x1a8,'header-referenced entry instructions'),
 ('message send import slot',0x2ef9c,0x2efb0,'entire indirect import wrapper'),
 ('message allocation and metadata import slots',0x2f028,0x2f0b4,'bounded contiguous wrappers'),
 ('JPEG descriptor selector and four-word updater',0x1f4320,0x1f43ac,'entire functions'),
 ('HEIF descriptor updater',0x1f490c,0x1f4990,'entire function'),
 ('capture resource Encode',0x1f9d8c,0x1f9e90,'entire function'),
 ('capture stage notify routers',0x1fd6c0,0x1fd9f0,'bounded sequence dispatch functions'),
 ('HEIF PostEncode descriptor population',0x2002f4,0x200444,'selected prefix and first descriptor write'),
 ('Stage_JpgEncode_Main',0x200c04,0x200e68,'entire entry and unwind tail'),
 ('JPEG PrepareEncode descriptor materialization',0x2010dc,0x201170,'selected output resource descriptor copy'),
 ('JPEG ExecEncode argument capture',0x201340,0x2013e8,'bounded prefix'),
 ('JPEG ExecEncode submit wait and output',0x201580,0x20172c,'selected submission through return'),
 ('JPEG PostEncode descriptor population',0x20172c,0x201a08,'bounded prefix through descriptor writes'),
 ('Stage_NotifyStillInfo',0x203510,0x203730,'entire function; next entry'),
 ('JPEG callback wait wrapper',0x2077d0,0x20785c,'entire function'),
 ('JPEG callback',0x207a74,0x207acc,'entire function'),
 ('codec Encode API wrapper',0x2a58f0,0x2a59cc,'entire entry'),
]


def ident(path):
 b=path.read_bytes();return {'path':str(path.relative_to(ROOT)),'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}


def raw_regions(b):
 md=Cs(CS_ARCH_ARM64,CS_MODE_ARM);out=[]
 for name,lo,hi,scope in RANGES:
  raw=b[lo:hi];ins=list(md.disasm(raw,BASE+lo))
  assert sum(x.size for x in ins)==hi-lo,(name,hex(lo))
  out.append({'source':'av-cam','name':name,'file_offset':hex(lo),'end_file_offset_exclusive':hex(hi),
   'linked_va':hex(BASE+lo),'end_linked_va_exclusive':hex(BASE+hi),'isa':'AArch64 little endian',
   'scope':scope,'sha256':hashlib.sha256(raw).hexdigest(),
   'instructions':[{'linked_va':hex(i.address),'file_offset':hex(i.address-BASE),
                    'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str} for i in ins]})
 return out


def raw_machine(b):
 uc=Uc(UC_ARCH_ARM64,UC_MODE_ARM)
 uc.mem_map(NORMAL,(len(b)+4095)&~4095);uc.mem_write(NORMAL,b)
 uc.mem_map(0x10000000,0x40000)
 uc.reg_write(UC_ARM64_REG_SP,0x1003f000);uc.reg_write(UC_ARM64_REG_LR,0x1003ff00)
 return uc


def ret(uc,value=0):
 uc.reg_write(UC_ARM64_REG_X0,value);uc.reg_write(UC_ARM64_REG_PC,uc.reg_read(UC_ARM64_REG_LR))


def test_sender(b,ap):
 """Actual sender plus actual AP selected transform for all nine format pairs."""
 uc=raw_machine(b);ctx=0x10000000;wrapper=0x10002000;wire=0x10004000
 dest=0x10006000;frame=0x10008000;calls=[];sent=[];scenario={}
 def hook(u,a,s,_):
  a-=NORMAL
  if a==0xf00:
   x0,x1,x2=[u.reg_read(r) for r in (UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2)]
   assert x2==0x364;u.mem_write(x0,bytes(u.mem_read(x1,x2)));calls.append(['memcpy',hex(x1-ctx),hex(x0-wire),x2]);ret(u,x0)
  elif a==0x2f028:
   args=[u.reg_read(r) for r in (UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X3)]
   assert args[0]==0x80020002 and args[2:]==[0x384,1]
   calls.append(['allocate',args[0],args[2],args[3]])
   u.mem_write(args[1],struct.pack('<Q',wire));ret(u,scenario.get('alloc',0))
  elif a==0x2f0a0:
   assert u.reg_read(UC_ARM64_REG_X0)==wire and u.reg_read(UC_ARM64_REG_X1)==0x90030001
   calls.append(['set_source',0x90030001]);ret(u)
  elif a==0x2ef9c:
   assert u.reg_read(UC_ARM64_REG_X0)==0x80020002 and u.reg_read(UC_ARM64_REG_X1)==wire
   sent.append(bytes(u.mem_read(wire,0x384)));calls.append(['send',0x80020002]);ret(u,scenario.get('send',0))
  elif a==0x2f03c:
   assert u.reg_read(UC_ARM64_REG_X1)==wire;calls.append(['free']);ret(u)
  elif a in (0x1ffae4,0x206fbc,0x207030,0x20710c,0x2f4d8,0x20b5a4):ret(u)
 uc.hook_add(UC_HOOK_CODE,hook)
 # AP transformation isolated at original ELF addresses with host memcpy only.
 au=Uc(UC_ARCH_ARM64,UC_MODE_ARM)
 for start,length in [(0x3533000,0x1000),(0x165d000,0x1000),(0x3682000,0x1000),(0x4c7d000,0x1000)]:
  au.mem_map(start,length);au.mem_write(start,ap.bytes_at(start,length))
 au.mem_map(0x10000000,0x40000)
 def aphook(u,a,s,_):
  if a==0x165d3e0:
   d,p,n=[u.reg_read(r) for r in (UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2)]
   assert n<=0x150;u.mem_write(d,bytes(u.mem_read(p,n)));ret(u,d)
  elif a==0x36824f0:ret(u)
 au.hook_add(UC_HOOK_CODE,aphook)
 formats={1:(0,0xc0,0xc),2:(2,0x150,0xcc),4:(1,0xe0,0x21c)};cases=[]
 modes=[{'types':x} for x in itertools.product(formats,repeat=2)]
 modes += [{'types':(1,4),'send':5},{'types':(1,4),'alloc':7},{'types':(2,2),'quiet':1}]
 for scenario in modes:
  body=bytearray((i*37+19)&255 for i in range(0x364))
  for i,t in enumerate(scenario['types']):struct.pack_into('<I',body,0xa4+i*0x160,t)
  uc.mem_write(ctx,bytes(0x1000));uc.mem_write(ctx+0x440,bytes(body));uc.mem_write(wrapper,struct.pack('<Q',ctx))
  for off,value in [(0x7d4,0x12345678),(0x7bc,0x34567890),(0x9c,0x55667788)]:uc.mem_write(ctx+off,struct.pack('<I',value))
  uc.mem_write(ctx+0x7da,bytes([scenario.get('quiet',0)]))
  for i,off in enumerate((0x7c2,0x7c4,0x7c6,0x7c8,0x7d0)):uc.mem_write(ctx+off,struct.pack('<H',0xa100+i))
  uc.mem_write(wire,bytes([0xa5])*0x384);calls.clear();sent.clear()
  uc.reg_write(UC_ARM64_REG_X0,wrapper);uc.reg_write(UC_ARM64_REG_SP,0x1003f000);uc.reg_write(UC_ARM64_REG_LR,0x1003ff00)
  uc.emu_start(NORMAL+0x203510,0x1003ff00,count=1000)
  expectedret=2 if scenario.get('alloc',0) or scenario.get('send',0) else 0
  assert uc.reg_read(UC_ARM64_REG_X0)==expectedret
  if scenario.get('alloc',0):assert not sent and not any(c[0]=='free' for c in calls)
  else:
   assert len(sent)==1;msg=sent[0]
   assert msg[0]==4 and struct.unpack_from('<H',msg,2)[0]==0x1006
   assert struct.unpack_from('<4I',msg,4)==(0x12345678,0xf000,0x34567890,0x55667788)
   assert msg[0x14:0x378]==body
   assert struct.unpack_from('<5H',msg,0x378)==tuple(0xa100+i for i in range(5))
   assert any(c[0]=='free' for c in calls)==bool(scenario.get('send',0))
   if not scenario.get('send',0):
    au.mem_write(wire,msg);au.mem_write(dest,bytes(0x68c));au.mem_write(frame,bytes(0x1000))
    for r,v in [(UC_ARM64_REG_X0,dest),(UC_ARM64_REG_X19,wire),(UC_ARM64_REG_X29,frame),
                (UC_ARM64_REG_X26,frame+0xb0),(UC_ARM64_REG_X27,frame+0xc0),(UC_ARM64_REG_X21,0x4c7d070),
                (UC_ARM64_REG_SP,frame+0x800)]:au.reg_write(r,v)
    au.emu_start(0x3533214,0x35332f8,count=1024)
    for i,t in enumerate(scenario['types']):
     local,n,off=formats[t];out=dest+i*0x2fc;src=0xb8+i*0x160
     assert struct.unpack('<I',au.mem_read(out,4))[0]==local
     assert bytes(au.mem_read(out+off,n))==msg[src+0x10:src+0x10+n]
    assert bytes(au.mem_read(dest+0x5f8,0x90))==msg[0x24:0xb4]
  cases.append({'scenario':scenario,'return':expectedret,'operations':list(calls),'actual_AP_transform':not scenario.get('send',0) and not scenario.get('alloc',0)})
 return cases


def test_area_writers(b):
 uc=raw_machine(b);acc=0x10000000;cases=[]
 for name,start,fmt,slots in [('JPEG',0x1f4320,1,[(1,0x144,0xa4),(0x10,0x2a4,0x204)]),
                              ('HEIF',0x1f490c,4,[(None,0x154,0xa4),(None,0x2b4,0x204)])]:
  for selector,off,typeoff in slots:
   for mask in range(16):
    uc.mem_write(acc,bytes([0x55])*0x400);uc.mem_write(acc+0xa4,struct.pack('<I',0));uc.mem_write(acc+0x204,struct.pack('<I',0))
    uc.mem_write(acc+typeoff,struct.pack('<I',fmt));before=bytes(uc.mem_read(acc,0x400))
    args=[0x12345678,0x4d4,0x1000,0xabcdef];args=[0xffffffff if mask>>i&1 else v for i,v in enumerate(args)]
    regvals=[acc,selector,*args] if name=='JPEG' else [acc,*args]
    for r,v in zip((UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X3,UC_ARM64_REG_X4,UC_ARM64_REG_X5),regvals):uc.reg_write(r,v)
    uc.reg_write(UC_ARM64_REG_LR,0x1003ff00);uc.emu_start(NORMAL+start,0x1003ff00,count=200)
    expected=bytearray(before)
    for i,p in enumerate((0,0xc,4,8)):
     if args[i]!=0xffffffff:struct.pack_into('<I',expected,off+p,args[i])
    assert bytes(uc.mem_read(acc,0x400))==expected
    cases.append({'format':name,'slot_type_offset':hex(typeoff),'area_offset':hex(off),'preserve_mask':mask})
 return cases


def test_callback_and_post(b):
 uc=raw_machine(b);ctx=0x10000000;event=0x10002000;output=0x10003000;cases=[];signals=[]
 def hook(u,a,s,_):
  a-=NORMAL
  if a==0x20b6b4:ret(u)
  elif a==0x2e980:
   signals.append([u.reg_read(UC_ARM64_REG_X0),u.reg_read(UC_ARM64_REG_X1)]);ret(u)
 uc.hook_add(UC_HOOK_CODE,hook)
 for status,size in [(0,12345),(7,0),(0,0x76543210)]:
  uc.mem_write(event,struct.pack('<3I',0x40,0xaaaaaaaa,0xbbbbbbbb));signals.clear()
  for r,v in [(UC_ARM64_REG_X0,status),(UC_ARM64_REG_X1,size),(UC_ARM64_REG_X2,event),(UC_ARM64_REG_LR,0x1003ff00)]:uc.reg_write(r,v)
  uc.emu_start(NORMAL+0x207a74,0x1003ff00,count=100)
  assert bytes(uc.mem_read(event,12))==struct.pack('<3I',0x40,status,size)
  assert signals==[[0x2e,0x40]]
  cases.append({'kind':'actual callback','status_input':status,'length_input':size,'event':[0x2e,0x40]})
 for slot,area,typeoff in [(1,0x144,0xa4),(0x10,0x2a4,0x204)]:
  for prefix in (0,0x1000):
   # Actual ExecEncode final output stores, then actual JPEG PostEncode branch.
   uc.mem_write(ctx,bytes(0x1000));uc.mem_write(ctx+0x440+typeoff,struct.pack('<I',1))
   uc.mem_write(output,struct.pack('<5I',0x4d4,0x11223344,0,0,prefix))
   sp=0x1003e000;uc.mem_write(sp+0xd8,struct.pack('<I',12345))
   lengthptr=event+0x20;countptr=event+0x24
   for r,v in [(UC_ARM64_REG_SP,sp),(UC_ARM64_REG_X21,output),(UC_ARM64_REG_X22,lengthptr),(UC_ARM64_REG_X19,countptr),(UC_ARM64_REG_X24,0)]:uc.reg_write(r,v)
   uc.emu_start(NORMAL+0x2016e8,NORMAL+0x201704,count=30)
   length=struct.unpack('<I',uc.mem_read(lengthptr,4))[0];assert length==12345+prefix
   for r,v in [(UC_ARM64_REG_X21,ctx),(UC_ARM64_REG_X23,output),(UC_ARM64_REG_X26,ctx+0x440),
               (UC_ARM64_REG_X24,slot),(UC_ARM64_REG_X25,length),(UC_ARM64_REG_X19,0)]:uc.reg_write(r,v)
   uc.emu_start(NORMAL+0x201914,NORMAL+0x2019fc,count=100)
   assert bytes(uc.mem_read(ctx+0x440+area,16))==struct.pack('<4I',0x11223344,0,length,0x4d4)
   cases.append({'kind':'actual result store plus selected PostEncode branch','slot':slot,'codec_length':12345,'prefix':prefix,'area_words':[0x11223344,0,length,0x4d4]})
 return cases


def main():
 b=AV.read_bytes();assert hashlib.sha256(b).hexdigest()==EXPECTED
 ap=Image(LIB/'appFw.so');assert ident(ap.path)['sha256']=='2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
 assert b[8:16]==b'ORIL\x05\0\0\0'
 header=[{'header_file_offset':hex(o),'pointer':hex(struct.unpack_from('<Q',b,o)[0]),
          'target_file_offset':hex(struct.unpack_from('<Q',b,o)[0]-BASE)} for o in (0,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x48,0x50)]
 assert all(0x58<=int(x['target_file_offset'],16)<0x1a8 for x in header)
 assert struct.unpack_from('<Q',b,0x3777b8)[0]==BASE+0x1f8dc0
 assert b[0x32a4a0:0x32a4a0+len(b'N6Camera2MP7Capture11CapRsrc_VFXE')]==b'N6Camera2MP7Capture11CapRsrc_VFXE'
 regions=raw_regions(b)
 callers=[]
 for a in range(0,0x420000,4):
  w=struct.unpack_from('<I',b,a)[0]
  if w&0x7c000000==0x14000000:
   n=w&0x3ffffff;n=n-0x4000000 if n&0x2000000 else n
   if a+n*4==0x203510:callers.append(hex(a))
 assert callers==['0x1fd71c','0x1fd848','0x1fd918','0x1fd98c']
 sender=test_sender(b,ap);writers=test_area_writers(b);callback=test_callback_and_post(b)
 out={'schema_version':1,'evidence_class':'ordinary offline raw/ELF instruction and isolated Unicorn evidence; not REA Evidence',
  'sources':{'av-cam':ident(AV),'appFw':ident(ap.path)},
  'address_model':{'av-cam':{'container':'ORIL version5 raw AArch64 little endian','file_offset_to_linked_va_add':hex(BASE),
    'header_pointers':header,'independent_absolute_table':{'file_offset':'0x3777a8','words':[hex(x) for x in struct.unpack_from('<4Q',b,0x3777a8)],'RTTI_name_file_offset':'0x32a4a0','RTTI_name':'N6Camera2MP7Capture11CapRsrc_VFXE'},
    'runtime_mapping_observed':False,'host_emulation_base':hex(NORMAL),'host_normalization':'PC relative instructions retain original deltas; tested paths do not traverse absolute linked pointer tables; no firmware file edits'},
   'appFw':'ELF VA mapped through original PT_LOAD; prior ELF coordinate rules unchanged'},
  'regions':regions,'notify_direct_callers':{'bounded_scan_file_range':['0x0','0x420000'],'sites':callers},
  'producer':{'entry_file_offset':'0x203510','context':'x21 = [entry x0]','message_bytes':900,'kind':4,'tag':'0x1006',
   'destination':'0x80020002','source_argument':'0x90030001','body_copy':{'capture_context_offset':'0x440','wire_offset':'0x14','bytes':868,'call_file_offset':'0x203618'},
   'header_word_sources':{'wire+4':'context+0x7d4','wire+8':'constant0xf000','wire+c':'context+0x7bc','wire+10':'context+0x9c'},
   'tail_halfword_sources':{'wire+378':'context+0x7c2','wire+37a':'context+0x7c4','wire+37c':'context+0x7c6','wire+37e':'context+0x7c8','wire+380':'context+0x7d0'},
   'roles':'allocation/source-metadata/send roles inferred from arguments, control flow and matching AP receiver; wrappers dispatch through imported function table, not traced into OS',
   'send_result':'return zero => sender returns0 after local invalidation; nonzero => free and return2; allocation failure returns2; no receiver completion established'},
  'descriptor_population':{'wire_unions':['0xb8','0x218'],'accumulator_unions':['0xa4','0x204'],'capture_context_unions':['0x4e4','0x644'],
   'union_stride':'0x160','union_descriptor_offset':'0x10','JPEG_area_accumulator_offsets':['0x144','0x2a4'],'JPEG_area_descriptor_offset':'0x90',
   'HEIF_area_accumulator_offsets':['0x154','0x2b4'],'HEIF_area_descriptor_offset':'0xa0',
   'four_word_writer':{'file_offset':'0x1f435c','argument_to_field':{'w2':'+0','w3':'+c','w4':'+4','w5':'+8'},'preserve_sentinel':'0xffffffff'},
   'JPEG_selected_stage_path':['0x200c04 Stage_JpgEncode_Main','0x200d50 ->0x201340 ExecEncode','0x2015f4 ->0x1f9d8c resource Encode ->0x2a58f0 codec wrapper',
    'callback0x207a74 stores status-input w0 / length-input w1 at userdata+4/+8, signals0x2e980','0x201630 ->0x2077d0 event wait wrapper',
    '0x2016e8 reads callback length, adds outputdescriptor+0x10, writes output argument x6','0x200e10 loads outputs;0x200e38 ->0x20172c PostEncode',
    'mode0/context+7dc==0:0x201944 ->0x1f4320 ->0x1f435c'],
   'JPEG_area_selected_values':['outputdescriptor+4','0','callback length + outputdescriptor+0x10','outputdescriptor+0'],
   'JPEG_descriptor_prepare':'0x2010dc ->0x2097fc output resources;0x201154 copies stack+18 qword to outputdescriptor+8;0x201160/168 copies stack+20 to outputdescriptor+10',
   'HEIF_selected_population':'0x20043c ->0x1f490c writes four words into every type4 slot from output descriptor words and w24; full codec callback origin not traced in this report',
   'JPEG_diagnostic':'file0x4308b6: streamLength / targetLength / codecLength; file0x4309ad StillMP_Stage_EncodeJpg.cpp; file0x4309c9 Stage_JpgEncode_Main'},
  'validation':{'method':'isolated original AArch64 instructions on host Unicorn; invented buffers; imported operations, logs and transport intercepted',
   'sender_cases':sender,'area_writer_cases':writers,'callback_and_post_cases':callback,
   'case_count':len(sender)+len(writers)+len(callback),'producer_to_actual_AP_transform_cases':sum(c['actual_AP_transform'] for c in sender)},
  'prior_evidence':ident(ROOT/'research/evidence/encoder-save-link.json'),
  'limits':['No runtime capture, camera, hardware, network or transport used.',
   'Linked base supported by header and independent absolute tables plus PC-relative code/string references; actual runtime mapping unobserved.',
   'Callback inputs are source-local values; CP encoder response scalar has not been equated to these callback values.',
   'No physical pixel allocation identity between MLUT processing and this encoder input has been established.',
   'Host cases validate local copying, header construction, sentinel updates and selected result arithmetic; no encoding, image quality, scheduling, DMA lifetime or end-to-end saved-image behavior tested.',
   'First/second unions are two descriptor slots; do not assume frame indices or that every capture runs each stage.',
   'HEIF capture stage result and separate host MC encoder route remain distinct until an actual correspondence is recovered.']}
 p=ROOT/'research/evidence/still-info-producer.json';p.write_text(json.dumps(out,indent=2)+'\n')
 print(json.dumps({'evidence':str(p.relative_to(ROOT)),'regions':len(regions),'instructions':sum(len(r['instructions']) for r in regions),
                   'cases':out['validation']['case_count'],'AP_transform_cases':out['validation']['producer_to_actual_AP_transform_cases']}))

if __name__=='__main__':main()
