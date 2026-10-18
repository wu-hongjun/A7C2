"""Offline av-cam HEIF submit/completion/output lifetime evidence.

Capstone and original AArch64 in isolated Unicorn; explicit synthetic IPC/resource
stubs. Does not execute a hardware codec or communicate with any camera.
"""
import hashlib
import itertools
import json
import struct
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
from unicorn import UC_HOOK_CODE
from unicorn.arm64_const import *
from trace_still_info_producer import AV, BASE, NORMAL, ROOT, EXPECTED, raw_machine, ret, ident

REGIONS=[
 ('still-info publication and context invalidation',0x203510,0x203730),
 ('capture HIF codec release stage',0x1fee18,0x1fef4c),
 ('capture HIF main stage',0x1ff7b0,0x1ffa54),
 ('capture HIF prepare',0x1ffb9c,0x1ffee0),
 ('capture HIF execute and wait',0x1ffee0,0x2002f4),
 ('capture HIF post',0x2002f4,0x2006a4),
 ('HEIF primary and auxiliary area routers',0x1f484c,0x1f4990),
 ('grid pointer resolution',0x1f4a84,0x1f4b38),
 ('four word area updater',0x1f435c,0x1f43ac),
 ('native resource frame submit',0x1f8f28,0x1f91bc),
 ('reply identifier allocator',0x1f91bc,0x1f9210),
 ('native MC frame envelope sender',0x1f9210,0x1f931c),
 ('native MC completion wait and parser',0x1f931c,0x1f9560),
 ('local tracker constructor destructor',0x207204,0x2072dc),
 ('tracker handle search',0x207338,0x207388),
 ('tracker output detach',0x2075f8,0x20762c),
 ('resource release wrapper',0x208c5c,0x208ccc),
]
REGS=[UC_ARM64_REG_X0+i for i in range(8)]
# Unicorn's X registers are contiguous through X28; X29/X30 have separate constants.
END=0x1003ff00

def invoke(u,start,args):
 for r,v in zip(REGS,args):u.reg_write(r,v)
 u.reg_write(UC_ARM64_REG_SP,0x1003f000);u.reg_write(UC_ARM64_REG_LR,END)
 u.emu_start(NORMAL+start,END,count=10000)
 assert u.reg_read(UC_ARM64_REG_PC)==END,hex(u.reg_read(UC_ARM64_REG_PC))
 return u.reg_read(UC_ARM64_REG_X0)

