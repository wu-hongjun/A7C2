"""Offline Astia/FL23 composition audit and bounded HIF-gamma tracing.

Composes bytes only in host memory. Never writes a firmware candidate, imports
USB transport, runs an installer, or accesses a camera/card or G HUB.
"""
import hashlib
import itertools
import json
from pathlib import Path
import struct
import sys

from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from unicorn import Uc, UC_ARCH_ARM, UC_ARCH_ARM64, UC_MODE_THUMB, UC_MODE_ARM
from unicorn.arm_const import *
from unicorn.arm64_const import *

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / 'research'))
import core
from review_cp import Image
from check_native_looks import packing

REF = ROOT / 'research/build/qaqleonard-reference-20261020'
WORK = ROOT / 'research/build/astia-native-20261020'
DEV = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev'
BASE = 0x108000


def sha(data):
    return hashlib.sha256(data).hexdigest()


def fnv(data):
    value = 2166136261
    for byte in data:
        value = ((value ^ byte) * 16777619) & 0xffffffff
    return value


def compose():
    original = [(DEV / 'nflasha15_unpacked/lib/appFw.so').read_bytes(),
                (DEV / 'nflasha8_unpacked/cpapp-b.bin').read_bytes()]
    ours = [core.patch_program(raw, chip) for chip, raw in enumerate(original)]
    layout_path = REF / 'src/looks/assets/runtime-v1.json'
    layout = json.loads(layout_path.read_text())
    spec = next(s for s in layout['cp_sources'] if s['kind'] == 'fl23')
    combined = [bytearray(raw) for raw in ours]
    overlaps = []
    for window in spec['windows']:
        at = window['offset']; before = bytes.fromhex(window['before']); after = bytes.fromhex(window['after'])
        assert combined[1][at:at + len(before)] == before, hex(at)
        combined[1][at:at + len(after)] = after
        for own in core.read_json(ROOT / 'assets/patches.json')[1]['writes']:
            lo = int(own['file_offset'], 16); hi = lo + len(bytes.fromhex(own['after_hex']))
            start, end = max(lo, at), min(hi, at + len(after))
            if start < end:
                overlaps.append({'own_purpose': own['purpose'], 'start': hex(start), 'bytes': end - start,
                                 'classification': 'shared hook/descriptor, needs behavioral checks'})
    # Standalone AP mask 1 imports only custom slot 1. No recipe/resource writes.
    ap_windows = [(0x35287f0, bytes.fromhex(layout['ap_templates'][1])),
                  (0x1b5cd7c, bytes.fromhex(layout['title_branch'])),
                  (0x4c7cf7c, bytes.fromhex(layout['map'])),
                  (0x59896f0, struct.pack('<I', 0x0200ab2c))]
    for at, data in ap_windows:
        combined[0][at:at + len(data)] = data
    raw = (WORK / 'slot1-astia.raw-s8.bin').read_bytes()
    fit = json.loads((ROOT / 'research/evidence/astia-native-fit.json').read_text())
    assert sha(raw) == fit['raw_sha256'] and len(raw) == 1215
    combined[1][0x8020:0x8020 + 1215] = raw
    struct.pack_into('<I', combined[1], 0x8008, fnv(raw))
    # No silent regressions to our corrected movie fallback helper/dispatch.
    for at, count in [(0x1384a4, 4), (0x138832, 24)]:
        assert combined[1][at:at + count] == ours[1][at:at + count]
    shared = [(int(row['start'], 16), int(row['start'], 16) + row['bytes']) for row in overlaps]
    checked = 0
    for window in core.read_json(ROOT / 'assets/patches.json')[1]['writes']:
        at = int(window['file_offset'], 16)
        for i in range(at, at + len(bytes.fromhex(window['after_hex']))):
            if not any(lo <= i < hi for lo, hi in shared):
                assert combined[1][i] == ours[1][i], hex(i)
                checked += 1
    ap_checked = 0
    for window in core.read_json(ROOT / 'assets/patches.json')[0]['writes']:
        at = int(window['file_offset'], 16)
        for i in range(at, at + len(bytes.fromhex(window['after_hex']))):
            if not any(lo <= i < lo + len(data) for lo, data in ap_windows):
                assert combined[0][i] == ours[0][i], hex(i)
                ap_checked += 1
    extents = []
    for field in [0x124, 0x130, 0x13c, 0x148]:
        _, size, start = struct.unpack_from('<III', combined[1], 0x20000 + field)
        extents.append({'descriptor_field': hex(field), 'start': hex(start), 'bytes': size, 'end': hex(start + size)})
    assert int(extents[0]['end'], 16) == 0xc2ffb0
    assert int(extents[2]['end'], 16) <= int(extents[3]['start'], 16)
    gap = int(extents[3]['start'], 16) - int(extents[2]['end'], 16)
    source_matches = {'AP': sha(ours[0]) in [s['sha256'] for s in layout['ap_sources']],
                      'CP': sha(ours[1]) in [s['sha256'] for s in layout['cp_sources']]}
    return original, ours, [bytes(raw) for raw in combined], {
        'upstream_exact_source_accepts_our_programs': source_matches,
        'upstream_FL23_CP_source_sha256': spec['sha256'], 'our_FL23_CP_sha256': core.FINAL[1],
        'runtime_source_windows_matched': len(spec['windows']), 'overlaps': overlaps,
        'other_FL23_CP_patch_bytes_preserved': checked, 'other_FL23_AP_patch_bytes_preserved': ap_checked,
        'AP_import_mask': 1,
        'AP_windows': [{'offset': hex(at), 'bytes': len(data)} for at, data in ap_windows],
        'in_memory_composed_hashes': [sha(raw) for raw in combined],
        'firmware_candidate_written': False, 'recipe_or_resources_modified': False,
        'layout_source_sha256': sha(layout_path.read_bytes()), 'CP_descriptor_extents': extents,
        'initialized_data_to_BSS_gap_bytes': gap, 'bootstrap_prefix_loading_replayed': False}


