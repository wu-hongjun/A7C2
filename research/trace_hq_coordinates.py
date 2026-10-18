#!/usr/bin/env python3
"""Source-bound static and mathematical HQ identity-coordinate checks; no camera access.

Requires a rebuilt synthetic quantizer capture from trace_hq_lut.py. The retained
JSON embeds the twenty observed axis floats so its conclusions remain readable
without the ignored synthetic fixture. This is not a general LUT resampler.
"""
import hashlib
import itertools
import json
import math
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked/lib/appFw.so'
SOURCE_SHA = '2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
CASE = ROOT / 'research/evidence/hq-lut-cases/17-identity.json'
FIXTURE = ROOT / 'research/build/hq-lut/17-identity.quantizer-inputs.f32'
OUTPUT = ROOT / 'research/evidence/hq-coordinates.json'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


def word_index(c, r, g, b):
    return c * 8000 + (4 * (r % 2) + 2 * (g % 2) + b % 2) * 1000 + (r // 2) * 100 + (g // 2) * 10 + b // 2


def main():
    source = TARGET.read_bytes()
    assert sha(source) == SOURCE_SHA
    case_bytes = CASE.read_bytes()
    case = json.loads(case_bytes)
    assert case['source']['sha256'] == SOURCE_SHA
    assert case['result']['grid'] == 17 and case['result']['pattern'] == 'identity'
    assert case['result']['finished'] and case['result']['quantizer_oracle']['exact_match']
    fixture = FIXTURE.read_bytes()
    assert len(fixture) == 24000 * 4
    captured = struct.unpack('<24000f', fixture)
    low, = struct.unpack('<d', source[0x51a04a8:0x51a04b0])
    high, = struct.unpack('<d', source[0x51a04a0:0x51a04a8])
    # The original double reciprocal followed by a reciprocal is retained rather
    # than silently replacing that sequence by an algebraic simplification.
    scale = f32(1. / (1. / (high - low)))
    offset = f32(low)
    expected = [f32(math.fma(f32(f32(i) / f32(19)), scale, offset)) for i in range(20)]
    observed = [captured[word_index(0, i, 0, 0)] for i in range(20)]
    registers = case['result']['register_u32_le'][11:31]
    expected_registers = [int(v * 8192.) for v in expected]
    assert observed == expected
    assert registers == expected_registers
    mismatch_count = 0
    max_delta = 0.
    for c, r, g, b in itertools.product(range(3), range(20), range(20), range(20)):
        actual = captured[word_index(c, r, g, b)]
        delta = abs(actual - expected[(r, g, b)[c]])
        mismatch_count += delta != 0
        max_delta = max(max_delta, delta)
    ranges = [(0x36f5344, 0x36f5434, 'Format/flags select fixed endpoint constants; offset and reciprocal scale'),
              (0x36f5d50, 0x36f5d94, 'Calls endpoint helper and reverses offset/scale before affine mapping'),
              (0x36f5ecc, 0x36f5f2c, 'Float32 index divided by dimension minus one, then affine helper'),
              (0x36f5744, 0x36f57e0, 'Float32 affine operation on each RGB coordinate')]
    md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    with TARGET.open('rb') as file:
        elf = ELFFile(file)
        for a, b, _ in ranges:
            segment = next(s for s in elf.iter_segments() if s.header.p_type == 'PT_LOAD' and s.header.p_vaddr <= a and b <= s.header.p_vaddr + s.header.p_filesz)
            assert a - segment.header.p_vaddr + segment.header.p_offset == a
    evidence = {
        'schema': 1, 'method': 'Original instruction bytes plus independent host mathematics and prior bounded-emulator capture; not REA Evidence',
        'generator': {'path': str(Path(__file__).relative_to(ROOT)), 'sha256': sha(Path(__file__).read_bytes())},
        'source': {'path': str(TARGET.relative_to(ROOT)), 'sha256': SOURCE_SHA},
        'case': {'path': str(CASE.relative_to(ROOT)), 'sha256': sha(case_bytes),
                 'input_grid': 17, 'pattern': 'identity', 'output_48128_sha256': case['result']['output_48128_sha256']},
        'synthetic_capture': {'path': str(FIXTURE.relative_to(ROOT)), 'sha256': sha(fixture), 'bytes': len(fixture),
                              'meaning': '24000 Float32 inputs immediately before original final output quantizer; source layout is channel/parity banks'},
        'camera_access': False, 'network_access': False, 'target_execution_in_this_script': False,
        'constants': [{'va': hex(a), 'file_offset': a, 'bytes': source[a:a+8].hex(),
                       'float64_value': struct.unpack('<d', source[a:a+8])[0]}
                      for a in [0x51a04a0, 0x51a04a8]],
        'formula': 'f32(fma(f32(f32(i)/f32(19)), f32(1/(1/(high-low))), f32(low)))',
        'observed_axis_float32': observed, 'independent_expected_axis_float32': expected,
        'published_register_words_11_to_30': registers,
        'analytic_truncation_expected_register_words': expected_registers,
        'validation': {'axis_values_compared': 20, 'axis_exact_match': observed == expected,
                       'axis_max_abs_error': max(abs(a-b) for a,b in zip(observed, expected)),
                       'register_words_compared': 20, 'register_analytic_truncation_exact': registers == expected_registers,
                       'all_identity_float_samples_compared': 24000,
                       'all_identity_float_samples_mismatched': mismatch_count,
                       'all_identity_float_samples_max_abs_error': max_delta},
        'static_ranges': [{'start_va': hex(a), 'end_va_exclusive': hex(b), 'file_offset': a, 'role': role,
                           'sha256': sha(source[a:b]),
                           'instructions': [{'va': hex(i.address), 'bytes': i.bytes.hex(),
                                             'mnemonic': i.mnemonic, 'operands': i.op_str}
                                            for i in md.disasm(source[a:b], a)]}
                          for a,b,role in ranges],
        'limits': ['The examined format/flag path selects fixed constants; no adaptive-grid optimization has been established.',
                   'Only previously completed finite synthetic identity captures establish the numerical comparison.',
                   'Register values match analytic truncation; this script does not identify the exact register-generator instructions.',
                   'The comparison does not recover all interpolation, color-domain transforms, nonlinear input behavior or camera rendering.',
                   'These coordinate anchors are not identified as camera black/white clipping levels, legal RGB bounds, a color standard or image-domain limits; interpolation/extrapolation outside anchors remains unknown.',
                   'The ignored synthetic capture is required to regenerate this JSON, but the twenty observed values are embedded here.']}
    OUTPUT.write_text(json.dumps(evidence, indent=2) + '\n')
    print(json.dumps(evidence['validation'], indent=2))


if __name__ == '__main__':
    main()
