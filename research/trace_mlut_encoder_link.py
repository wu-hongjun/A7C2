"""Offline CP same-context MLUT/pixel resource and descriptor transport.

No camera, USB, network, target process, driver submission or image processing.
Unicorn executes selected original instruction bodies against synthetic host RAM;
all external service and imaging boundaries are explicitly intercepted.
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
    path=next((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob('cpapp-b.bin'))
    blob=path.read_bytes();assert hashlib.sha256(blob).hexdigest()==DIGEST
    dis=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
    def region(start,end):
        lo,hi=start-BASE,end-BASE
        items=list(dis.disasm(blob[lo:hi],start))
        assert sum(i.size for i in items)==hi-lo,(hex(start),hex(end))
        return {'runtime_start':hex(start),'runtime_end_exclusive':hex(end),'file_start':hex(lo),
                'ISA':'Thumb-2','instructions':[{'runtime_address':hex(i.address),'file_offset':hex(i.address-BASE),
                'bytes':i.bytes.hex(),'operation':i.mnemonic,'operands':i.op_str}for i in items]}
    cpu=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
    cpu.mem_map(BASE,(len(blob)+4095)&~4095);cpu.mem_write(BASE,blob)
    arena,stack,stop=0x5000000,0x6000000,0x7000000
    for a,n in [(arena,0x20000),(stack,0x10000),(stop,0x1000),(0x1a53000,0x1000),
                (0x1a6a000,0x1000),(0x1b1e000,0x1000),(0x172f000,0x1000)]:cpu.mem_map(a,n)
    ctx=arena+0x1000;sp=stack+0x8000;owner=0x12345678
    mode='';predicate=1;events=[]
    def hook(u,a,n,_):
        value=0
        if a in(0xaf4c06,0x45f4c4) and mode=='protocol':
            events.append({'selected_virtual_entry':hex(a),'object':hex(u.reg_read(UC_ARM_REG_R0)),
                           'second_argument':hex(u.reg_read(UC_ARM_REG_R1))})
        elif a==0xb2bf66 and mode=='callback':value=predicate
        elif a==0x4e876c and mode=='callback':
            events.append({'call':'MLUT_consumer','owner':hex(u.reg_read(UC_ARM_REG_R0)),
                           'record':hex(u.reg_read(UC_ARM_REG_R1))})
        elif a==0x4c96b0 and mode=='callback':events.append({'call':'pixel_config_context','context':hex(u.reg_read(UC_ARM_REG_R0))})
        elif a==0xb44b96 and mode in('producer','pipeline'):pass
        elif a==0x61ed00 and mode=='producer':
            p=u.reg_read(UC_ARM_REG_R0);length=u.reg_read(UC_ARM_REG_R1);u.mem_write(p,b'\0'*length)
        elif a in(0x511cb4,0x511edc) and mode=='producer':
            token=u.reg_read(UC_ARM_REG_R0);p=u.reg_read(UC_ARM_REG_R1)
            events.append({'query':hex(a),'token':hex(token),'selector':u.reg_read(UC_ARM_REG_R2)})
            if a==0x511cb4:
                u.mem_write(p+0xc,struct.pack('<I',0x3000));u.mem_write(p+0x10,struct.pack('<Q',0x12345678000))
            else:u.mem_write(p+0x10,struct.pack('<I',0x1000))
        elif a==0xb0a4a0 and mode=='resource':value=0
        elif a==0x46f9c4 and mode=='resource':
            p=u.reg_read(UC_ARM_REG_SP)
            selector=u.reg_read(UC_ARM_REG_R1)
            out=struct.unpack('<I',u.mem_read(p+0x14,4))[0]
            events.append({'call':'resource_query_boundary','resource_class':u.reg_read(UC_ARM_REG_R0),
                           'selector':hex(selector),'output_pointer_argument':hex(out)})
            if out:u.mem_write(out,struct.pack('<I',owner if selector==0x10 else 0x5555))
            value=1
        elif a==0x46f8e8 and mode=='pipeline':value=1
        elif a in (0xbb30ce,0xb0a308,0x4bce70,0xb61ae2,0xb436e4,0xb4457a) and mode=='pipeline':pass
        elif a==0xb2ddb4 and mode=='pipeline':
            value=arena+0xa000;u.mem_write(value+0x60,struct.pack('<I',arena+0xa100))
        elif a==0x54d9c8 and mode=='pipeline':
            args=[u.reg_read(r)for r in (UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3)]
            args+=list(struct.unpack('<2I',u.mem_read(u.reg_read(UC_ARM_REG_SP),8)))
            ports=[]
            for index,p in enumerate(args[1:5]):
                data=bytes(u.mem_read(p,0x318))
                ports.append({'argument_position':index+1,'host_pointer':hex(p),
                              'all_words':[hex(w)for w in struct.unpack('<198I',data)],
                              'address_u64_at_160':hex(struct.unpack_from('<Q',data,0x160)[0]),
                              'geometry_words_168_16c':list(struct.unpack_from('<2I',data,0x168)),
                              'block_158_flag':hex(struct.unpack_from('<I',data,0x158)[0])})
            events.append({'call':'pixel_driver_boundary','owner_at_final_pointer':hex(struct.unpack('<I',u.mem_read(args[5],4))[0]),
                           'ports':ports})
        else:return
        u.reg_write(UC_ARM_REG_R0,value);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR))
    cpu.hook_add(UC_HOOK_CODE,hook)
    initial=cpu.context_save()
    def reset():
        cpu.context_restore(initial);cpu.mem_write(arena,b'\0'*0x20000);cpu.mem_write(stack,b'\0'*0x10000)
        cpu.reg_write(UC_ARM_REG_SP,sp);cpu.reg_write(UC_ARM_REG_LR,stop|1);events.clear()
    def run(address,end=stop,registers=(),budget=10000):
        cpu.reg_write(UC_ARM_REG_LR,stop|1)
        for r,v in registers:cpu.reg_write(r,v)
        cpu.emu_start(address|1,end,count=budget)
        assert cpu.reg_read(UC_ARM_REG_PC)==end,hex(cpu.reg_read(UC_ARM_REG_PC))
    callback=[];mode='callback'
    for predicate in (0,1):
        reset();cpu.mem_write(ctx+0x13b4,struct.pack('<I',owner));cpu.mem_write(ctx+0x12ac,struct.pack('<I',2))
        run(0xb2b57c,registers=[(UC_ARM_REG_R0,ctx)])
        expected=[] if predicate==0 else [{'call':'MLUT_consumer','owner':hex(owner),'record':hex(ctx+0x13b8)},
                                         {'call':'pixel_config_context','context':hex(ctx)}]
        assert events==expected
        callback.append({'synthetic_predicate':predicate,'calls':list(events)})
    # Actual resource-query call setup and routing helper. Final service is a
    # named stub, so output owner is a fixture, not a camera allocation.
    reset();mode='resource'
    cpu.mem_write(ctx+0x1500,struct.pack('<I',1))
    run(0x4c8718,0x4c87c0,[(UC_ARM_REG_R11,ctx),(UC_ARM_REG_R4,0x12b4),
                           (UC_ARM_REG_R6,ctx+0x14af),(UC_ARM_REG_R8,0x222),(UC_ARM_REG_R9,0)])
    assert events==[{'call':'resource_query_boundary','resource_class':1,'selector':'0x10','output_pointer_argument':hex(ctx+0x13b4)},
                    {'call':'resource_query_boundary','resource_class':1,'selector':'0x11','output_pointer_argument':hex(ctx+0x1528)}]
    resource={'calls':list(events),'fixture_written_owner':hex(struct.unpack('<I',cpu.mem_read(ctx+0x13b4,4))[0]),
              'qualification':'Actual caller/helper transport the output pointer; final resource service is stubbed. No live allocation or ownership is asserted.'}
    # Pure router constructs descriptor pointers; no service calls occur.
    reset();mode='router'
    outputs=[arena+0x10000+i*0x100 for i in range(10)]
    for i,value in enumerate(outputs[4:]):cpu.mem_write(sp+4*i,struct.pack('<I',value))
    run(0xb2dcec,registers=list(zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3],outputs[:4])))
    families=[(0,4),(8,5),(0x30,4),(0x6c,4),(0x80,4),(0x13c,4),(0xbc,5),(0xdc,5),(0xfc,5),(0x11c,5)]
    router=[]
    for pointer,(offset,count)in zip(outputs,families):
        observed=struct.unpack('<'+'I'*count,cpu.mem_read(pointer,4*count))
        expected=tuple(0x1a6a144+offset+0x150*i for i in range(min(count,4)))
        if count==5:expected+=(0x1a6a144+{8:0x540,0xbc:0x550,0xdc:0x558,0xfc:0x560,0x11c:0x568}[offset],)
        assert observed==expected
        router.append({'caller_argument_index':len(router),'family_offset':hex(offset),'pointers':[hex(v)for v in observed]})
    # Resource-backed ring: actual initialization/selection/allocation bodies;
    # service responses and record constructor are explicit fixture stubs.
    producer=[];mode='producer';pool=0x172fad0
    for token in (0x43f,0x4c4):
        reset();cpu.mem_write(pool,b'\0'*32)
        run(0xb2cc16,registers=[(UC_ARM_REG_R0,pool),(UC_ARM_REG_R1,token)])
        words=struct.unpack('<I4xQQII',cpu.mem_read(pool,32))
        assert words==(token,0x12345678000,0x12345678000,0x3000,0x1000)
        assert events==[{'query':'0x511cb4','token':hex(token),'selector':5},{'query':'0x511edc','token':hex(token),'selector':5}]
        producer.append({'kind':'pool_initialization','token':hex(token),'pool_fields':{'token':hex(words[0]),'base':hex(words[1]),'current':hex(words[2]),'capacity':words[3],'stride':words[4]},'service_calls':list(events)})
    for selector in (0x295,0x2d9,0x43f,0x4fa,0x4c3,0x4c4):
        reset()
        for p,t,b in [(0x172fad0,0x43f,0x12345678000),(0x172faf0,0x4c4,0x23456789000)]:
            cpu.mem_write(p,struct.pack('<I4xQQII',t,b,b,0x3000,0x1000))
        run(0x4d3728,registers=[(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,selector),(UC_ARM_REG_R2,ctx+0x1d0)])
        chosen=0x172faf0 if selector in(0x4c3,0x4c4) else 0x172fad0
        expected_base=0x23456789000 if chosen==0x172faf0 else 0x12345678000
        expected_token=0x4c4 if chosen==0x172faf0 else 0x43f
        record=bytes(cpu.mem_read(ctx+0x1d0,0x1c))
        assert struct.unpack_from('<I',record,0)[0]==0xffffffff
        assert struct.unpack_from('<H',record,4)[0]==1
        assert struct.unpack_from('<I',record,8)[0]==expected_token
        assert struct.unpack_from('<Q',record,0x10)[0]==expected_base
        assert struct.unpack_from('<I',record,0x18)[0]==0x1000
        producer.append({'kind':'pool_selection','selector':hex(selector),'selected_pool':hex(chosen),'record_pool_token':hex(expected_token),
                         'record_address':hex(expected_base),'record_stride':0x1000,'record_bytes':record.hex()})
    reset();cpu.mem_write(pool,struct.pack('<I4xQQII',0x43f,0x12345678000,0x12345678000,0x3000,0x1000))
    for index in range(4):
        run(0xb2cc72,registers=[(UC_ARM_REG_R0,pool),(UC_ARM_REG_R1,ctx+0x1d0)])
        address=struct.unpack('<Q',cpu.mem_read(ctx+0x1e0,8))[0]
        assert address==0x12345678000+(index%3)*0x1000
        producer.append({'kind':'ring_advance_wrap','allocation_index':index,'record_address':hex(address),
                         'next_current':hex(struct.unpack('<Q',cpu.mem_read(pool+0x10,8))[0])})
    # Actual caller block + complete 0x4e7edc parameter builder, intercepted at
    # final pixel driver. Test dimensions and address transport independently.
    pipeline=[];mode='pipeline'
    address64=0x12345678000
    for width,height in [(7008,4672),(1920,1080)]:
        for fmt in range(1,6):
            for enabled in (False,True):
                reset()
                cpu.mem_write(ctx+0x13b4,struct.pack('<I',owner))
                cpu.mem_write(ctx+0x1b8,struct.pack('<HH',width,height))
                cpu.mem_write(ctx+0x1c0,struct.pack('<I',fmt))
                cpu.mem_write(ctx+0x1d4,struct.pack('<H',int(enabled)))
                cpu.mem_write(0x172fad0,struct.pack('<I4xQQII',0x43f,address64,address64,0x3000,0x1000))
                run(0x4d3728,registers=[(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,0x295),(UC_ARM_REG_R2,ctx+0x1d0)])
                # Disabled fixture explicitly overrides allocator's valid flag.
                cpu.mem_write(ctx+0x1d4,struct.pack('<H',int(enabled)))
                cpu.mem_write(ctx+0x1500,struct.pack('<I',0x5555))
                # Real earlier router returns family +0x80 here. Populate four
                # synthetic metadata records at those actual global locations.
                records=[0x1a6a144+0x80+0x150*i for i in range(4)]
                cpu.mem_write(sp+0xb0,struct.pack('<4I',*records))
                for k,p in enumerate(records):cpu.mem_write(p,struct.pack('<9I',*[0x1000+k*0x100+j for j in range(9)]))
                cpu.mem_write(sp+0x34,struct.pack('<I',ctx+0x1b8))
                for offset in (0x1c4,0x234,0x2a4,0x314):cpu.mem_write(ctx+offset,b'\1')
                run(0x4c9946,0x4c99e0,[(UC_ARM_REG_R4,ctx),(UC_ARM_REG_R5,ctx+0x1500),(UC_ARM_REG_R11,0)])
                assert len(events)==1 and events[0]['owner_at_final_pointer']==hex(owner)
                for k,port in enumerate(events[0]['ports']):
                    assert port['address_u64_at_160']==hex(address64 if enabled else 0)
                    assert port['geometry_words_168_16c']==[width,height]
                    words=[int(v,16)for v in port['all_words']]
                    assert words[0x18//4]==0x1002+k*0x100
                    assert words[0x170//4]==0x1005+k*0x100
                pipeline.append({'synthetic_dimensions':[width,height],'context_format_word_1c0':fmt,
                                 'context_halfword_1d4_nonzero':enabled,'context_address_u64_1e0':hex(address64),
                                 'address_producer':'actual4d3728(selector295) ->b2cc72(pool172fad0) ->context+1e0; disabledcasesoverridevalidflag',
                                 'driver_call':events[0]})
    # Native JPEG handler's actual inner-command gate. Actual class vtable
    # is used; lifecycle/delivery entry is intercepted before any work executes.
    protocol=[];mode='protocol'
    for command in (7,1,2):
        reset();obj,msg=arena+0x11000,arena+0x12000
        cpu.mem_write(obj,struct.pack('<I',0xe70d48))
        cpu.mem_write(msg+0x24,struct.pack('<H',command))
        run(0xaf1bdc,registers=[(UC_ARM_REG_R0,obj),(UC_ARM_REG_R1,msg)])
        expected={'selected_virtual_entry':'0xaf4c06' if command==7 else '0x45f4c4','object':hex(obj),
                  'second_argument':hex(msg) if command==7 else '0x2'}
        assert events==[expected]
        protocol.append({'raw_message_halfword_24':command,'events':list(events)})
    ranges={
      'JPEG_inner_command7_gate':(0xaf1bdc,0xaf1bfe),
      'JPEG_raw_plus_1c_header_bridge':(0xaf4df4,0xaf4e46),
      'JPEG_diagnostic_plus_1c_header_bridge':(0x464108,0x46412e),
      'selected_fallback_address_producer_branch':(0x4c8ad2,0x4c8b44),
      'ring_startup_initialization':(0x4d3680,0x4d36e4),
      'resource_backed_ring_initializer':(0xb2cc16,0xb2cc72),
      'ring_selector':(0x4d3728,0x4d3780),
      'ring_record_and_advance':(0xb2cc72,0xb2ccf6),
      'selected_fallback_context_entry':(0x4c79c0,0x4c79e6),
      'selected_fallback_geometry_and_LUT_builder':(0x4c7d70,0x4c7d88),
      'selected_fallback_resource_query_setup':(0x4c8718,0x4c87c0),
      'resource_query_helper':(0xb30e92,0xb310d0),
      'resource_query_service_boundary':(0x46f9c4,0x46fa88),
      'selected_fallback_LUT_callback_registration':(0x4c8e62,0x4c8e7e),
      'same_context_LUT_then_pixel_callback':(0xb2b57c,0xb2b5d6),
      'pixel_context_entry_and_resource_router_call':(0x4c96b0,0x4c9742),
      'pixel_context_dimension_pointer_setup':(0x4c9810,0x4c98a4),
      'pixel_context_owner_dimensions_and_address_call':(0x4c9946,0x4c99e0),
      'resource_descriptor_router_wrapper':(0xb2dcec,0xb2dd24),
      'resource_descriptor_router':(0xb2db88,0xb2dcbe),
      'complete_pixel_parameter_builder':(0x4e7edc,0x4e86c4),
      'LUT_consumer_owner_entry':(0x4e876c,0x4e8788),
      'pixel_driver_same_owner_to_IP_lookup':(0x54d9c8,0x54da2e),
    }
    result={'source':{'path':str(path.relative_to(ROOT)),'sha256':DIGEST,'bytes':len(blob)},
      'address_model':'Retained CP file-backed runtime addresses equal file offsets +0x108000; odd pointers select Thumb.',
      'camera_accessed':False,'target_executed':False,'host_emulation_performed':True,
      'scope':'No USB, network, driver, DMA, codec, image processing or saved-file operation executes.',
      'observed':{
        'JPEG_message_alignment':{'class_vtable':'ApiEncodeJpeg vtable 0xe70d48: slot +8 points to 0xaf1bdd; slot +0x4c points to 0xaf4c07',
           'inner_gate':'0xaf1bdc reconstructs a u16 from raw+0x24/+0x25 and routes command 7 to virtual +0x4c lifecycle; command 1/other takes error/delivery path.',
           'header_bridge':'0xaf4df4 copies the first 0x34 raw bytes and passes copy+0x1c to header printer 0xaefcdc. Independent 0x464108 passes raw+0x1c to header printer 0x45f954.',
           'cross_processor_comparison':'Parent-reported AP layout has inner command 7 at wire+8 and input YC triple at wire+0x18/+0x1c/+0x20. CP inner+0x24 and input+0x34/+0x38/+0x3c agree with a uniform +0x1c displacement. AP bytes are not independently validated by this generator.',
           'qualification':'The CP inner-7 gate and +0x1c header references are concrete. Receive demultiplexing from AP category 0x2000/message 4 to this object, and actual prefix construction, are not traced here.'},
        'same_context_chain':['selected fallback 0x4c79c0 context=*sequence_object','register callback0xb2b57d with context userdata',
          'callback invokes MLUT4e876c(owner=context+13b4,record=context+13b8)','callback invokes4c96b0(samecontext)',
          '4c96b0 passes sameowner/context dimensions/address to4e7edc','4e7edc builds port records and calls54d9c8',
          '54d9c8 resolves owner through54d788, same mapper used by MLUT'],
        'resource_owner_query':'Fallback passes context+13b4 as output argument to46f9c4(selector0x10), routed throughb30e92.',
        'address_producer':{'pools':['0x172fad0','0x172faf0'],'selected_context_record':'4d3728 selector=context+1c8, output=context+1d0 inretainedcallerbranch',
          'fields':'b2cc72 record+0=ffffffff,+4u16=1 ifaddressandstridearenonzero,+8=pooltoken,+10u64=currentaddress,+18=stride; poolcurrentadvancesstrideandwraps.',
          'resource_initialization':'b2cc16 calls511cb4/511edc(token,output,5); responsefieldsfillpoolbase/current/capacity/stride. Queryserviceandnativebufferidentityunresolved.'},
        'pixel_descriptor_transport':{'conditional_address':'context halfword+1d4 nonzero -> pass context+1e0 pointer; else NULL',
          'geometry_source':'context halfwords+1b8/+1ba','four_port_blocks':'Each0x318bytes; address copied to+0x160, geometry to32bit+0x168/+0x16c',
          'metadata_tree':'0xb2dcec/0xb2db88 constructs global1a6a144 pointer families; pixel caller uses family+80 with0x150stride.',
          'qualification':'Address direction (source/destination), actual frame dimensions, native SDYC unit ownership and live port selection remain unresolved.'}},
      'host_tests':{'protocol_gate_cases':protocol,'callback_cases':callback,'resource_query_case':resource,'descriptor_router_case':router,'producer_cases':producer,'pixel_parameter_cases':pipeline,
        'stubs':{'protocol':['af4c06 lifecycle and45f4c4 delivery entries captureargumentsandreturn0'], 'producer':['61ed00 hostzero','511cb4/511edc syntheticbase/capacity/stride response','b44b96 recordconstructor returns0'], 'callback':['b2bf66 returns fixturepredicate','4e876c captures arguments','4c96b0 captures context'],
                 'resource':['b0a4a0 returns0','46f9c4 captures pointer and writes synthetic owner/result'],
                 'pipeline':['46f8e8 returns1 enabling modeled parameter branches','bb30ce,b0a308,4bce70,b61ae2 diagnostics/ancillary return0',
                             'b436e4,b4457a diagnostics intercepted','b2ddb4 returns fixture descriptor withtaildiagnostic disabled',
                             '54d9c8 captures parameter blocks andreturns0; no driver instruction executes']},
        'qualification':'Synthetic fixtures verify instruction-level transport and identity, not live graph execution, valid buffer layout, memory capacity, cache coherence, MLUT application or a camera capture.'},
      'unresolved':['No continuous dataflow from these port addresses to native JPEG message inputYcSdsUnitType/Handle/DataOffset at+34/+38/+3c.',
        'No descriptor relation between MLUT owner, physical address candidate, and logical SDYC/SDS unit handle.',
        'No proof which direction port+160 represents, which live modes select the graph, or whether7008x4672 is supplied by a live frame.',
        'No proof MLUT applies to the encoded image or that the JPEG/HEIF saved-file queue consumes these buffers.'],
      'disassembled_regions':{name:region(*r)for name,r in ranges.items()},
      'verification':{'SHA256_pinned':True,'decoded_regions':len(ranges),'host_cases':len(callback)+1+1+len(pipeline)+len(producer)+len(protocol)}}
    out=ROOT/'research/evidence/mlut-encoder-link.json';out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'output':str(out.relative_to(ROOT)),**result['verification']}))


if __name__=='__main__':main()
