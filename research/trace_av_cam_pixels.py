"""Offline av-cam capture context -> encoder pixel address and resize handoff.

Retained raw instructions and isolated synthetic host cases. No camera/IPC/codec.
"""
import hashlib
import json
import struct
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
from unicorn import UC_HOOK_CODE
from unicorn.arm64_const import *
from trace_still_info_producer import AV, BASE, NORMAL, ROOT, EXPECTED, raw_machine, ret, ident

REGIONS=[
 ('capture slot index',0x1f4148,0x1f4190),
 ('JPEG PrepareEncode',0x200e68,0x201340),
 ('HEIF normal input selection',0x1ffc34,0x1ffc84),
 ('HEIF materialized descriptor copies',0x1ffda8,0x1ffdd4),
 ('JPEG codec parameter input geometry',0x201340,0x2013e8),
 ('JPEG resource Encode forwarding',0x1f9d8c,0x1f9e90),
 ('resize input selection',0x205b2c,0x205ce0),
 ('resize output input-context handoff',0x2065ec,0x2066d0),
 ('resource ownership tracker constructor',0x207204,0x207240),
 ('resource ownership tracker destructor',0x207240,0x2072dc),
 ('tracker handle search',0x207338,0x207388),
 ('resource output allocator tracker',0x207388,0x207420),
 ('resource tracker detach',0x2075f8,0x20762c),
 ('input handle address lookup wrapper',0x208f3c,0x208fec),
 ('input lookup output allocation materializer',0x2097fc,0x2098d4),
 ('DMM GetUnitAddr transport',0x20cb7c,0x20ce58),
 ('JPEG codec API wrapper',0x2a58f0,0x2a59cc),
 ('JPEG request input setup',0x2acb14,0x2accec),
 ('JPEG pixel address geometry builder',0x2aa3e8,0x2aa760),
 ('CPU alias to hardware address conversion',0x2aeed4,0x2aef50),
]


def regions(b):
 md=Cs(CS_ARCH_ARM64,CS_MODE_ARM);out=[]
 for name,lo,hi in REGIONS:
  raw=b[lo:hi];ins=list(md.disasm(raw,BASE+lo));assert sum(i.size for i in ins)==hi-lo
  out.append({'name':name,'source':'av-cam','file_offset':hex(lo),'end_file_offset_exclusive':hex(hi),
   'linked_va':hex(BASE+lo),'end_linked_va_exclusive':hex(BASE+hi),'isa':'AArch64 little endian',
   'sha256':hashlib.sha256(raw).hexdigest(),
   'instructions':[{'file_offset':hex(i.address-BASE),'linked_va':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str} for i in ins]})
 return out


