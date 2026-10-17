"""Source-bound CP architecture and LUT trace. Reads firmware; never uses USB.

Capstone is used for this raw ARM/Thumb image because REA's native provider
does not import arbitrary raw Thumb targets. All addresses retain their kind.
"""
import hashlib
import json
from pathlib import Path
import re
import struct

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB
from capstone.arm import ARM_REG_R4
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_R0, UC_ARM_REG_R1, UC_ARM_REG_R2, UC_ARM_REG_R3, UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_PC

ROOT = Path(__file__).resolve().parents[1]
BASE = 0x108000
EXPECTED = "f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428"


def hx(value):
    return hex(value)


def loader_microchecks(blob):
    """Execute only bounded metadata builders with external operations stubbed.

    This is host emulation, not a model of queue timing, DMA or the camera.
    """
    arena,stack,finish=0x5000000,0x6000000,0x7000000
    cpu=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
    cpu.mem_map(BASE,(len(blob)+4095)&~4095)
    cpu.mem_write(BASE,blob)
    for address,size in [(arena,0x10000),(stack,0x10000),(finish,0x1000),(0x262d000,0x10000)]:
        cpu.mem_map(address,size)
    captured=[]
    def hook(engine,address,size,user):
        result=0
        if address==0x61ed00:
            engine.mem_write(engine.reg_read(UC_ARM_REG_R0),b"\0"*engine.reg_read(UC_ARM_REG_R1))
        elif address==0xb7a0aa:
            result=3
        elif address==0xb83282:
            captured.append(bytes(engine.mem_read(engine.reg_read(UC_ARM_REG_R0),0x38)))
        else:
            return
        engine.reg_write(UC_ARM_REG_R0,result)
        engine.reg_write(UC_ARM_REG_PC,engine.reg_read(UC_ARM_REG_LR))
    cpu.hook_add(UC_HOOK_CODE,hook)
    initial=cpu.context_save()
    def execute(address,args):
        cpu.context_restore(initial)
        cpu.mem_write(stack,b"\0"*0x10000)
        cpu.reg_write(UC_ARM_REG_SP,stack+0x8000)
        cpu.reg_write(UC_ARM_REG_LR,finish|1)
        for register,value in zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3],args):
            cpu.reg_write(register,value)
        cpu.emu_start(address|1,finish,count=20000)
        assert cpu.reg_read(UC_ARM_REG_PC)==finish
        return cpu.reg_read(UC_ARM_REG_R0)
    cases=[]
    for address,length,selector in [(0xb799de,0xbb80,1),(0xb79ab4,0x7c,3)]:
        for ip in [0,6,7]:
            for size in [0,length-4,length,length+4]:
                for pointer in [arena+0x100,arena+0x101]:
                    captured.clear()
                    cpu.mem_write(arena,struct.pack("<II",pointer,size))
                    result=execute(address,[ip,arena,0xdeadbeef])
                    accepted=ip<=6 and size==length
                    assert result==(0 if accepted else 0xfffffff3)
                    assert len(captured)==int(accepted)
                    row={"function_runtime_address":hx(address),"IP_index":ip,
                         "source_pointer":hx(pointer),"length_bytes":size,"accepted":accepted,
                         "return_u32":hx(result)}
                    if accepted:
                        words=struct.unpack("<14I",captured[0])
                        assert words[0]==selector and words[2]==ip and words[3]==0xdeadbeef
                        assert words[4]==0x2ae085a and words[5]==pointer and words[6]==length//4
                        row["queued_metadata_words"]=[hx(w) for w in words]
                    cases.append(row)
    descriptors=[]
    for ip in [0,6]:
        for pointer in [arena+0x100,arena+0x101]:
            execute(0xb8336c,[ip,pointer,12000,arena+0x400])
            summary=struct.unpack("<III",cpu.mem_read(arena+0x400,12))
            assert summary==(ip,48,0x262d0f0)
            entries=[]
            for n in range(48):
                address=summary[2]+n*0x8c
                source,offset,words=struct.unpack("<III",cpu.mem_read(address,12))
                if n&1:
                    assert source==pointer+(n//2)*0x7d0 and offset==0x3000 and words==500
                else:
                    expected_control_source=struct.unpack_from('<I',blob,0xe953fc-BASE+(n//2)*4)[0]
                    assert source==expected_control_source and offset==0x408 and words==1
                entries.append({"source_runtime_address":hx(source),"MLUT_register_relative_destination":hx(offset),"word_count":words})
            descriptors.append({"IP_index":ip,"source_pointer":hx(pointer),"descriptor_count":48,"descriptors":entries})
    return {"method":"Isolated Thumb Unicorn execution; memory clearing, diagnostic mode and queue submission are stubbed. No on-camera execution or DMA.",
            "wrapper_cases":len(cases),"wrapper_results":cases,
            "SRAM_descriptor_builder_cases":len(descriptors),"SRAM_descriptor_results":descriptors,
            "interpretation":"Wrapper accepts unaligned source pointers at this layer; downstream hardware requirements remain unresolved."}


def run():
    path = next((ROOT / "research/extracted/ILCE-7CM2-2.01").rglob("cpapp-b.bin"))
    blob = path.read_bytes()
    digest = hashlib.sha256(blob).hexdigest()
    assert digest == EXPECTED, "Require the pinned official CP, never the candidate"
    u32 = lambda offset: struct.unpack_from("<I", blob, offset)[0]
    def cstring(address):
        offset = address - BASE
        if not 0 <= offset < len(blob):
            return None
        stop = blob.find(b"\0", offset)
        raw = blob[offset:stop]
        return raw.decode("ascii") if raw and all(32 <= n < 127 for n in raw) else None

    def code(start, stop, thumb=True):
        decoder = Cs(CS_ARCH_ARM, CS_MODE_THUMB if thumb else CS_MODE_ARM)
        instructions = list(decoder.disasm(blob[start:stop], BASE + start))
        assert sum(ins.size for ins in instructions) == stop-start, (hx(start), hx(stop))
        return {"file_start": hx(start), "file_end_exclusive": hx(stop),
                "runtime_start": hx(BASE+start), "isa": "Thumb-2" if thumb else "ARM",
                "instructions": [{"file_offset": hx(ins.address-BASE),
                                  "runtime_address": hx(ins.address),
                                  "bytes": ins.bytes.hex(),
                                  "operation": ins.mnemonic, "operands": ins.op_str}
                                 for ins in instructions]}

    descriptor = 0x20000
    names = ["code_candidate", "read_only_data_candidate", "initialized_data_candidate",
             "zero_initialized_data_candidate"]
    segments = []
    for name, field in zip(names, [0x124, 0x130, 0x13c, 0x148]):
        relative, size, address = struct.unpack_from("<III", blob, descriptor+field)
        offset = address-BASE
        backed = offset+size <= len(blob)
        assert relative + u32(descriptor+0x16c) == address
        segments.append({"role": name, "descriptor_field_file_offset": hx(descriptor+field),
                         "relative_backing_address": hx(relative), "bytes": size,
                         "runtime_start": hx(address), "runtime_end_exclusive": hx(address+size),
                         "file_backed": backed,
                         "file_start": hx(offset) if backed else None,
                         "file_end_exclusive": hx(offset+size) if backed else None})
    initialized_end=int(segments[2]["file_end_exclusive"], 16)
    assert len(blob)-initialized_end==0x40000 and not any(blob[initialized_end:])
    stacks=[]
    for cpu in range(4):
        ranges=[]
        for slot, mode in enumerate(["SVC", "IRQ", "FIQ", "UND", "ABT", "SYS"]):
            off=descriptor+4+cpu*0x48+slot*12
            start,size,top=struct.unpack_from("<III",blob,off)
            assert start+size==top
            ranges.append({"cpu_mode":mode,"runtime_start":hx(start),"bytes":size,
                           "initial_stack_pointer":hx(top),"descriptor_file_offset":hx(off)})
        stacks.append({"cpu_index":cpu,"stacks":ranges})
    assert u32(descriptor)==0x619ea0 and u32(descriptor+0x178)==4

    strings=[{"file_offset":hx(m.start()),"runtime_address":hx(BASE+m.start()),
              "text":m[0].decode("ascii")} for m in re.finditer(rb"[ -~]{8,}",blob)]
    selected={}
    groups={"operating_system":["TOPPERS-FMP", "SOLID_", "CPAPP_RT", "CPAPP_BE"],
            "ipc_and_memory":["UIPC", "uipc", "DmmWrapper", "GetBaseAddress", "DMM"],
            "image_processing":["MLUT", "CamCore", "ENCODE_JPEG_WITH_WORK", "SAStill", "StillProc"],
            "compiler_clues":["gcc.gnu.org/bugs", "[layoutSize =", "N5CpApp"]}
    for name,needles in groups.items():
        hits=[s for s in strings if any(n in s["text"] for n in needles)]
        selected[name]={"matching_string_count":len(hits),"samples":hits[:60],
                        "samples_truncated":len(hits)>60}
    paths=[s for s in strings if "/BuildWorkSpace/" in s["text"]]
    components={}
    for s in paths:
        component=s["text"].split("/BuildWorkSpace/",1)[1].split("/",1)[0]
        components.setdefault(component,[]).append(s)

    markers={"user_lut_command":b"FW: SET_USER_LUT_BINARY_DATA",
             "camcore_create":b"CC: Create3DLUT 0x%X",
             "mlut_flow_help":b"ddl_mlut_set_mode_flow [IPTYPE]",
             "camcore_table_start":b"CC: CamCore 3D-Lut table Write Start",
             "camcore_table_end":b"CC: CamCore 3D-Lut table Write End",
             "user_17_register_name":b"MLUT_L3D_REG_USER_LUT_17",
             "vfx_table_submit_log":b"FW: call ddl_VfxSetMlut lookname="}
    marker_rows={}
    for name,needle in markers.items():
        at=blob.index(needle)
        marker_rows[name]={"file_offset":hx(at),"runtime_address":hx(at+BASE),
                           "text":blob[at:blob.index(b"\0",at)].decode("ascii"),
                           "reference_kind":"not_yet_traced"}

    # Scan halfword-aligned candidates, then independently decode each match.
    # No whole-image disassembly or automatic code/data claim is made.
    targets={int(r["runtime_address"],16):n for n,r in marker_rows.items()}
    pending={}
    pairs=[]
    state_references=[]
    branch_targets={0x35c54c,0x23dc2c,0xb78b88,0xb829d4,0xb781e6,
                    0x8dad54,0x8dad4a,0xb2e8e0,0xb2ebce,0xb2c406,
                    0x4e876c,0x54e15c,0x54e398,0xb799de,0xb79ab4,0xb78c60}
    direct_calls=[]
    for off in range(0x2017c,0xb27490-3,2):
        a,b=struct.unpack_from("<HH",blob,off)
        if a&0xfbf0 in (0xf240,0xf2c0) and not b&0x8000:
            imm=(a&15)<<12 | (a&0x400)<<1 | (b&0x7000)>>4 | b&255
            register=b>>8&15
            if a&0x80:
                if register in pending:
                    previous,low=pending[register]
                    full=imm<<16|low
                    if off-previous<=80 and full in (0x165a720,0x172fe50,0x173efe8):
                        state_references.append({"low_file_offset":hx(previous),"high_file_offset":hx(off),
                                                 "register":f"r{register}","runtime_value":hx(full)})
                    if off-previous<=80 and full in targets:
                        name=targets[full]
                        marker_rows[name]["reference_kind"]="MOVW_MOVT_address_materialization"
                        pairs.append({"marker":name,"destination_register":f"r{register}",
                                      "resolved_runtime_address":hx(full),
                                      "low_instruction":code(previous,previous+4),
                                      "high_instruction":code(off,off+4)})
            else:
                pending[register]=(off,imm)
        if a&0xf800==0xf000 and b&0xd000==0xd000:
            sign=a>>10&1
            imm=(sign<<24 | (1^((b>>13&1)^sign))<<23 | (1^((b>>11&1)^sign))<<22 |
                 (a&1023)<<12 | (b&2047)<<1)
            if sign:imm-=1<<25
            destination=BASE+off+4+imm
            if destination in branch_targets:
                direct_calls.append({"target_runtime_address":hx(destination),
                                     "instruction":code(off,off+4)})

    commands=[]
    for i in range(100):
        off=0x1031cac+12*i
        key,help_address,handler=struct.unpack_from("<III",blob,off)
        key_text=cstring(key)
        if not key_text or not key_text.isdigit():break
        help_text=cstring(help_address)
        assert help_text and int(segments[0]["runtime_start"],16)<=handler<int(segments[0]["runtime_end_exclusive"],16)
        commands.append({"key":key_text,"help":help_text,"table_row_file_offset":hx(off),
                         "handler_runtime_pointer":hx(handler),
                         "handler_runtime_address":hx(handler&~1),
                         "handler_file_offset":hx((handler&~1)-BASE),
                         "isa":"Thumb-2" if handler&1 else "ARM"})
    assert len(commands)==64
    marker_rows["mlut_flow_help"]["reference_kind"]="help_pointer_in_debug_command_table"
    adr_offset=0x324df0
    halfword=struct.unpack_from("<H",blob,adr_offset)[0]
    assert halfword&0xf800==0xa000
    adr_target=((BASE+adr_offset+4)&~3)+(halfword&255)*4
    assert adr_target==int(marker_rows["camcore_table_start"]["runtime_address"],16)
    marker_rows["camcore_table_start"]["reference_kind"]="PC_relative_ADR_instruction"
    marker_rows["camcore_table_start"]["address_instruction"]=code(adr_offset,adr_offset+2)
    register_table=struct.unpack_from("<7I",blob,0xe9515c-BASE)
    assert list(register_table)==[0xf4009000+i*0x100000 for i in range(7)]
    message_rows=[]
    for off in range(0xff43ec,0xff4450,20):
        key,handler,flag,reserved,other=struct.unpack_from("<5I",blob,off)
        message_rows.append({"table_row_file_offset":hx(off),"candidate_command_id":hx(key),
                             "handler_runtime_pointer":hx(handler),"flag_unknown":flag,
                             "reserved_unknown":reserved,"other_callback_runtime_pointer":hx(other)})
    assert any(r["candidate_command_id"]=="0x4155" and r["handler_runtime_pointer"]=="0x35c54d" for r in message_rows)
    routines={
        "reset_startup":code(0,0xc,False),
        "startup_cpu_and_cache":code(0x10,0x70,False),
        "descriptor_copy_and_mapping":code(0x398,0x538,False),
        "descriptor_entry_dispatch":code(0x6d4,0x768,False),
        "arm_entry_stack_and_fpu_setup":code(0x511ea0,0x511f50,False),
        "user_lut_message_handler":code(0x25454c,0x254694),
        "camcore_create_prologue":code(0x135c2c,0x135c42),
        "camcore_create_log_and_context":code(0x135d1e,0x135df2),
        "camcore_packed_table_loop_tail":code(0x13618c,0x1361f2),
        "camcore_table_copy_and_submit":code(0x324db0,0x324e1e),
        "mlut_debug_mode_flow_handler":code(0xa72136,0xa721b0),
        "mlut_mode_flow_validation":code(0xa70b88,0xa70c1c),
        "mlut_mode_flow_setters":code(0xa7a9d4,0xa7a9f2),
        "mlut_flow_mmio":code(0xa701e6,0xa7023e),
        "mlut_ip_base_calculation":code(0xa6fff8,0xa7000e),
        "mlut_sram_debug_submit_tail":code(0xa72ee2,0xa72f10),
        "global_parameter_pointer_getter":code(0x7d2d4a,0x7d2d54),
        "global_parameter_snapshot_copy":code(0x7d2d54,0x7d2dca),
        "parameter_snapshot_initialization":code(0xa26140,0xa26168),
        "context_bind_call":code(0xa26656,0xa2666c),
        "context_bind_wrapper":code(0xa268e0,0xa268ec),
        "context_pointer_constructor":code(0xa26bce,0xa26be6),
        "context_pointer_getter":code(0xa25db4,0xa25dbe),
        "mlut_parameter_record_builder":code(0xa24406,0xa244c8),
        "vfx_mlut_consumer":code(0x3e076c,0x3e0818),
        "vfx_mlut_single_payload_call":code(0x3e086e,0x3e0880),
        "vfx_dual_payload_driver_prologue":code(0x446398,0x446474),
        "vfx_dual_payload_mapped_driver_calls":code(0x446508,0x446618),
        "vfx_single_payload_driver_prologue":code(0x44615c,0x446224),
        "vfx_single_payload_driver_calls":code(0x4462aa,0x44634a),
        "mlut_builder_caller_contract":code(0xa2311e,0xa23138),
        "vfx_consumer_caller_contract_1":code(0xa234c2,0xa234e8),
        "vfx_consumer_caller_contract_2":code(0xa2357c,0xa2359a),
        "vfx_consumer_caller_contract_3":code(0xa239e8,0xa23a12),
        "mlut_sram_payload_wrapper":code(0xa719de,0xa71ab4),
        "mlut_register_payload_wrapper":code(0xa71ab4,0xa71b84),
        "mlut_metadata_enqueue":code(0xa7b282,0xa7b36c),
        "mlut_queue_worker":code(0xa7ac22,0xa7acb6),
        "mlut_operation_dispatch_prefix":code(0x42d33c,0x42d376),
        "mlut_operation_dispatch_cases":code(0x42d37c,0x42d3c4),
        "mlut_sram_worker":code(0xa7ad7c,0xa7adbe),
        "mlut_sram_descriptor_builder":code(0xa7b36c,0xa7b3f4),
        "mlut_register_worker":code(0xa7ae00,0xa7ae5a),
        "mlut_dma_submission_prefix":code(0xa7b062,0xa7b132),
        "mlut_dma_completion_wait":code(0xa7b176,0xa7b1cc),
        "mlut_dma_callback":code(0xa7b1f8,0xa7b252),
    }
    verifier=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
    verifier.detail=True
    # Callee-saved r4 is the initialized snapshot base throughout this range.
    # This check records direct definitions, not an interprocedural ABI proof.
    definitions=[i for i in verifier.disasm(blob[0xa26140:0xa2666c],BASE+0xa26140)
                 if ARM_REG_R4 in i.regs_access()[1]]
    assert [(i.address-BASE,i.mnemonic) for i in definitions]==[(0xa26154,"movw"),(0xa2615e,"movt")]
    report={"date":"2026-10-17","mode":"offline_static_analysis_and_bounded_host_emulation", "camera_accessed":False,
            "target_executed":False,"host_emulation_performed":True,
            "target_execution_definition":"No execution on camera hardware or a live camera operating environment; isolated metadata-building instructions are emulated on the host with external operations stubbed.",
            "firmware_modified":False,
            "source":{"path":str(path.relative_to(ROOT)),"sha256":digest,"bytes":len(blob)},
            "address_model":{"file_to_runtime_addend":hx(BASE),
                             "qualification":"Corroborated for descriptor and file-backed segments; not an ASLR assumption or live observation.",
                             "initial_file_bytes_are_arm_instructions_not_container_header":True,
                             "descriptor_file_offset":hx(descriptor),"descriptor_bytes":0x17c,
                             "descriptor_entry_runtime_address":hx(u32(descriptor)),
                             "segments":segments,"per_core_stacks":stacks,
                             "zero_tail_after_initialized_data":{"file_start":hx(initialized_end),
                                                                 "bytes":len(blob)-initialized_end,
                                                                 "all_zero":True},
                             "translation_table_regions":[
                                 {"runtime_start":hx(u32(descriptor+0x15c)),"bytes":u32(descriptor+0x158)},
                                 {"runtime_start":hx(u32(descriptor+0x168)),"bytes":u32(descriptor+0x164)}]},
            "bounded_string_evidence":selected,
            "build_components":{k:{"path_count":len(v),"examples":v[:3]} for k,v in sorted(components.items())},
            "markers":marker_rows,"instruction_address_references":pairs,
            "selected_direct_thumb_calls":direct_calls,"mlut_debug_commands":commands,
            "parameter_state_downstream_trace":{
                "materialization_scan":{"file_start":"0x2017c","file_end_exclusive":"0xb27490",
                                        "maximum_MOVW_MOVT_separation_bytes":80,
                                        "references":state_references,
                                        "qualification":"Halfword-aligned address candidates; selected coherent functions are decoded separately."},
                "snapshot_bytes":0xd64,"incoming_parameter_global":"0x165a720",
                "initialized_snapshot":"0x172fe50","context_pointer_table":"0x173efe8",
                "r4_direct_definitions_during_context_initialization":[
                    {"file_offset":hx(i.address-BASE),"operation":i.mnemonic,"operands":i.op_str}
                    for i in definitions],
                "user_lut_copy":{"source":"context_pointer_table[0] + 0x68",
                                 "destination":"output_context + 0x13d0","bytes":0x60,
                                 "condition":"output_context[0x13c0] == 0x0a",
                                 "call_file_offset":"0xa244c4"},
                "vfx_consumer":{"runtime_address":"0x4e876c","record_at":"consumer_context + 0x13b8",
                                "caller_neighborhoods":"disassembled_regions.vfx_consumer_caller_contract_1 through _3",
                                "relationship_to_builder":"Producer and consumer use the same record offsets/layout. A particular context instance traversing both calls in a frame/state-machine transition is not proven.",
                                "user_payload_path_condition":"record[0x8] == 0x0a",
                                "payload_address":"record[0x18..0x1f]",
                                "payload_length":"record[0x28..0x2b]",
                                "register_address":"record[0x20..0x27]",
                                "register_length":"record[0x2c..0x2f]",
                                "single_payload_driver":"0x54e15c","dual_payload_driver":"0x54e398"},
                "driver_links":{"SRAM_payload":"0xb799de","look_selection":"0xb78c60",
                                "register_payload":"0xb79ab4",
                                "qualification":"Names are inferred from independent diagnostic-table callers, not recovered symbol exports."},
                "remaining_gaps":["Mapped IP is selected through resource/handle lookup, not a proven fixed still IP index.",
                                  "Producer output and consumer input have matching layout but their per-instance lifecycle/state-machine edge remains unproven.",
                                  "Capture-frame scheduling, payload validity and physical table ownership are not established.",
                                  "Snapshot/context links rely on coherent direct calls and ordinary callee-saved-register ABI; no device observations."]},
            "candidate_message_dispatch_rows":message_rows,
            "mlut_register_base_table":{"file_offset":hx(0xe9515c-BASE),"runtime_address":"0xe9515c",
                                        "values":[hx(v) for v in register_table]},
            "mlut_payload_loader_constraints":{
                "SRAM":{"required_bytes":0xbb80,"queued_word_count":0x2ee0,"queue_operation":1,
                        "transfer_descriptors":48,"payload_chunks":24,"payload_bytes_per_chunk":0x7d0,
                        "control_word_register_offset":"0x408","payload_window_register_offset":"0x3000"},
                "registers":{"required_bytes":0x7c,"queued_word_count":0x1f,"queue_operation":3,
                             "transfer_register_offset":"0x400","transfer_words":31},
                "queue_id":"0x2ae085a","copied_queue_metadata_bytes":0x38,
                "payload_copied_by_enqueue":False,
                "alignment_precision_and_channel_order":"Not established. Wrapper and descriptor builder accept an unaligned source pointer in isolated emulation.",
                "completion_qualification":"Separate worker and DMA-style descriptor/callback path are present. Sender return is not proof of payload consumption or frame completion; OSAL and hardware lifetime contract is unresolved.",
                "host_microchecks":loader_microchecks(blob)},
            "disassembled_regions":routines,
            "limitations":[
                "Code/data labels are segment-role inference corroborated by examined bytes, not recovered ELF section names.",
                "Only selected function regions are decoded; no complete function boundary or call graph recovery.",
                "Halfword scan can find overlapping instruction candidates; selected references are corroborated in coherent routines.",
                "0x4155 is a candidate message dispatch ID until its dispatcher and AP producer are joined.",
                "Seven MLUT IP indices and register accesses do not identify which path carries full-resolution still images.",
                "CamCore Create3DLUT and MLUT user-LUT facilities must not be assumed to share table encodings.",
                "No CUBE channel order, bit precision, SRAM packing, DMA/cache ownership, capture routing, or safe runtime call established.",
                "Compiler diagnostic and C++ RTTI establish linked-runtime clues, not exact compiler/version.",
                "TOPPERS-FMP source-path version is an embedded build clue, not a complete RTOS provenance or patch-level attestation."]}
    output=ROOT/"research/evidence/cp-architecture.json"
    output.write_text(json.dumps(report,indent=2)+"\n")
    print(json.dumps({"evidence":str(output.relative_to(ROOT)),"segments":len(segments),
                      "per_core_stack_groups":len(stacks),"mlut_debug_commands":len(commands),
                      "address_materializations":len(pairs),"direct_calls":len(direct_calls),
                      "disassembled_regions":len(routines),"source_sha256":digest,
                      "host_wrapper_cases":48,"host_descriptor_cases":4,"camera_accessed":False},indent=2))


if __name__ == "__main__":
    run()
