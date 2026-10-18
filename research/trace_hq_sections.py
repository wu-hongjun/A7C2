#!/usr/bin/env python3
"""Offline original HQ section/padding trace; imports the unchanged HQ harness."""
import json
from pathlib import Path

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
from unicorn.arm64_const import UC_ARM64_REG_PC, UC_ARM64_REG_X0, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X30

from trace_hq_lut import HQMachine, ROOT, TARGET, SHA, sha

OUTPUT = ROOT / 'research/evidence/hq-sections.json'


class SectionMachine(HQMachine):
    def __init__(self, data):
        super().__init__(data)
        self.sections = []
        self.pending_section = None
        self.return_hooks = set()
        self.padding_writes = []
        self.hooks.append(self.u.hook_add(UC_HOOK_CODE, self.section_entry, begin=0x36fc5a4, end=0x36fc5a4))
        self.hooks.append(self.u.hook_add(UC_HOOK_MEM_WRITE, self.tail_write, begin=self.OUTPUT + 48124, end=self.OUTPUT + 48127))

    def section_entry(self, u, address, size, _):
        lr = u.reg_read(UC_ARM64_REG_X30)
        self.pending_section = len(self.sections)
        dest = u.reg_read(UC_ARM64_REG_X3)
        self.sections.append({'index': len(self.sections), 'entry_va': hex(address), 'return_va': hex(lr),
                              'input_length_x2': u.reg_read(UC_ARM64_REG_X2),
                              'format_flag_x5': u.reg_read(UC_ARM64_REG_X5),
                              'destination_x3': hex(dest), 'destination_relative_to_native_base': dest - self.OUTPUT,
                              'destination_offset_x4': u.reg_read(UC_ARM64_REG_X4)})
        if lr not in self.return_hooks:
            self.hooks.append(u.hook_add(UC_HOOK_CODE, self.section_return, begin=lr, end=lr))
            self.return_hooks.add(lr)

    def section_return(self, u, address, size, _):
        if self.pending_section is None:
            return
        section = self.sections[self.pending_section]
        if section['return_va'] != hex(address):
            return
        section['returned_length_x0'] = u.reg_read(UC_ARM64_REG_X0)
        self.pending_section = None

    def tail_write(self, u, access, address, size, value, _):
        self.padding_writes.append({'pc': hex(u.reg_read(UC_ARM64_REG_PC)),
                                    'native_blob_offset': address - self.OUTPUT,
                                    'bytes': size, 'value': value,
                                    'section_index': self.pending_section})


def main():
    data = TARGET.read_bytes()
    assert sha(data) == SHA
    generator_sha = sha(Path(__file__).read_bytes())
    base_path = ROOT / 'research/trace_hq_lut.py'
    base_sha = sha(base_path.read_bytes())
    align = HQMachine(data)
    lengths = [0, 1, 2, 15, 16, 30, 31, 32, 33, 63, 64, 95, 96, 123, 124, 125, 127, 128, 129,
               1999, 2000, 2015, 2016, 47999, 48000, 48001, 48124, 48128]
    alignment_cases = []
    for length in lengths:
        assert align.invoke(0x36fe634, [length]) and align.failure is None
        actual = align.u.reg_read(UC_ARM64_REG_X0)
        expected = (length + 31) & ~31
        assert actual == expected
        alignment_cases.append({'input': length, 'actual': actual, 'independent_expected': expected})
    del align
    machine = SectionMachine(data)
    result = machine.run(17, 'identity')
    assert result['finished'] and result['failure'] is None
    assert len(machine.sections) == 25
    diagnostic = ROOT / 'research/build/hq-lut/section-observations.json'
    diagnostic.parent.mkdir(parents=True, exist_ok=True)
    diagnostic.write_text(json.dumps({'sections': machine.sections, 'padding_writes': machine.padding_writes}, indent=2) + '\n')
    registers = next(s for s in machine.sections if s['input_length_x2'] == 124)
    banks = [s for s in machine.sections if s is not registers]
    assert all(s['input_length_x2'] == 2000 and s['format_flag_x5'] == 1 and s['returned_length_x0'] == 2000 for s in banks)
    assert registers['input_length_x2'] == 124 and registers['format_flag_x5'] == 0
    assert registers['returned_length_x0'] == 128
    assert registers['destination_relative_to_native_base'] == 48000 and registers['destination_offset_x4'] == 0
    assert machine.padding_writes == [{'pc': '0x36fc964', 'native_blob_offset': n, 'bytes': 1, 'value': 0, 'section_index': registers['index']} for n in range(48124,48128)]
    case = ROOT / 'research/evidence/hq-lut-cases/17-identity.json'
    case_data = case.read_bytes()
    assert result['output_48128_sha256'] == json.loads(case_data)['result']['output_48128_sha256']
    md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    ranges = [(0x36fe634, 0x36fe654, '32-byte length alignment helper'),
              (0x36fc5a4, 0x36fc630, 'Emitter argument save and initial alignment'),
              (0x36fc934, 0x36fc9a0, 'Flag-controlled final alignment, explicit zero-fill and return')]
    evidence = {'schema': 1, 'method': 'Source-bound original ARM64 emulation with explicit library stubs; not REA Evidence',
                'source': {'path': str(TARGET.relative_to(ROOT)), 'sha256': SHA},
                'generator': {'path': str(Path(__file__).relative_to(ROOT)), 'sha256': generator_sha},
                'base_harness': {'path': str(base_path.relative_to(ROOT)), 'sha256': base_sha},
                'previous_identity_case': {'path': str(case.relative_to(ROOT)), 'sha256': sha(case_data)},
                'camera_access': False, 'network_access': False, 'syscall_forwarding': False,
                'alignment_cases': alignment_cases, 'sections': machine.sections,
                'last_four_writes': machine.padding_writes,
                'validation': {'completed': True, 'alignment_cases_passed': len(alignment_cases),
                    'bank_sections_checked': 24, 'register_section_checked': True,
                    'native_48128_sha256': result['output_48128_sha256'],
                    'output_equals_previous_identity': True, 'output_guard_unchanged': result['output_guard_unchanged'],
                    'last_four_zero_writes_from_original_padding_loop': True},
                'result_summary': {k: result[k] for k in ['sram_reported_bytes','register_reported_bytes','written_byte_coverage','heap_high_water_bytes','relative_relocations_applied','entered_functions','stub_calls']},
                'static_ranges': [{'start_va': hex(a), 'end_va_exclusive': hex(b), 'file_offset': a, 'role': role,
                    'sha256': sha(data[a:b]), 'instructions': [{'va': hex(i.address), 'bytes': i.bytes.hex(), 'mnemonic': i.mnemonic, 'operands': i.op_str} for i in md.disasm(data[a:b], a)]} for a,b,role in ranges],
                'conclusion': 'The tested register section contains 124 content bytes and 4 zero alignment bytes because flag 0 selects 32-byte padding; flag 1 SRAM banks remain 2000 bytes.',
                'limits': ['Uses unchanged HQMachine bounded memory, relocation and explicit libc/libstdc++/host-libm stubs; no native process or hardware forwarding.',
                           'The alignment helper was tested only for bounded nonnegative lengths without integer overflow.',
                           'Observed per-section argument semantics apply to this original IPSX identity path; other formats/flags remain untested.',
                           'This resolves emitted native-file padding and does not establish camera image-domain behavior or still-frame routing.']}
    OUTPUT.write_text(json.dumps(evidence, indent=2) + '\n')
    print(json.dumps(evidence['validation'], indent=2))


if __name__ == '__main__':
    main()