def pixel_cases(b):
 """Actual slot read, DMM request/parser, materializer, geometry and address builders.

 All DMM replies/allocations are invented. A matched fixture is never evidence
 that a CP processed buffer has this handle or these pixels.
 """
 uc=raw_machine(b);ctx=0x10000000;indesc=0x10002000;outdesc=0x10002100;geom=0x10002200
 tracker=0x10003000;ininfo=0x10004000;outinfo=0x10005000;out_handle=0x10006000
 wrapper=0x10007000;dest=0x10008000;state={};calls=[]
 def hook(u,a,s,_):
  a-=NORMAL
  if a in (0xd80,0xf00):
   d,p,n=[u.reg_read(r) for r in (UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2)]
   assert n<=0x200;u.mem_write(d,bytes(u.mem_read(p,n)));ret(u,d)
  elif a==0x2f67c:
   d,c,n=[u.reg_read(r) for r in (UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2)]
   assert n<=0x200;u.mem_write(d,bytes([c&255])*n);ret(u,d)
  elif a==0x20b6b4:ret(u)
  elif a==0x20b32c:raise AssertionError('unexpected source diagnostic assertion')
  elif a==0x2f438:
   req=u.reg_read(UC_ARM64_REG_X0);blob=bytes(u.mem_read(req,0xe8))
   assert struct.unpack_from('<H',blob,8)[0]==0x2a
   assert struct.unpack_from('<I',blob,0x10)[0]==state['input_handle']
   assert struct.unpack_from('<H',blob,0x1c)[0]==0x55
   calls.append({'kind':'intercepted GetUnitAddr','input_handle':hex(state['input_handle']),
                 'request_bytes':blob.hex(),'category':'0x2a','id':'0x55'})
   # Synthetic successful reply in the exact layout parsed by original wrapper.
   reply=bytearray(0xe8)
   struct.pack_into('<H',reply,8,0x802a);struct.pack_into('<H',reply,0xe,0x55)
   struct.pack_into('<H',reply,0x10,1);struct.pack_into('<Q',reply,0x20,state['input_alias'])
   struct.pack_into('<I',reply,0x28,0x400000)
   u.mem_write(req,bytes(reply));ret(u)
  elif a==0x208dfc:
   unit,refcount,wait,p=[u.reg_read(r) for r in (UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2,UC_ARM64_REG_X3)]
   assert (unit,refcount,wait)==(0x4d4,1,0xffffffff)
   v=bytearray(0xd8);struct.pack_into('<I',v,0xc,0x55abcdef)
   struct.pack_into('<I',v,0x10,0x4d4);struct.pack_into('<Q',v,0x18,0x420000000);struct.pack_into('<I',v,0x20,0x200000)
   u.mem_write(p,bytes(v));calls.append({'kind':'intercepted output allocation','unit_type':hex(unit),'handle':'0x55abcdef'});ret(u)
 uc.hook_add(UC_HOOK_CODE,hook)
 cases=[]
 samples=[(1,0x400000000,0),(0x10,0x410000000,0x10000000),(1,0x510000000,0x110000000),(0x10,0xfe010000,0x3fe010000)]
 for selector,alias,physical in samples:
  state.update(input_handle=0x12345600+selector,input_alias=alias);calls.clear()
  uc.mem_write(ctx,bytes(0x1000));uc.mem_write(indesc,bytes(0x400));uc.mem_write(wrapper,struct.pack('<Q',ctx))
  index=0 if selector==1 else 1
  geometry=struct.pack('<6H',1536,1024,1000,768,16,8)
  uc.mem_write(ctx+0x410+index*0x18,geometry)
  uc.mem_write(ctx+0x41c+index*0x18,struct.pack('<I',state['input_handle']))
  for r,v in [(UC_ARM64_REG_X0,ctx+0x440),(UC_ARM64_REG_X1,selector),(UC_ARM64_REG_LR,0x1003ff00)]:uc.reg_write(r,v)
  uc.emu_start(NORMAL+0x1f4148,0x1003ff00,count=100);assert uc.reg_read(UC_ARM64_REG_X0)==index
  for r,v in [(UC_ARM64_REG_X20,ctx),(UC_ARM64_REG_X22,indesc),(UC_ARM64_REG_X27,geom)]:uc.reg_write(r,v)
  uc.emu_start(NORMAL+0x200f74,NORMAL+0x200fcc,count=100)
  assert bytes(uc.mem_read(geom,12))==geometry
  assert struct.unpack('<I',uc.mem_read(indesc+4,4))[0]==state['input_handle']
  uc.reg_write(UC_ARM64_REG_X0,tracker);uc.reg_write(UC_ARM64_REG_LR,0x1003ff00)
  uc.emu_start(NORMAL+0x207204,0x1003ff00,count=100)
  for r,v in [(UC_ARM64_REG_X0,tracker),(UC_ARM64_REG_X1,state['input_handle']),(UC_ARM64_REG_X2,0x4d4),
              (UC_ARM64_REG_X3,ininfo),(UC_ARM64_REG_X4,outinfo),(UC_ARM64_REG_X5,out_handle),(UC_ARM64_REG_LR,0x1003ff00)]:uc.reg_write(r,v)
  uc.emu_start(NORMAL+0x2097fc,0x1003ff00,count=3000)
  assert uc.reg_read(UC_ARM64_REG_X0)==0
  assert struct.unpack('<Q',uc.mem_read(ininfo+0x18,8))[0]==alias
  assert bytes(uc.mem_read(tracker,8))==struct.pack('<IHH',0x55abcdef,2,0)
  assert bytes(uc.mem_read(tracker+0x28,1))==b'\x01'
  # Execute original post-materializer selected descriptor copy.
  sp=0x10030000;uc.reg_write(UC_ARM64_REG_SP,sp)
  uc.mem_write(sp+0xf0,bytes(uc.mem_read(ininfo+0x18,12)))
  uc.mem_write(sp+0x18,bytes(uc.mem_read(outinfo+0x18,12)))
  for r,v in [(UC_ARM64_REG_X22,indesc),(UC_ARM64_REG_X27,outdesc)]:uc.reg_write(r,v)
  uc.emu_start(NORMAL+0x201144,NORMAL+0x20115c,count=20)
  assert struct.unpack('<Q',uc.mem_read(indesc+8,8))[0]==alias
  # Original JPEG codec parameter builder takes the materialized input address.
  for r,v in [(UC_ARM64_REG_X2,indesc),(UC_ARM64_REG_X3,geom),(UC_ARM64_REG_X4,outdesc),
              (UC_ARM64_REG_X5,outdesc),(UC_ARM64_REG_X29,wrapper),(UC_ARM64_REG_X6,0x10009000)]:uc.reg_write(r,v)
  uc.emu_start(NORMAL+0x20136c,NORMAL+0x2013e8,count=100)
  codec=sp+0x50;assert struct.unpack('<Q',uc.mem_read(codec,8))[0]==alias
  for r,v in [(UC_ARM64_REG_X0,codec),(UC_ARM64_REG_X1,codec+0x28),(UC_ARM64_REG_X2,dest),
              (UC_ARM64_REG_X3,3),(UC_ARM64_REG_X4,0),(UC_ARM64_REG_X5,0),(UC_ARM64_REG_X6,0),(UC_ARM64_REG_X7,0),
              (UC_ARM64_REG_LR,0x1003ff00)]:uc.reg_write(r,v)
  uc.emu_start(NORMAL+0x2aa3e8,0x1003ff00,count=500)
  expected=physical+2*(8*1536+16)
  words=struct.unpack('<2I',uc.mem_read(dest,8))
  assert words==((expected>>24)&0xffffff00,expected&0xffffffff)
  cases.append({'selector':hex(selector),'context_slot_offset':hex(0x41c+index*0x18),
   'synthetic_input_handle':hex(state['input_handle']),'synthetic_input_alias':hex(alias),
   'synthetic_corresponding_physical':hex(physical),'geometry_halfwords':[1536,1024,1000,768,16,8],
   'pixel_byte_offset':2*(8*1536+16),'actual_hardware_address_words':[hex(x) for x in words],
   'output_handle_tracked_separately':'0x55abcdef','intercepted_operations':list(calls)})
 return cases


