"""Index retained REA MCP results and render pseudocode for offline review."""
import json
import argparse
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import core


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--normalize-readable-prefix', default='',
                        help='Trim whitespace only in derived C views for matching session names; raw Evidence stays unchanged')
    args = parser.parse_args()
    base = ROOT / 'research/evidence/rea'
    readable = base / 'readable'
    readable.mkdir(exist_ok=True)
    pending = [p.parent.name for p in base.glob('*/open.json')
               if not (p.parent/'receipts.json').exists()]
    if pending:
        raise ValueError('REA sessions still lack final receipts: ' + ', '.join(sorted(pending)))
    records, errors, lifecycle = [], [], []
    for path in sorted(base.glob('*/*.json')):
        if path.parent == readable or path.name == 'tool-schemas.json':
            continue
        value = core.read_json(path)
        if 'content' not in value:
            continue  # Bundles and receipts have their own structures.
        parsed = value.get('structuredContent')
        if parsed is None:
            text = next((v['text'] for v in value['content'] if v['type'] == 'text'), None)
            if text is None:
                continue
            parsed = json.loads(text)
        relative = str(path.relative_to(ROOT))
        if value.get('isError'):
            error = parsed.get('error', {})
            errors.append({'path': relative, 'code': error.get('code'),
                           'issues': error.get('details', {}).get('issues')})
            continue
        if 'evidence_id' not in parsed:
            lifecycle.append({'path': relative, 'result': parsed.get('result', parsed)})
            continue
        subject = parsed.get('subject', {})
        row = {'path': relative, 'sha256': core.sha(path.read_bytes()),
               'evidence_id': parsed['evidence_id'], 'operation': parsed['operation'],
               'subject': subject, 'parameters': parsed.get('parameters'),
               'analysis_profile': parsed.get('analysis_profile'),
               'limitations': parsed.get('limitations'),
               'relationship_coverage': parsed.get('relationship_coverage')}
        if parsed['operation'] == 'procedure_pseudo_code':
            code = parsed['normalized_result']
            if not isinstance(code, str):
                raise ValueError('Unexpected pseudocode representation')
            if args.normalize_readable_prefix and path.parent.name.startswith(args.normalize_readable_prefix):
                code = '\n'.join(line.rstrip() for line in code.splitlines()).rstrip() + '\n'
            filename = path.parent.name + '-' + path.stem + '.c'
            output = readable / filename
            output.write_text('// Decompiled pseudocode, not original source.\n'
                              '// REA Evidence: ' + parsed['evidence_id'] + '\n'
                              '// Source record: ' + relative + '\n' + code)
            row['readable_pseudocode'] = str(output.relative_to(ROOT))
            row['pseudocode_warnings'] = [line.strip() for line in code.splitlines()
                                          if 'WARNING:' in line]
        records.append(row)
    # Every fully consumed session must have an explicit successful close.
    session_status = []
    for receipt in sorted(base.glob('*/receipts.json')):
        data = core.read_json(receipt)
        ops = data['operations']
        closed = any(r.get('operation') == 'close_binary' and not r.get('isError') for r in ops)
        if not closed:
            raise ValueError('Session lacks successful close: ' + str(receipt))
        if data['camera_accessed'] or data['target_execution_requested']:
            raise ValueError('Unexpected target effects')
        session_status.append({'session': receipt.parent.name, 'closed': closed,
                               'exported_bundle': (receipt.parent/'evidence-bundle.json').exists(),
                               'operations': len(ops)})
    index = {'mode': 'offline_REA_Ghidra_analysis', 'camera_accessed': False,
             'successful_evidence_records': len(records),
             'pseudocode_records': sum('readable_pseudocode' in r for r in records),
             'subjects_by_sha256': sorted({r['subject']['digest']['sha256'] for r in records}),
             'records': records, 'failed_requests': errors, 'sessions': session_status,
             'lifecycle_results': lifecycle,
             'limits': ['Pseudocode and database observations are not original source or runtime execution.',
                        'Failed queries are retained and excluded from successful Evidence counts.',
                        'Ghidra shared-object addresses include analysis load bias; compare mapped file offsets.',
                        'A missing modeled output-pointer/virtual-call effect can remove valid pseudocode branches.']}
    core.save_json(base/'index.json', index)
    print(json.dumps({k: index[k] for k in ['successful_evidence_records', 'pseudocode_records',
                                           'subjects_by_sha256', 'camera_accessed']}, indent=2))


if __name__ == '__main__':
    main()
