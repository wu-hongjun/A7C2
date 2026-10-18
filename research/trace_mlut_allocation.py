"""Offline CP MLUT pixel allocation modes, DMM provenance and conditional share.

Original Thumb instructions execute against synthetic host RAM. External queues,
allocation, DMM replies and diagnostics are intercepted; no target/hardware runs.
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
    cs=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
    def region(start,end):
        ins=list(cs.disasm(blob[start-BASE:end-BASE],start))
        assert sum(i.size for i in ins)==end-start,(hex(start),hex(end))
        return {'runtime_start':hex(start),'runtime_end_exclusive':hex(end),'file_start':hex(start-BASE),
                'ISA':'Thumb-2','instructions':[{'runtime_address':hex(i.address),'file_offset':hex(i.address-BASE),
                 'bytes':i.bytes.hex(),'operation':i.mnemonic,'operands':i.op_str}for i in ins]}
    arena,stack,stop=0x5000000,0x6000000,0x7000000
    cpu=Uc(UC_ARCH_ARM,UC_MODE_THUMB);cpu.mem_map(BASE,(len(blob)+4095)&~4095);cpu.mem_write(BASE,blob)
    for a,n in [(arena,0x20000),(stack,0x10000),(stop,0x1000),(0x1a53000,0x1000),(0x1ac5000,0x1000),
                (0x1acb000,0x1000),(0x1b1e000,0x1000),(0x172f000,0x1000),(0x128a000,0x1000),(0x1725000,0x1000)]:cpu.mem_map(a,n)
    ctx=arena+0x1000;sp=stack+0x8000;message=arena+0x18000
    events=[];count=2;fault=None
    allocated_handle=0xa0012345;shared_handle=0xb0012345
    pixel_base=0x500100000
    def hook(u,a,n,_):
        value=0
        if a==0x61ed00:
            p=u.reg_read(UC_ARM_REG_R0);n=u.reg_read(UC_ARM_REG_R1);u.mem_write(p,b'\0'*n);value=p
        elif a==0x61e60c:
            p=u.reg_read(UC_ARM_REG_R0);q=u.reg_read(UC_ARM_REG_R1);n=u.reg_read(UC_ARM_REG_R2)
            u.mem_write(p,bytes(u.mem_read(q,n)));value=p
        elif a==0x565d24:
            out=u.reg_read(UC_ARM_REG_R2)
            if u.reg_read(UC_ARM_REG_R0)==0x2a and out:u.mem_write(out,struct.pack('<I',0x90550001))
        elif a==0x565204:
            destination=u.reg_read(UC_ARM_REG_R0);out=u.reg_read(UC_ARM_REG_R1);length=u.reg_read(UC_ARM_REG_R2)
            assert destination==0x61003c and length==0xd8
            u.mem_write(message-0x20,b'\0'*0x100);u.mem_write(out,struct.pack('<I',message))
            events.append({'operation':'synthetic_message_allocation','destination':hex(destination),'bytes':length})
        elif a==0x565300:pass
        elif a==0xbec01c:
            destination=u.reg_read(UC_ARM_REG_R0);p=u.reg_read(UC_ARM_REG_R1)
            assert destination==0x61003c and p==message
            body=bytes(u.mem_read(p,0xd8));command=struct.unpack_from('<H',body)[0]
            assert command in(0x18,0x1a,0x21,0x26)
            token=struct.unpack_from('<I',body,8 if command==0x1a else 4)[0]
            event={'operation':'synthetic_DMM_exchange','destination':hex(destination),'command':hex(command),
                   'token_field':hex(token),'request_bytes':body.hex()}
            if command==0x21:event['source_handle']=hex(struct.unpack_from('<I',body,8)[0])
            events.append(event)
            response=bytearray(0xd8)
            struct.pack_into('<Hh',response,0,command|0x8000,-1 if fault=='negative_status' else 0)
            struct.pack_into('<H',response,6,0x56 if fault=='wrong_sequence' else 0x55)
            if command==0x1a:
                struct.pack_into('<I',response,0xc,0x3000);struct.pack_into('<Q',response,0x10,pixel_base)
            elif command==0x18:struct.pack_into('<I',response,0x10,0x1000)
            else:
                handle=allocated_handle if command==0x26 else shared_handle
                if fault=='invalid_handle':handle=0xffffffff
                struct.pack_into('<H',response,8,count);struct.pack_into('<I',response,0xc,handle)
                for index in range(count):struct.pack_into('<QI',response,0x18+16*index,pixel_base+0x10000+index*0x1000,0x1000)
            u.mem_write(p,bytes(response));event['response_bytes']=response.hex()
        elif a==0xb24e6c:
            events.append({'operation':'bookkeeping_word_boundary','word':hex(struct.unpack('<I',u.mem_read(0x17259f0,4))[0])})
        elif a==0x4d111c:value=1
        elif a in(0xb4457a,0xb4361c):pass
        else:return
        u.reg_write(UC_ARM_REG_R0,value);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR))
    cpu.hook_add(UC_HOOK_CODE,hook);initial=cpu.context_save()
    def reset():
        cpu.context_restore(initial);cpu.mem_write(arena,b'\0'*0x20000);cpu.mem_write(stack,b'\0'*0x10000);events.clear()
        for a,n in [(0x1a53000,0x1000),(0x1ac5000,0x1000),(0x1acb000,0x1000),(0x1b1e000,0x1000),(0x128a000,0x1000),(0x1725000,0x1000)]:cpu.mem_write(a,b'\0'*n)
        cpu.mem_write(0x10f7e28,struct.pack('<I',0xffffffff))
    def execute(address,args=(),end=stop,registers=(),stack_words=(),budget=30000):
        cpu.reg_write(UC_ARM_REG_SP,sp);cpu.reg_write(UC_ARM_REG_LR,stop|1)
        for r,v in zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3],args):cpu.reg_write(r,v)
        for r,v in registers:cpu.reg_write(r,v)
        for i,v in enumerate(stack_words):cpu.mem_write(sp+4*i,struct.pack('<I',v))
        cpu.emu_start(address|1,end,count=budget)
        assert cpu.reg_read(UC_ARM_REG_PC)==end,hex(cpu.reg_read(UC_ARM_REG_PC))
        return cpu.reg_read(UC_ARM_REG_R0)
    def record(pointer):
        data=bytes(cpu.mem_read(pointer,0x50))
        n=struct.unpack_from('<H',data,4)[0]
        return {'handle':hex(struct.unpack_from('<I',data)[0]),'extent_count':n,'owning_byte_6':data[6],
                'token_at_8':hex(struct.unpack_from('<I',data,8)[0]),
                'extents':[{'address':hex(struct.unpack_from('<Q',data,0x10+16*i)[0]),'bytes':struct.unpack_from('<I',data,0x18+16*i)[0]}for i in range(min(n,4))],
                'bytes':data.hex()}
    gates=[]
    for mask in(0,1):
        for cfg in(0,1):
            for state in(0,1):
                reset();cpu.mem_write(0x1ac5694,bytes([mask]));cpu.mem_write(0x1a53487,bytes([cfg]));cpu.mem_write(0x1b1e1e5,bytes([state]))
                ret=execute(0xb2cbde,[1]);assert ret==int(bool(mask and(cfg or state)))
                gates.append({'argument_mask':1,'global_1ac5694':mask,'global_1a53487_low_nibble':cfg,'global_1b1e1e5':state,'ring_branch_predicate':ret})
    pools=[]
    for token in(0x43f,0x2d9,0x4c4):
        reset();fault=None
        ret=execute(0xb2cc16,[0x172fad0,token]);assert ret==1
        fields=struct.unpack('<I4xQQII',cpu.mem_read(0x172fad0,32))
        assert fields==(token,pixel_base,pixel_base,0x3000,0x1000)
        assert[e['command']for e in events if e['operation']=='synthetic_DMM_exchange']==['0x1a','0x18']
        pools.append({'token':hex(token),'pool_words':[hex(v)for v in fields],'events':list(events)})
    allocations=[]
    for token in(0x43f,0x2d9,0x4c4):
        for count in(0,1,2,4,5):
            reset();fault=None
            ret=execute(0xb449a2,[ctx+0x1d0,token,0x500,1],stack_words=[0,5])
            if count==0:assert ret==0xfffffff9
            else:
                assert ret==0
                actual=record(ctx+0x1d0)
                assert actual['handle']==hex(allocated_handle) and actual['token_at_8']==hex(token)
                assert actual['extent_count']==min(count,4) and actual['owning_byte_6']==1
                assert actual['extents'][0]=={'address':hex(pixel_base+0x10000),'bytes':0x1000}
            allocations.append({'selector_token':hex(token),'synthetic_response_extent_count':count,'return_u32':hex(ret),
                                'record':record(ctx+0x1d0),'events':list(events)})
    count=2
    for fault in('wrong_sequence','invalid_handle'):
        reset();ret=execute(0xb449a2,[ctx+0x1d0,0x43f,0x500,1],stack_words=[0,5])
        assert ret==({'wrong_sequence':0xfffffffb,'invalid_handle':0xfffffffa}[fault])
        allocations.append({'synthetic_fault':fault,'return_u32':hex(ret),'events':list(events)})
    branches=[];fault=None
    for token in(0x43f,0x2d9,0x4c4,0x63d):
        for ring_mode in(False,True):
            if token==0x63d and ring_mode:continue
            reset()
            pool=0x172faf0 if token==0x4c4 else 0x172fad0
            assert execute(0xb2cc16,[pool,token])==1
            cpu.mem_write(ctx+0x1b0,struct.pack('<I',1));cpu.mem_write(ctx+0x1c8,struct.pack('<I',token))
            cpu.mem_write(ctx+0x1d0,struct.pack('<I',0xffffffff))
            cpu.mem_write(0x1ac5694,bytes([int(ring_mode)]));cpu.mem_write(0x1a53487,b'\1')
            cpu.mem_write(sp+0x3c,struct.pack('<I',4));events.clear()
            execute(0x4c8ad2,end=0x4c8b4a,registers=[(UC_ARM_REG_R11,ctx),(UC_ARM_REG_R5,0),(UC_ARM_REG_R9,0)])
            actual=record(ctx+0x1d0)
            if ring_mode:
                assert actual['handle']=='0xffffffff' and actual['owning_byte_6']==0 and actual['extent_count']==1
                assert actual['extents'][0]=={'address':hex(pixel_base),'bytes':0x1000}
                assert not[e for e in events if e['operation']=='synthetic_DMM_exchange']
            else:
                assert actual['handle']==hex(allocated_handle) and actual['owning_byte_6']==1
                assert actual['extents'][0]['address']==hex(pixel_base+0x10000)
                assert[e['command']for e in events if e['operation']=='synthetic_DMM_exchange']==['0x26']
            branches.append({'selector_token':hex(token),'synthetic_ring_mode':ring_mode,'record':actual,'events':list(events)})
    shares=[]
    for state,valid in[(0,True),(2,True),(2,False)]:
        reset();source_handle=allocated_handle if valid else 0xffffffff
        cpu.mem_write(0x128ac74,struct.pack('<I',state))
        if valid:
            # Continuous original selected-fallback fragment writes selector63d,
            # allocates its real handle-bearing record, then shares that record.
            cpu.mem_write(ctx+0x1b0,struct.pack('<I',1))
            cpu.mem_write(ctx+0x1d0,struct.pack('<I',0xffffffff))
            cpu.mem_write(arena+0x6060,struct.pack('<I',arena+0x6100));cpu.mem_write(arena+0x6120,b'\1')
            cpu.mem_write(sp+0x3c,struct.pack('<I',4))
            execute(0x4c8a9c,end=0x4c8b60,registers=[(UC_ARM_REG_R11,ctx),(UC_ARM_REG_R10,arena+0x6000),(UC_ARM_REG_R9,0)])
            assert record(ctx+0x1d0)['handle']==hex(source_handle)
            assert record(ctx+0x1d0)['token_at_8']=='0x63d'
        else:
            cpu.mem_write(ctx+0x1d0,struct.pack('<I',source_handle));cpu.mem_write(ctx+0x1d8,struct.pack('<I',0x63d))
            execute(0x4c8b52,end=0x4c8b60,registers=[(UC_ARM_REG_R11,ctx),(UC_ARM_REG_R9,1)])
        cached=struct.unpack('<I',cpu.mem_read(0x10f7e28,4))[0]
        published_state=struct.unpack('<I',cpu.mem_read(0x128ac74,4))[0]
        if state==2 and valid:
            assert cached==shared_handle and published_state==3
            exchange=[e for e in events if e['operation']=='synthetic_DMM_exchange']
            assert len(exchange)==2 and [e['command']for e in exchange]==['0x26','0x21'] and exchange[1]['source_handle']==hex(source_handle)
            words=[e['word']for e in events if e['operation']=='bookkeeping_word_boundary']
            assert words==['0xffff0003',hex(shared_handle),'0x63d','0x1000']
        else:assert cached==0xffffffff
        shares.append({'synthetic_state':state,'source_handle':hex(source_handle),'cached_shared_handle':hex(cached),
                       'resulting_state':published_state,'continuous_allocation_then_share_fragment_executed':valid,'events':list(events)})
    ranges={
      'selected_fallback_special_selector_and_continuous_share':(0x4c8a9c,0x4c8b60),
      'allocation_mode_predicate':(0xb2cbde,0xb2cc16),
      'selected_fallback_two_allocator_branches':(0x4c8ad2,0x4c8b4a),
      'pool_startup_tokens':(0x4d3680,0x4d36e4),
      'pool_initializer':(0xb2cc16,0xb2cc72),
      'direct_ring_record':(0xb2cc72,0xb2ccf6),
      'DMM_record_allocator':(0xb449a2,0xb44a2c),
      'DMM_AllocNewRdv_command26':(0x510b04,0x510cf0),
      'DMM_extent_info_command1a':(0x511cb4,0x511e36),
      'DMM_unit_size_command18':(0x511edc,0x51205e),
      'synchronous_message_constructor':(0x638176,0x6381ba),
      'message_buffer_allocator':(0x512b24,0x512b98),
      'record_cleanup':(0xb44b96,0xb44be2),
      'conditional_context_record_share_call':(0x4c8b52,0x4c8b60),
      'conditional_share_and_handle_cache':(0x73b086,0x73b14a),
      'share_record_wrapper':(0xb44abc,0xb44b56),
      'DMM_AllocShare_command21':(0x510d7c,0x510f50),
      'shared_handle_bookkeeping':(0xb231e0,0xb2323c),
      'shared_handle_cache_release':(0x73af56,0x73af72),
      'shared_handle_cache_allocation_guard':(0x73afac,0x73b082),
    }
    result={'source':{'path':str(path.relative_to(ROOT)),'sha256':DIGEST,'bytes':len(blob)},
      'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},
      'camera_accessed':False,'target_executed':False,'host_emulation_performed':True,
      'scope':'No camera, USB, network, native DMM allocation, queue, target process, pixel read, driver or codec executes.',
      'observed':{
        'allocation_modes':'Selected fallback tests b2cbde(mask1). True uses direct ring record; false calls b449a2/DMMAllocNewRdv to obtain an actual handle-bearing record.',
        'mode_predicate':'For maskm, ((byte[1ac5694]&m)&(byte[1b1e1e5]|(byte[1a53487]&0xf)))==m. Live global values are unobserved.',
        'record_schema':{'offset0':'DMM handle or ffffffff','offset4_u16':'extent count','offset6_u8':'ownership/cleanup flag','offset8':'selector/pool token','offset10':'first u64 address','offset18':'first u32 extent length; more slots stride16'},
        'DMM_destination':'Both pool queries and handle allocation/share send to0x61003c. Earlier SDFquery24 also uses that destination; common endpoint does not equate keys.',
        'commands':{'0x1a':'GetExtentAreaInfo: token request+8; response base+10u64 and capacity+c are used by pool initializer.',
                    '0x18':'GetUnitSize: token request+4; response+10 is used as ring stride.',
                    '0x26':'AllocNewRdv: selector request+4; reply handle+c, extent count+8, first base+18u64/length+20.',
                    '0x21':'AllocShare: request token+4/sourceHandle+8; reply handle+c and extent table populate shared record.'},
        'sequence':'These wrappers check response command and sequence55; unlike SDFquery24 generated sequences.',
        'share_consumer':'Conditional selected-fallback call passes C+1d0 to73b086. State2 requests a share via b44abc/510d7c using actual record handle/token, caches returned handle at10f7e28, state->3, and sends bookkeeping words throughb231e0.',
        'cache_limits':'Other retained neighbors close cached handle or skip/reallocate depending on cache sentinel. No export to AP SDYC metadata, JPEG input or saved image is established.'},
      'host_tests':{'mode_predicate_cases':gates,'pool_protocol_cases':pools,'handle_allocation_cases':allocations,
                    'actual_branch_cases':branches,'conditional_share_cases':shares,
        'stubs':['565d24 returns synthetic reply queue handle;565204 supplies host message storage;565300 cleanup intercepted',
                 'bec01c synthetic DMM response at actual destination61003c; no real queue/service executes',
                 '61e60c hostmemcpy;61ed00 hostzero','b4457a/b4361c diagnostics intercepted',
                 'b24e6c captures bookkeeping words; no notification/telemetry recipient executes','4d111c returns1 toselect special63d branch in continuousallocation/share fixtures'],
        'qualification':'Different fixture handle/base values intentionally preserve distinction between selector, returned handle and address. Service replies do not prove live aliasing or shared physical allocation.'},
      'unresolved':['No observed live allocation-mode globals or live unit handles.',
         'No actual writer from C+1d0 or cached shared handle10f7e28 into AP av-cam encoder context+41c/+434 or nested SSYC pixel descriptor.',
         'No equality between static pool token43f/2d9/4c4, logical pixel handle, and its queried extent table.',
         'No semantic identity for conditional73b086 state machine or proof it belongs to saved-photo encoding.',
         'No pixel-port direction, live full-resolution selection or JPEG/HEIF saved-buffer ownership proof.'],
      'disassembled_regions':{n:region(*r)for n,r in ranges.items()},
      'verification':{'SHA256_pinned':True,'decoded_regions':len(ranges),'host_cases':len(gates)+len(pools)+len(allocations)+len(branches)+len(shares)}}
    out=ROOT/'research/evidence/mlut-allocation.json';out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'output':str(out.relative_to(ROOT)),**result['verification']}))


if __name__=='__main__':main()