def resize_cases(b):
 uc=raw_machine(b);ctx=0x10000000;old=0x10002000;new=0x10002100;tracker=0x10003000;calls=[]
 def hook(u,a,s,_):
  if a-NORMAL==0x208c5c:
   args=[u.reg_read(r) for r in (UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2)]
   assert args==[0x11223344,2,0x61003c];calls.append(args);ret(u)
  elif a-NORMAL==0x20b32c:raise AssertionError('unexpected diagnostic')
 uc.hook_add(UC_HOOK_CODE,hook);cases=[]
 for selector in (1,0x10):
  for target in (0,3):
   uc.mem_write(ctx,bytes(0x1000));uc.mem_write(old,struct.pack('<2I',0,0x11223344));uc.mem_write(new,struct.pack('<2I',0x4c7,0x55667788))
   uc.mem_write(tracker,struct.pack('<IHH',0x55667788,2,0)+bytes(0x20)+b'\x01'+bytes(0x40))
   calls.clear()
   for r,v in [(UC_ARM64_REG_X21,ctx),(UC_ARM64_REG_X22,old+4),(UC_ARM64_REG_X24,new),(UC_ARM64_REG_X26,ctx+0x440),
               (UC_ARM64_REG_X25,selector),(UC_ARM64_REG_X23,tracker),(UC_ARM64_REG_X19,target)]:uc.reg_write(r,v)
   uc.emu_start(NORMAL+0x2065ec,NORMAL+0x2066d0,count=500)
   off=0x41c+(0 if selector==1 else 0x18)
   assert struct.unpack('<I',uc.mem_read(ctx+off,4))[0]==0x55667788
   assert bytes(uc.mem_read(tracker,8))==struct.pack('<IHH',0xffffffff,0,0)
   assert len(calls)==1
   cases.append({'selector':hex(selector),'target':target,'released_input':'0x11223344','new_context_slot':hex(off),
                 'transferred_output':'0x55667788','tracker_detached':True})
 return cases


