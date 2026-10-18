"""Create a local, non-installing bundle of the ELF, 11 LUT blobs and notices."""
import argparse
import json
from pathlib import Path
import shutil

from camera_worker.pack import lut_binary
from photo_worker.worker import sha,read_limited

ROOT=Path(__file__).resolve().parents[1]


def bundle(binary,luts,output):
    binary,luts,output=Path(binary),Path(luts),Path(output)
    raw=binary.read_bytes()
    if raw[:4]!=b'\x7fELF' or int.from_bytes(raw[18:20],'little')!=183:
        raise ValueError('Expected the offline-checked ARM64 ELF')
    expected=json.loads((ROOT/'camera_worker/dist/build.json').read_text())['executable']['sha256']
    if sha(raw)!=expected:raise ValueError('Binary differs from the reviewed build artifact')
    profiles=sorted(luts.glob('*.clf'))
    if len(profiles)!=11:raise ValueError('Expected all 11 validated converted CLFs')
    proof=json.loads((ROOT/'research/evidence/fujifilm-srgb-checks.json').read_text())
    known={r['CLF_file']:r['CLF_sha256'] for r in proof['profiles']}
    looks=[]
    for p in profiles:
        raw=read_limited(p,8*1024*1024)
        if sha(raw)!=known.get(p.name):raise ValueError('LUT differs from validated CLF: '+p.name)
        looks.append((p,lut_binary(raw)))
    output.mkdir(parents=True,exist_ok=False);(output/'looks').mkdir();(output/'licenses').mkdir()
    shutil.copyfile(binary,output/'jpeg-worker-aarch64');(output/'jpeg-worker-aarch64').chmod(0o755)
    records=[]
    for source,raw in looks:
        target=output/'looks'/(source.stem+'.bin');target.write_bytes(raw)
        records.append({'file':str(target.relative_to(output)),'bytes':len(raw),'sha256':sha(raw),
                        'source_CLF_sha256':sha(source.read_bytes())})
    shutil.copyfile(ROOT/'photo_worker/lens-presets.example.json',output/'lens-presets.example.json')
    shutil.copyfile(ROOT/'NOTICE.md',output/'NOTICES.md')
    for p in (ROOT/'licenses').iterdir():
        if p.name.startswith(('Adobe-DNG','libjpeg-turbo','musl-','Zig-')):shutil.copyfile(p,output/'licenses'/p.name)
    (output/'README.txt').write_text('Offline-validated, hardware-untested prototype. No installer or launch script.\n'
        'Do not copy or run on the camera as part of this offline task.\n'
        'The native runner consumes host-prepared job directories; it does not parse preset JSON or watch camera captures.\n'
        'See camera_worker/README.md in the source repository.\n'
        'This software is based in part on the work of the Independent JPEG Group.\n')
    manifest={'schema':1,'camera_installed':False,'hardware_tested':False,
        'ELF':{'file':'jpeg-worker-aarch64','sha256':sha(binary.read_bytes())},
        'looks':records,'preset_JSON_role':'Example definitions for the host job preparer; not read by the native runner',
        'files':[{'file':str(p.relative_to(output)),'bytes':p.stat().st_size,'sha256':sha(p.read_bytes())}
                 for p in sorted(output.rglob('*')) if p.is_file()]}
    (output/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n');return manifest


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--binary',type=Path,required=True);p.add_argument('--luts',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();print(json.dumps(bundle(a.binary,a.luts,a.output),indent=2))
