#!/usr/bin/env python3
"""Offline HQ LUT converter investigation using original ARM64 in Unicorn.

No target process execution, camera transport, network or syscall forwarding.
External allocator/libc/libstdc++ operations are bounded explicit host stubs.
"""
from collections import Counter
import argparse
import hashlib
import itertools
import json
import math
from pathlib import Path
import re
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE, UC_HOOK_MEM_INVALID, UC_HOOK_MEM_WRITE
from unicorn.arm64_const import UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X6, UC_ARM64_REG_X7, UC_ARM64_REG_X8, UC_ARM64_REG_X30, UC_ARM64_REG_PC, UC_ARM64_REG_SP, UC_ARM64_REG_D0, UC_ARM64_REG_D1, UC_ARM64_REG_CPACR_EL1

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked/lib/appFw.so'
SHA = '2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
X = [UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X6, UC_ARM64_REG_X7, UC_ARM64_REG_X8]


def sha(data):
    return hashlib.sha256(data).hexdigest()


class HQMachine:
    INPUT = 0x20000000
    CONFIG = 0x21000000
    OUTPUT = 0x22000000
    HEAP = 0x30000000
    HEAP_BYTES = 0x4000000
    STACK = 0x40000000
    RETURN = 0x50000000
    ERRNO = 0x51000000
    WATCH = [0x36f6900, 0x36f7c34, 0x36f9d00, 0x36f0b8c, 0x36f3d30, 0x36fd0d8, 0x36fc5a4, 0x36fcf94]

    def __init__(self, data):
        self.u = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
        self.calls = Counter()
        self.entries = Counter()
        self.notes = []
        self.note_count = 0
        self.chunks = []
        self.output_written = bytearray(48128)
        self.quantizer_inputs = [None] * 24000
        self.heap_next = self.HEAP
        self.failure = None
        self.hooks = []
        with TARGET.open('rb') as f:
            elf = ELFFile(f)
            for seg in elf.iter_segments():
                if seg.header.p_type != 'PT_LOAD':
                    continue
                start = seg.header.p_vaddr & ~0xfff
                end = (seg.header.p_vaddr + seg.header.p_memsz + 0xfff) & ~0xfff
                self.u.mem_map(start, end - start)
                self.u.mem_write(seg.header.p_vaddr, seg.data())
            syms = elf.get_section_by_name('.dynsym')
            plt = elf.get_section_by_name('.plt')
            self.imports = {plt.header.sh_addr + 32 + 16 * n: syms.get_symbol(r.entry.r_info_sym).name
                            for n, r in enumerate(elf.get_section_by_name('.rela.plt').iter_relocations())}
            import_addresses = {name: addr for addr, name in self.imports.items()}
            self.relative_relocations = 0
            self.function_pointer_relocations = Counter()
            for r in elf.get_section_by_name('.rela.dyn').iter_relocations():
                if r.entry.r_info_type == 1027:
                    self.u.mem_write(r.entry.r_offset, struct.pack('<Q', r.entry.r_addend))
                    self.relative_relocations += 1
                elif r.entry.r_info_sym:
                    symbol = syms.get_symbol(r.entry.r_info_sym)
                    if symbol.name in import_addresses:
                        self.u.mem_write(r.entry.r_offset, struct.pack('<Q', import_addresses[symbol.name] + r.entry.r_addend))
                        self.function_pointer_relocations[symbol.name] += 1
        for address, size in [(self.INPUT, 0x800000), (self.CONFIG, 0x1000),
                              (self.OUTPUT, 0x10000), (self.HEAP, self.HEAP_BYTES),
                              (self.STACK, 0x400000), (self.RETURN, 0x1000), (self.ERRNO, 0x1000)]:
            self.u.mem_map(address, size)
        self.u.reg_write(UC_ARM64_REG_CPACR_EL1, 3 << 20)
        self.u.hook_add(UC_HOOK_CODE, self.external, begin=plt.header.sh_addr, end=plt.header.sh_addr + plt.header.sh_size - 1)
        self.u.hook_add(UC_HOOK_MEM_INVALID, self.invalid_memory)
        self.u.hook_add(UC_HOOK_MEM_WRITE, self.output_write, begin=self.OUTPUT, end=self.OUTPUT + 48128 - 1)
        self.hooks.append(self.u.hook_add(UC_HOOK_CODE, self.capture_quantizer, begin=0x3702650, end=0x3702650))
        for address in self.WATCH:
            self.hooks.append(self.u.hook_add(UC_HOOK_CODE, self.watch, begin=address, end=address))

    def capture_quantizer(self, u, address, size, _):
        # Original loop at 3702638 indexes channel, bank and 1000 scalar nodes.
        from unicorn.arm64_const import UC_ARM64_REG_X24, UC_ARM64_REG_X25, UC_ARM64_REG_X26
        channel = u.reg_read(UC_ARM64_REG_X24) // 4
        bank = u.reg_read(UC_ARM64_REG_X25)
        local = u.reg_read(UC_ARM64_REG_X26) // 24
        ptr = u.reg_read(UC_ARM64_REG_X0) + channel * 4
        self.quantizer_inputs[channel * 8000 + bank * 1000 + local] = struct.unpack('<f', u.mem_read(ptr, 4))[0]

    def watch(self, u, address, size, _):
        self.entries[hex(address)] += 1

    def output_write(self, u, access, address, size, value, _):
        start = max(0, address - self.OUTPUT)
        end = min(48128, start + size)
        self.output_written[start:end] = b'\x01' * (end - start)

    def note(self, value):
        self.note_count += 1
        if len(self.notes) < 64:
            self.notes.append(value)

    def invalid_memory(self, u, access, address, size, value, _):
        self.failure = {'kind': 'unmapped_memory', 'pc': hex(u.reg_read(UC_ARM64_REG_PC)),
                        'access': access, 'address': hex(address), 'bytes': size}
        return False

    def alloc(self, n):
        size = (max(1, n) + 15) & ~15
        ptr = self.heap_next
        self.heap_next += size
        if self.heap_next > self.HEAP + self.HEAP_BYTES:
            raise RuntimeError('Bounded 64 MiB stub heap exhausted')
        return ptr

    def read_c(self, p, limit=0x10000):
        out = bytearray()
        while len(out) < limit:
            count = min(limit - len(out), 0x1000 - (p & 0xfff))
            block = bytes(self.u.mem_read(p, count))
            if b'\0' in block:
                return bytes(out + block.split(b'\0', 1)[0])
            out.extend(block)
            p += count
        raise RuntimeError('Unterminated bounded C string')

    def read_string(self, p):
        ptr, n = struct.unpack('<2Q', self.u.mem_read(p, 16))
        if n > 0x100000:
            raise RuntimeError('Oversized stub string')
        return bytes(self.u.mem_read(ptr, n)) if n else b''

    def put_string(self, p, value):
        if len(value) <= 15:
            ptr = p + 16
            self.u.mem_write(p, struct.pack('<2Q', ptr, len(value)))
        else:
            ptr = self.alloc(len(value) + 1)
            self.u.mem_write(p, struct.pack('<3Q', ptr, len(value), len(value)))
        self.u.mem_write(ptr, value + b'\0')

    def ret(self, value=0):
        self.u.reg_write(UC_ARM64_REG_X0, value & 0xffffffffffffffff)
        self.u.reg_write(UC_ARM64_REG_PC, self.u.reg_read(UC_ARM64_REG_X30))

    def external(self, u, address, size, _):
        name = self.imports.get(address)
        a = [u.reg_read(r) for r in X]
        self.calls[name or hex(address)] += 1
        string = 'basic_stringIcSt11char_traitsIcESaIcEE'
        try:
            if name in {'_Znwm', '_Znam', 'malloc'}:
                self.ret(self.alloc(a[0]))
            elif name in {'_ZdlPv', '_ZdlPvm', 'free'}:
                self.ret()  # Fixture-lifetime arena: never aliases freed storage.
            elif name in {'memcpy', 'memmove'} or (name and '13_S_copy_charsEPcPKcS7_' in name) or (name and '13_S_copy_charsEPcS5_S5_' in name):
                n = a[2] - a[1] if name not in {'memcpy', 'memmove'} else a[2]
                u.mem_write(a[0], bytes(u.mem_read(a[1], n)))
                self.ret(a[0])
            elif name == 'memset':
                u.mem_write(a[0], bytes([a[1] & 255]) * a[2])
                self.ret(a[0])
            elif name == 'strlen':
                self.ret(len(self.read_c(a[0])))
            elif name == 'memcmp':
                aa, bb = bytes(u.mem_read(a[0], a[2])), bytes(u.mem_read(a[1], a[2]))
                self.ret((aa > bb) - (aa < bb))
            elif name and string in name:
                if 'C1ERKS4_' in name or '9_M_assignERKS4_' in name:
                    self.put_string(a[0], self.read_string(a[1]))
                    self.ret(a[0])
                elif 'C1EOS4_' in name or 'aSEOS4_' in name:
                    self.put_string(a[0], self.read_string(a[1]))
                    self.put_string(a[1], b'')
                    self.ret(a[0])
                elif '10_M_disposeEv' in name:
                    self.ret()
                elif '7compareEPKc' in name:
                    aa, bb = self.read_string(a[0]), self.read_c(a[1])
                    self.note({'compare_call_return_va': hex(u.reg_read(UC_ARM64_REG_X30)),
                                       'left': aa.decode('ascii', 'replace'), 'right': bb.decode('ascii', 'replace')})
                    self.ret((aa > bb) - (aa < bb))
                elif '9_M_createERmm' in name:
                    n, = struct.unpack('<Q', u.mem_read(a[1], 8))
                    self.ret(self.alloc(n + 1))
                elif '6substrEmm' in name:
                    old = self.read_string(a[0])
                    self.put_string(a[8], old[a[1]:] if a[2] == 0xffffffffffffffff else old[a[1]:a[1] + a[2]])
                    self.ret()
                elif 'aSEPKc' in name:
                    self.put_string(a[0], self.read_c(a[1]))
                    self.ret(a[0])
                elif '12_M_constructEmc' in name:
                    self.put_string(a[0], bytes([a[2] & 255]) * a[1])
                    self.ret()
                elif '6appendERKS4_' in name or 'pLERKS4_' in name:
                    self.put_string(a[0], self.read_string(a[0]) + self.read_string(a[1]))
                    self.ret(a[0])
                elif '6appendEPKcm' in name:
                    self.put_string(a[0], self.read_string(a[0]) + bytes(u.mem_read(a[1], a[2])))
                    self.ret(a[0])
                elif '6insertEmRKS4_' in name or '6insertEmPKc' in name:
                    old = self.read_string(a[0])
                    new = self.read_string(a[2]) if 'RKS4_' in name else self.read_c(a[2])
                    self.put_string(a[0], old[:a[1]] + new + old[a[1]:])
                    self.ret(a[0])
                elif '9push_backEc' in name:
                    self.put_string(a[0], self.read_string(a[0]) + bytes([a[1] & 255]))
                    self.ret()
                else:
                    raise RuntimeError('Unsupported string stub: ' + name)
            elif name == 'strtol':
                text = self.read_c(a[0]).decode('ascii')
                base = a[2]
                pattern = r'\s*[+-]?(?:0[xX])?[0-9a-fA-F]+' if base == 16 else r'\s*[+-]?\d+'
                match = re.match(pattern, text)
                end = match.end() if match else 0
                if a[1]:
                    u.mem_write(a[1], struct.pack('<Q', a[0] + end))
                self.ret(int(match.group(), base or 0) if match else 0)
            elif name in {'vsnprintf', 'snprintf'}:
                fmt = self.read_c(a[2]).decode('ascii')
                if name == 'vsnprintf':
                    stack, gr_top, vr_top, gr_off, vr_off = struct.unpack('<QQQii', u.mem_read(a[3], 32))
                    ptr = gr_top + gr_off if gr_off < 0 else stack
                    value, = struct.unpack('<Q', u.mem_read(ptr, 8))
                else:
                    value = a[3]
                if fmt == '%d':
                    value &= 0xffffffff
                    value = value - 0x100000000 if value & 0x80000000 else value
                    out = str(value).encode()
                elif fmt in {'%u', '%x', '%X', '%08X'}:
                    value &= 0xffffffff
                    out = (fmt % value).encode()
                else:
                    raise RuntimeError('Unsupported bounded printf format: ' + fmt)
                if a[1]:
                    u.mem_write(a[0], out[:a[1] - 1] + b'\0')
                self.ret(len(out))
            elif name == '__errno_location':
                self.ret(self.ERRNO)
            elif name in {'pow', 'log10'}:
                d0 = struct.unpack('<d', struct.pack('<Q', u.reg_read(UC_ARM64_REG_D0)))[0]
                d1 = struct.unpack('<d', struct.pack('<Q', u.reg_read(UC_ARM64_REG_D1)))[0]
                out = math.pow(d0, d1) if name == 'pow' else math.log10(d0)
                u.reg_write(UC_ARM64_REG_D0, struct.unpack('<Q', struct.pack('<d', out))[0])
                self.ret()
            elif name in {'tolower', 'toupper'}:
                val = a[0]
                ch = chr(val)
                self.ret(ord(ch.lower() if name == 'tolower' else ch.upper()))
            elif name == '__cxa_guard_acquire':
                self.ret(not bool(u.mem_read(a[0], 1)[0]))
            elif name == '__cxa_guard_release':
                u.mem_write(a[0], b'\x01')
                self.ret()
            elif name == '__cxa_atexit':
                self.ret()
            elif name and name.startswith('_ZStls') and name.endswith('PKc'):
                self.note({'stream_text': self.read_c(a[1]).decode('ascii', 'replace')})
                self.ret(a[0])
            elif name and name.startswith('_ZSt4endl'):
                self.ret(a[0])
            else:
                raise RuntimeError('Unstubbed external: ' + str(name))
        except Exception as exc:
            self.failure = {'kind': 'stub_failure', 'pc': hex(address), 'name': name,
                            'arguments': [hex(v) for v in a], 'message': str(exc)}
            u.emu_stop()

    def invoke(self, address, args):
        self.u.reg_write(UC_ARM64_REG_SP, self.STACK + 0x3ff000)
        self.u.reg_write(UC_ARM64_REG_X30, self.RETURN)
        for r, val in zip(X, args):
            self.u.reg_write(r, val)
        try:
            for chunk in range(4):
                pc = address if chunk == 0 else self.u.reg_read(UC_ARM64_REG_PC)
                self.u.emu_start(pc, self.RETURN, timeout=60_000_000, count=150_000_000)
                if self.u.reg_read(UC_ARM64_REG_PC) == self.RETURN or self.failure:
                    break
                status = {'chunk': chunk + 1, 'pc': hex(self.u.reg_read(UC_ARM64_REG_PC)),
                          'entered_functions': dict(self.entries), 'heap_bytes': self.heap_next - self.HEAP}
                self.chunks.append(status)
                print(json.dumps({'bounded_progress': status}), flush=True)
        except Exception as exc:
            if self.failure is None:
                self.failure = {'kind': 'emulator_failure', 'pc': hex(self.u.reg_read(UC_ARM64_REG_PC)),
                    'lr': hex(self.u.reg_read(UC_ARM64_REG_X30)),
                    'arguments': [hex(self.u.reg_read(r)) for r in X], 'message': str(exc)}
        finished = self.u.reg_read(UC_ARM64_REG_PC) == self.RETURN
        if not finished and self.failure is None:
            self.failure = {'kind': 'time_or_instruction_bound', 'pc': hex(self.u.reg_read(UC_ARM64_REG_PC))}
        return finished

    def run(self, n=17, pattern='identity'):
        pixels = []
        for r, g, b in itertools.product(range(n), repeat=3):
            rr, gg, bb = r / (n - 1), g / (n - 1), b / (n - 1)
            value = (rr, gg, bb) if pattern == 'identity' else (bb, rr, gg)
            pixels.extend((*value, 0.))
        raw = struct.pack('<' + str(len(pixels)) + 'f', *pixels)
        self.u.mem_write(self.INPUT, raw)
        processor, config, wrappers, first, second = [self.CONFIG + i for i in [0, 0x100, 0x200, 0x300, 0x400]]
        self.u.mem_write(config, bytes(0x30))
        self.u.mem_write(config, b'\x01IPSX\0')
        self.u.mem_write(config + 0x23, bytes([n]))
        self.u.mem_write(config + 0x28, struct.pack('<Q', self.INPUT))
        self.u.mem_write(wrappers, struct.pack('<2Q', first, second))
        self.u.mem_write(first, struct.pack('<QI4x', self.OUTPUT, 0))
        self.u.mem_write(second, struct.pack('<QI4x', self.OUTPUT + 48000, 0))
        self.u.mem_write(self.OUTPUT, bytes(48128) + b'\xa5' * 32)
        self.invoke(0x36f662c, [processor])
        finished = self.invoke(0x36f6e50, [processor, config, wrappers])
        first_size, = struct.unpack('<I', self.u.mem_read(first + 8, 4))
        second_size, = struct.unpack('<I', self.u.mem_read(second + 8, 4))
        output = bytes(self.u.mem_read(self.OUTPUT, 48128))
        self.output_bytes = output
        return {'grid': n, 'pattern': pattern, 'finished': finished, 'failure': self.failure,
                'stub_calls': dict(self.calls), 'entered_functions': dict(self.entries),
                'notes': self.notes,
                'note_count': self.note_count, 'note_preview_limit': 64,
                'continuation_chunks': self.chunks,
                'heap_high_water_bytes': self.heap_next - self.HEAP,
                'relative_relocations_applied': self.relative_relocations,
                'external_function_pointer_relocations': dict(self.function_pointer_relocations),
                'input_float4_sha256': sha(raw), 'sram_reported_bytes': first_size,
                'register_reported_bytes': second_size, 'output_48128_sha256': sha(output),
                'sram_sha256': sha(output[:48000]), 'register_124_sha256': sha(output[48000:48124]),
                'register_u32_le': list(struct.unpack('<31I', output[48000:48124])),
                'trailing_four_hex': output[48124:].hex(),
                'written_byte_coverage': {'sram': sum(self.output_written[:48000]),
                                          'register_124': sum(self.output_written[48000:48124]),
                                          'trailing_four': sum(self.output_written[48124:])},
                'nonzero_output_bytes': sum(v != 0 for v in output),
                'trailing_128_hex': output[48000:].hex(),
                'output_guard_unchanged': bytes(self.u.mem_read(self.OUTPUT + 48128, 32)) == b'\xa5' * 32,
                'quantizer_oracle': verify_quantizer(self.quantizer_inputs, output)}


