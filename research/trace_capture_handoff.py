"""Offline capture-request -> ChipTransfer -> encoder-slot provenance.

Only original instructions run under Unicorn against invented RAM/service replies.
No device, transport, codec, DMA or firmware writes are performed.
"""
import hashlib
import itertools
import json
import struct
from capstone import Cs, CS_ARCH_ARM64, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB
from unicorn import UC_HOOK_CODE
from unicorn.arm64_const import *
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB
from unicorn.arm_const import *
from trace_still_info_producer import AV, BASE, NORMAL, ROOT, EXPECTED, raw_machine, ret, ident
from trace_mlut_allocation import BASE as CPBASE, DIGEST as CPDIGEST

AV_REGIONS = [
    ('request initialization and source handles', 0x1f3268, 0x1f3320),
    ('mode1 request geometry', 0x1f33cc, 0x1f340c),
    ('mode1 main YC transfer', 0x202ebc, 0x202fb4),
    ('second input shared from first', 0x2032ec, 0x203398),
    ('ChipTransfer', 0x208168, 0x2084c4),
    ('PCIe DMA import thunk', 0x2f44c, 0x2f460),
    ('JPEG target-dependent input', 0x200f00, 0x200fcc),
    ('HEIF slot input fragment', 0x1ffc50, 0x1ffc84),
]
CP_REGIONS = [
    ('cache allocation and reuse', 0x73afac, 0x73b082),
    ('shared-handle publication', 0xb231e0, 0xb2323c),
    ('selector72 export record', 0xb23788, 0xb23808),
    ('publication notification envelope', 0xb24e6c, 0xb24f1c),
]


def retained(blob, ranges, base, thumb=False):
    md = Cs(CS_ARCH_ARM if thumb else CS_ARCH_ARM64, CS_MODE_THUMB if thumb else CS_MODE_ARM)
    rows = []
    for name, lo, hi in ranges:
        raw = blob[lo-base:hi-base]
        instructions = list(md.disasm(raw, lo))
        assert sum(i.size for i in instructions) == hi-lo
        rows.append({'name': name, 'start': hex(lo), 'end_exclusive': hex(hi),
                     'sha256': hashlib.sha256(raw).hexdigest(),
                     'instructions': [{'address': hex(i.address), 'bytes': i.bytes.hex(),
                                       'mnemonic': i.mnemonic, 'operands': i.op_str} for i in instructions]})
    return rows


