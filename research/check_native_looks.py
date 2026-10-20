"""Offline checks of the reviewed native-look selector and stock CP packer.

No USB, devices, compiler, subprocess, network or native firmware execution.
Unicorn runs only in synthetic memory. This is not a color-accuracy test.
"""
import argparse
import hashlib
import itertools
import json
import random
import struct
from pathlib import Path

from unicorn import Uc, UC_ARCH_ARM, UC_ARCH_ARM64, UC_MODE_THUMB, UC_MODE_ARM
from unicorn.arm_const import UC_ARM_REG_R0, UC_ARM_REG_R1, UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_PC
from unicorn.arm64_const import UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_LR, UC_ARM64_REG_PC

ROOT = Path(__file__).resolve().parents[1]
CP = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha8_unpacked/cpapp-b.bin'
CP_SHA = 'f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428'
REFERENCE = ROOT / 'research/build/qaqleonard-reference-20261020'
COMMIT = 'c046013c53883cd6f0fe660a9f5fec7a48d94722'


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def selectors(asset):
    """Execute published templates; independently calculate expected routing."""
    native = bytes([0, 3, 2, 1, 16, 15, 17, 18, 5, 13, 19, 20])
    cases = 0
    for mask, template in enumerate(asset['ap_templates']):
        u = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
        for start in [0x3528000, 0x4c7c000, 0x30000000, 0x31000000]:
            u.mem_map(start, 4096)
        u.mem_write(0x35287f0, bytes.fromhex(template))
        u.mem_write(0x4c7cf7c, native)
        for selector, binding in itertools.product([0, 9, 10, 11, 12, 13, 14, 15, 16, 17, 0xffffffff], [0, 1, 10, 11, 12]):
            u.mem_write(0x30000000, b'\xa5')
            for reg, value in [(UC_ARM64_REG_X0, selector), (UC_ARM64_REG_X1, binding),
                               (UC_ARM64_REG_X2, 0x30000000), (UC_ARM64_REG_LR, 0x31000000)]:
                u.reg_write(reg, value)
            # Invalid bindings branch to the original out-of-range handler.
            end = 0x3528874 if binding > 11 and selector != 16 else 0x31000000
            u.emu_start(0x35287f0, end, count=100)
            assert u.reg_read(UC_ARM64_REG_PC) == end
            expected = 255 if selector == 16 else (
                249 + selector - 10 if binding == 0 and 10 <= selector <= 15 and mask & (1 << (selector - 10))
                else native[binding] if binding <= 11 else 0xa5)
            assert bytes(u.mem_read(0x30000000, 1)) == bytes([expected])
            cases += 1
    assert len(asset['ap_templates']) == 64
    return cases


