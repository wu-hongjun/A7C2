"""Inspect downloaded A7C II firmware for still-LUT research leads. No USB I/O.

This records strings, dynamic symbols and disassembly, not a working image
pipeline or a claim that a named interface is available to still capture.
"""
import json
from pathlib import Path
import struct
import sys

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import core

DEV = ROOT / 'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev'
LIB = DEV / 'nflasha15_unpacked/lib'


def literals(blob, markers):
    result = {}
    for marker in markers:
        needle = marker.encode('ascii')
        rows = []
        begin = 0
        while len(rows) < 4:
            at = blob.find(needle, begin)
            if at < 0:
                break
            start = blob.rfind(b'\0', max(0, at - 512), at) + 1
            stop = blob.find(b'\0', at)
            if stop < 0:
                stop = len(blob)
            text = blob[start:min(stop, start + 600)].decode('ascii', 'replace')
            rows.append({'match_file_offset': hex(at), 'string_file_offset': hex(start), 'text': text})
            begin = at + len(needle)
        if not rows:
            raise ValueError('Expected research marker absent: ' + marker)
        result[marker] = rows
    return result


def elf_info(path, interesting, functions=()):
    with path.open('rb') as file:
        elf = ELFFile(file)
        table = list(elf.get_section_by_name('.dynsym').iter_symbols())
        selected = [s for s in table if any(t in s.name for t in interesting)]
        result = {'elf_class': elf.elfclass, 'machine': elf['e_machine'],
                  'symbols': [{'name': s.name, 'value': hex(s['st_value']), 'bytes': s['st_size'],
                               'defined': s['st_shndx'] != 'SHN_UNDEF'} for s in selected]}
        labels = {s['st_value']: s.name for s in table if s['st_shndx'] != 'SHN_UNDEF'}
        plt = elf.get_section_by_name('.plt')
        relocations = elf.get_section_by_name('.rela.plt')
        if plt and relocations:
            for index, relocation in enumerate(relocations.iter_relocations()):
                labels[plt['sh_addr'] + 32 + index * 16] = table[relocation['r_info_sym']].name + '@plt'
        # A direct call proves a reference to an entry point, not its mode or
        # parameter semantics. Keep addresses as offline ELF addresses only.
        targets = {address: label for address, label in labels.items()
                   if 'SetLutData' in label and label.endswith('@plt')}
        section = elf.get_section_by_name('.text')
        calls = []
        for index, (word,) in enumerate(struct.iter_unpack('<I', section.data())):
            if word & 0xfc000000 != 0x94000000:
                continue
            address = section['sh_addr'] + 4 * index
            immediate = word & 0x3ffffff
            if immediate & 0x2000000:
                immediate -= 0x4000000
            destination = address + 4 * immediate
            if destination in targets:
                calls.append({'elf_virtual_address': hex(address),
                              'destination': hex(destination), 'target_symbol': targets[destination]})
        result['direct_calls_to_lut_imports'] = calls
        result['disassembled_functions'] = []
        decoder = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
        for name in functions:
            symbol = next(s for s in table if s.name == name and s['st_shndx'] != 'SHN_UNDEF')
            address = symbol['st_value']
            segment = next(s for s in elf.iter_segments()
                           if s['p_type'] == 'PT_LOAD' and s['p_vaddr'] <= address
                           and address + symbol['st_size'] <= s['p_vaddr'] + s['p_filesz'])
            file.seek(segment['p_offset'] + address - segment['p_vaddr'])
            blob = file.read(symbol['st_size'])
            instructions = []
            for ins in decoder.disasm(blob, address):
                row = {'elf_virtual_address': hex(ins.address), 'operation': ins.mnemonic,
                       'operands': ins.op_str}
                if ins.mnemonic in ['bl', 'b'] and ins.op_str.startswith('#0x'):
                    target = int(ins.op_str[1:], 16)
                    if target in labels:
                        row['target_symbol'] = labels[target]
                instructions.append(row)
            if len(instructions) * 4 != len(blob):
                raise ValueError('Incomplete function disassembly')
            result['disassembled_functions'].append({'name': name, 'instructions': instructions})
        return result