def transfer_cases(blob):
    cases = []
    for (width, height), submit_failure in itertools.product([(1536, 1024), (7008, 4672)], (False, True)):
        uc = raw_machine(blob)
        ctx, request, geometry, descriptor = 0x10000000, 0x10001000, 0x10002000, 0x10002100
        source_handle, destination_handle = 0xa1234567, 0xb7654321
        size = width * height * 5 // 2
        calls = []
        def hook(u, a, n, _):
            a -= NORMAL
            if a == 0xd80:
                d, s, length = [u.reg_read(r) for r in (UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2)]
                assert length == 12
                u.mem_write(d, bytes(u.mem_read(s, length))); ret(u, d)
            elif a == 0x2093ec:
                calls.append({'boundary': 'stage scheduling', 'stage': u.reg_read(UC_ARM64_REG_X0)}); ret(u)
            elif a == 0x208dfc:
                unit, count, wait, output = [u.reg_read(r) for r in (UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3)]
                assert (unit, count, wait) == (0x4c8, 1, 0xffffffff)
                info = bytearray(0xd8)
                struct.pack_into('<I', info, 0xc, destination_handle)
                struct.pack_into('<Q', info, 0x18, 0x410000000)
                struct.pack_into('<I', info, 0x20, size + 4096)
                u.mem_write(output, bytes(info))
                calls.append({'boundary': 'synthetic AP allocation', 'unit': hex(unit), 'handle': hex(destination_handle)}); ret(u)
            elif a == 0x2f44c:
                p = u.reg_read(UC_ARM64_REG_X0)
                src, dst, count_bytes, count, callback, userdata = struct.unpack('<QQIIQQ', bytes(u.mem_read(p, 40)))
                source = struct.unpack('<II', u.mem_read(src, 8))
                dest = struct.unpack('<II', u.mem_read(dst, 8))
                assert source == (source_handle, 0) and dest == (destination_handle, 0)
                assert (count_bytes, count, userdata) == (size, 1, 4)
                assert callback == NORMAL + 0x2084bc
                calls.append({'boundary': 'intercepted PCIe DMA submit', 'source': [hex(v) for v in source],
                              'destination': [hex(v) for v in dest], 'bytes': count_bytes, 'count': count,
                              'callback_file_offset': '0x2084bc', 'userdata': userdata,
                              'synthetic_status': 9 if submit_failure else 0}); ret(u, 9 if submit_failure else 0)
            elif a == 0x2e9a8:
                calls.append({'boundary': 'synthetic completion event wait'}); ret(u)
            elif a in (0x2e994, 0x207ba4, 0x2e890): ret(u)
            elif a == 0x208c5c:
                args = [u.reg_read(r) for r in (UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2)]
                assert args == [source_handle, 2, 0x9061003c]
                calls.append({'boundary': 'intercepted source release', 'arguments': [hex(v) for v in args]}); ret(u)
            elif a in (0x20b32c, 0x20b5a4):
                assert submit_failure
                calls.append({'boundary': 'intercepted diagnostic', 'file_offset': hex(a)}); ret(u)
            elif a == 0x200f58:
                # Literal linked pointers need rebasing into the host's raw-image mapping.
                linked = u.reg_read(UC_ARM64_REG_X8)
                assert linked == BASE + 0x200f74
                u.reg_write(UC_ARM64_REG_X8, NORMAL + linked - BASE)
        uc.hook_add(UC_HOOK_CODE, hook)
        uc.mem_write(request + 0x2c4, struct.pack('<I', source_handle))
        uc.mem_write(request + 0x2b4, struct.pack('<6H', width, height, width, height, 0, 0))
        uc.mem_write(ctx + 0x7dc, struct.pack('<I', 1))
        # Real mode-dependent request import. Stops before unrelated initialization.
        for r, v in [(UC_ARM64_REG_X19, ctx), (UC_ARM64_REG_X20, request)]: uc.reg_write(r, v)
        uc.emu_start(NORMAL+0x1f32cc, NORMAL+0x1f3320, count=100)
        assert struct.unpack('<I', uc.mem_read(ctx+0x324, 4))[0] == source_handle
        uc.reg_write(UC_ARM64_REG_X21, ctx+0x3bc)
        uc.emu_start(NORMAL+0x1f33d8, NORMAL+0x1f33e8, count=100)
        # Real caller and complete ChipTransfer body; only named remote boundaries stubbed.
        uc.reg_write(UC_ARM64_REG_X0, ctx); uc.reg_write(UC_ARM64_REG_LR, 0x1003ff00)
        uc.emu_start(NORMAL+0x202ebc, 0x1003ff00, count=3000)
        assert uc.reg_read(UC_ARM64_REG_PC) == 0x1003ff00
        assert struct.unpack('<I', uc.mem_read(ctx+0x41c, 4))[0] == destination_handle
        assert struct.unpack('<I', uc.mem_read(ctx+0x324, 4))[0] == 0xffffffff
        submissions = [e for e in calls if e['boundary'] == 'intercepted PCIe DMA submit']
        waits = [e for e in calls if e['boundary'] == 'synthetic completion event wait']
        assert len(submissions) == (5 if submit_failure else 1)
        assert len(waits) == (0 if submit_failure else 1)
        for r, v in [(UC_ARM64_REG_X20, ctx), (UC_ARM64_REG_X21, 3), (UC_ARM64_REG_X22, descriptor),
                     (UC_ARM64_REG_X26, 1), (UC_ARM64_REG_X27, geometry), (UC_ARM64_REG_X28, ctx+0x440)]: uc.reg_write(r, v)
        uc.emu_start(NORMAL+0x200f00, NORMAL+0x200fcc, count=100)
        assert struct.unpack('<I', uc.mem_read(descriptor+4, 4))[0] == destination_handle
        assert bytes(uc.mem_read(geometry, 12)) == bytes(uc.mem_read(request+0x2b4, 12))
        cases.append({'dimensions': [width, height], 'context_mode': 1, 'JPEG_target': 3,
                      'selector': 1, 'request_source_field': '+0x2c4', 'context_source_field': '+0x324',
                      'encoder_input_handle': hex(destination_handle), 'source_handle': hex(source_handle),
                      'source_invalidated': True, 'all_submit_attempts_failed': submit_failure, 'events': calls,
                      'qualification': 'Synthetic service outcomes. Failed submissions also reach handle publication under returning diagnostic stubs. No pixels copied or encoded; full-size arithmetic acceptance is not hardware capacity.'})
    return cases


