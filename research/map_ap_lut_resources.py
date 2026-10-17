#!/usr/bin/env python3
"""Bounded static AP resource-helper disassembly. No target execution/USB."""
import hashlib
import json
from pathlib import Path
import struct
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked/lib/appFw.so'
EXPECTED = '2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
RANGES = [
    (0x3020430, 0x3020454, 'native LUT filename formatter'),
    (0x301fadc, 0x301fb48, 'bounded fopen/fread/fclose helper'),
    (0x3021228, 0x3021338, 'native LUT resource read descriptor'),
    (0x30210c8, 0x3021228, 'copy resource descriptor into mapped memory and dispatch'),
    (0x3428510, 0x342863c, 'outer selection/read/copy helper'),
    (0x342888c, 0x34289a8, 'LUT message input builder'),
    (0x3552f54, 0x3553038, 'open/map helper'),
    (0x3553038, 0x35530ec, 'unmap/close helper'),
    (0x34d37e8, 0x34d3828, 'open/map wrapper'),
    (0x34d3828, 0x34d3868, 'unmap/close wrapper'),
    (0x34cb5e8, 0x34cb628, 'resource dispatch wrapper'),
]


def main():
    data = TARGET.read_bytes()
    assert hashlib.sha256(data).hexdigest() == EXPECTED
    with TARGET.open('rb') as f:
        elf = ELFFile(f)
        def file_offset(va):
            sections = [s for s in elf.iter_sections()
                        if s.header.sh_addr <= va < s.header.sh_addr + s.header.sh_size
                        and s.header.sh_type != 'SHT_NOBITS']
            assert len(sections) == 1, hex(va)
            s = sections[0]
            return s.header.sh_offset + va - s.header.sh_addr
        def cstring(va):
            off = file_offset(va)
            return {'va': va, 'file_offset': off,
                    'text': data[off:data.index(b'\0', off)].decode('ascii')}
        syms = elf.get_section_by_name('.dynsym')
        plt = elf.get_section_by_name('.plt')
        imports = {}
        for i, r in enumerate(elf.get_section_by_name('.rela.plt').iter_relocations()):
            va = plt.header.sh_addr + 32 + 16 * i
            imports[va] = syms.get_symbol(r.entry.r_info_sym).name
        rows = []
        md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
        for start, end, label in RANGES:
            assert end > start
            off = file_offset(start)
            body = data[off:off + end - start]
            instructions = []
            for i in md.disasm(body, start):
                row = {'va': i.address, 'file_offset': file_offset(i.address),
                       'bytes': bytes(i.bytes).hex(), 'mnemonic': i.mnemonic,
                       'operands': i.op_str}
                if i.mnemonic in {'bl', 'b'} and i.op_str.startswith('#'):
                    target = int(i.op_str[1:], 16)
                    if target in imports:
                        row['import'] = imports[target]
                instructions.append(row)
            assert sum(len(bytes.fromhex(i['bytes'])) for i in instructions) == len(body)
            rows.append({'label': label, 'start_va': start, 'end_va_exclusive': end,
                         'file_offset': off, 'bytes': body.hex(),
                         'sha256': hashlib.sha256(body).hexdigest(),
                         'instructions': instructions})
        selected = {0x5b70df8, 0x5bbdeb8, 0x5b8f438, 0x5b7e8f8}
        relocations = []
        for r in elf.get_section_by_name('.rela.dyn').iter_relocations():
            if r.entry.r_offset in selected:
                relocations.append({'got_va': r.entry.r_offset, 'type': r.entry.r_info_type,
                                    'addend_va': r.entry.r_addend,
                                    'symbol': syms.get_symbol(r.entry.r_info_sym).name})
        result = {'schema': 1, 'method': 'Ordinary offline ELF metadata + Capstone; not REA Evidence',
            'camera_access': False, 'target_execution': False,
            'source': {'path': str(TARGET.relative_to(ROOT)), 'bytes': len(data), 'sha256': EXPECTED},
            'generator': {'path': str(Path(__file__).relative_to(ROOT)),
                          'sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},
            'strings': [cstring(v) for v in [0x4bfa3b2, 0x4bfa3d3, 0x4170334,
                                             0x371004a, 0x48d171c, 0x4bfa25a, 0x4bfa222]],
            'ranges': rows, 'global_pointer_relocations': relocations,
            'native_lut_matches_in_regular_extraction': [str(p.relative_to(ROOT)) for p in
                (ROOT / 'research/extracted/ILCE-7CM2-2.01').rglob('*')
                if p.is_file() and p.suffix.lower() in {'.lut', '.cube'}],
            'measured_constants': {'normal_read_capacity': 0xc000, 'alternate_read_capacity': 0xbc00,
                                   'builder_two_u32_values': 0xbb80, 'builder_last_u32_value': 0x7c,
                                   'internal_max_index_inclusive': 0x31},
            'limits': ['Static source-bound instructions only; no runtime mapping or payload read.',
                       'Internal index validation is not a count of user-visible slots.',
                       'Native .LUT file format, grid packing and any 4-byte header remain unresolved.',
                       'Read capacities are upper bounds, not validated file lengths.']}
    assert result['strings'][0]['text'] == '%s/LUT%03d.LUT'
    assert result['strings'][1]['text'] == '/usr/data/lut'
    out = ROOT / 'research/evidence/ap-lut-resources.json'
    out.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'output': str(out), 'ranges': len(rows), 'instruction_count': sum(len(r['instructions']) for r in rows)}))


if __name__ == '__main__':
    main()