def f32(v):
    return struct.unpack('<f', struct.pack('<f', v))[0]


def quantize_reference(value):
    """Independent transcription of 3702650..3702664 for finite bounded cases.

    Hardware uses signed FCVTZS; these fixtures stay in 0..65535. No inferred
    clipping contract or HDR/negative input support is promised here.
    """
    adjusted = f32(value + f32(0.0000005))
    return int(f32(math.fma(adjusted, 8192., .5)))


def verify_quantizer(inputs, output):
    if any(v is None for v in inputs):
        return {'complete': False, 'captured': sum(v is not None for v in inputs)}
    expected = b''.join(struct.pack('<H', quantize_reference(v)) for v in inputs)
    actual = output[:48000]
    return {'complete': True, 'compared_samples': len(inputs),
            'expected_sha256': sha(expected), 'actual_sha256': sha(actual),
            'exact_match': expected == actual,
            'input_min': min(inputs), 'input_max': max(inputs),
            'first_plane_first_bank_float_prefix': inputs[:20]}


def layout_checks(identity, permutation, larger):
    """Independent coordinate addressing + metamorphic checks; no copied native code."""
    words = struct.unpack('<24000H', identity[:48000])
    def index(c, r, g, b):
        return c * 8000 + (4 * (r % 2) + 2 * (g % 2) + b % 2) * 1000 + (r // 2) * 100 + (g // 2) * 10 + b // 2
    levels = [words[index(0, i, 0, 0)] for i in range(20)]
    mismatches = sum(words[index(c, r, g, b)] != levels[(r, g, b)[c]]
                     for c, r, g, b in itertools.product(range(3), range(20), range(20), range(20)))
    permutation_expected = identity[32000:48000] + identity[:16000] + identity[16000:32000] + identity[48000:]
    # Ordinary 17-point identity has 17 valid nodes/axis and zero padding to 20.
    ordinary = [0] * 24000
    for c, r, g, b in itertools.product(range(3), range(17), range(17), range(17)):
        ordinary[index(c, r, g, b)] = 512 * (r, g, b)[c]
    normal = struct.pack('<24000H', *ordinary)
    larger_words = struct.unpack('<24000H', larger[:48000])
    deltas = [abs(a - b) for a, b in zip(words, larger_words)]
    return {'identity_coordinate_samples_checked': 24000, 'identity_axis_mismatches': mismatches,
            'identity_axis_levels_u16': levels,
            'permutation_full_48128_bytes_exact': permutation == permutation_expected,
            'permutation_expected_sha256': sha(permutation_expected),
            'ordinary_identity_sram_sha256': sha(normal),
            'ordinary_vs_hq_differing_bytes': sum(a != b for a, b in zip(normal, identity[:48000])),
            'ordinary_vs_hq_differing_u16': sum(a != b for a, b in zip(ordinary, words)),
            'grid17_vs_grid33_sram_exact': identity[:48000] == larger[:48000],
            'grid17_vs_grid33_max_u16_delta': max(deltas),
            'grid17_vs_grid33_differing_u16': sum(v != 0 for v in deltas),
            'grid17_vs_grid33_register_128_exact': identity[48000:] == larger[48000:]}


def static_evidence(data):
    ranges = [(0x36f6900, 0x36f6e50, 'HQ orchestrator'),
              (0x3702604, 0x3702710, 'float quantizer and bank formatter'),
              (0x36fc66c, 0x36fc698, 'serialized bytes copied to destination'),
              (0x36fcf94, 0x36fd17c, 'section dispatch and output count accumulation')]
    md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    return {'ranges': [{'start_va': hex(a), 'end_va_exclusive': hex(b), 'file_offset': a,
                        'role': role, 'sha256': sha(data[a:b]),
                        'instructions': [{'va': hex(i.address), 'bytes': i.bytes.hex(),
                                          'mnemonic': i.mnemonic, 'operands': i.op_str}
                                         for i in md.disasm(data[a:b], a)]}
                       for a, b, role in ranges],
            'constants': [{'va': hex(a), 'file_offset': a, 'bytes': data[a:a+n].hex(),
                           'value': struct.unpack('<' + fmt, data[a:a+n])[0]}
                          for a, n, fmt in [(0x51a06d4, 4, 'f'), (0x4bfa180, 8, 'd')]]}


def metadata(generator_sha):
    return {'schema': 2,
            'method': 'Source-bound original ARM64 in Unicorn with bounded explicit library stubs; not REA Evidence',
            'source': {'path': str(TARGET.relative_to(ROOT)), 'sha256': SHA},
            'generator': {'path': str(Path(__file__).relative_to(ROOT)), 'sha256': generator_sha},
            'camera_access': False, 'network_access': False, 'syscall_forwarding': False,
            'target_native_process_execution': False,
            'bounds': {'heap_bytes': HQMachine.HEAP_BYTES, 'max_continuation_chunks': 4,
                       'per_chunk_wall_clock_seconds': 60, 'per_chunk_instruction_limit': 150000000,
                       'output_guard_bytes': 32},
            'limits': ['Heap is an arena with no address reuse on free; libc/libstdc++ are explicit semantic stubs.',
                       'Unknown external calls abort rather than forwarding to hardware or a native process.',
                       'Library pow/log10 use host Python math and do not prove bit-exact target libm behavior.',
                       'String/printf stubs implement only the observed canonical finite fixture operations.',
                       'A failed/bounded run is partial evidence and establishes no complete payload.',
                       'No claim of the complete resampling/color-domain transform is established by quantizer checks.',
                       'Completed synthetic runs do not demonstrate camera performance or still-photo routing.']}


def summarize(data, generator_sha, output):
    cases, payloads = [], []
    names = ['17-identity', '17-permutation', '33-identity']
    for name in names:
        path = ROOT / f'research/evidence/hq-lut-cases/{name}.json'
        item = json.loads(path.read_text())
        result = item['result']
        assert item['source']['sha256'] == SHA
        assert result['finished'] and result['failure'] is None
        assert result['output_guard_unchanged']
        assert result['sram_reported_bytes'] == 48000 and result['register_reported_bytes'] == 128
        assert result['written_byte_coverage'] == {'sram': 48000, 'register_124': 124, 'trailing_four': 4}
        assert result['quantizer_oracle']['exact_match']
        payload = (ROOT / result['synthetic_payload_path']).read_bytes()
        assert len(payload) == 48128 and sha(payload) == result['output_48128_sha256']
        payloads.append(payload)
        cases.append({'name': name, 'evidence_path': str(path.relative_to(ROOT)), 'evidence_sha256': sha(path.read_bytes()),
                      'grid': result['grid'], 'pattern': result['pattern'], 'finished': result['finished'],
                      **{k: result[k] for k in ['output_48128_sha256', 'sram_sha256', 'register_124_sha256',
                         'sram_reported_bytes', 'register_reported_bytes', 'written_byte_coverage',
                         'trailing_four_hex', 'register_u32_le', 'quantizer_oracle', 'output_guard_unchanged']}})
    checks = layout_checks(*payloads)
    assert checks['identity_axis_mismatches'] == 0 and checks['permutation_full_48128_bytes_exact']
    evidence = metadata(generator_sha)
    evidence.update({'cases': cases, 'validation': checks, 'static_evidence': static_evidence(data),
                     'interpretation': {'native_output_bytes': 48128, 'sram_bytes': 48000,
                         'second_serialized_region_bytes': 128, 'downstream_register_request_bytes': 124,
                         'last_four': 'Explicitly written zero bytes in tested cases; exact field semantics unknown.',
                         'cube20_addressing': 'u16 word=c*8000+(4*(r%2)+2*(g%2)+(b%2))*1000+(r//2)*100+(g//2)*10+b//2',
                         'base_qualification': 'Offsets here are relative to emitted native blob, not an inferred CP address.'}})
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(evidence, indent=2) + '\n')
    print(json.dumps({'cases': len(cases), 'validation': checks, 'evidence': str(output)}, indent=2))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--grid', type=int, choices=[17, 33], default=17)
    p.add_argument('--pattern', choices=['identity', 'permutation'], default='identity')
    p.add_argument('--output', type=Path)
    p.add_argument('--summarize', action='store_true', help='Validate three saved synthetic runs and write aggregate evidence')
    args = p.parse_args()
    data = TARGET.read_bytes()
    assert sha(data) == SHA
    generator_sha = sha(Path(__file__).read_bytes())
    if args.summarize:
        summarize(data, generator_sha, args.output or ROOT / 'research/evidence/hq-lut-format.json')
        return
    machine = HQMachine(data)
    result = machine.run(args.grid, args.pattern)
    out = args.output or ROOT / f'research/evidence/hq-lut-cases/{args.grid}-{args.pattern}.json'
    out.parent.mkdir(parents=True, exist_ok=True)
    if result['finished']:
        payload = ROOT / f'research/build/hq-lut/{args.grid}-{args.pattern}.bin'
        payload.parent.mkdir(parents=True, exist_ok=True)
        payload.write_bytes(machine.output_bytes)
        if all(v is not None for v in machine.quantizer_inputs):
            quant = payload.with_suffix('.quantizer-inputs.f32')
            quant.write_bytes(struct.pack('<24000f', *machine.quantizer_inputs))
        result['synthetic_payload_path'] = str(payload.relative_to(ROOT))
    evidence = metadata(generator_sha)
    evidence['result'] = result
    out.write_text(json.dumps(evidence, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k not in {'notes', 'stub_calls', 'external_function_pointer_relocations'}}, indent=2))


if __name__ == '__main__':
    main()