def cp_export_cases(blob):
    uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
    uc.mem_map(CPBASE, (len(blob)+4095)&~4095); uc.mem_write(CPBASE, blob)
    for base in (0x1a53000, 0x1b1e000, 0x1725000, 0x5000000, 0x6000000): uc.mem_map(base, 0x1000)
    def hook(u, a, n, _):
        if a == 0xb24e6c: u.reg_write(UC_ARM_REG_PC, u.reg_read(UC_ARM_REG_LR))
    uc.hook_add(UC_HOOK_CODE, hook)
    out = []
    for handle, token, length in [(0xb0012345, 0x63d, 0x1000), (0xb0765432, 0x347, 0x2000)]:
        uc.reg_write(UC_ARM_REG_SP, 0x6000800); uc.reg_write(UC_ARM_REG_LR, 0x5000f01)
        for reg, value in zip((UC_ARM_REG_R0, UC_ARM_REG_R1, UC_ARM_REG_R2), (handle, token, length)): uc.reg_write(reg, value)
        uc.emu_start(0xb231e1, 0x5000f00, count=100)
        assert uc.reg_read(UC_ARM_REG_PC) == 0x5000f00
        record = 0x5000100; uc.mem_write(record, bytes(0x40))
        uc.reg_write(UC_ARM_REG_R0, record); uc.reg_write(UC_ARM_REG_LR, 0x5000f01)
        uc.emu_start(0xb23789, 0x5000f00, count=100)
        data = bytes(uc.mem_read(record, 0x1c))
        assert struct.unpack_from('<H', data)[0] == 0x108 and data[4] == 1
        assert struct.unpack_from('<I', data, 8)[0] == handle
        assert struct.unpack_from('<II', data, 0x10) == (token, 0)
        assert struct.unpack_from('<I', data, 0x18)[0] == length
        out.append({'handle': hex(handle), 'selector': hex(token), 'length': length,
                    'export_bytes': data.hex(), 'type': '0x108', 'table_selector': '0x72'})
    return out


def main():
    av = AV.read_bytes(); assert hashlib.sha256(av).hexdigest() == EXPECTED
    cp_path = next((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob('cpapp-b.bin'))
    cp = cp_path.read_bytes(); assert hashlib.sha256(cp).hexdigest() == CPDIGEST
    # Exact table identities, rather than interpreting a matching string as a call.
    assert struct.unpack_from('<II', cp, 0xe782b8-CPBASE) == (0x72, 0xb23789)
    jpeg_targets = [hex(struct.unpack_from('<Q', av, 0x3779b0+i*8)[0]-BASE) for i in range(4)]
    assert jpeg_targets == ['0x200f5c', '0x200fa8', '0x200fb4', '0x200f74']
    transfers, exports = transfer_cases(av), cp_export_cases(cp)
    result = {'evidence_class': 'raw Capstone and original-instruction Unicorn, not REA Evidence',
              'sources': {'av-cam': ident(AV), 'cpapp-b': ident(cp_path)},
              'generator': ident(__import__('pathlib').Path(__file__)),
              'camera_accessed': False, 'network_used': False, 'target_executed': False,
              'address_models': {'av-cam': 'file offsets; linked VA=file offset+0xffffffc001000000; host base0x01000000',
                                 'CP': 'Thumb runtime address; file offset=address-0x108000'},
              'AV_regions': retained(av, AV_REGIONS, 0), 'CP_regions': retained(cp, CP_REGIONS, CPBASE, True),
              'tables': {'JPEG_input_targets_0_to_3': jpeg_targets,
                         'CP_export': {'table_address': '0xe782b8', 'selector': '0x72', 'handler': '0xb23788'}},
              'host_cases': {'capture_transfer': transfers, 'CP_export': exports},
              'limitations': ['All DMM allocations, PCIe DMA submission/completion, stage scheduling and releases are intercepted.',
                              'No native DMA or pixel processing; no inference that a 7008x4672 buffer fits actual allocations.',
                              'Request+2c4 writer and live context-mode selection not recovered; no MLUT lineage to that request.',
                              'CP export type108 is not av-cam still-info message1006. No AP consumer or cross-processor identity proved.',
                              'Failure-path diagnostics are intercepted as returning; their actual runtime termination/assertion policy is unknown.',
                              'Only mode1, JPEG target3/selector1 is continuously tested; other capture variants remain separate.',
                              'HEIF selected fragment retained but not a continuous tested transfer-to-HEIF path.'],
              'verification': {'host_cases': len(transfers)+len(exports), 'retained_regions': len(AV_REGIONS)+len(CP_REGIONS)}}
    output = ROOT/'research/evidence/capture-handoff.json'
    output.write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps({'output': str(output.relative_to(ROOT)), **result['verification']}))


if __name__ == '__main__': main()