def main():
 b=AV.read_bytes();assert hashlib.sha256(b).hexdigest()==EXPECTED
 rs=regions(b);pc=pixel_cases(b);rc=resize_cases(b)
 maps=[{'file_offset':hex(a),'source_base':hex(x),'reserved':hex(y),'destination_base':hex(z),'length':hex(n)}
       for a in range(0x3320d8,0x332158,0x20) for x,y,z,n in [struct.unpack_from('<4Q',b,a)]]
 out={'schema_version':1,'evidence_class':'ordinary offline raw AArch64 and isolated Unicorn; not REA Evidence',
  'sources':{'av-cam':ident(AV)},'address_model':{'file_offset_to_linked_va_add':hex(BASE),'isa':'AArch64 little endian',
   'runtime_mapping_observed':False,'host_normalized_base':hex(NORMAL),'validation_reference':'research/evidence/still-info-producer.json'},
  'regions':rs,'address_translation_table':maps,
  'input_selection':{'normal_path_condition':'capture context+0x7dc ==0; JPEG target0 or3',
   'slot_index':'1f4148 maps selector1→0 and0x10→1','geometry':'context+0x410+0x18*index, twelve bytes',
   'handle':'context+0x41c+0x18*index','JPEG_refs':['0x200f74','0x200f80','0x200fc8'],
   'HEIF_refs':['0x1ffc50','0x1ffc68','0x1ffc80'],
   'alternate_context_path':'context+7dc==1 uses target-dependent context handles/geometry via absolute dispatch tables; not host tested'},
  'materializer':{'entry':'0x2097fc','input':'existing input handle→208f3c→20cb7c GetUnitAddr category2a/id55, controller61003c',
   'output':'independent outputunitType→207388→208dfc, allocator output registered in tracker',
   'input_descriptor_address':'GetUnitAddr info+0x18 first extent u64→PrepareEncode inputdesc+8',
   'input_descriptor_length':'GetUnitAddr info+0x20 u32→PrepareEncode inputdesc+10',
   'materializer_return':'0 on successful input query/output allocation,2 on failure',
   'ownership_limit':'input query does not insert inputhandle into local tracker; output allocation inserts returnedhandle and mode2; actual DMM ownership is external'},
  'JPEG_codec_input':{'resource':'1f9d8c forwards full parameter pointer,callback anduserdata→2a58f0→2acb14',
   'base_address':'ExecEncode20136c/380 inputdesc+8→codecparams+0',
   'geometry_fields':{'codec+8':'geom+0 u16','codec+c':'geom+2 u16','codec+10':'geom+8 u16','codec+14':'geom+a u16',
    'codec+20':'zero','codec+24':'zero','codec+28':'geom+4 u16','codec+2c':'geom+6 u16'},
   'request_builder':'2acbf0..2acc18 calls2aa3e8(params,params+28,request+90,3,0,0,0,modeflag)',
   'pixel_adjustment':'base+2*((field+24 + field+14)*stride + field+10 + field+20); with current ExecEncode zeros =>base+2*(geom+a*geom+0 + geom+8)',
   'address_normalization':'2aeed4 matches four static range entries, destination=address-source_base+destination_base; writes [(destination>>24)&0xffffff00,destination&0xffffffff]',
   'mode_limit':'2aa3e8 skips direct-address construction when flagw7 bit0 is set; normal direct-address cases tested only'},
  'resize_handoff':{'source':'PostResize at2065f0 reads outputdesc+4; index1f4148;206694 stores context+41c+18*index',
   'old_input_release':'206614 calls208c5c(oldinputhandle,2,61003c)',
   'new_output_detach':'2066cc calls2075f8 tracker search, writesFFFFFFFF andmode0 in matchedentry',
   'meaning':'actual shared context slot establishes local resize-output handle→later JPEG/HEIF input selection; no claim that resize input was MLUT-processed'},
  'validation':{'method':'eight isolated original-instruction host cases; DMM calls/replies, allocations, logs and memcpy intercepted; no codec/IPC/hardware executed',
   'pixel_chain_cases':pc,'resize_handoff_cases':rc,'case_count':len(pc)+len(rc),
   'fixture_identity_warning':'Synthetic same-value buffers validate local dataflow only; matching them never proves a live CP allocation or MLUT-to-encoder correspondence.'},
  'limits':['CP MLUT direct ring pooltoken43f/2d9 and actual DMM handles are distinct concepts; this mapper never equates them.',
   'The capture context fields can be written by multiple decode/transfer/resize paths; stage selection is not a runtime observation.',
   'No actual allocation/share/export lineage links a CP MLUT output record to the selected av-cam inputhandle yet; different shared handles may alias backing.',
   'Virtual CPU aliases must be translated before comparing them with CP hardware addresses; the two request words are not a normal little-endian u64.',
   'Address builder skips direct construction in certain modes; those modes and alternative context+7dc paths remain untested.',
   'HEIF resource encode path is separate from JPEG1f9d8c/2a58f0; this report proves common normal input selection and materializer, not shared codec execution.',
   'No saved-image quality, LUT effect, DMA completion, capture mode availability or concurrent ownership behavior was tested.']}
 p=ROOT/'research/evidence/av-cam-pixels.json';p.write_text(json.dumps(out,indent=2)+'\n')
 print(json.dumps({'regions':len(rs),'instructions':sum(len(r['instructions']) for r in rs),'cases':out['validation']['case_count'],'path':str(p.relative_to(ROOT))}))

if __name__=='__main__':main()
