"""Refresh the repository's research artifact index without indexing firmware copies."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main():
    output = ROOT/'research/evidence/index.json'
    data = json.loads(output.read_text())
    paths = {ROOT/row['path'] for row in data['artifacts']}
    paths.update(ROOT/name for name in ('README.md', 'README.zh-TW.md', '.gitignore', '.gitattributes'))
    research = ROOT/'research'
    for pattern in ('*.md', '*.py', '*.mjs', 'requirements*.txt'):
        paths.update(research.glob(pattern))
    for directory in ('evidence', 'toolchains', 'rea-jobs'):
        paths.update(p for p in (research/directory).rglob('*') if p.is_file())
    paths.discard(output)
    data['artifacts'] = []
    for path in sorted(paths):
        with path.open('rb') as stream:
            digest = hashlib.file_digest(stream, 'sha256').hexdigest()
        data['artifacts'].append({'path': str(path.relative_to(ROOT)),
                                  'bytes': path.stat().st_size, 'sha256': digest})
    data['deep_dive_scope'] = {'camera_accessed': False, 'target_executed': False,
                               'source': 'official A7C II 2.01 offline image',
                               'report': 'research/A7C2_FIRMWARE_ATLAS.md'}
    output.write_text(json.dumps(data, indent=2) + '\n')
    print(json.dumps({'indexed_artifacts': len(data['artifacts']), 'output': str(output)}, indent=2))


if __name__ == '__main__':
    main()
