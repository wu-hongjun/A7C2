"""Source-bound offline still-info notification to recording handoff.

No camera, transport, installation, native target execution or media encoding.
Isolated Unicorn cases use invented descriptors and intercept memcpy/logging.
"""
import hashlib
import itertools
import json
import struct
from pathlib import Path

from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import *
from map_ap_interfaces import Image, LIB

ROOT=Path(__file__).resolve().parents[1]
EXPECTED='2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
REGIONS=[
 ('sequence callback construction',0x3368c98,0x3368e40,'bounded constructor prefix'),
 ('still-info callback registration use',0x349e850,0x349e8bc,'selected registration block'),
 ('still-info registration adapter',0x34d2278,0x34d22bc,'entire wrapper; next entry'),
 ('still-info global registration',0x352f510,0x352f5ec,'entire entry and its assertion branch'),
 ('callback member thunk',0x310572c,0x3105780,'entire tail dispatch; next entry'),
 ('callback diagnostic setter',0x3105780,0x3105788,'entire leaf'),
 ('registered diagnostic preprocessing callback',0x34c55e8,0x34c55f4,'entire three-instruction tail wrapper'),
 ('local queue push',0x34a9c94,0x34a9e4c,'entire deque push and growth branches'),
 ('local queue enqueue',0x34a9e4c,0x34a9f10,'entire enqueue and gating branches'),
 ('local tag9 callback',0x34a9f7c,0x34a9f88,'entire three-instruction tail wrapper'),
 ('AP worker receive and source gates',0x35314fc,0x3531600,'bounded worker prefix'),
 ('AP notification dispatch',0x3531650,0x3531734,'selected dispatch branches and cleanup'),
 ('still-info notification descriptor transformation',0x3533200,0x3533398,'entire selected case blocks'),
 ('local queue receive and tag dispatch',0x34ae1ac,0x34ae280,'selected receive and dispatch slice'),
 ('local queue tag9 consumer case',0x34ae304,0x34ae314,'entire selected dispatch case'),
 ('result descriptor classifier',0x34acd48,0x34aceb4,'bounded handler prefix and recording call'),
 ('result descriptor classifier selected branches',0x34acf70,0x34acfb8,'three selected classification blocks'),
 ('RecordStill instrumentation wrapper',0x34d40c8,0x34d4108,'entire wrapper'),
 ('RecordStill AP adapter',0x357c9f0,0x357cde0,'bounded contiguous range; next prologue'),
 ('payload allocator/deallocator wrappers',0x2dafe04,0x2dafe34,'three entire tail wrappers'),
]
# Wire type, local queued type, role, copied bytes, queued descriptor offset.
FORMATS=[(1,0,'JPEG',0xc0,0xc),(2,2,'RAW',0x150,0xcc),(4,1,'HEIF',0xe0,0x21c)]