def gamma_dispatch(ours, composed):
    factory = next((ROOT / 'research/extracted/ILCE-7CM2-2.01').rglob('CX89700_ALLLANG.bin'))
    records = core.backup_records(factory.read_bytes())
    units = core.merge_colors(*(records[1][key].data for key in core.CP_KEYS))
    before, after = Image(ours, units), Image(composed, units)
    gamma = dispatch = custom = 0
    for mode, column, override in itertools.product(range(21), [0, 1], range(8)):
        assert before.gamma(mode, column, override) == after.gamma(mode, column, override), (mode, column, override)
        gamma += 1
    for marker, column, override in itertools.product(range(249, 255), [0, 1], range(8)):
        assert after.gamma(marker, column, override) == before.gamma(0, column, override)
        custom += 1
    for mode, still, variant, flag in itertools.product(range(21), [False, True], range(4), [0, 1]):
        assert before.dispatch(mode, still, variant, flag) == after.dispatch(mode, still, variant, flag)
        dispatch += 1
    return {'existing_gamma_cases': gamma, 'custom_markers_ST_gamma_cases': custom,
            'existing_color_dispatch_cases': dispatch, 'corrected_FL2_FL3_movie_gamma_preserved': True,
            'scope': 'Isolated instructions/factory template units, not a camera pipeline'}


def helper_cases(image):
    """Execute actual composed helper + checksum with signed Astia table."""
    u = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
    u.mem_map(BASE, (len(image) + 4095) & ~4095); u.mem_write(BASE, image)
    u.mem_map(0x30000000, 0x20000); u.mem_map(0x31000000, 4096)
    ctx, root, env, out, state, work = [0x30000000 + i * 4096 for i in range(6)]
    u.mem_write(ctx, struct.pack('<II', root, env)); u.mem_write(out + 12, struct.pack('<I', work))
    rows = []
    for root_flag, direct, bank, corrupt in itertools.product([0, 1, 2], [0, 1, 2], [False, True], [False, True]):
        u.mem_write(root + 1, bytes([root_flag])); u.mem_write(root + 0x2e, b'\xf9')
        u.mem_write(env + 6, bytes([direct])); u.mem_write(work + 0x6b0, struct.pack('<I', 0x30009000 if bank else 0))
        original = image[0x8020]; u.mem_write(0x110020, bytes([original ^ int(corrupt)]))
        for reg, value in [(UC_ARM_REG_R0, 7), (UC_ARM_REG_R1, 9), (UC_ARM_REG_R2, 0x3000b000),
                           (UC_ARM_REG_R5, state), (UC_ARM_REG_R8, out), (UC_ARM_REG_R9, ctx),
                           (UC_ARM_REG_R10, root), (UC_ARM_REG_SP, 0x3001f000), (UC_ARM_REG_LR, 0x31000001)]:
            u.reg_write(reg, value)
        u.emu_start(0xc2f491, 0x31000000, count=20000)
        expected = not corrupt and ((root_flag == 1 and direct <= 1) or (root_flag == 0 and (direct == 1 or (direct == 0 and bank))))
        actual = struct.unpack('<IBB', u.mem_read(state + 0x184, 6))
        assert actual == ((0x110020, 1, 2) if expected else (0x3000b000, 9, 7)), (root_flag, direct, bank, corrupt, actual)
        assert u.reg_read(UC_ARM_REG_PC) == 0x31000000 and u.reg_read(UC_ARM_REG_SP) == 0x3001f000
        rows.append({'root_flag': root_flag, 'direct': direct, 'bank_present': bank, 'corrupt_table': corrupt,
                     'Astia_selected': bool(expected)})
    return rows


