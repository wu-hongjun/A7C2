"""Verify retained offline research identities and important cross-report facts."""
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import core


def resolve_subject_path(original_path):
    """Keep raw REA provenance intact while checking a relocated repository copy."""
    marker = '/research/extracted/'
    if marker in original_path:
        return ROOT/'research/extracted'/original_path.split(marker, 1)[1]
    return Path(original_path)


def main():
    names = ['firmware-structure', 'ap-interfaces', 'cp-architecture']
    reports = {name: core.read_json(ROOT/'research/evidence'/f'{name}.json') for name in names}
    checked = {}

    def walk(value):
        if isinstance(value, dict):
            if isinstance(value.get('path'), str) and isinstance(value.get('sha256'), str):
                path = ROOT/value['path']
                if value['path'].startswith('research/') and not path.is_file():
                    raise FileNotFoundError('Required research input missing: ' + str(path))
                if path.is_file():
                    if path not in checked:
                        with path.open('rb') as stream:
                            checked[path] = hashlib.file_digest(stream, 'sha256').hexdigest()
                    if checked[path] != value['sha256']:
                        raise ValueError('Source hash mismatch: ' + str(path.relative_to(ROOT)))
                    if 'bytes' in value and path.stat().st_size != value['bytes']:
                        raise ValueError('Source length mismatch: ' + str(path.relative_to(ROOT)))
            for item in value.values():
                walk(item)
        elif isinstance(value, list):
            for item in value:
                walk(item)

    for report in reports.values():
        walk(report)
    resources = core.read_json(ROOT/'research/evidence/ap-lut-resources.json')
    walk(resources)
    assert len(resources['ranges']) == 11
    assert sum(len(r['instructions']) for r in resources['ranges']) == 488
    assert resources['camera_access'] is False and resources['target_execution'] is False
    assert resources['measured_constants']['builder_two_u32_values'] == 48000
    assert resources['measured_constants']['builder_last_u32_value'] == 124
    structure, ap, cp = [reports[n] for n in names]
    assert structure['dat_crc32']['valid'] is True
    assert structure['regular_file_count'] == 1369
    assert structure['elf_summary']['count'] == 650
    assert len(structure['partitions']) == 30
    assert ap['counts']['ELF_files'] == 619
    assert ap['counts']['DT_NEEDED_edges'] == 2714
    assert cp['source']['sha256'] == core.ORIGINAL[1]
    assert len(cp['disassembled_regions']) == 46
    assert len(cp['mlut_debug_commands']) == 64
    assert cp['mlut_register_base_table']['values'] == [
        hex(0xf4009000 + i * 0x100000) for i in range(7)]
    assert cp['camera_accessed'] is False and cp['target_executed'] is False
    assert ap['camera_accessed'] is False and ap['target_executed'] is False
    loader = cp['mlut_payload_loader_constraints']
    assert loader['SRAM']['required_bytes'] == 48000
    assert loader['registers']['required_bytes'] == 124
    assert loader['payload_copied_by_enqueue'] is False
    assert cp['host_emulation_performed'] is True
    assert loader['host_microchecks']['wrapper_cases'] == 48
    assert loader['host_microchecks']['SRAM_descriptor_builder_cases'] == 4
    native = core.read_json(ROOT/'research/evidence/native-lut-format.json')
    layout = core.read_json(ROOT/'research/evidence/mlut-layout.json')
    route = core.read_json(ROOT/'research/evidence/still-mlut-route.json')
    recording = core.read_json(ROOT/'research/evidence/still-recording-boundary.json')
    for report in (native, layout, route, recording):
        walk(report)
    assert native['all_checks_passed'] is True
    assert not native['camera_access'] and not native['hardware_execution'] and not native['syscall_forwarding']
    assert len(native['packer_checks']) == 10 and len(native['parser_checks']) == 18
    assert all(row['passed'] for row in native['packer_checks']+native['parser_checks'])
    assert sum(row['compared_bytes'] for row in native['packer_checks']) == 480000
    assert sum(row['unpacked_samples_compared'] for row in native['packer_checks']) == 147390
    assert layout['identity_layout']['matched_halfwords'] == 24000
    assert layout['diagnostic_not_LUT_pattern']['matched_words'] == 12000
    assert layout['switch_builder_emulation']['cases'] == 128
    assert layout['counts'] == {'named_records': 402, 'SRAM_records': 312, 'register31_records': 17}
    assert not layout['camera_accessed'] and not layout['on_camera_execution']
    assert len(route['disassembled_regions']) == 32
    assert route['lifecycle']['host_test']['cases'] == 1
    assert len(route['resource_routing']['host_test_cases']) == 32
    assert len(route['dimension_contract']['host_test_cases']) == 48
    modes = route['upstream_sequence_classification']['mode_lookup_emulation']
    assert modes['u16_cases'] == 65536 and modes['nonzero_property_cases'] == 0
    assert len(modes['u32_boundary_cases']) == 4
    assert len(modes['recognized_modes']) == 30
    assert not route['camera_accessed'] and not route['target_executed_on_camera']
    assert len(recording['regions']) == 8 and len(recording['REA_records']) == 10
    assert sum(len(row['instructions']) for row in recording['regions']) == 4627
    assert not recording['camera_accessed'] and not recording['target_executed_on_camera']
    rea = core.read_json(ROOT/'research/evidence/rea/index.json')
    for row in rea['records']:
        path = ROOT/row['path']
        value = core.read_json(path)
        record = value.get('structuredContent') or json.loads(value['content'][0]['text'])
        assert record['evidence_id'] == row['evidence_id'] and not value.get('isError')
        assert core.sha(path.read_bytes()) == row['sha256']
        subject = record['subject']
        subject_path = resolve_subject_path(subject['local_path'])
        assert subject_path.is_file(), str(subject_path)
        walk({'path': str(subject_path), 'sha256': subject['digest']['sha256']})
    for session in rea['sessions']:
        assert session['closed'] is True
    assets = core.assets()
    baseline = core.read_json(ROOT/'research/evidence/still-lut-static.json')
    assert assets['files'] == baseline['deployment_assets_unchanged']
    toolchain = core.read_json(ROOT/'research/toolchains/rea.json')
    walk(toolchain)
    lut = core.read_json(ROOT/'research/evidence/lut-reference-checks.json')
    walk(lut)
    assert lut['passed'] and not lut['failures'] and not lut['errors']
    assert lut['opencolorio']['performed'] and lut['counts']['opencolorio_rgb_cases'] == 8016
    assert lut['camera_access'] is False and lut['target_execution'] is False
    result = {'camera_accessed': False, 'target_executed': False,
              'target_execution_definition': 'No execution on camera hardware; bounded CP metadata builders were emulated on the host with external operations stubbed.',
              'host_CP_emulation_performed': True,
              'source_files_sha256_verified': len(checked),
              'container_crc_verified': True, 'partition_count': 30,
              'extracted_regular_files': 1369, 'whole_extraction_ELF_files': 650,
              'AP_scope_ELF_files': 619, 'CP_decoded_regions': 46,
              'AP_native_LUT_resource_instruction_ranges': 11,
              'host_CP_wrapper_cases': 48, 'host_CP_descriptor_builder_cases': 4,
              'MLUT_SRAM_payload_bytes': 48000, 'MLUT_register_payload_bytes': 124,
              'ordinary_native_LUT_packer_cases': 10, 'native_parser_writer_boundary_cases': 18,
              'native_packer_bytes_compared': 480000, 'native_unpacked_samples_compared': 147390,
              'MLUT_stock_identity_halfwords_verified': 24000,
              'MLUT_diagnostic_pattern_words_verified': 12000,
              'MLUT_register_builder_cases': 128,
              'still_route_regions': 32, 'still_route_u16_mode_cases': 65536,
              'still_route_mode_property_nonzero_cases': 0,
              'still_route_callback_resource_dimension_cases': 81,
              'still_route_total_host_cases': 65621,
              'still_recording_regions': 8, 'still_recording_REA_records': 10,
              'full_resolution_saved_still_MLUT_route_proven': False,
              'MLUT_diagnostic_entries': 64,
              'REA_successful_records_verified': len(rea['records']),
              'REA_pseudocode_records': rea['pseudocode_records'],
              'REA_closed_sessions': len(rea['sessions']),
              'deployment_assets_unchanged_from_pre_investigation': True,
              'host_LUT_reference_test_groups': lut['tests_run'],
              'host_LUT_OpenColorIO_comparisons': lut['counts']['opencolorio_rgb_cases'],
              'limits': ['Source-bound artifact verification and explicitly bounded host emulation, not camera or image-pipeline validation.',
                         'Ordinary LUT packing is recovered; alternate serialization and live saved-still routing remain unresolved.']}
    core.save_json(ROOT/'research/evidence/deep-dive-verification.json', result)
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