def identity(path):
 b=path.read_bytes()
 return {'path':str(path.relative_to(ROOT)),'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}


def offset(im,address):
 for s in im.elf.iter_segments():
  if s['p_type']=='PT_LOAD' and s['p_vaddr']<=address<s['p_vaddr']+s['p_filesz']:
   return s['p_offset']+address-s['p_vaddr']
 raise ValueError(hex(address))


def run_flow_cases(im):
 """Original code slices with synthetic data and host-only memcpy/log hooks."""
 uc=Uc(UC_ARCH_ARM64,UC_MODE_ARM)
 for start,length in [(0x3531000,0x3000),(0x34a9000,0x5000),(0x3105000,0x1000),
                       (0x357c000,0x2000),(0x165d000,0x1000),(0x4c7d000,0x1000),
                       (0x34c5000,0x1000),(0x3682000,0x1000)]:
  uc.mem_map(start,length);uc.mem_write(start,im.bytes_at(start,length))
 uc.mem_map(0x10000000,0x40000)
 wire,payload,frame,owner,entry,output,record=(0x10000000,0x10002000,0x10008000,
                                            0x10010000,0x10014000,0x10008140,0x1001c000)
 calls=[];log_calls=[]
 def memcpy_hook(machine,address,size,_):
  if address==0x36824f0:
   log_calls.append([machine.reg_read(r) for r in [UC_ARM64_REG_X0,UC_ARM64_REG_X1]])
   machine.reg_write(UC_ARM64_REG_PC,machine.reg_read(UC_ARM64_REG_LR));return
  if address!=0x165d3e0:return
  dest,src,n=[machine.reg_read(r) for r in [UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2]]
  assert n<=0x150
  machine.mem_write(dest,bytes(machine.mem_read(src,n)))
  calls.append({'src':hex(src),'dest':hex(dest),'bytes':n})
  machine.reg_write(UC_ARM64_REG_PC,machine.reg_read(UC_ARM64_REG_LR))
 uc.hook_add(UC_HOOK_CODE,memcpy_hook)
 cases=[]
 for formats,preprocess in itertools.product(list(itertools.product(FORMATS,repeat=2)),[0,0x34c55e8]):
  uc.mem_write(wire,bytes(0x400));uc.mem_write(payload,bytes([0xa5])*0x68c)
  uc.mem_write(frame,bytes(0x1000));uc.mem_write(owner,bytes(0x1000));uc.mem_write(output,bytes(0xe0))
  expected_descriptors=[]
  for i,(wire_type,queued_type,name,length,descriptor_offset) in enumerate(formats):
   source=wire+0xb8+i*0x160
   data=bytes(((j*37+i*73+wire_type)&255) for j in range(length))
   uc.mem_write(source,struct.pack('<I',wire_type));uc.mem_write(source+0xc,struct.pack('<I',0x51a00000+i))
   uc.mem_write(source+0x10,data);expected_descriptors.append(data)
  meta=bytes((j*13+7)&255 for j in range(0x90))
  uc.mem_write(wire+0x24,meta);uc.mem_write(wire+0xc,struct.pack('<I',0x77773333))
  for r,v in [(UC_ARM64_REG_X0,payload),(UC_ARM64_REG_X19,wire),(UC_ARM64_REG_X29,frame),
              (UC_ARM64_REG_X26,frame+0xb0),(UC_ARM64_REG_X27,frame+0xc0),(UC_ARM64_REG_X21,0x4c7d070),
              (UC_ARM64_REG_SP,frame+0x800)]:uc.reg_write(r,v)
  calls.clear();uc.emu_start(0x3533214,0x35332f8,count=1024)
  for i,(_,queued_type,name,length,destination) in enumerate(formats):
   assert struct.unpack('<I',uc.mem_read(payload+i*0x2fc,4))[0]==queued_type
   id_offset=8 if queued_type==2 else 4
   assert struct.unpack('<I',uc.mem_read(payload+i*0x2fc+id_offset,4))[0]==0x51a00000+i
   assert bytes(uc.mem_read(payload+i*0x2fc+destination,length))==expected_descriptors[i]
  assert bytes(uc.mem_read(payload+0x5f8,0x90))==meta
  assert struct.unpack('<I',uc.mem_read(payload+0x688,4))[0]==0x77773333
  copy_calls=list(calls)
  # The registered delegate subobject has owner/memberfn/adjustment fields.
  callback=owner+0x180
  uc.mem_write(callback+8,struct.pack('<QQQ',owner,0x34a9f7c,0))
  uc.mem_write(callback+0x20,struct.pack('<Q',preprocess))
  uc.reg_write(UC_ARM64_REG_X0,callback);uc.reg_write(UC_ARM64_REG_X1,payload)
  log_calls.clear()
  uc.emu_start(0x310572c,0x34a9e4c,count=128)
  assert [uc.reg_read(r) for r in [UC_ARM64_REG_X0,UC_ARM64_REG_X1,UC_ARM64_REG_X2]]==[owner,9,payload]
  assert log_calls==([[0xa,0xd]] if preprocess else [])
  # Run the original entry construction and non-growing deque push only.
  uc.mem_write(owner+0x300,struct.pack('<Q',entry))
  uc.mem_write(owner+0x310,struct.pack('<Q',entry+0x200))
  for r,v in [(UC_ARM64_REG_X19,owner),(UC_ARM64_REG_X21,9),(UC_ARM64_REG_X22,payload),
              (UC_ARM64_REG_X29,frame),(UC_ARM64_REG_SP,frame+0x800)]:uc.reg_write(r,v)
  uc.emu_start(0x34a9eb0,0x34a9ec4,count=128)
  assert struct.unpack('<I',uc.mem_read(entry,4))[0]==9
  queued_pointer=struct.unpack('<Q',uc.mem_read(entry+8,8))[0]
  assert queued_pointer==payload
  assert struct.unpack('<Q',uc.mem_read(owner+0x300,8))[0]==entry+0x10
  # Reproduce the consumer's existing classifier on the identical payload.
  for r,v in [(UC_ARM64_REG_X29,frame),(UC_ARM64_REG_X20,output),
              (UC_ARM64_REG_X22,queued_pointer),(UC_ARM64_REG_X24,frame+0x100)]:uc.reg_write(r,v)
  uc.emu_start(0x34acdbc,0x34ace78,count=512)
  expected=[0]*6;counts={'JPEG':0,'RAW':0,'HEIF':0}
  for i,(_,_,name,_,destination) in enumerate(formats):
   slot={'JPEG':0,'RAW':1,'HEIF':2}[name]+counts[name]*3
   expected[slot]=payload+i*0x2fc+destination;counts[name]+=1
  actual=list(struct.unpack('<6Q',uc.mem_read(output,48)));assert actual==expected
  assert struct.unpack('<Q',uc.mem_read(output+0x30,8))[0]==payload+0x5f8
  uc.reg_write(UC_ARM64_REG_X19,output);uc.reg_write(UC_ARM64_REG_X29,record-0xb8)
  uc.emu_start(0x357ca60,0x357ca98,count=64)
  # Final parameter groups J0/J1, R0/R1, H0/H1 plus copied auxiliary pointer.
  final_expected=[expected[x] for x in [0,3,1,4,2,5]]+[payload+0x5f8]
  final=list(struct.unpack('<7Q',uc.mem_read(record+8,56)));assert final==final_expected
  cases.append({'wire_types':[f[0] for f in formats],'queued_types':[f[1] for f in formats],
                'roles':[f[2] for f in formats],'memcpy_calls':copy_calls,
                'diagnostic_preprocessor_present':bool(preprocess),'diagnostic_helper_calls':list(log_calls),
                'same_payload_pointer_through_delegate_and_queue':hex(queued_pointer),
                'classifier_pointer_offsets':[hex(x-payload) if x else None for x in expected],
                'RecordStill_parameter_pointer_offsets':[hex(x-payload) if x else None for x in final]})
 return {'cases':len(cases),'results':cases,
         'scope':'Selected original notification-copy, member thunk, optional diagnostic wrapper, tag wrapper, queue fast push, classifier and parameter-copy slices. memcpy and diagnostic helper intercepted. No receive, lock, notify, allocation, encoding, callback-registration or file operation executed.'}


def main():
 im=Image(LIB/'appFw.so');assert hashlib.sha256(im.blob).hexdigest()==EXPECTED
 relocs={r['offset']:r for r in im.relocations}
 assert relocs[0x5b53b28]['addend']==0x34a9f7c
 assert relocs[0x5b9c200]['addend']==0x5b37ae8
 assert relocs[0x5b37af8]['addend']==0x310572c
 assert relocs[0x5b37b00]['addend']==0x3105780
 regions=[]
 for role,start,end,extent in REGIONS:
  raw=im.bytes_at(start,end-start);instructions=im.disasm(start,end)
  assert sum(len(bytes.fromhex(r['bytes'])) for r in instructions)==end-start
  for r in instructions:r['file_offset']=hex(offset(im,int(r['address'],16)))
  regions.append({'role':role,'source_path':str(im.path.relative_to(ROOT)),'start_ELF_VA':hex(start),
                  'end_exclusive_ELF_VA':hex(end),'file_offset':hex(offset(im,start)),
                  'extent_basis':extent,'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest(),
                  'instructions':instructions})
 switch=list(struct.unpack('<5b',im.bytes_at(0x4c7d070,5)))
 assert [0x35332ac+n*4 for n in switch]==[0x353337c,0x35332ac,0x3533358,0x35332d0,0x3533330]
 local=list(struct.unpack('<41h',im.bytes_at(0x4c5f418,82)))
 assert 0x34ae264+local[9]*4==0x34ae304
 prior=ROOT/'research/evidence/still-sink.json';old=json.loads(prior.read_text())
 assert any(s['sha256']==EXPECTED for s in old['sources'])
 result={'camera_accessed':False,'camera_transport_opened':False,'target_media_encoded':False,
         'method':'Ordinary original-file ELF/Capstone evidence and selected host Unicorn slices; no new native REA session.',
         'sources':[identity(im.path)],'generator':identity(Path(__file__)),
         'coordinate_convention':'ELF virtual addresses; PT_LOAD maps file offsets; Ghidra shared-library image base would add0x100000.',
         'regions':regions,'host_dataflow_cases':run_flow_cases(im),
         'notification':{'AP_worker_ELF_VA':'0x35314fc','wire_command_halfword_offset':2,'wire_command':0x1006,
                         'dispatch_match_ELF_VA':'0x3531718','case_ELF_VA':'0x3533200',
                         'accepted_source_ids':['0x90030001','0x90040193','0x8102021f','0x90bf0274'],
                         'source_ids_caution':'Case1006 is not shown to originate uniquely from CP; producer is not resolved here.',
                         'allocated_queued_payload_bytes':0x68c,'wire_union_offsets':['0xb8','0x218'],
                         'wire_union_stride':0x160,'wire_descriptor_offset_in_union':0x10,
                         'wire_auxiliary_word_offset_in_union':0xc,'local_record_stride':0x2fc,
                         'format_map':[{'wire_type':wire,'queued_type':queued,'role':name,'copy_bytes':n,
                                        'queued_descriptor_offset':hex(at),'queued_auxiliary_word_offset':8 if queued==2 else 4}
                                       for wire,queued,name,n,at in FORMATS],
                         'auxiliary_0x90_block':{'wire_offset':'0x24','queued_offset':'0x5f8','copy_bytes':0x90},
                         'extra_word':{'wire_offset':'0xc','queued_offset':'0x688'},
                         'type_switch':{'table_ELF_VA':'0x4c7d070','signed_bytes':switch,'branch_base_ELF_VA':'0x35332ac'}},
         'delegate':{'registration_ELF_VA':'0x352f510','diagnostic_name':im.read_string(0x4c7f140),
                     'global_cell_ELF_VA':'0x9116610','worker_invoke_ELF_VA':'0x3533320','vtable_call_slot':0,
                     'sequence_callback_subobject_offset':'0x180','constructor_member_store_ELF_VA':'0x3368dc0',
                     'callback_GOT_relocation':relocs[0x5b53b28],
                     'common_vtable_GOT_relocation':relocs[0x5b9c200],
                     'common_vptr_ELF_VA':'0x5b37af8','common_invoke_relocation':relocs[0x5b37af8],
                     'member_thunk_ELF_VA':'0x310572c','memberfn_ELF_VA':'0x34a9f7c'},
         'queue':{'enqueue_ELF_VA':'0x34a9e4c','push_ELF_VA':'0x34a9c94','local_tag':9,'entry_bytes':16,
                  'entry_tag_offset':0,'entry_payload_pointer_offset':8,'payload_deep_copied_at_enqueue':False,
                  'dispatcher_receive_ELF_VA':'0x34ae1ac','case_callsite_ELF_VA':'0x34ae30c',
                  'consumer_ELF_VA':'0x34acd48','payload_free_callsite_after_consumer_ELF_VA':'0x34ae27c',
                  'no_registered_delegate_payload_free_callsite_ELF_VA':'0x3533390',
                  'image_buffer_ownership':'Not established by descriptor/outer-payload allocation and free.'},
         'previous_file_sink_evidence':identity(prior),
         'direct_callsites':im.direct_sites({0x352f510,0x34d2278,0x34a9f7c,0x34a9e4c,0x34a9c94,0x34acd48,0x357c9f0}),
         'limits':['The incoming1006 producer and its correspondence to JPEG response scalar/header+c are unresolved.',
                   'Copied JPEG/RAW/HEIF descriptors preserve embedded fields, but this does not recover the encoder input/output buffer ABI.',
                   'The same outer payload pointer is statically preserved through callback and queue; no actual camera execution trace was captured.',
                   'Queued payload ownership does not establish ownership/completion/cancellation for referenced image buffers.',
                   'Wire types0/3/>4 are not assigned JPEG/RAW/HEIF semantics here.',
                   'No MLUT-to-encoder pixel-path or safe custom-program ABI follows from this notification-to-file boundary.']}
 out=ROOT/'research/evidence/encoder-save-link.json';out.write_text(json.dumps(result,indent=2)+'\n')
 print(json.dumps({'regions':len(regions),'instructions':sum(len(r['instructions']) for r in regions),
                   'host_cases':result['host_dataflow_cases']['cases'],'output':str(out)},indent=2))


if __name__=='__main__':main()
