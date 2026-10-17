#!/usr/bin/env python3
"""Source-bound AP native LUT producer trace and isolated host emulation.

Runs selected original ARM64 instructions in Unicorn with explicit libc stubs.
No camera, system call forwarding, native target process, or firmware mutation.
"""
from collections import Counter
import hashlib
import itertools
import json
import math
from pathlib import Path
import random
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X30, UC_ARM64_REG_PC, UC_ARM64_REG_SP, UC_ARM64_REG_CPACR_EL1

ROOT = Path(__file__).resolve().parents[1]
AP = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked/lib/appFw.so'
EXPECTED = '2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
RANGES = [
    (0x302002c, 0x30201d0, 'legacy parity-bank packer'),
    (0x30201d0, 0x3020308, 'bounded text line reader'),
    (0x3020308, 0x302040c, 'CUBE size/input-range scanner'),
    (0x302040c, 0x3020430, 'saved text CUBE filename'),
    (0x3020430, 0x3020454, 'saved native LUT filename'),
    (0x30208d4, 0x3020e40, 'CUBE parser and two conversion paths'),
    (0x3020e40, 0x3020f70, 'paired native and original-text writer'),
    (0x367f414, 0x367f480, 'raw open/write/fsync/close helper'),
    (0x3021338, 0x30213e0, 'native and text file deletion helpers'),
    (0x2887b70, 0x2887b84, 'HQ conversion current-value getter'),
    (0x2b9c168, 0x2b9c210, 'bounded HQ setter and parameter ID context'),
    (0x3088858, 0x30889ac, 'first import caller: flag read and delete-before-write'),
    (0x30894a0, 0x30896e8, 'second import caller: flag read and delete-before-write'),
    (0x36f6900, 0x36f6e54, 'IPSX/LosGatos conversion orchestrator'),
    (0x36fcfb0, 0x36fd188, 'alternate output serialization dispatch'),
]
XREGS = [UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def quantize_sample(value):
    """Finite scalar reference for the observed FMADD/clamp/FCVTZU sequence."""
    if not math.isfinite(value):
        raise ValueError('Reference quantizer requires a finite value')
    return math.trunc(min(65535., max(0., math.fma(value, 8192., .5))))


def pack_reference(values, n):
    """Independent direct destination formula, not the source loop structure."""
    if n not in (17, 33) or len(values) != n ** 3 * 3:
        raise ValueError('Expected red-fastest RGB17 or RGB33 quantized grid')
    if any(type(v) is not int or not 0 <= v <= 65535 for v in values):
        raise ValueError('Quantized samples must be uint16 integers')
    out = [0] * 24000
    step = (n - 1) // 16
    for b, g, r in itertools.product(range(17), repeat=3):
        bank = (r % 2) * 4 + (g % 2) * 2 + b % 2
        local = (r // 2) * 100 + (g // 2) * 10 + b // 2
        src = ((b * step * n + g * step) * n + r * step) * 3
        for channel in range(3):
            out[channel * 8000 + bank * 1000 + local] = values[src + channel]
    return struct.pack('<24000H', *out)


def unpack_reference(payload):
    """Recover the retained red-fastest RGB17 grid, without inventing 33-point nodes."""
    if len(payload) != 48000:
        raise ValueError('Ordinary native LUT payload must be exactly 48000 bytes')
    words = struct.unpack('<24000H', payload)
    out = []
    for b, g, r in itertools.product(range(17), repeat=3):
        bank = (r % 2) * 4 + (g % 2) * 2 + b % 2
        local = (r // 2) * 100 + (g // 2) * 10 + b // 2
        out.extend(words[c * 8000 + bank * 1000 + local] for c in range(3))
    return out


class Machine:
    CODE = 0x302002c
    INPUT = 0x10000000
    DESCRIPTOR = 0x11000000
    OUTPUT = 0x12000000
    ALT_OUTPUT = 0x12010000
    AUX_OUTPUT = 0x12020000
    AUX_FLOATS = 0x12030000
    RETURN = 0x13000000
    HEAP = 0x14000000
    STACK = 0x20000000

    def __init__(self, data, imports):
        self.u = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
        self.calls = Counter()
        self.logs = []
        self.converter_capture = None
        self.file_writes = []
        self.file_removals = []
        self.fds = {}
        self.heap_next = self.HEAP
        self.imports = imports
        self.u.reg_write(UC_ARM64_REG_CPACR_EL1, 3 << 20)
        for va, size in [(0x301f000, 0x4000), (0x367f000, 0x1000), (0x4bfa000, 0x1000), (0x51a0000, 0x1000), (0x48d1000, 0x1000),
                         (0x36f6000, 0x1000), (0x3702000, 0x1000)]:
            self.u.mem_map(va, size)
            self.u.mem_write(va, data[va:va + size])
        for va, size in [(0x165d000, 0x4000), (0x3680000, 0x1000),
                         (0x5b8f000, 0x1000), (0x5b7e000, 0x1000),
                         (0x5b85000, 0x1000), (0x5bac000, 0x1000),
                         (self.INPUT, 0x800000), (self.DESCRIPTOR, 0x1000),
                         (self.OUTPUT, 0x500000), (self.RETURN, 0x1000),
                         (self.HEAP, 0x100000), (self.STACK, 0x800000)]:
            self.u.mem_map(va, size)
        for slot, val in [(0x5b8f438, self.OUTPUT), (0x5b7e8f8, self.AUX_OUTPUT),
                          (0x5b853f8, self.ALT_OUTPUT), (0x5bac6b8, self.AUX_FLOATS)]:
            self.u.mem_write(slot, struct.pack('<Q', val))
        self.u.hook_add(UC_HOOK_CODE, self.stub, begin=0x165d000, end=0x1660fff)
        self.u.hook_add(UC_HOOK_CODE, self.stub, begin=0x36804c8, end=0x36804c8)
        self.u.hook_add(UC_HOOK_CODE, self.capture_converter, begin=0x36f6e50, end=0x36f6e50)

    def cstring(self, p, limit=4096):
        out = bytearray()
        while len(out) < limit:
            count = min(limit - len(out), 0x1000 - (p & 0xfff))
            block = bytes(self.u.mem_read(p, count))
            if b'\0' in block:
                return (out + block.split(b'\0', 1)[0]).decode('ascii')
            out.extend(block)
            p += count
        raise AssertionError('Unterminated emulated C string')

    def returning(self, value=0):
        self.u.reg_write(UC_ARM64_REG_X0, value & 0xffffffffffffffff)
        self.u.reg_write(UC_ARM64_REG_PC, self.u.reg_read(UC_ARM64_REG_X30))

    def stub(self, u, address, size, _):
        args = [u.reg_read(r) for r in XREGS]
        name = self.imports.get(address, 'diagnostic' if address == 0x36804c8 else '')
        self.calls[name] += 1
        if name == 'memset':
            u.mem_write(args[0], bytes([args[1] & 255]) * args[2])
            self.returning(args[0])
        elif name == 'memcpy':
            u.mem_write(args[0], bytes(u.mem_read(args[1], args[2])))
            self.returning(args[0])
        elif name == 'strncmp':
            a, b = bytes(u.mem_read(args[0], args[2])).split(b'\0', 1)[0], bytes(u.mem_read(args[1], args[2])).split(b'\0', 1)[0]
            self.returning((a > b) - (a < b))
        elif name == 'sscanf':
            text, fmt = self.cstring(args[0]), self.cstring(args[1])
            tokens = text.split()
            count = 0
            try:
                if fmt == '%29s %d':
                    u.mem_write(args[2], tokens[0][:29].encode() + b'\0')
                    count = 1
                    u.mem_write(args[3], struct.pack('<i', int(tokens[1])))
                    count = 2
                elif fmt == '%99s %f %f':
                    u.mem_write(args[2], tokens[0][:99].encode() + b'\0')
                    count = 1
                    for i in range(2):
                        u.mem_write(args[3 + i], struct.pack('<f', float(tokens[1 + i])))
                        count += 1
                elif fmt == '%lf %lf %lf':
                    for i in range(3):
                        u.mem_write(args[2 + i], struct.pack('<d', float(tokens[i])))
                        count += 1
                else:
                    raise AssertionError(('Unsupported scanf format', fmt))
            except (ValueError, IndexError):
                pass
            self.returning(count)
        elif name == '_Znwm':
            out = self.heap_next
            self.heap_next += (args[0] + 15) & ~15
            assert self.heap_next < self.HEAP + 0x100000
            self.returning(out)
        elif name == '_ZdlPvm':
            self.returning()
        elif name == 'snprintf':
            fmt, directory = self.cstring(args[2]), self.cstring(args[3])
            assert fmt in {'%s/LUT%03d.LUT', '%s/LUT%03d.cube'}
            text = (fmt % (directory, args[4])).encode('ascii')
            u.mem_write(args[0], text[:args[1] - 1] + b'\0')
            self.returning(len(text))
        elif name == 'open':
            fd = len(self.fds) + 101
            self.fds[fd] = {'path': self.cstring(args[0]), 'flags': args[1], 'mode': args[2]}
            self.returning(fd)
        elif name == 'write':
            raw = bytes(u.mem_read(args[1], args[2]))
            self.file_writes.append({**self.fds[args[0]], 'bytes': len(raw), 'sha256': digest(raw)})
            self.returning(len(raw))
        elif name in {'fsync', 'close'}:
            self.returning()
        elif name == 'remove':
            self.file_removals.append(self.cstring(args[0]))
            self.returning()
        elif name == 'diagnostic':
            self.logs.append({'format': self.cstring(args[0]), 'line': args[1]})
            self.returning()
        else:
            raise AssertionError(('Unstubbed external', hex(address), name))

    def capture_converter(self, u, address, size, _):
        # Do not claim this stub's output is a real alternate native LUT.
        _, config, outputs = [u.reg_read(r) for r in XREGS[:3]]
        n = u.mem_read(config + 0x23, 1)[0]
        ptr = struct.unpack('<Q', u.mem_read(config + 0x28, 8))[0]
        first, second = struct.unpack('<2Q', u.mem_read(outputs, 16))
        a = struct.unpack('<Q', u.mem_read(first, 8))[0]
        b = struct.unpack('<Q', u.mem_read(second, 8))[0]
        samples = []
        for rr, gg, bb in [(0, 0, 0), (1, 2, 3), (n - 1, n - 1, n - 1)]:
            idx = (rr * n + gg) * n + bb
            samples.append({'coordinate': [rr, gg, bb], 'float4': list(struct.unpack('<4f', u.mem_read(ptr + 16 * idx, 16)))})
        self.converter_capture = {'converter_va': address, 'grid_size': n,
            'config_first_36_bytes': bytes(u.mem_read(config, 36)).hex(),
            'float4_buffer_bytes': n ** 3 * 16,
            'float4_buffer_sha256': digest(bytes(u.mem_read(ptr, n ** 3 * 16))),
            'samples': samples, 'output_pointer_difference': b - a,
            'converter_was_stubbed': True, 'payload_generated': False}
        self.returning()

    def invoke(self, start, args):
        self.u.reg_write(UC_ARM64_REG_SP, self.STACK + 0x7ff000)
        self.u.reg_write(UC_ARM64_REG_X30, self.RETURN)
        for reg, val in zip(XREGS, args):
            self.u.reg_write(reg, val)
        self.u.emu_start(start, self.RETURN, timeout=30_000_000, count=20_000_000)
        assert self.u.reg_read(UC_ARM64_REG_PC) == self.RETURN, 'Emulation instruction/time bound reached'
        return self.u.reg_read(UC_ARM64_REG_X0)

    def pack(self, values, n):
        raw = struct.pack('<' + str(len(values)) + 'H', *values)
        self.u.mem_write(self.INPUT, raw)
        self.invoke(0x302002c, [self.INPUT, n, self.DESCRIPTOR])
        size, ptr = struct.unpack('<I4xQ', self.u.mem_read(self.DESCRIPTOR, 16))
        assert size == 48000
        return bytes(self.u.mem_read(ptr, size))

    def parse(self, text, flag=0):
        raw = text.encode('ascii')
        self.u.mem_write(self.INPUT + 0x1000, raw + b'\0' * 512)
        self.u.mem_write(self.INPUT, struct.pack('<Q Q I', 0, self.INPUT + 0x1000, len(raw)))
        status = self.invoke(0x30208d4, [self.INPUT, self.DESCRIPTOR, flag])
        size, ptr = struct.unpack('<I4xQ', self.u.mem_read(self.DESCRIPTOR, 16))
        output = bytes(self.u.mem_read(ptr, size)) if status and size else b''
        return status, output


def make_cube(n, fn):
    rows = [f'LUT_3D_SIZE {n}']
    for b, g, r in itertools.product(range(n), repeat=3):
        rows.append(' '.join(format(v, '.17g') for v in fn(r, g, b)))
    return '\n'.join(rows) + '\n'


def main():
    data = AP.read_bytes()
    assert digest(data) == EXPECTED
    with AP.open('rb') as f:
        elf = ELFFile(f)
        syms = elf.get_section_by_name('.dynsym')
        plt = elf.get_section_by_name('.plt')
        imports = {plt.header.sh_addr + 32 + 16 * n: syms.get_symbol(r.entry.r_info_sym).name
                   for n, r in enumerate(elf.get_section_by_name('.rela.plt').iter_relocations())}
        text = elf.get_section_by_name('.text')
        call_targets = {0x302002c, 0x30208d4, 0x3020e40, 0x3020430, 0x302040c,
                        0x36f6e50, 0x36fd0d8}
        calls = []
        for n, (word,) in enumerate(struct.iter_unpack('<I', text.data())):
            if word & 0xfc000000 == 0x94000000:
                imm = word & 0x3ffffff
                if imm & 0x2000000:
                    imm -= 0x4000000
                site = text.header.sh_addr + n * 4
                target = site + imm * 4
                if target in call_targets:
                    calls.append({'call_va': site, 'target_va': target})
    md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    ranges = []
    for start, end, label in RANGES:
        raw = data[start:end]
        ins = []
        for i in md.disasm(raw, start):
            row = {'va': i.address, 'bytes': bytes(i.bytes).hex(), 'mnemonic': i.mnemonic, 'operands': i.op_str}
            if i.mnemonic in {'bl', 'b'} and i.op_str.startswith('#'):
                dst = int(i.op_str[1:], 16)
                if dst in imports:
                    row['import'] = imports[dst]
            ins.append(row)
        assert len(ins) * 4 == len(raw)
        ranges.append({'label': label, 'start_va': start, 'end_va_exclusive': end,
                       'file_offset': start, 'bytes': raw.hex(), 'sha256': digest(raw), 'instructions': ins})
    cases = []
    rng = random.Random(71201)
    for n in (17, 33):
        for pattern in ('identity', 'random16', 'coordinate_code', 'channel_permutation', 'nonlinear'):
            values = []
            for b, g, r in itertools.product(range(n), repeat=3):
                if pattern == 'identity':
                    values.extend((r * 8192 // (n - 1), g * 8192 // (n - 1), b * 8192 // (n - 1)))
                elif pattern == 'random16':
                    values.extend(rng.randrange(65536) for _ in range(3))
                elif pattern == 'coordinate_code':
                    values.extend(((r + 41 * g + 1453 * b) % 65536,
                                   (7 * r + 97 * g + 2011 * b) % 65536,
                                   (13 * r + 131 * g + 3001 * b) % 65536))
                elif pattern == 'channel_permutation':
                    values.extend((b * 8192 // (n - 1), r * 8192 // (n - 1), g * 8192 // (n - 1)))
                else:
                    rr, gg, bb = r / (n - 1), g / (n - 1), b / (n - 1)
                    values.extend(quantize_sample(v) for v in
                                  (rr * rr, gg * bb, .5 * bb + .5 * rr * gg))
            machine = Machine(data, imports)
            actual = machine.pack(values, n)
            wanted = pack_reference(values, n)
            assert actual == wanted, (n, pattern)
            step = (n - 1) // 16
            decoded = unpack_reference(actual)
            wanted_decoded = []
            for b, g, r in itertools.product(range(17), repeat=3):
                pos = ((b * step * n + g * step) * n + r * step) * 3
                wanted_decoded.extend(values[pos:pos + 3])
            assert decoded == wanted_decoded
            cases.append({'kind': 'original_packer_vs_independent_formula', 'grid': n, 'pattern': pattern,
                          'compared_bytes': len(actual), 'sha256': digest(actual), 'passed': True,
                          'unpacked_samples_compared': len(decoded),
                          'external_stub_calls': dict(machine.calls)})
    parser_cases = []
    for n in (17, 33):
        fn = lambda r, g, b: (r / (n - 1), g / (n - 1), b / (n - 1))
        source = make_cube(n, fn)
        for label, text, accepted in [
            ('identity', source, True),
            ('domain_minmax_ignored', 'DOMAIN_MIN -1 -1 -1\nDOMAIN_MAX 2 2 2\n' + source, True),
            ('input_range_01', 'LUT_3D_INPUT_RANGE 0 1\n' + source, True),
            ('input_range_nondefault_rejected', 'LUT_3D_INPUT_RANGE -1 2\n' + source, False),
            ('truncated_row_count_rejected', source.rsplit('\n', 2)[0] + '\n', False),
            ('extra_complete_row_ignored', source + '0.1 0.2 0.3\n', True),
        ]:
            machine = Machine(data, imports)
            status, output = machine.parse(text)
            assert bool(status) == accepted, (n, label, status)
            if accepted:
                values = []
                for b, g, r in itertools.product(range(n), repeat=3):
                    values.extend(quantize_sample(v) for v in fn(r, g, b))
                assert output == pack_reference(values, n), (n, label)
            parser_cases.append({'grid': n, 'case': label, 'return_value': status,
                'output_bytes': len(output), 'output_sha256': digest(output) if output else None,
                'passed': True, 'external_stub_calls': dict(machine.calls), 'diagnostics': machine.logs})
        # The actual writer is executed with in-memory fd/write stubs: no host or
        # camera filesystem receives these writes. It must write raw native bytes
        # and the complete original CUBE text to separately numbered paths.
        machine = Machine(data, imports)
        status, output = machine.parse(source)
        assert status == 1
        assert machine.invoke(0x3021338, [0]) == 0
        assert machine.invoke(0x302138c, [0]) == 0
        write_status = machine.invoke(0x3020e40, [0, 0, machine.INPUT, machine.DESCRIPTOR, machine.DESCRIPTOR + 0x100])
        assert write_status == 1 and len(machine.file_writes) == 2
        assert machine.file_writes[0]['path'] == '/usr/data/lut/LUT001.LUT'
        assert machine.file_writes[0]['sha256'] == digest(output)
        assert machine.file_writes[1]['path'] == '/usr/data/lut/LUT001.cube'
        assert machine.file_writes[1]['sha256'] == digest(source.encode())
        assert machine.file_removals == ['/usr/data/lut/LUT001.LUT', '/usr/data/lut/LUT001.cube']
        parser_cases.append({'grid': n, 'case': 'native_and_original_text_writer_in_memory',
            'passed': True, 'file_calls_were_stubbed': True, 'actual_file_writes': False,
            'remove_calls': machine.file_removals,
            'write_calls': machine.file_writes, 'external_stub_calls': dict(machine.calls)})
        clamp_source = make_cube(n, lambda r, g, b: (-.5, .5, 9.))
        machine = Machine(data, imports)
        status, output = machine.parse(clamp_source)
        assert status == 1 and output == pack_reference([0, 4096, 65535] * n ** 3, n)
        parser_cases.append({'grid': n, 'case': 'negative_midpoint_and_high_clamp', 'passed': True,
            'output_sha256': digest(output), 'expected_unpadded_rgb_words': [0, 4096, 65535],
            'external_stub_calls': dict(machine.calls)})
        machine = Machine(data, imports)
        status, _ = machine.parse(source, flag=1)
        assert status == 1 and machine.converter_capture['output_pointer_difference'] == 48000
        capture = machine.converter_capture
        capture['parser_output_descriptor_bytes'] = struct.unpack('<I', machine.u.mem_read(machine.DESCRIPTOR, 4))[0]
        assert capture['parser_output_descriptor_bytes'] == 48128
        for row in capture['samples']:
            expected = [v / (n - 1) for v in row['coordinate']] + [0.]
            assert max(abs(a - b) for a, b in zip(row['float4'], expected)) < 1e-7
        parser_cases.append({'grid': n, 'case': 'alternate_converter_boundary_capture',
                             'passed': True, 'capture': capture, 'external_stub_calls': dict(machine.calls)})
    pool_base = 0x5063468
    names = []
    for off in [0x504605c, 0x504606c, 0x504607c]:
        param_id, size, delta, flags = struct.unpack_from('<4I', data, off)
        raw = data[pool_base + delta:pool_base + delta + size]
        names.append({'record_va': off, 'record_bytes': data[off:off + 16].hex(),
                      'parameter_id': param_id, 'string_bytes': size,
                      'string_pool_relative_offset': delta, 'record_flags': flags,
                      'string_va': pool_base + delta,
                      'name': raw.rstrip(b'\0').decode('ascii')})
    assert names[1]['parameter_id'] == 0x1a5d and names[1]['name'] == 'import_lut_hq_conversion'
    enums = []
    for off in [0x5867650, 0x5867660]:
        value, size, ptr = struct.unpack_from('<IIQ', data, off)
        enums.append({'record_va': off, 'record_bytes': data[off:off + 16].hex(),
                      'value': value, 'string_bytes': size, 'string_va': ptr,
                      'name': data[ptr:data.index(0, ptr)].decode('ascii')})
    assert [(e['value'], e['name']) for e in enums] == [(0, 'IMPORT_LUT_HQ_CONVERSION_OFF'), (1, 'IMPORT_LUT_HQ_CONVERSION_ON')]
    result = {'schema': 1, 'method': 'Ordinary ELF/Capstone and isolated Unicorn instruction emulation; not REA Evidence',
        'source': {'path': str(AP.relative_to(ROOT)), 'sha256': EXPECTED, 'bytes': len(data)},
        'generator': {'path': str(Path(__file__).relative_to(ROOT)), 'sha256': digest(Path(__file__).read_bytes())},
        'camera_access': False, 'hardware_execution': False, 'syscall_forwarding': False,
        'constants': {'quantization_scale_f64': struct.unpack_from('<d', data, 0x4bfa180)[0],
                      'quantization_clip_f64': struct.unpack_from('<d', data, 0x4bfa188)[0],
                      'bank_seeds_u32': list(struct.unpack_from('<24I', data, 0x4bfa190))},
        'incoming_direct_calls': calls, 'ranges': ranges, 'packer_checks': cases,
        'hq_conversion_parameter': {'string_pool_base_va': pool_base,
            'current_getter_va': 0x2887b70, 'current_value_static_bss_va': 0x7eda538,
            'setter_context_va': 0x2b9c168, 'parameter_name_records': names,
            'enum_value_records': enums,
            'runtime_value_observed': False,
            'limits': 'String metadata, setter ID and getter references identify the parameter; no camera runtime value or complete product/default gate was established.'},
        'parser_checks': parser_cases, 'all_checks_passed': True,
        'emulation_limits': ['Only mapped original instruction ranges execute in Unicorn; no host/native target execution.',
            'memset/memcpy/strncmp/sscanf/new/delete/diagnostic calls explicitly stubbed; sscanf supports only the three observed formats and canonical whitespace-separated synthetic input.',
            'Alternate vendor converter is intercepted at entry to capture input/output contracts, not executed; its payload packing is unresolved.',
            'No camera resource mapping, filesystem mount/write, image pipeline, or hardware driver call is executed. Native writer file calls are intercepted by in-memory path/hash stubs.',
            'Pure packer tests compare every output byte with an independently arranged destination-address formula.']}
    out = ROOT / 'research/evidence/native-lut-format.json'
    out.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'output': str(out), 'packer_cases': len(cases), 'parser_cases': len(parser_cases),
                      'compared_packer_bytes': sum(c['compared_bytes'] for c in cases), 'passed': True}))


if __name__ == '__main__':
    main()
