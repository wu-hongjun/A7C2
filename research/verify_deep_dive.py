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


def verify_instruction_bytes(report):
    """Check retained instruction bytes against original files, not just counts."""
    default_source = report.get('source', {}).get('path')
    regions = report.get('regions', report.get('disassembled_regions', []))
    if isinstance(regions, dict):
        regions = regions.values()
    cache, count = {}, 0
    for region in regions:
        path = ROOT / region.get('source_path', default_source)
        if path not in cache:
            cache[path] = path.read_bytes()
        for instruction in region['instructions']:
            offset = instruction.get('file_offset')
            if offset is None:
                offset = region['file_offset'] + int(instruction['va'], 0) - int(region['start_va'], 0)
            offset = int(offset, 0) if isinstance(offset, str) else offset
            raw = bytes.fromhex(instruction['bytes'])
            assert raw and cache[path][offset:offset+len(raw)] == raw, (path, offset)
            count += 1
    return count


def main():
    names = ['firmware-structure', 'ap-interfaces', 'cp-architecture']
    reports = {name: core.read_json(ROOT/'research/evidence'/f'{name}.json') for name in names}
    checked = {}

    def walk(value):
        if isinstance(value, dict):
            if isinstance(value.get('path'), str) and isinstance(value.get('sha256'), str):
                path = ROOT/value['path']
                optional_synthetic = value['path'].startswith('research/build/hq-lut/')
                if value['path'].startswith('research/') and not path.is_file() and not optional_synthetic:
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
    transfer = core.read_json(ROOT/'research/evidence/lut-transfer.json')
    sink = core.read_json(ROOT/'research/evidence/still-sink.json')
    encoder = core.read_json(ROOT/'research/evidence/still-encoder-route.json')
    for report in (transfer, sink, encoder):
        walk(report)
    instruction_counts = {name: verify_instruction_bytes(report) for name, report in (
        ('LUT_transfer', transfer), ('AP_file_sink', sink), ('CP_JPEG_encoder', encoder))}
    assert transfer['all_checks_passed'] and not transfer['camera_accessed']
    assert len(transfer['AP_copy_cases']) == 5 and len(transfer['AP_selection_cases']) == 2
    assert len(transfer['kernel_scatter_cases']) == 8
    assert sum(row['exact_requested_length'] for row in transfer['kernel_scatter_cases']) == 4
    assert sorted(row['excess_bytes'] for row in transfer['kernel_scatter_cases']) == [0,0,0,0,6811,6811,6939,6939]
    assert not transfer['findings']['CP_resolved_resource_equality_proven']
    assert not sink['camera_accessed'] and not sink['filesystem_write_invoked']
    assert len(sink['classes']) == 3 and len(sink['REA_records']) == 19
    assert sink['host_tests']['addition_leaves']['cases'] == 80
    assert sink['host_tests']['writer_scatter_loop']['cases'] == 5
    assert sink['host_tests']['appfw_input_handoff']['cases'] == 10
    assert encoder['verification']['host_cases'] == 1311
    assert not encoder['target_executed'] and encoder['host_emulation_performed']
    hq = core.read_json(ROOT/'research/evidence/hq-lut-format.json')
    walk(hq)
    assert hq['schema'] == 2 and not hq['camera_access'] and not hq['network_access']
    assert not hq['syscall_forwarding'] and not hq['target_native_process_execution']
    assert len(hq['cases']) == 3
    samples = 0
    for row in hq['cases']:
        path = ROOT/row['evidence_path']
        assert core.sha(path.read_bytes()) == row['evidence_sha256']
        case = core.read_json(path)
        walk(case)
        assert case['generator'] == hq['generator'] and case['source'] == hq['source']
        outcome = case['result']
        assert outcome['finished'] and outcome['failure'] is None
        assert outcome['sram_reported_bytes'] == 48000 and outcome['register_reported_bytes'] == 128
        assert outcome['written_byte_coverage'] == {'sram':48000,'register_124':124,'trailing_four':4}
        assert outcome['output_guard_unchanged'] and outcome['trailing_four_hex'] == '00000000'
        oracle = outcome['quantizer_oracle']
        assert oracle['complete'] and oracle['exact_match'] and oracle['compared_samples'] == 24000
        assert oracle['expected_sha256'] == oracle['actual_sha256'] == outcome['sram_sha256']
        samples += oracle['compared_samples']
    assert hq['validation']['identity_coordinate_samples_checked'] == 24000
    assert hq['validation']['identity_axis_mismatches'] == 0
    assert hq['validation']['permutation_full_48128_bytes_exact']
    assert hq['validation']['grid17_vs_grid33_sram_exact']
    instruction_counts['HQ_converter'] = verify_instruction_bytes({
        'source':hq['source'],'regions':hq['static_evidence']['ranges']})
    coordinates = core.read_json(ROOT/'research/evidence/hq-coordinates.json')
    walk(coordinates)
    assert not coordinates['camera_access'] and not coordinates['network_access']
    assert not coordinates['target_execution_in_this_script']
    cv = coordinates['validation']
    assert cv['axis_exact_match'] and cv['register_analytic_truncation_exact']
    assert cv['all_identity_float_samples_compared'] == 24000
    assert cv['all_identity_float_samples_mismatched'] == cv['all_identity_float_samples_max_abs_error'] == 0
    assert coordinates['observed_axis_float32'] == coordinates['independent_expected_axis_float32']
    assert coordinates['published_register_words_11_to_30'] == hq['cases'][0]['register_u32_le'][11:31]
    instruction_counts['HQ_coordinates'] = verify_instruction_bytes({
        'source':coordinates['source'],'regions':coordinates['static_ranges']})
    sections = core.read_json(ROOT/'research/evidence/hq-sections.json')
    walk(sections)
    assert not sections['camera_access'] and not sections['network_access'] and not sections['syscall_forwarding']
    sv = sections['validation']
    assert sv['completed'] and sv['alignment_cases_passed'] == 28 and sv['bank_sections_checked'] == 24
    assert sv['output_equals_previous_identity'] and sv['output_guard_unchanged']
    assert sv['last_four_zero_writes_from_original_padding_loop']
    assert sv['native_48128_sha256'] == hq['cases'][0]['output_48128_sha256']
    assert len(sections['sections']) == 25
    register = sections['sections'][0]
    assert register['input_length_x2'] == 124 and register['format_flag_x5'] == 0
    assert register['returned_length_x0'] == 128 and register['destination_relative_to_native_base'] == 48000
    assert register['destination_offset_x4'] == 0
    assert [row['destination_offset_x4'] for row in sections['sections'][1:]] == list(range(0,48000,2000))
    assert all(row['input_length_x2'] == row['returned_length_x0'] == 2000 and row['format_flag_x5'] == 1
               for row in sections['sections'][1:])
    assert [row['native_blob_offset'] for row in sections['last_four_writes']] == list(range(48124,48128))
    assert all(row['pc'] == '0x36fc964' and row['value'] == 0 and row['bytes'] == 1
               for row in sections['last_four_writes'])
    assert all(row['actual'] == row['independent_expected'] == ((row['input']+31)//32)*32
               for row in sections['alignment_cases'])
    instruction_counts['HQ_sections'] = verify_instruction_bytes({
        'source':sections['source'],'regions':sections['static_ranges']})
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
              'target_execution_definition': 'No execution on camera hardware; selected original AP conversion and AP/CP metadata/descriptor routines run only in bounded host emulation with documented external stubs.',
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
              'LUT_transfer_host_characterizations': 15,
              'LUT_transfer_fragmented_counterexamples': 4,
              'AP_file_sink_host_cases': 95, 'AP_file_sink_REA_records': 19,
              'CP_JPEG_encoder_host_cases': encoder['verification']['host_cases'],
              'new_instruction_records_byte_verified': instruction_counts,
              'HQ_converter_completed_fixtures': len(hq['cases']),
              'HQ_quantizer_samples_compared': samples,
              'HQ_identity_coordinate_samples': 24000,
              'HQ_fixed_affine_float_samples_compared': 24000,
              'HQ_fixed_affine_axis_and_register_values': [20,20],
              'HQ_native_bytes': 48128, 'HQ_second_serialized_region_bytes': 128,
              'HQ_section_alignment_cases': 28,
              'HQ_register_content_bytes': 124, 'HQ_zero_alignment_padding_bytes': 4,
              'full_resolution_saved_still_MLUT_route_proven': False,
              'MLUT_diagnostic_entries': 64,
              'REA_successful_records_verified': len(rea['records']),
              'REA_pseudocode_records': rea['pseudocode_records'],
              'REA_closed_sessions': len(rea['sessions']),
              'deployment_assets_unchanged_from_pre_investigation': True,
              'host_LUT_reference_test_groups': lut['tests_run'],
              'host_LUT_OpenColorIO_comparisons': lut['counts']['opencolorio_rgb_cases'],
              'limits': ['Source-bound artifact verification and explicitly bounded host emulation, not camera or image-pipeline validation.',
                         'Ordinary packing and synthetic HQ serialization are recovered; upstream resampling and live saved-still routing remain unresolved.',
                         'Four synthetic fragmented transfer layouts emit excess descriptor bytes; retained counterexamples are not observed hardware overruns.']}
    core.save_json(ROOT/'research/evidence/deep-dive-verification.json', result)
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