def hif_gamma(original):
    ap, cp = original
    assert struct.unpack_from('<5I', cp, 0xff3f64) == (0x4141, 0x356341, 1, 0, 0x3642fd)
    assert ap[0x4c7c4a7:].split(b'\0', 1)[0] == b'PID_SET_HEIF_GAMMA'
    u = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
    for address, count in [(0x3528000, 4096), (0x30000000, 0x10000), (0x31000000, 4096)]: u.mem_map(address, count)
    u.mem_write(0x3528000, ap[0x3528000:0x3529000])
    converter_cases = 0
    for value, selector in itertools.product([0, 1], range(30)):
        u.mem_write(0x30000000, b'\xa5')
        for reg, n in [(UC_ARM64_REG_X0, value), (UC_ARM64_REG_X1, selector), (UC_ARM64_REG_X2, 0x30000000),
                       (UC_ARM64_REG_SP, 0x3000f000), (UC_ARM64_REG_LR, 0x31000000)]: u.reg_write(reg, n)
        u.emu_start(0x3528564, 0x31000000, count=100)
        assert bytes(u.mem_read(0x30000000, 1)) == bytes([1 if value == 0 and selector <= 11 else 0])
        assert u.reg_read(UC_ARM64_REG_SP) == 0x3000f000
        converter_cases += 1
    v = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
    v.mem_map(BASE, (len(cp) + 4095) & ~4095); v.mem_write(BASE, cp)
    for addr, n in [(0x165a000, 0x2000), (0x1a53000, 0x1000), (0x30000000, 4096), (0x31000000, 4096)]: v.mem_map(addr, n)
    for value in range(256):
        v.mem_write(0x30000008, bytes([value])); v.reg_write(UC_ARM_REG_R0, 0x30000000); v.reg_write(UC_ARM_REG_LR, 0x31000001)
        v.emu_start(0x356341, 0x31000000, count=100)
        assert struct.unpack('<I', v.mem_read(0x165a81c, 4))[0] == value
    return {'command': '0x4141', 'CP_dispatch_row_file': '0xff3f64',
            'AP_name': 'PID_SET_HEIF_GAMMA', 'AP_converter_cases': converter_cases,
            'CP_setter_cases': 256, 'CP_destination': '0x165a720 + 0xfc',
            'CP_validation_callback': '0x3642fc',
            'AP_to_CP_transport_executed': False, 'debug_logging_path_executed': False,
            'numeric_values_assigned_SDR_HLG_names': False,
            'native_table_to_HIF_encoder_pixel_join_proved': False}


def main():
    original, ours, combined, composition = compose()
    report = {'schema': 1, 'camera_accessed': False, 'card_accessed': False, 'ghub_accessed': False,
              'compiler_invoked': False, 'firmware_written': False, 'network_used': False,
              'composition': composition, 'behavior': gamma_dispatch(ours[1], combined[1]),
              'native_helper_cases': helper_cases(combined[1]),
              'actual_Astia_stock_bank_cases': packing(original[1], [(WORK / 'slot1-astia.raw-s8.bin').read_bytes()]),
              'HIF_gamma': hif_gamma(original),
              'generator_sha256': sha(Path(__file__).read_bytes()), 'original_hashes': [sha(b) for b in original]}
    # Retain bounded original-instruction evidence, not firmware payloads.
    ranges = {'AP': [(0x354182c, 0x35418bc), (0x3528564, 0x3528598), (0x3528630, 0x3528648)],
              'CP': [(0x356340, 0x356370), (0x3642fc, 0x364324),
                     (0x74cb3a, 0x74cb72), (0x23d4e8, 0x23d4fa),
                     (0x24e54a, 0x24e558)]}
    regions = []
    for chip, items in ranges.items():
        md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN) if chip == 'AP' else Cs(CS_ARCH_ARM, CS_MODE_THUMB)
        image = original[0 if chip == 'AP' else 1]
        for lo, hi in items:
            off = lo if chip == 'AP' else lo - BASE
            data = image[off:off + hi - lo]
            ins = list(md.disasm(data, lo)); assert sum(i.size for i in ins) == hi - lo
            regions.append({'chip': chip, 'start_VA': hex(lo), 'end_VA': hex(hi), 'file_offset': hex(off),
                            'sha256': sha(data), 'instructions': [{'VA': hex(i.address), 'mnemonic': i.mnemonic, 'operands': i.op_str} for i in ins]})
    report['source_regions'] = regions
    (ROOT / 'research/evidence/astia-native-integration.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'composition': composition, 'behavior': report['behavior'],
                      'helper_cases': len(report['native_helper_cases']), 'HIF_gamma': report['HIF_gamma']}, indent=2))


if __name__ == '__main__': main()