def regions(b):
 md=Cs(CS_ARCH_ARM64,CS_MODE_ARM);out=[]
 for name,lo,hi in REGIONS:
  raw=b[lo:hi];ins=list(md.disasm(raw,BASE+lo));assert sum(i.size for i in ins)==hi-lo
  out.append({'name':name,'source':'av-cam','file_offset':hex(lo),'end_file_offset_exclusive':hex(hi),
   'linked_va':hex(BASE+lo),'end_linked_va_exclusive':hex(BASE+hi),'isa':'AArch64 little endian',
   'sha256':hashlib.sha256(raw).hexdigest(),'instructions':[{'file_offset':hex(i.address-BASE),
    'linked_va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}for i in ins]})
 return out

def post_cases(b):
 u=raw_machine(b);ctx=0x10000000;tracker=ctx+0x2000;inp=ctx+0x3000;out=ctx+0x3100;info=ctx+0x3200
 state={};ops=[]
 def hook(u,a,s,_):
  a-=NORMAL;args=[u.reg_read(r)for r in REGS]
  if a in (0x20b5a4,0x20b32c,0x20b6b4):ret(u)
  elif a in (0x207e70,0x207fd8,0x2080c4):ret(u,0x400000)
  elif a in (0x1f2890,0x1f2934,0x1f29b4,0x1f2d68):
   if a in (0x1f2890,0x1f2934):assert args[0]==ctx+0x4100
   ops.append({'kind':'grid_metadata_stub','offset':hex(a),'arguments':[hex(x)for x in args[:5]]});ret(u)
  elif a==0x20bc30:
   ops.append({'kind':'release_service_stub','handle':hex(args[0]),'mode':args[1],'operation':args[2],'tag':hex(args[3])})
   ret(u,state['release_error'] if args[0]==0x11111111 else 0)
 u.hook_add(UC_HOOK_CODE,hook);cases=[]
 for mode,slot,release_error,debug in itertools.product(range(3),(1,0x10),(0,7),(0,1)):
  state['release_error']=release_error;ops.clear();u.mem_write(ctx,bytes(0x1000));u.mem_write(tracker,bytes(0x100))
  invoke(u,0x207204,[tracker])
  # Only new output and an unrelated temporary belong to the tracker. Input is borrowed from capture context.
  u.mem_write(tracker,struct.pack('<IH2xIH2x',0x22222222,2,0x33333333,0));u.mem_write(tracker+0x28,b'\x02')
  u.mem_write(inp,struct.pack('<IIQI',0x101,0x11111111,0x80000000,0x400000))
  outunit=(0x4d0,0x4d2,0x4d3)[mode]
  u.mem_write(out,struct.pack('<IIQI',outunit,0x22222222,0x90000000,0x500000))
  u.mem_write(info,struct.pack('<III',0,2,0x500000));u.mem_write(ctx+0x7da,bytes([debug]))
  for off in (0xa4,0x204):u.mem_write(ctx+0x440+off,struct.pack('<I',4))
  idx=0 if slot==1 else 1
  u.mem_write(ctx+0x7e8,struct.pack('<Q',ctx+0x4000))
  u.mem_write(ctx+0x440+0xa4+0x160*idx+0x54,struct.pack('<I',0x100))
  inputoff=(0x41c+24*idx,0x33c,0x340)[mode];outputoff=(0x424+24*idx,0x364,0x368)[mode]
  u.mem_write(ctx+inputoff,struct.pack('<I',0x11111111));u.mem_write(ctx+outputoff,struct.pack('<I',0xffffffff))
  u.mem_write(0x1003f000,b'\x06')
  invoke(u,0x2002f4,[slot,mode,ctx,tracker,inp,out,info,0x123456])
  areas=[0x144+0x160*idx]if mode==0 else([0x164,0x2c4]if mode==1 else[0x154,0x2b4])
  words=[list(struct.unpack('<4I',u.mem_read(ctx+0x440+off,16)))for off in areas]
  assert words==[[0x22222222,0,0x123456,outunit]]*len(areas),(mode,slot,words)
  input_after=struct.unpack('<I',u.mem_read(ctx+inputoff,4))[0]
  assert input_after==(0x11111111 if release_error else 0xffffffff)
  assert struct.unpack('<I',u.mem_read(ctx+outputoff,4))[0]==0x22222222
  assert struct.unpack('<IH',u.mem_read(tracker,6))==(0xffffffff,0)
  invoke(u,0x207240,[tracker])
  releases=[x for x in ops if x['kind']=='release_service_stub']
  assert [x['handle']for x in releases]==['0x11111111','0x33333333']
  assert [x['mode']for x in releases]==[2,0]
  cases.append({'mode':mode,'slot':hex(slot),'debug':debug,'input_release_rc':release_error,
   'input_context_offset':hex(inputoff),'input_after':hex(input_after),'output_context_offset':hex(outputoff),
   'output_after':'0x22222222','output_unit':hex(outunit),'area_accumulator_offsets':[hex(x)for x in areas],'area_words':words,
   'tracker_detached':True,'operations':list(ops)})
 # Without Post, stage destructor still owns and releases the output.
 for mode in (0,2):
  ops.clear();state['release_error']=0;invoke(u,0x207204,[tracker])
  u.mem_write(tracker,struct.pack('<IH',0x22222222,mode));u.mem_write(tracker+0x28,b'\x01')
  invoke(u,0x207240,[tracker]);assert [x['handle']for x in ops]==['0x22222222']
  cases.append({'scenario':'cleanup_without_post','tracker_release_mode':mode,'operations':list(ops)})
 return cases

def wait_cases(b):
 u=raw_machine(b);obj=0x10000000;out=obj+0x2000;state={};ops=[]
 def hook(u,a,s,_):
  a-=NORMAL;args=[u.reg_read(r)for r in REGS]
  if a in (0x1f7430,0x1f74ac,0x1f8c4c,0x967f0,0x94760,0x94784,0x96804):ret(u)
  elif a==0x1f84c0:ret(u,state['valid'])
  elif a==0x9681c:
   assert args[1]==0x55667788 and args[3]&0xffffffff==0xffffffff
   state['recv']+=1;assert state['recv']<=3;ops.append({'kind':'receive_stub','reply_id':hex(args[1]),'timeout':'0xffffffff'});ret(u)
  elif a==0x94bd0:
   source=0xdeadbeef if state['wrong_source']and state['recv']==1 else 0x8057060a
   u.mem_write(args[1],struct.pack('<I',source));ops.append({'kind':'source_stub','value':hex(source)});ret(u)
  elif a==0x94978:
   assert args[2]==32;u.mem_write(args[1],state['reply']);ops.append({'kind':'copy_reply_stub','bytes':32});ret(u)
 u.hook_add(UC_HOOK_CODE,hook);cases=[]
 for status,length,wrong,valid in itertools.product((0,0x1234),(0,0xabcdef,0x100000005),(False,True),(0,1)):
  reply=bytearray(32);struct.pack_into('<4H',reply,0,0x2003,0x810e,32,1)
  struct.pack_into('<QIQ',reply,8,0x123456789abcdef, status,0) # final size explicitly at +18 below
  struct.pack_into('<Q',reply,0x18,length)
  state.update({'valid':valid,'wrong_source':wrong,'recv':0,'reply':bytes(reply)});ops.clear()
  u.mem_write(out,struct.pack('<I',0xcccccccc))
  result=invoke(u,0x1f931c,[obj,2,0x55667788,out]);value=struct.unpack('<I',u.mem_read(out,4))[0]
  expected=(status<<32) if valid and status else(1 if valid else 0)
  assert result==expected,(result,expected)
  assert value==((length&0xffffffff)if valid and not status else 0)
  assert state['recv']==((2 if wrong else 1)if valid else 0)
  cases.append({'status':hex(status),'es_length_u64':hex(length),'wrong_source_first':wrong,
   'validation_stub':valid,'packed_return':hex(result),'output_length_u32':hex(value),'operations':list(ops)})
 return cases

def envelope_cases(b):
 u=raw_machine(b);obj=0x10000000;body=obj+0x2000;state={};ops=[]
 def hook(u,a,s,_):
  a-=NORMAL;args=[u.reg_read(r)for r in REGS]
  if a==0xd80:
   assert args[2]==88;u.mem_write(args[0],bytes(u.mem_read(args[1],args[2])));ret(u,args[0])
  elif a==0x9479c:
   assert args[2]==96;state['message']=bytes(u.mem_read(args[1],96));ret(u)
  elif a==0x1f84c0:ret(u,1)
  elif a==0x2f078:
   assert args[:2]==[0x2a,5];u.mem_write(args[2],struct.pack('<I',0x55667788));ret(u)
  elif a==0x97058:
   assert args[1]==0x8057060a;ops.append({'kind':'send_stub','destination':hex(args[1]),'message_hex':state['message'].hex()});ret(u,state['send_rc'])
  elif a in (0x94760,0x9702c,0x97040,0x94784,0x94a94,0x1f74ac,0x1f7430):ret(u)
 u.hook_add(UC_HOOK_CODE,hook);cases=[];payload=bytes((i*37+11)&255 for i in range(88));u.mem_write(body,payload)
 for sendrc in (0,7):
  state['send_rc']=sendrc;ops.clear();result=invoke(u,0x1f9210,[obj,0x55667788,0x10e,body])
  assert state['message']==struct.pack('<4H',0x2003,0x10e,96,1)+payload
  assert result==(0 if sendrc==0 else 0xfffffffd)
  cases.append({'send_rc':sendrc,'return_u32':hex(result),'operations':list(ops)})
 # Original frame producer, reply-ID allocator and original envelope sender execute together.
 for mode,quality,flag,gamma,color in itertools.product(range(3),range(4),range(2),range(2),range(2)):
  state['send_rc']=0;ops.clear()
  u.mem_write(obj+0x1b8,struct.pack('<Q',0x123456789abcdef))
  replyout=obj+0x3000
  u.mem_write(0x1003f000,struct.pack('<8Q',0x80001000,640,480,672,512,0x90002000,0x400000,replyout))
  result=invoke(u,0x1f8f28,[obj,2,mode,quality,flag,gamma,color,0x11111111]);msg=state['message'];p=msg[8:]
  assert result==1 and struct.unpack('<I',u.mem_read(replyout,4))[0]==0x55667788
  assert struct.unpack_from('<4H',msg)==(0x2003,0x10e,96,1)
  assert struct.unpack_from('<2Q3I',p)==(0x123456789abcdef,2,0x55667788,mode,quality+0x100)
  assert p[0x1c:0x1f]==bytes([0 if flag else 2,0,0])
  assert struct.unpack_from('<I',p,0x20)[0]==0x11111111
  assert struct.unpack_from('<Q',p,0x28)[0]==0x80001000
  assert struct.unpack_from('<H2B2H',p,0x30)==(0x100,0x58 if gamma else 0x57,0 if color else 1,640,480)
  assert p[0x38:0x44]==bytes(12)
  assert struct.unpack_from('<2H2Q',p,0x44)==(672,512,0x90002000,0x400000)
  cases.append({'scenario':'original_resource_submit','mode':mode,'quality':quality,'flag':flag,'gamma_flag':gamma,
   'color_flag':color,'reply_id':'0x55667788','packed_return':hex(result),'operations':list(ops)})
 return cases

def publication_cases(b):
 u=raw_machine(b);ctx=0x10000000;wrapper=ctx+0x2000;wire=ctx+0x4000;state={};ops=[]
 def hook(u,a,s,_):
  a-=NORMAL;args=[u.reg_read(r)for r in REGS]
  if a==0xf00:
   assert args[2]==0x364;u.mem_write(args[0],bytes(u.mem_read(args[1],args[2])));ret(u,args[0])
  elif a==0x2f028:
   assert args[0]==0x80020002 and args[2:4]==[0x384,1]
   u.mem_write(args[1],struct.pack('<Q',wire));ret(u)
  elif a==0x2f0a0:ret(u)
  elif a==0x2ef9c:
   state['sent']=bytes(u.mem_read(args[1],0x384));ret(u,state['send_rc'])
  elif a==0x2f03c:ops.append('message_free_after_send_failure');ret(u)
  elif a==0x1f40e8:ret(u,state['slots'])
  elif a in (0x1ffae4,0x206fbc,0x207030,0x20710c,0x2f4d8,0x20b5a4):ret(u)
 u.hook_add(UC_HOOK_CODE,hook);cases=[];handles=(0x364,0x368,0x424,0x43c)
 for enabled,slots,sendrc in itertools.product((0,4),(1,0x10,0x11),(0,7)):
  state.update({'slots':slots,'send_rc':sendrc});ops.clear();u.mem_write(ctx,bytes(0x1000))
  u.mem_write(wrapper,struct.pack('<Q',ctx));u.mem_write(ctx+0x84,struct.pack('<I',enabled))
  before=[]
  for idx,off in enumerate(handles):
   handle=0x22222220+idx;before.append(handle);u.mem_write(ctx+off,struct.pack('<I',handle))
  area=struct.pack('<4I',0x22222222,0,0x123456,0x4d0)
  u.mem_write(ctx+0x440+0x144,area);u.mem_write(ctx+0x440+0x2a4,area)
  result=invoke(u,0x203510,[wrapper]);after=[struct.unpack('<I',u.mem_read(ctx+off,4))[0]for off in handles]
  expected=list(before)
  if enabled and not sendrc:
   expected[:2]=[0xffffffff]*2
   if slots&1:expected[2]=0xffffffff
   if slots&0x10:expected[3]=0xffffffff
  assert after==expected and result==(2 if sendrc else 0)
  assert state['sent'][0x14+0x144:0x14+0x154]==area
  assert state['sent'][0x14+0x2a4:0x14+0x2b4]==area
  cases.append({'capture_flag':hex(enabled),'selected_slots':hex(slots),'send_rc':sendrc,'return':result,
   'context_offsets':[hex(o)for o in handles],'before':[hex(x)for x in before],'after':[hex(x)for x in after],
   'published_primary_areas_preserved':True,'operations':list(ops)})
 return cases

def codec_release_cases(b):
 u=raw_machine(b);ctx=0x10000000;wrapper=ctx+0x2000;resource=ctx+0x3000;state={};ops=[]
 def hook(u,a,s,_):
  a-=NORMAL;args=[u.reg_read(r)for r in REGS]
  if a in (0x1fea44,0x207dbc,0x20b5a4,0x20b32c):ret(u)
  elif a==0x2097f0:ret(u,state['index'])
  elif a==0x1f78c0:
   assert args[:2]==[resource,0x1234];ops.append({'kind':'codec_close_stub','resource':hex(args[0]),'channel':hex(args[1])});ret(u,state['close_result'])
 u.hook_add(UC_HOOK_CODE,hook);cases=[]
 for index,present,close_result in itertools.product((0,1),(False,True),(1,0x123400000000)):
  state.update({'index':index,'close_result':close_result});ops.clear();u.mem_write(ctx,bytes(0x1000))
  u.mem_write(wrapper,struct.pack('<Q',ctx));u.mem_write(ctx+0x858+index*8,struct.pack('<Q',resource if present else 0))
  u.mem_write(ctx+0x868+index*4,struct.pack('<I',0x1234))
  u.mem_write(ctx+0x364,struct.pack('<II',0x22222222,0x33333333))
  result=invoke(u,0x1fee18,[wrapper]);after=struct.unpack('<Q',u.mem_read(ctx+0x858+index*8,8))[0]
  channel=struct.unpack('<I',u.mem_read(ctx+0x868+index*4,4))[0]
  assert result==0 and after==0 and channel==(0 if present else 0x1234)
  assert struct.unpack('<II',u.mem_read(ctx+0x364,8))==(0x22222222,0x33333333)
  cases.append({'index':index,'resource_present':present,'packed_close_stub_result':hex(close_result),
   'stage_return':result,'resource_pointer_after':hex(after),'channel_after':hex(channel),
   'output_handles_unchanged':True,'operations':list(ops)})
 return cases

def main():
 b=AV.read_bytes();assert hashlib.sha256(b).hexdigest()==EXPECTED
 output={'schema':1,'method':'Original raw AArch64 Capstone and isolated Unicorn; not REA Evidence',
  'source':ident(AV),'generator':ident(__import__('pathlib').Path(__file__)),
  'helper':ident(ROOT/'research/trace_still_info_producer.py'),
  'mapping':{'offline_linked_base':hex(BASE),'emulation_base':hex(NORMAL),'runtime_mapping_proved':False},
  'regions':regions(b),'post_cases':post_cases(b),'wait_cases':wait_cases(b),'envelope_cases':envelope_cases(b),'publication_cases':publication_cases(b),'codec_release_cases':codec_release_cases(b),
  'findings':{'capture_request':{'category':'0x2003','selector':'0x10e','bytes':96,'count':1,'destination':'0x8057060a'},
   'completion':{'source_filter':'0x8057060a','reply_copy_bytes':32,'status_offset':'0x10','es_length_u64_offset':'0x18','output_length_bits':32},
   'post_handoff':{'input_release_mode':2,'output_tracker_detach':'0x2075f8','mode_output_context_offsets':{'0':'0x424 + 24*slot_index','1':'0x364','2':'0x368'}},
   'publication':{'send_success_required':True,'HEIF_context_clear_flag':'ctx+0x84 bit2','message_destination':'0x80020002','tag':'0x1006'}},
  'limitations':['Every IPC, resource service, grid object operation and codec response is synthetic; no codec is executed.',
   'Raw ORIL linked-base mapping is offline only; absolute linked pointer tables are not traversed.',
   'Final capture output release after still-info publication remains untraced.',
   'Receive failure and general asynchronous cancellation are not tested; successful receives and explicit MC statuses only.',
   'Pixel format, HEVC bitstream validity, hardware synchronization and camera compatibility remain unproved.']}
 output['counts']={'regions':len(output['regions']),'instructions':sum(len(r['instructions'])for r in output['regions']),
  'post_cases':len(output['post_cases']),'wait_cases':len(output['wait_cases']),'envelope_cases':len(output['envelope_cases']),'publication_cases':len(output['publication_cases']),'codec_release_cases':len(output['codec_release_cases'])}
 output['counts']['total_cases']=sum(output['counts'][x]for x in ('post_cases','wait_cases','envelope_cases','publication_cases','codec_release_cases'))
 p=ROOT/'research/evidence/heif-ownership.json';p.write_text(json.dumps(output,indent=2)+'\n')
 print(json.dumps(output['counts']))
if __name__=='__main__':main()