def packing(image):
    """Execute unpatched 2.01 builder with type 1 / transform 2 tables."""
    base = 0x108000
    order = struct.unpack_from('<405H', image, 0xec7c20 - base)
    assert sorted(order) == list(range(405))
    rng = random.Random(201)
    patterns = [bytes(1215), bytes([64 * 3]) * 1215, bytes([63]) * 1215,
                bytes((i % 128 - 64) & 255 for i in range(1215)),
                bytes(rng.randrange(-64, 64) & 255 for _ in range(1215))]
    cases = 0
    for table, level, direct, slot in itertools.product(patterns, [0, 1, 7, 8], [0, 1], range(3)):
        u = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
        u.mem_map(base, (len(image) + 4095) & ~4095)
        u.mem_write(base, image)
        u.mem_map(0x30000000, 0x20000)
        u.mem_map(0x31000000, 4096)
        ctx, root, env, out, state, work, meta, bank = [0x30000000 + i * 0x1000 for i in range(8)]
        raw = 0x30010000  # Separate from all three 1620-byte output banks.
        u.mem_write(ctx, struct.pack('<II', root, env))
        u.mem_write(out + 8, struct.pack('<III', state, work, meta))
        u.mem_write(work + 0x6b0, struct.pack('<I', bank))
        u.mem_write(root + 0x2f, bytes([level]))
        u.mem_write(env + 6, bytes([direct]))
        u.mem_write(meta + 0xb, bytes([slot]))
        u.mem_write(state + 0x184, struct.pack('<IBB', raw, 1, 2))
        u.mem_write(raw, table)
        for reg, value in [(UC_ARM_REG_R0, ctx), (UC_ARM_REG_R1, out),
                           (UC_ARM_REG_SP, 0x3001f000), (UC_ARM_REG_LR, 0x31000001)]:
            u.reg_write(reg, value)
        u.emu_start(0x23dc2d, 0x31000000, count=200000)
        assert u.reg_read(UC_ARM_REG_PC) == 0x31000000
        assert u.reg_read(UC_ARM_REG_SP) == 0x3001f000
        expected = bytearray(1620)
        for i in range(405):
            values = [struct.unpack_from('b', table, j * 405 + i)[0] for j in range(3)]
            if 1 <= level <= 7:
                values = [v * level >> 3 for v in values]
            word = ((values[0] & 127) << 22) | ((values[1] & 127) << 12) | ((values[2] & 127) << 2)
            struct.pack_into('<I', expected, order[i] * 4, word)
        destination = work + 0x5c if direct else bank + slot * 1620
        assert bytes(u.mem_read(destination, 1620)) == expected, (patterns.index(table), level, direct, slot)
        cases += 1
    return cases


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reference', type=Path, default=REFERENCE)
    parser.add_argument('--output', type=Path, default=ROOT / 'research/evidence/native-looks-checks.json')
    args = parser.parse_args()
    image = CP.read_bytes()
    assert sha(image) == CP_SHA
    names = ['src/looks/assets/runtime-v1.json', 'src/looks/assets/rendering-model-v1.json',
             'src/looks/looktool/native.py', 'src/looks/looktool/references.py',
             'src/looks/looktool/calibration.py', 'src/looks/looktool/standalone.py',
             'src/looks/looktool/sixbuild.py', 'src/looks/looktool/device_layout.py',
             'docs/en/technical/validation.md', 'docs/en/technical/formats.md', 'docs/en/profiles.md']
    sources = {name: sha((args.reference / name).read_bytes()) for name in names}
    asset = json.loads((args.reference / names[0]).read_text())
    model = json.loads((args.reference / names[1]).read_text())
    report = {'schema': 1, 'review_date': '2026-10-20', 'upstream_commit': COMMIT,
              'camera_accessed': False, 'ghub_accessed': False, 'network_used': False,
              'compiler_invoked': False, 'native_firmware_executed': False,
              'firmware_mutated': False, 'source_hashes': sources,
              'stock_cp_sha256': CP_SHA, 'stock_cp_bytes': len(image),
              'AP_selector_cases': selectors(asset), 'stock_CP_bank_cases': packing(image),
              'CP_configuration': {'source_type': 1, 'transform': 2, 'root_0x36': 0,
                                   'levels': [0, 1, 7, 8], 'bank_indices': [0, 1, 2], 'direct': [0, 1]},
              'native_raw_bytes': 1215, 'hardware_bank_bytes': 1620,
              'hardware_words': 405, 'permutation_bijective': True,
              'word_fields': {'Y': [22, 7], 'C1': [12, 7], 'C2': [2, 7]},
              'model_full_camera_characterization': model['full_camera_characterization'],
              'color_accuracy_tested': False, 'physical_HIF_tested': False,
              'limits': ['Synthetic contexts, no real preview scheduling or image hardware',
                         'AP templates reviewed from pinned local source; not rebuilt with an assembler',
                         'Original CP packer only; custom CP helper, allocator and uploader not run',
                         'Does not prove DCP fidelity, HIF, AUTO selection or compatibility with our installed patch']}
    report['generator_sha256'] = sha(Path(__file__).read_bytes())
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: report[k] for k in ['AP_selector_cases', 'stock_CP_bank_cases', 'color_accuracy_tested', 'camera_accessed']}))


if __name__ == '__main__':
    main()
