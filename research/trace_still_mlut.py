"""Bounded offline CP callback lifecycle, MLUT resources and dimensions.

No USB, target process or camera execution. Selected CP instructions are
emulated with host-memory stand-ins and named external-call stubs.
"""
from collections import deque
import hashlib
import json
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB, CS_GRP_JUMP
from capstone.arm import ARM_OP_IMM, ARM_REG_R11, ARM_REG_R2
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *

ROOT=Path(__file__).resolve().parents[1]
BASE=0x108000
DIGEST="f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428"


def main():
    path=next((ROOT/"research/extracted/ILCE-7CM2-2.01").rglob("cpapp-b.bin"))
    blob=path.read_bytes()
    assert hashlib.sha256(blob).hexdigest()==DIGEST
    decoder=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
    decoder.detail=True
    def decode(lo,hi):
        items=list(decoder.disasm(blob[lo:hi],BASE+lo))
        assert sum(i.size for i in items)==hi-lo,(hex(lo),hex(hi))
        return items
    def region(lo,hi):
        return {"file_start":hex(lo),"file_end_exclusive":hex(hi),"runtime_start":hex(BASE+lo),
                "ISA":"Thumb-2","instructions":[{"file_offset":hex(i.address-BASE),"runtime_address":hex(i.address),
                  "bytes":i.bytes.hex(),"operation":i.mnemonic,"operands":i.op_str} for i in decode(lo,hi)]}
    # Register preservation and syntactic control-flow existence, not proof of
    # branch feasibility for a particular live frame or a modeled OS scheduler.
    preservation=decode(0x3bde30,0x3bef8e)
    assert not [i for i in preservation if ARM_REG_R11 in i.regs_access()[1]]
    assert not [i for i in decode(0x3beed2,0x3bef8a) if ARM_REG_R2 in i.regs_access()[1]]
    graph={}
    by_address={i.address:i for i in preservation}
    for ins in preservation:
        fall=ins.address+ins.size
        if ins.mnemonic.startswith("pop") and "pc" in ins.op_str:
            successors=[]
        elif ins.mnemonic=="bx" or ins.mnemonic in ("tbb","tbh"):
            successors=[]
        elif ins.group(CS_GRP_JUMP):
            dest=[o.imm for o in ins.operands if o.type==ARM_OP_IMM]
            successors=dest+([] if ins.mnemonic in ("b","b.w") else [fall])
        else:
            successors=[fall]
        graph[ins.address]=[s for s in successors if s in by_address]
    start,target=BASE+0x3bde36,BASE+0x3bef8a
    queue=deque([start]);previous={start:None}
    while queue and target not in previous:
        at=queue.popleft()
        for nxt in graph[at]:
            if nxt not in previous:
                previous[nxt]=at;queue.append(nxt)
    assert target in previous
    route=[];at=target
    while at is not None:
        route.append(at);at=previous[at]
    route.reverse()

    arena,stack,finish=0x5000000,0x6000000,0x7000000
    cpu=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
    cpu.mem_map(BASE,(len(blob)+4095)&~4095);cpu.mem_write(BASE,blob)
    for address,size in [(arena,0x20000),(stack,0x10000),(finish,0x1000),
                         (0x1a69000,0x2000),(0x1a76000,0x1000),(0x1a7e000,0x1000)]:
        cpu.mem_map(address,size)
    register_bases=[0xf4009000+i*0x100000 for i in range(7)]
    for address in register_bases:cpu.mem_map(address,0x1000)
    seen=[]
    node,context=arena+0x1000,arena+0x4000
    def hook(engine,address,size,user):
        result=0
        if address==0xb2953a:result=node
        elif address==0x4e876c:
            seen.append({"resource_argument":engine.reg_read(UC_ARM_REG_R0),
                         "record_pointer":hex(engine.reg_read(UC_ARM_REG_R1))})
        elif address in (0xb2ffd8,0x4e1f14,0x5ee5a0,0x5ee450,0x5ee988,0x5ee8a8,0xb7d358):pass
        elif address==0x5f267c:result=1
        elif address==0xb7a0aa:result=3
        else:return
        engine.reg_write(UC_ARM_REG_R0,result)
        engine.reg_write(UC_ARM_REG_PC,engine.reg_read(UC_ARM_REG_LR))
    cpu.hook_add(UC_HOOK_CODE,hook)
    initial=cpu.context_save()
    def execute(address,args=(),until=finish,extra_registers=()):
        cpu.context_restore(initial)
        cpu.mem_write(stack,b"\0"*0x10000)
        cpu.reg_write(UC_ARM_REG_SP,stack+0x8000);cpu.reg_write(UC_ARM_REG_LR,finish|1)
        for register,value in zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3],args):cpu.reg_write(register,value)
        for register,value in extra_registers:cpu.reg_write(register,value)
        cpu.emu_start(address|1,until,count=30000)
        assert cpu.reg_read(UC_ARM_REG_PC)==until
        return cpu.reg_read(UC_ARM_REG_R0)
    head=0x1a69b34
    # Actual setup materializes the callback pair and passes the context.
    # Owner/refcount fixture is null; allocation and final imaging are stubbed.
    cpu.mem_write(head,struct.pack("<4I",head,head,0,0))
    cpu.mem_write(context+0x13b4,struct.pack("<I",0x12345678))
    execute(BASE+0x3bee7a,until=BASE+0x3bef8e,
            extra_registers=[(UC_ARM_REG_R11,context),(UC_ARM_REG_R9,0)])
    words=struct.unpack("<10I",cpu.mem_read(node,40))
    assert words[:2]==(head,head) and words[2:5]==(0xb2b4ad,0,0xb2b4c3)
    assert words[7]==context
    execute(BASE+0x3b750e,until=BASE+0x3b754e)
    assert seen==[{"resource_argument":0x12345678,"record_pointer":hex(context+0x13b8)}]
    lifecycle_test={"cases":1,"synthetic_context_pointer":hex(context),"actual_list_node_words":[hex(w)for w in words],
                    "observed_final_consumer_arguments":seen,
                    "qualification":"Allocator, final consumer, ancillary update and tail continuation are stubbed; actual callback setup/registration/list insertion/dispatch/adapter/caller instructions execute on the host. Owner/refcount fixture is null. This is not a captured camera frame."}

    # Synthesize ownership records; the real bitmap-to-IP lookup executes.
    resource_cases=[]
    for direction in (0,1):
        for mask in range(16):
            for slot in range(4):
                cpu.mem_write(0x1a7e578+8*slot,struct.pack("<II",0x7777 if mask>>slot&1 else 0,
                                                                      direction+1 if mask>>slot&1 else 0))
            cpu.mem_write(arena+0x700,struct.pack("<I",0xeeeeeeee))
            result=execute(0x54d788,[direction,0x7777,arena+0x700])
            value=struct.unpack("<I",cpu.mem_read(arena+0x700,4))[0]
            expected={1:0,2:1,4:2,8:3,15:4,3:5,12:6}.get(mask)
            assert result==(0 if expected is not None else 5)
            assert value==(expected if expected is not None else 0xeeeeeeee)
            resource_cases.append({"direction_selector":direction,"matched_ownership_slot_bitmap":hex(mask),
                                   "return_code":result,"MLUT_IP_index":expected})
    table=struct.unpack_from("<14I",blob,0xe9d588-BASE)
    assert list(table)==[v for i in range(7)for v in (i,i)]
    dimension_cases=[]
    dimensions=[(7008,4672),(4672,7008),(0,0),(32767,32767),(32768,1),(1,32768)]
    for ip in range(8):
        for width,height in dimensions:
            cpu.mem_write(arena+0x800,struct.pack("<II",width,height))
            if ip<7:
                cpu.mem_write(register_bases[ip]+0x50,struct.pack("<II",0xa5a58000,0x5a5a8000))
            result=execute(0xb78c1c,[ip,arena+0x800])
            accepted=ip<7 and width<32768 and height<32768
            assert result==(0 if accepted else 0xfffffff3)
            row={"IP_index":ip,"supplied_width":width,"supplied_height":height,"accepted":accepted,
                 "return_u32":hex(result)}
            if ip<7:
                values=struct.unpack("<II",cpu.mem_read(register_bases[ip]+0x50,8))
                expected=((0xa5a58000|width,0x5a5a8000|height) if accepted else (0xa5a58000,0x5a5a8000))
                assert values==expected
                row["host_memory_register_words"]=[hex(v)for v in values]
            dimension_cases.append(row)
    # The sole found sequence-pointer construction is gated on this property.
    # This pure leaf routine needs no OS/hardware stubs and exhausts u16 modes.
    recognized=[]
    extras=[]
    for mode in list(range(65536))+[0x10000,0x7fffffff,0x80000000,0xffffffff]:
        cpu.mem_write(arena+0x900,b"\0"*16)
        cpu.reg_write(UC_ARM_REG_R0,mode);cpu.reg_write(UC_ARM_REG_R1,0)
        cpu.reg_write(UC_ARM_REG_R2,arena+0x900);cpu.reg_write(UC_ARM_REG_LR,finish|1)
        cpu.emu_start(0x273a3d,finish,count=200)
        assert cpu.reg_read(UC_ARM_REG_PC)==finish
        values=struct.unpack("<4I",cpu.mem_read(arena+0x900,16))
        result=cpu.reg_read(UC_ARM_REG_R0)
        assert values[1]==0
        row={"mode":hex(mode),"recognized":bool(result),"output_words":list(values)}
        if result:recognized.append(row)
        if mode>=65536:extras.append(row)
    assert len(recognized)==30
    gate_jump_table=[BASE+0x16baaa+2*blob[0x16baaa+i]for i in range(5)]
    pointer_materializations=[];pending={}
    for offset in range(0x2017c,0xb27490-3,2):
        first,second=struct.unpack_from("<HH",blob,offset)
        if first&0xfbf0 not in (0xf240,0xf2c0) or second&0x8000:continue
        immediate=(first&15)<<12|(first&0x400)<<1|(second&0x7000)>>4|second&255
        register=second>>8&15
        if first&0x80:
            if register in pending:
                previous,low=pending[register]
                if offset-previous<=80 and immediate<<16|low==0x4c5a7d:
                    pointer_materializations.append([hex(previous),hex(offset)])
        else:pending[register]=(offset,immediate)
    assert pointer_materializations==[["0x3b8972","0x3b897e"],["0xa224be","0xa224c2"],["0xa2263c","0xa22648"]]
    assert struct.pack("<I",0x4c5a7d) not in blob
    ranges={
        "sequence_context_entry":(0x3bda7c,0x3bda9e),
        "same_context_builder_call":(0x3bde24,0x3bde3c),
        "same_context_preservation_to_consumer_registration":(0x3bde30,0x3bef8e),
        "consumer_callback_registration":(0x3bee7a,0x3bef8e),
        "callback_registrar":(0xa20c16,0xa20c5c),
        "callback_list_copy":(0xa214e2,0xa2153a),
        "callback_dispatch":(0x3b750e,0x3b755a),
        "callback_adapter":(0xa234ac,0xa234b2),
        "consumer_wrapper":(0xa234c2,0xa234e8),
        "identity_IP_lookup":(0xaac2da,0xaac302),
        "resource_bitmap_lookup_prefix":(0x445788,0x44589c),
        "resource_bitmap_lookup_results":(0x4458ac,0x4458d8),
        "resource_allocation_part":(0x4452d4,0x44535e),
        "resource_allocation_cases":(0x445366,0x4453b4),
        "dimension_wrapper":(0xa70c1c,0xa70c60),
        "dimension_validation":(0xa74d46,0xa74d6a),
        "dimension_setters_call":(0xa7a9f2,0xa7aa0c),
        "dimension_register_fields":(0xa7023e,0xa70296),
        "upstream_sequence_selection":(0x3b8932,0x3b89b0),
        "sequence_descriptor_submission":(0x3b8a6e,0x3b8a78),
        "mode_property_getter":(0x66ca60,0x66ca98),
        "mode_default_lookup_prefix":(0x16ba3c,0x16baaa),
        "mode_default_lookup_leaves":(0x16bab0,0x16bbba),
        "alternative_sequence_callback_setup":(0x3c0e62,0x3c0e7e),
        "alternative_sequence_builder_call":(0x3bfd82,0x3bfd8c),
        "alternative_sequence_entry":(0x3bf9c0,0x3bf9e6),
        "alternative_consumer_callback":(0xa2357c,0xa2359a),
        "alternative_geometry_initializer_call":(0x3bfd70,0x3bfd88),
        "geometry_context_initializer":(0xa241b8,0xa24260),
        "geometry_dependent_unit_format":(0xa2429a,0xa24344),
        "area_size_helper_prefix":(0x3cb554,0x3cb566),
        "area_size_helper_arithmetic":(0x3cb56a,0x3cb5a2),
    }
    report={"date":"2026-10-17","mode":"offline_static_and_bounded_host_emulation",
            "camera_accessed":False,"target_executed_on_camera":False,"host_emulation_performed":True,
            "source":{"path":str(path.relative_to(ROOT)),"sha256":DIGEST,"bytes":len(blob)},
            "address_model":"For these CP file-backed regions, runtime address = file offset + 0x108000.",
            "lifecycle":{"builder_runtime_address":"0xb2c406","consumer_runtime_address":"0x4e876c",
                         "sequence_function_runtime_entry":"0x4c5a7c",
                         "context_source":"fp = *(incoming sequence object)",
                         "context_register_redefinitions_after_builder_before_registration":[],
                         "syntactic_CFG_path_file_offsets":[hex(v-BASE)for v in route],
                         "CFG_qualification":"Calls treated as returning and callee-saved registers preserved; path existence does not prove a particular branch condition or actual frame.",
                         "callback_list_head":"0x1a69b34","node_userdata_offset":"0x1c",
                         "adapter":"0xb2b4ac","registered_callback":"0xb2b4c2",
                         "sequence_diagnostic":{"runtime_address":"0xfc7705","file_offset":hex(0xfc7705-BASE),
                                                "text":blob[0xfc7705-BASE:blob.index(b"\0",0xfc7705-BASE)].decode("ascii"),
                                                "qualification":"Diagnostic label only; not a still/movie classification."},
                         "result":"The same context register used by the builder is registered as callback userdata; the real list dispatcher and adapter forward that exact pointer to the consumer wrapper.",
                         "host_test":lifecycle_test},
            "resource_routing":{"ownership_table":"0x1a7e578","record_bytes":8,
                                "lookup_runtime_address":"0x54d788","host_test_cases":resource_cases,
                                "IP_identity_lookup":{"runtime_address":"0xbb42da","table_runtime_address":"0xe9d588",
                                                      "pairs":[[i,i]for i in range(7)]},
                                "unresolved":"Slot owners and direction selectors are numeric resource state; not yet identified with still/movie/display paths or simultaneous pixel streams."},
            "dimension_contract":{"wrapper":"0xb78c1c","validator":"0xb7cd46",
                                  "accepted_u32_range_per_dimension":[0,32767],
                                  "register_offsets":["0x50","0x54"],"field_width_bits":15,
                                  "host_test_cases":dimension_cases,
                                  "qualification":"Real instructions write only host-memory stand-ins for MMIO. 7008x4672 acceptance proves representability/API validation, not camera execution, actual hardware capability, routing or saved-image processing."},
            "upstream_sequence_classification":{
                "requested_sequence_pointer":"0x4c5a7d",
                "materialization_search":{"file_start":"0x2017c","file_end_exclusive":"0xb27490",
                                          "maximum_halfword_MOVW_MOVT_separation":80,
                                          "found_pairs":pointer_materializations,"absolute_pointer_word_occurrences":0,
                                          "limitation":"Halfword Thumb candidate scan, not complete recovery of ARM instructions, computed aliases or dynamic targets."},
                "found_materializations":[{"file_low":"0x3b8972","file_high":"0x3b897e","role":"Construct sequence callback when mode property equals 1"},
                                          {"file_low":"0xa224be","file_high":"0xa224c2","role":"Compare existing callback pointer during descriptor classification"},
                                          {"file_low":"0xa2263c","file_high":"0xa22648","role":"Compare existing callback pointer during descriptor classification"}],
                "selector_gate":"0x774a60(mode) == 1",
                "gate_getter":"Returns word at output+4 from 0x273a3c when recognized; otherwise logs and returns 0.",
                "mode_lookup_emulation":{"u16_cases":65536,"nonzero_property_cases":0,
                                         "recognized_modes":recognized,"u32_boundary_cases":extras,
                                         "qualification":"Original leaf-mode lookup executed on host. No code patches, OS callbacks or hardware operations; this is not a live camera mode observation."},
                "mode_lookup_jump_table":{"file_offset":"0x16baaa","bytes":blob[0x16baaa:0x16bab0].hex(),
                                          "first_five_destinations":[hex(v)for v in gate_jump_table]},
                "fallback_sequence_pointer":"0x4c79c1",
                "fallback_note":"Selector materializes this alternative when the tested property is not 1. Nearby alternative routine also calls the LUT parameter builder and registers a separate LUT consumer callback, so the gate finding does not establish that all MLUT routes are inactive.",
                "mode_output_layout":{"offset_0":"One byte, semantic enum/flag name unresolved.",
                                      "offsets_1_to_3":"Not written by this routine; zero in emulator output because the caller/test zero-initializes the buffer.",
                                      "offset_4":"32-bit value, always zero in this implementation's recognized-mode leaves.",
                                      "offset_8":"32-bit enum/value, observed 0..3; semantic name unresolved.",
                                      "offset_c":"32-bit enum/value, observed 0,2,3,14,15; semantic name unresolved.",
                                      "geometry":"No recognized-mode output contains established width/height fields; none of the semantic enum names has been recovered."},
                "fallback_geometry_inputs":{"initializer":"0xb2c1b8","call_file_offset":"0x3bfd7e",
                                            "source_descriptor":"0xb2ddbe(context[0x1526]) result",
                                            "first_source_record":"descriptor[0xc] + 0xa34",
                                            "second_source_record":"descriptor[0xc] + 0xa84",
                                            "first_hword_pair_copy":"source +0x3c/+0x3e -> context +0x50c/+0x50e",
                                            "per_unit_records":4,"source_unit_stride":"0x18","destination_unit_stride":"0x70",
                                            "first_unit_pairs":[["source+0x8/+0xa","context+0x1b4/+0x1b6"],
                                                                ["source+0xc/+0xe","context+0x1b8/+0x1ba"],
                                                                ["source+0x10/+0x12","context+0x1bc/+0x1be"]],
                                            "area_calculation_evidence":"Subsequent code reads the first halfword pair of later units and calls 0x4d3554, which multiplies its first two arguments before applying format-dependent size arithmetic.",
                                            "qualification":"These are runtime descriptor-sourced geometry-like values. No source-bound actual 7008/4672 values, still/mov/display enum names, or saved-frame ownership were recovered."},
                "conclusion":"The reconstructed context callback path is structurally present, but its observed upstream selector cannot choose this particular variant for any of the 65536 tested u16 modes. No saved-still classification is established."},
            "disassembled_regions":{name:region(*r)for name,r in ranges.items()},
            "remaining_unknowns":["No full-resolution still frame is traced from sensor/development buffer through MLUT to JPEG/HEIF encoder.",
                                  "No observed running-camera mode, dimensions, allocator ownership or capture throughput.",
                                  "Sequence diagnostic names are not sufficient evidence that this callback path belongs to saved stills.",
                                  "LUT payload ownership, DMA/cache lifecycle and color encoding remain separate unanswered contracts."]}
    out=ROOT/"research/evidence/still-mlut-route.json"
    out.write_text(json.dumps(report,indent=2)+"\n")
    print(json.dumps({"evidence":str(out.relative_to(ROOT)),"disassembled_regions":len(ranges),
                      "callback_identity_cases":1,"resource_bitmap_cases":len(resource_cases),
                      "dimension_cases":len(dimension_cases),"mode_lookup_u16_cases":65536,
                      "mode_lookup_u32_boundary_cases":len(extras),"CFG_path_instructions":len(route),
                      "camera_accessed":False},indent=2))


if __name__=="__main__":main()