def main():
    ap_path = LIB / 'appFw.so'
    cp_path = DEV / 'nflasha8_unpacked/cpapp-b.bin'
    mpr_path = LIB / 'libmpr.so'
    ap, cp, mpr = ap_path.read_bytes(), cp_path.read_bytes(), mpr_path.read_bytes()
    if [core.sha(ap), core.sha(cp)] != core.ORIGINAL:
        raise ValueError('Require the pinned official A7C II 2.01 programs')
    before_assets = {name: core.sha((ROOT/'assets'/name).read_bytes()) for name in core.assets()['files']}
    report = {'date': '2026-10-17', 'mode': 'offline_static_research', 'camera_accessed': False,
              'camera_uploads': 0, 'camera_writes': 0, 'deployable_lut_program_built': False,
              'files': {str(p.relative_to(ROOT)): {'bytes': len(b), 'sha256': core.sha(b)}
                        for p, b in [(ap_path, ap), (cp_path, cp), (mpr_path, mpr)]}}
    report['ap_literals'] = literals(ap, [
        'model_user_lut_files read3DLUTFile!!', 'LUT_3D_SIZE', '/usr/data/lut',
        'read3DLUTFileForCube', 'sequence_select_lut::send_lut_procam(',
        'sequence_select_lut::send_lut(', 'set_user_lut_binary_data',
        'SELECT_LUT_SDR_STILL', 'm_user_lut_binary_data_msg'])
    report['cp_literals'] = literals(cp, [
        'init_cc_3DLUT', 'CC: CamCore 3D-Lut table Write Start', 'CC: Create3DLUT',
        'FW: SET_USER_LUT_BINARY_DATA', 'CC: CamCore 3D-Lut table Write End',
        'MLUT_MODE_FLOW_3DLUT', 'MLUT_L3D_REG_USER_LUT_17'])
    report['ap_elf'] = elf_info(ap_path, ['SetLutData', 'RecordStill', 'ReadStillImage'])
    report['mpr_elf'] = elf_info(mpr_path, ['SetLutData', 'ResetLutData', 'GetLutNrtInfo',
                                          'RecordStill', 'ReadStillImage'], [
        '_ZN3mpr10SetLutDataERKNS_17setLutDataInParamE',
        '_ZN3mpr17ExecuteSetLutDataERKNS_17setLutDataInParamE'])
    pixels = 7008 * 4672
    report['calculated_storage_not_camera_ram_measurements'] = {
        'image_dimensions': [7008, 4672], 'pixels': pixels,
        'one_rgb8_buffer_bytes': pixels * 3,
        'one_rgb16_buffer_bytes': pixels * 6,
        'one_rgb_float32_buffer_bytes': pixels * 12,
        'lut_payloads': {str(n): {'rgb_uint16_bytes': n**3*6, 'rgb_float32_bytes': n**3*12}
                         for n in [17, 33, 65]}}
    report['interpretation'] = [
        'Official firmware includes LUT import, selection and CP table-programming vocabulary.',
        'AP imports SetLutData; libmpr defines it and ExecuteSetLutData.',
        'SetLutData disassembly includes mutex locking and reset/execute dispatch.',
        'Names containing STILL or stillplay do not establish a full-resolution capture LUT path.',
        'C++ parameter layouts, execution context, buffer ownership and still restrictions remain unresolved.',
        'Imaging Edge desktop ZcTask3DLut research is not an A7C II firmware entry point.',
        'No runtime RAM, throughput, CPU/GPU/ISP availability or image-quality measurement is implied.']
    after_assets = {name: core.sha((ROOT/'assets'/name).read_bytes()) for name in core.assets()['files']}
    if before_assets != after_assets:
        raise ValueError('Deployment asset changed during research')
    report['deployment_assets_unchanged'] = after_assets
    destination = ROOT / 'research/evidence/still-lut-static.json'
    core.save_json(destination, report)
    print(json.dumps({'evidence': str(destination.relative_to(ROOT)),
                      'AP_marker_groups': len(report['ap_literals']),
                      'CP_marker_groups': len(report['cp_literals']),
                      'native_functions_disassembled': len(report['mpr_elf']['disassembled_functions']),
                      'camera_accessed': False, 'deployment_assets_unchanged': True}, indent=2))


if __name__ == '__main__':
    main()
