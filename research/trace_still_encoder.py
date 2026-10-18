"""Offline, byte-bound CP JPEG boundary evidence; no device execution.

Host Unicorn tests execute isolated CP instruction bodies with explicitly named
external service stubs. They do not encode an image or model a camera graph.
"""
import hashlib
import json
from pathlib import Path
import struct
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *

ROOT=Path(__file__).resolve().parents[1]
BASE=0x108000
DIGEST='f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428'


def main():
    source=next((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob('cpapp-b.bin'))
    blob=source.read_bytes()
    assert hashlib.sha256(blob).hexdigest()==DIGEST
    decoder=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
    def region(lo,hi):
        ins=list(decoder.disasm(blob[lo:hi],BASE+lo))
        assert sum(i.size for i in ins)==hi-lo,(hex(lo),hex(hi))
        return {'file_start':hex(lo),'file_end_exclusive':hex(hi),'runtime_start':hex(BASE+lo),
                'ISA':'Thumb-2','instructions':[{'file_offset':hex(i.address-BASE),
                'runtime_address':hex(i.address),'bytes':i.bytes.hex(),'operation':i.mnemonic,
                'operands':i.op_str}for i in ins]}
    def string(address):
        offset=address-BASE
        assert 0<=offset<len(blob)
        return blob[offset:blob.index(b'\0',offset)].decode('ascii')
    tables=[]
    for name,first,typeinfo in [('ApiEncodeJpeg',0xd68d48,0xd68dcc),
                               ('ApiEncodeJpegBase',0xd68de0,0xd68e64),
                               ('ApiEncodeJpegWithWork',0xd68e78,0xd68efc)]:
        rtti=struct.unpack_from('<3I',blob,typeinfo)
        assert string(rtti[1]).endswith(name)
        header=struct.unpack_from('<2I',blob,first-8)
        assert header==(0,typeinfo+BASE)
        slots=struct.unpack_from('<33I',blob,first)
        assert slots[0x40//4]==0x45f4c5 and slots[0x48//4]==0xaf2525
        tables.append({'class':name,'RTTI_name':string(rtti[1]),
                       'RTTI_file_offset':hex(typeinfo),'RTTI_words':[hex(x)for x in rtti],
                       'vtable_file_offset':hex(first),'vtable_runtime_address':hex(first+BASE),
                       'vtable_header_words':[hex(x)for x in header],
                       'slots':[{'byte_offset':hex(i*4),'pointer':hex(v)}for i,v in enumerate(slots)]})
    assert tables[2]['slots'][0x74//4]['pointer']=='0x4603a1'
    arena,stack,finish=0x5000000,0x6000000,0x7000000
    cpu=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
    cpu.mem_map(BASE,(len(blob)+4095)&~4095);cpu.mem_write(BASE,blob)
    for address,size in [(arena,0x10000),(stack,0x10000),(finish,0x1000)]:cpu.mem_map(address,size)
    extents=[];capacity=0;in_layout=False
    diagnostics=[];capture_diagnostics=False;submission=[];wait_flags=1;wait_return=0;wait_events=[]
    def hook(engine,address,size,user):
        nonlocal capacity
        value=0
        if address==0x68e3a2:
            if capture_diagnostics:
                text=string(engine.reg_read(UC_ARM_REG_R0))
                diagnostics.append({'format_runtime_address':hex(engine.reg_read(UC_ARM_REG_R0)),
                                    'format':text,'value':engine.reg_read(UC_ARM_REG_R1)})
        elif address in (0xaecc18,0xaecc26,0xaecb9c,0xaecb3c,0xaf263e,0x68af0c):pass
        elif address in (0x5e9cc4,0xafd3b6):pass
        elif address==0x5ea52c:
            pointer=engine.reg_read(UC_ARM_REG_R3)
            engine.mem_write(pointer,struct.pack('<I',wait_flags))
            wait_events.append({'channel':engine.reg_read(UC_ARM_REG_R0),'mask':engine.reg_read(UC_ARM_REG_R1),'mode':engine.reg_read(UC_ARM_REG_R2),'timeout_stack_word':struct.unpack('<I',engine.mem_read(engine.reg_read(UC_ARM_REG_SP),4))[0]})
            value=wait_return
        elif address==0x565204:
            target=engine.reg_read(UC_ARM_REG_R1);length=engine.reg_read(UC_ARM_REG_R2)
            engine.mem_write(target,struct.pack('<I',arena+0x7000))
            submission.append({'stage':'message_allocation','handle':hex(engine.reg_read(UC_ARM_REG_R0)),'bytes':length,'flag':engine.reg_read(UC_ARM_REG_R3)})
        elif address==0x564b38:
            pointer=engine.reg_read(UC_ARM_REG_R1)
            submission.append({'stage':'queue_boundary','handle':hex(engine.reg_read(UC_ARM_REG_R0)),
                               'tag_at_payload_minus_12':hex(struct.unpack('<I',engine.mem_read(pointer-12,4))[0]),
                               'payload':bytes(engine.mem_read(pointer,16)).hex()})
        elif address==0x61e60c:
            dest=engine.reg_read(UC_ARM_REG_R0);src=engine.reg_read(UC_ARM_REG_R1)
            engine.mem_write(dest,bytes(engine.mem_read(src,engine.reg_read(UC_ARM_REG_R2))));value=dest
        elif address in (0xaef5a4,0xaf1dcc):pass
        elif address==0xaecc5c:
            value=0x11223344;engine.reg_write(UC_ARM_REG_R1,0x55667788)
        elif address==0xaefaec and in_layout:
            p=engine.reg_read(UC_ARM_REG_R1)
            extents.append({'return_address':hex(engine.reg_read(UC_ARM_REG_LR)&~1),
                            'descriptor_words':[hex(w)for w in struct.unpack('<3I',engine.mem_read(p,12))],
                            'extent_argument':engine.reg_read(UC_ARM_REG_R2)})
        elif address==0xaeccc0:value=capacity
        else:return
        engine.reg_write(UC_ARM_REG_R0,value)
        engine.reg_write(UC_ARM_REG_PC,engine.reg_read(UC_ARM_REG_LR))
    cpu.hook_add(UC_HOOK_CODE,hook)
    initial=cpu.context_save()
    def execute(address,args=(),budget=10000):
        cpu.context_restore(initial)
        cpu.mem_write(stack,b'\0'*0x10000)
        cpu.reg_write(UC_ARM_REG_SP,stack+0x8000);cpu.reg_write(UC_ARM_REG_LR,finish|1)
        for register,value in zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3],args):cpu.reg_write(register,value)
        cpu.emu_start(address|1,finish,count=budget)
        assert cpu.reg_read(UC_ARM_REG_PC)==finish,hex(cpu.reg_read(UC_ARM_REG_PC))
        return cpu.reg_read(UC_ARM_REG_R0)
    # Pure message-name routine, with no external-call stubs reached.
    names=[]
    for mode in range(0x503):
        address=execute(0x45d6bc,[mode],budget=200)
        name=string(address)
        if name.startswith('MSG_ID_'):names.append({'numeric_ID':hex(mode),'string_runtime_address':hex(address),'name':name})
    expected={4:'MSG_ID_SDF_ENCODE_JPEG',6:'MSG_ID_SDF_ENCODE_JPEG_REC',8:'MSG_ID_SDF_ENCODE_JPEG_WITH_WORK'}
    assert {int(x['numeric_ID'],16):x['name']for x in names if int(x['numeric_ID'],16)in expected}==expected
    # Actual derived and base initializer bodies, synthetic direct-buffer branch.
    # Common service initializer, buffer resolution/validation and virtual tail
    # service are stubbed; this verifies field transport/arithmetic only.
    obj,msg,vtable=arena+0x1000,arena+0x3000,arena+0x4000
    layout=[];in_layout=True
    for w,h in [(7008,4672),(4672,7008),(1,1),(65535,65535)]:
        for q in (0xffffffff,0x12345678):
            cpu.mem_write(obj,b'\0'*0x400);cpu.mem_write(msg,b'\0'*0xc4)
            cpu.mem_write(obj,struct.pack('<I',vtable));cpu.mem_write(obj+0x38,struct.pack('<I',1))
            cpu.mem_write(vtable+0x80,struct.pack('<I',0xaf263f))
            cpu.mem_write(msg+0x5c,struct.pack('<HH',w,h))
            cpu.mem_write(msg+0x60,struct.pack('<HH',321,123))
            cpu.mem_write(msg+0x64,bytes([1,2,3,4,5,6,7,0]))
            cpu.mem_write(msg+0x6c,struct.pack('<I',q))
            extents.clear();result=execute(0x4603a0,[obj,msg])
            fields=struct.unpack('<4H',cpu.mem_read(obj+0x104,8))
            assert fields==(w,h,321,123) and result==0
            assert struct.unpack('<I',cpu.mem_read(obj+0x128,4))[0]==q
            assert cpu.mem_read(obj+0x124,1)==b'\x07'
            assert len(extents)==(1 if q==0xffffffff else 2)
            second=extents[1] if len(extents)>1 else None
            if second:
                assert second['descriptor_words']==['0x0',hex(q),'0x0']
                assert second['extent_argument']==(2*w*h)&0xffffffff
            layout.append({'message_halfwords_5c_5e':[w,h],'object_halfwords_104_106':list(fields[:2]),
                           'message_qParam_u32_6c':hex(q),'extra_extent_call':second,'return_code':result})
    # Isolate actual extent checker with synthetic capacity-query service.
    in_layout=False;extent_cases=[]
    for available,offset,required in [(100,0,100),(100,0,101),(100,20,80),(100,20,81),(100,101,0),(65482752,0,65482752),(65482751,0,65482752)]:
        capacity=available
        cpu.mem_write(arena+0x5000,struct.pack('<III',0,0x12345678,offset))
        result=execute(0xaefaec,[obj,arena+0x5000,required])
        assert result==(0 if available>=offset and available-offset>=required else 0x10)
        extent_cases.append({'synthetic_capacity':available,'descriptor_offset':offset,
                             'required_extent':required,'return_code':result})
    # Actual field printer transports each load to its firmware format string.
    capture_diagnostics=True
    printed_base=arena+0x8000
    fields=[(i,4)for i in range(0,0x28,4)]+[(i,2)for i in range(0x28,0x30,2)]+[(i,1)for i in range(0x30,0x36)]
    cpu.mem_write(printed_base,b'\0'*0x100)
    for offset,width in fields:cpu.mem_write(printed_base+offset,(offset+0x41).to_bytes(width,'little'))
    execute(0x46020c,[0,printed_base])
    assert len(diagnostics)==len(fields)==20
    for row,(offset,width)in zip(diagnostics,fields):
        assert row['value']==offset+0x41
        row['local_offset']=hex(offset);row['original_message_offset']=hex(offset+0x34);row['width_bytes']=width
    assert 'inputYcSdsUnitType' in diagnostics[0]['format']
    assert 'jpegImageSizeX' in diagnostics[10]['format']
    capture_diagnostics=False
    # Generic delivery wrapper: allocation/send are explicit host stubs.
    submission_cases=[]
    for kind in (0,1,2,3):
        request=arena+0x9000
        cpu.mem_write(request,b'\0'*0x100)
        cpu.mem_write(request+4,struct.pack('<I',kind))
        cpu.mem_write(request+8,struct.pack('<H',16))
        cpu.mem_write(request+12,struct.pack('<I',0x12345678))
        cpu.mem_write(request+0x14,bytes(range(16)))
        if kind==1:cpu.mem_write(request+0x1e,struct.pack('<H',16))
        submission.clear();result_code=execute(0x68c67a,[0,0x90560123,request])
        assert result_code=={0:0xffffffe5,1:0,2:0,3:0xffff}[kind]
        assert len(submission)==(2 if kind in(1,2)else 0)
        if kind in(1,2):
            assert submission[0]['bytes']==16 and submission[0]['flag']==1
            assert submission[1]['payload']==bytes(cpu.mem_read(request+0x14,16)).hex()
            assert submission[1]['tag_at_payload_minus_12']=='0x12345678'
        submission_cases.append({'synthetic_request_kind':kind,'return_u32':hex(result_code),'events':list(submission)})
    worker_first=0xe7189c
    worker_header=struct.unpack_from('<2I',blob,worker_first-BASE-8)
    worker_type=struct.unpack_from('<3I',blob,worker_header[1]-BASE)
    assert string(worker_type[1])=='16ModuleEncodeJpeg'
    worker_identity={'class_name':string(worker_type[1]),'vtable_runtime_address':hex(worker_first),
                     'vtable_header_words':[hex(x)for x in worker_header],'RTTI_words':[hex(x)for x in worker_type],
                     'constructor':'0xaf84f4','preflight_method':'0xaf20e4','preparation_call':'0x467f44',
                     'qualification':'The software module identity and preparation call are concrete; hardware codec invocation and completion are not established.'}
    # Actual result wrapper: synthetic event service, real channel lookup and
    # result transport. No codec start, wait, interrupt or hardware runs.
    for address in (0x1a2f000,0x171e000):cpu.mem_map(address,0x1000)
    wrapper_results=[]
    for channel,flags,wait_code in [(c,1,0)for c in range(0x25,0x2b)]+[(0x25,0,0),(0x25,1,0xffffffce)]:
        wait_flags=flags;wait_return=wait_code;wait_events.clear()
        for index in range(6):
            cpu.mem_write(0x1a2fe8c+4*index,struct.pack('<I',0x12340000+index))
            cpu.mem_write(0x171eb90+8*index,struct.pack('<Q',0x1122334400000000+index))
        wrapper,out=arena+0xa000,arena+0xa100
        cpu.mem_write(wrapper,b'\0'*16);cpu.mem_write(wrapper+4,struct.pack('<H',channel))
        cpu.mem_write(out,b'\0'*24)
        cpu.context_restore(initial);cpu.mem_write(stack,b'\0'*0x10000)
        cpu.reg_write(UC_ARM_REG_SP,stack+0x8000);cpu.reg_write(UC_ARM_REG_LR,finish|1)
        cpu.mem_write(stack+0x8000,struct.pack('<II',0,out))
        for register,value in zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3],[wrapper,0,0,0]):cpu.reg_write(register,value)
        cpu.emu_start(0xafdfd5,finish,count=1000)
        assert cpu.reg_read(UC_ARM_REG_PC)==finish
        code=cpu.reg_read(UC_ARM_REG_R0);scalar=struct.unpack('<I',cpu.mem_read(out,4))[0]
        address64=struct.unpack('<Q',cpu.mem_read(out+8,8))[0]
        assert scalar==0x12340000+channel-0x25 and address64==0x1122334400000000+channel-0x25
        assert code==(0x400 if wait_code else 0 if flags&1 else 0x80000000)
        assert wait_events==[{'channel':channel,'mask':3,'mode':1,'timeout_stack_word':20000}]
        wrapper_results.append({'channel':hex(channel),'synthetic_event_flags':flags,'synthetic_wait_return':hex(wait_code),
                                'return_code':hex(code),'output_scalar':hex(scalar),'output_u64_at_8':hex(address64),
                                'event_arguments':list(wait_events)})
    wrapper_first=0xe71d40
    h=struct.unpack_from('<2I',blob,wrapper_first-BASE-8);t=struct.unpack_from('<3I',blob,h[1]-BASE)
    assert string(t[1])=='14WrapJpegEncode'
    result_path={'wrapper_class':string(t[1]),'vtable_runtime_address':hex(wrapper_first),
                 'vtable_header_words':[hex(x)for x in h],'RTTI_words':[hex(x)for x in t],
                 'dispatch_sequence':'0xaf4c0a calls virtual +0x74 initializer, +0x78 processing, +0x40 message delivery in that order on success.',
                 'chain':['0xaf20e4','0x467f44','0xaf8a7c','0xaf8a96','0xafdebc','0xafdfd4'],
                 'start_boundary':'0xafdfd4 may invoke 0x52e73c, then calls event wait 0x5ea52c with channel, mask3, mode1, stack word20000. Units unspecified.',
                 'result_getters':'0x46a09c reads u32 from 0x1a2fe8c+4*(channel-0x25); 0x46a0f0 reads u64 from 0x171eb90+8*(channel-0x25), for channels0x25..0x2a.',
                 'transport':'Wrapper writes scalar to result+0 and u64 to result+8. Module 0xaf8a96 copies result scalar through caller output; 0x467f44 passes it to Api object+0x134; 0xaf20e4 mirrors to +0x138 on success; request builder copies +0x138 into header+0xc.',
                 'inference':'The scalar is an encoded-stream length candidate: Module code can add2 under addSoi==1, and the message contains streamValidLength. Its exact relation to saved-file descriptors remains unproved.',
                 'qualification':'Host wrapper tests skip codec start through stubbed channel lookup; globals and wait result are synthetic. No hardware result is observed.'}
    ranges={
      'JPEG_virtual_lifecycle':(0x9ecc0a,0x9ecc9a),
      'JPEG_module_result':(0x9f0a7c,0x9f0cbe),
      'JPEG_encode_wrapper_constructor_and_bridge':(0x9f5e92,0x9f5ef4),
      'JPEG_encode_wrapper_result_wait':(0x9f5fd4,0x9f6148),
      'JPEG_start_boundary':(0x42673c,0x4267dc),
      'JPEG_result_u32_getter_prefix':(0x36209c,0x3620a6),
      'JPEG_result_u32_getter_leaves':(0x3620ac,0x3620ee),
      'JPEG_result_u64_getter_prefix':(0x3620f0,0x3620fa),
      'JPEG_result_u64_getter_leaves':(0x362100,0x362146),
      'generic_delivery_wrapper':(0x58467a,0x584764),
      'delivery_tag_setter':(0xae4024,0xae402a),
      'queue_insertion_prefix':(0x45cbfc,0x45cc56),
      'queue_insertion_suffix':(0x45cc58,0x45cc9a),
      'JPEG_preflight':(0x9ea0e4,0x9ea4dc),
      'JPEG_module_constructor':(0x9f04f4,0x9f050a),
      'JPEG_module_prepare':(0x35ff44,0x3600e8),
      'JPEG_input_diagnostic_bridge':(0x9ea646,0x9ea66c),
      'JPEG_message_diagnostics_bridge':(0x35c108,0x35c12e),
      'JPEG_named_descriptor_printer':(0x35820c,0x358328),
      'JPEG_with_work_initializer':(0x3583a0,0x358422),
      'JPEG_base_initializer':(0x9e9bfe,0x9e9dbe),
      'extent_validation':(0x9e7aec,0x9e7bb0),
      'common_request_execution':(0x3574c4,0x357684),
      'request_builder':(0x9ea524,0x9ea59e),
      'request_field_getters':(0x9e750a,0x9e751a),
      'name_switch_entry':(0x3556bc,0x3556d6),
      'name_switch_1_to_9':(0x35573e,0x35574a),
      'JPEG_name_leaves':(0x35584c,0x35587e),
      'selected_fallback_entry':(0x3bf9c0,0x3bf9e6),
      'selected_fallback_geometry_and_LUT_builder':(0x3bfd70,0x3bfd88),
      'selected_fallback_callback_registration':(0x3c0e62,0x3c0e7e),
      'selected_fallback_LUT_callback':(0xa2357c,0xa235a2),
    }
    # Retain typed data separately: never disassemble inline switch bytes or RTTI.
    data={
      'name_switch_1_to_9_table':{'file_offset':'0x35574a','bytes':blob[0x35574a:0x355754].hex()},
      'u32_getter_switch_table':{'file_offset':'0x3620a6','bytes':blob[0x3620a6:0x3620ac].hex()},
      'u64_getter_switch_table':{'file_offset':'0x3620fa','bytes':blob[0x3620fa:0x362100].hex()},
      'WithWork_diagnostic':{'file_offset':'0x358434','runtime_address':'0x460434','text':string(0x460434)},
    }
    result={'source':{'path':str(source.relative_to(ROOT)),'sha256':DIGEST,'bytes':len(blob)},
      'address_model':'Retained CP file-backed addresses: runtime = file offset + 0x108000; odd code pointers select Thumb.',
      'target_executed':False,'host_emulation_performed':True,
      'scope':'No camera, USB, target process, hardware imaging, installation, frame capture or image encoding.',
      'observed':{'native_JPEG_class_tables':tables,'JPEG_module_identity':worker_identity,'JPEG_result_path':result_path,'named_message_fields':diagnostics,'message_name_switch':{'cases':0x503,'recognized':names},
         'message_layout':{'input_Yc_descriptor_words':'message +0x34/+0x38/+0x3c -> object +0x78/+0x7c/+0x80 (object+0x38==0 branch)', 'buffer_descriptor_words':'message +0x40/+0x44/+0x48 -> object +0xe0/+0xe4/+0xe8',
           'resolved_address':'helper 0xaecc5c result r0/r1 -> object +0xf0/+0xf4; resolution stubbed in layout tests',
           'geometry_halfwords':'message +0x5c/+0x5e -> object +0x104/+0x106',
           'secondary_halfwords':'message +0x60/+0x62 -> object +0x108/+0x10a',
           'byte_fields':'message +0x64..+0x69 -> object 32-bit fields +0x10c,+0x110,+0x114,+0x118,+0x11c,+0x120',
           'WithWork_extension':'message byte +0x6a -> object byte +0x124; u32 +0x6c -> +0x128',
           'work_extent':'If qParam != 0xffffffff, validate descriptor {0,qParam,0} for (2*geometry0*geometry1) mod 2^32 bytes. No pixel format established.'},
         'extent_check':'0xaefaec requires queried capacity >= descriptor[2] and capacity - descriptor[2] >= required extent; failure 0x10.',
         'request_sink':'Common execution virtual slot +0x48 constructs request; function 0x68c67a is then called with object+4, object+0x20 handle and request. This is generic delivery after the virtual processing method, potentially a result notification; the service destination remains unresolved.'},
      'host_tests':{'named_field_printer_cases':1,'delivery_cases':submission_cases,'result_wrapper_cases':wrapper_results,'result_wrapper_stubs':['0x5e9cc4 event clear returns0','0xafd3b6 channel lookup returns0 so codec start is skipped','0x5ea52c event wait returns synthetic flags/status','0xaecb3c/0xaecb9c diagnostic'],'delivery_stubs':['0x565204 writes synthetic allocated message pointer and returns 0','0x564b38 captures queued bytes and returns 0','0x61e60c host memcpy','0x68af0c must be stubbed for invalid-kind diagnostics'], 'message_name_switch_stubs':'None reached: pure getter executes from actual CP bytes.',
         'layout_cases':layout,'layout_stubs':['0xaecc18 scoped setup','0xaecc26 scoped teardown','0x61e60c host memcpy','0xaef5a4 instance getter -> 0','0xaf1dcc common initializer -> 0 (fixture object+0x38 already 1)','0xaecc5c address resolution -> synthetic 64-bit address','0xaefaec records arguments and returns 0','0xaf263e virtual tail -> 0','0x68e3a2 diagnostic'],
         'extent_cases':extent_cases,'extent_stubs':['0xaecc18/0xaecc26 setup/teardown','0xaeccc0 capacity query -> per-case fixture','0xaecb9c/0xaecb3c diagnostics'],
         'qualification':'Layout tests establish byte transport and arithmetic, not input acceptance or encoding, because validation/resolution are stubbed. Extent tests execute the checker but substitute capacity service. No hardware or OS is modeled.'},
      'unresolved':['No demonstrated dataflow from selected fallback 0x4c79c0 context or its MLUT output to a JPEG class input buffer.',
         'No runtime geometry values or still/movie/display semantic enum tied to selected fallback descriptors.',
         'No full-resolution saved-frame ownership, image encoder completion, or join to AP JPEG/HEIF recording queues.',
         'No HEIF encoder ABI or shared JPEG/HEIF pre-encode path established.',
         'No payload grid dimensions/channel order/precision inferred from MLUT payload length.'],
      'data_regions':data,'disassembled_regions':{n:region(*r)for n,r in ranges.items()},
      'verification':{'SHA256_pinned':True,'decoded_regions':len(ranges),'host_cases':0x503+len(layout)+len(extent_cases)+1+len(submission_cases)+len(wrapper_results)}}
    dest=ROOT/'research/evidence/still-encoder-route.json'
    dest.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'evidence':str(dest.relative_to(ROOT)),**result['verification']}))


if __name__=='__main__':main()
