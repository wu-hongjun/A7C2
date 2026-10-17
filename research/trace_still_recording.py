"""Source-bound AP still-recording boundary; no camera transport or execution."""
import hashlib
import json
from pathlib import Path

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = '4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9'
FUNCTIONS = {
    0xeb260: 'RecordStill', 0xe85a0: 'stillrecord::_record_still',
    0xf389c: 'SetStillRecParamInternal', 0xf3a8c: 'SetStillRecParam',
    0xf23b4: 'IntegRecordStillInit', 0xe5264: 'stillrecord::_make_add_if_t',
    0xe6864: 'stillrecord::_make_cnv_if_t', 0xc4de0: 'stillutil::creater',
}


def identity(path):
    raw = path.read_bytes()
    return {'path': str(path.relative_to(ROOT)), 'bytes': len(raw),
            'sha256': hashlib.sha256(raw).hexdigest()}


def main():
    paths = list((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob('libmpr.so'))
    assert len(paths) == 1
    path = paths[0]
    raw = path.read_bytes()
    assert hashlib.sha256(raw).hexdigest() == EXPECTED
    with path.open('rb') as stream:
        elf = ELFFile(stream)
        symbols = list(elf.get_section_by_name('.dynsym').iter_symbols())
        defined = {s['st_value']: s for s in symbols
                   if isinstance(s['st_shndx'], int) and s['st_size'] > 0}
        names = {va: s.name for va, s in defined.items()}
        plt = elf.get_section_by_name('.plt')['sh_addr']
        imports = {plt+32+16*i: symbols[r['r_info_sym']].name for i, r in enumerate(
            elf.get_section_by_name('.rela.plt').iter_relocations())}

        def offset(va):
            matches = [s for s in elf.iter_segments() if s['p_type'] == 'PT_LOAD'
                       and s['p_vaddr'] <= va < s['p_vaddr']+s['p_filesz']]
            assert len(matches) == 1
            s = matches[0]
            return va-s['p_vaddr']+s['p_offset']

        regions = []
        for va, role in FUNCTIONS.items():
            symbol = defined[va]
            start, length = offset(va), symbol['st_size']
            body = raw[start:start+length]
            decoded = list(Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN).disasm(body, va))
            assert sum(i.size for i in decoded) == length
            rows = []
            for ins in decoded:
                row = {'ELF_VA': hex(ins.address), 'file_offset': hex(offset(ins.address)),
                       'bytes': ins.bytes.hex(), 'mnemonic': ins.mnemonic, 'operands': ins.op_str}
                if ins.mnemonic in ('b', 'bl') and ins.op_str.startswith('#'):
                    target = int(ins.op_str[1:], 16)
                    row['target_ELF_VA'] = hex(target)
                    row['target_name_if_known'] = imports.get(target, names.get(target))
                rows.append(row)
            regions.append({'role': role, 'dynamic_symbol': symbol.name,
                            'ELF_VA': hex(va), 'file_offset': hex(start), 'bytes': length,
                            'sha256': hashlib.sha256(body).hexdigest(), 'instructions': rows})
    evidence = []
    for session in ('libmpr-still', 'libmpr-still-sink'):
        for record in sorted((ROOT/'research/evidence/rea'/session).glob('[0-9][0-9]-*.json')):
            value = json.loads(record.read_text())
            assert not value.get('isError')
            item = value['structuredContent']
            assert item['subject']['digest']['sha256'] == EXPECTED
            evidence.append({**identity(record), 'evidence_id': item['evidence_id'],
                             'operation': item['operation']})
    assert len(evidence) == 10
    result = {'camera_accessed': False, 'target_executed_on_camera': False,
              'host_emulation_performed': False,
              'method': 'Ordinary original-file ELF/Capstone evidence plus separately linked native REA Evidence.',
              'source': identity(path), 'generator': identity(Path(__file__)),
              'regions': regions, 'REA_records': evidence,
              'observations': {
                  'record_parameter_copy_bytes': 0x2a8,
                  'record_input_JpgInfo_copy_bytes': 0xc0,
                  'record_input_RawInfo_copy_bytes': 0x150,
                  'record_input_HifInfo_copy_bytes': 0xe0,
                  'large_image_conditional_pixel_count': 0x1fa400,
                  'add_interface_state_copy_bytes_from_REA': 0x870,
                  'conversion_virtual_slot_from_REA': '0x40',
                  'add_interface_virtual_slot_from_REA': '0x68'},
              'limits': [
                  'Named structures and virtual-call roles are binary-interface observations, not a safe callable SDK.',
                  'The _record_still decompiler reports type propagation not settling; instruction bytes remain authoritative.',
                  'No transitive virtual-call implementation, capture buffer or MLUT connection is proved here.',
                  'The size conditional is not an MLUT maximum-resolution check.',
                  'No assertion that every still-image path is absent from MLUT follows from this bounded pass.']}
    output = ROOT/'research/evidence/still-recording-boundary.json'
    output.write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps({'regions': len(regions), 'instructions': sum(len(r['instructions']) for r in regions),
                      'REA_records': len(evidence), 'output': str(output)}, indent=2))


if __name__ == '__main__':
    main()
