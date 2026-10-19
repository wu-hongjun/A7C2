"""Build a local launcher workspace from validated LUTs, lens presets and binaries.

No installation/transport. Contains private generated LUT arrays: do not commit
or redistribute this local bundle without the profile owner's permission.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
from camera_worker.pack import lut_binary
from photo_worker.worker import load_lenses, read_limited, sync_directory
ROOT=Path(__file__).resolve().parents[1]

def sha(v):return hashlib.sha256(v).digest()
def string(s,n):
    b=s.encode('ascii')
    if not b or len(b)>=n or any(x<32 or x>126 for x in b):raise ValueError('Preset text must be bounded printable ASCII')
    return b+bytes(n-len(b))

def bundle(output, *, launcher, jpeg, hif, luts, lenses, host_test=False):
    output=Path(output)
    bins={name:read_limited(path,(128 if host_test and name=='photo-launcher' else 16)*1024*1024) for name,path in [('photo-launcher',launcher),('jpeg-worker',jpeg),('hif-worker',hif)]}
    if not host_test:
        pins={'photo-launcher':ROOT/'camera_worker/dist/launcher-build.json',
              'jpeg-worker':ROOT/'camera_worker/dist/build.json','hif-worker':ROOT/'camera_worker/dist/heif-build.json'}
        for name,raw in bins.items():
            if raw[:4]!=b'\x7fELF' or int.from_bytes(raw[18:20],'little')!=183:raise ValueError('Expected static ARM64 candidates')
            doc=json.loads(pins[name].read_text());expected=doc['executable']['sha256'] if name=='jpeg-worker' else doc['binary']['sha256']
            if sha(raw).hex()!=expected:raise ValueError('Unreviewed binary: '+name)
    proof=json.loads((ROOT/'research/evidence/fujifilm-srgb-checks.json').read_text())
    known={r['CLF_file']:r['CLF_sha256'] for r in proof['profiles']}
    profiles=sorted(Path(luts).glob('*.clf'))
    if len(profiles)!=11:raise ValueError('Expected all 11 validated CLFs')
    looks=[('none','No LUT',b'')]
    for p in profiles:
        raw=read_limited(p,8*1024*1024)
        if sha(raw).hex()!=known.get(p.name):raise ValueError('CLF differs from validated conversion')
        looks.append((p.stem,p.stem.replace('-srgb','').replace('-',' ').title(),lut_binary(raw)))
    selected_lenses=load_lenses(lenses)
    if len(selected_lenses)>99:raise ValueError('Native catalog supports 99 manual lenses plus the reserved none entry')
    if any(l['id']=='none' for l in selected_lenses):raise ValueError('Lens id none is reserved')
    payload=b''
    for ident,label,raw in looks:
        payload+=string(ident,64)+string(label,128)+sha(raw)+struct.pack('<I',len(raw))+bytes(28)
    payload+=string('none',64)+string('Preserve source lens',128)+bytes(128+128+16+48)
    for l in selected_lenses:
        payload+=string(l['id'],64)+string(l['name'],128)+string(l['make'],128)+string(l['model'],128)+struct.pack('<dd',l['focal_length_mm'],l['max_aperture'])+bytes(48)
    catalog=struct.pack('<8s4I',b'A7CAT001',1,len(looks),len(selected_lenses)+1,len(payload))+sha(bins['jpeg-worker'])+sha(bins['hif-worker'])+sha(payload)+bytes(8)+payload
    assert len(catalog)==128+256*len(looks)+512*(len(selected_lenses)+1)
    selection=struct.pack('<8s4IdQ',b'A7SEL001',1,0,0,0,0,0)+sha(catalog)+bytes(24)
    assert len(selection)==96
    selection+=sha(selection)
    output.mkdir(mode=0o700,parents=False,exist_ok=False)
    for d in ('bin','looks','jobs','licenses'): (output/d).mkdir(mode=0o700)
    def write(path,raw,mode=0o600):
        with path.open('xb') as f:f.write(raw);f.flush();os.fsync(f.fileno())
        path.chmod(mode)
    for name,raw in bins.items():write(output/'bin'/name,raw,0o700)
    for ident,label,raw in looks:
        if raw:write(output/'looks'/(ident+'.bin'),raw)
    write(output/'catalog.bin',catalog);write(output/'selection.bin',selection)
    write(output/'lock',b'');write(output/'worker.lock',b'')
    shutil.copyfile(ROOT/'NOTICE.md',output/'NOTICE.md')
    for source in (ROOT/'licenses').iterdir():
        if source.is_file():write(output/'licenses'/source.name,source.read_bytes())
    write(output/'SOURCES.txt',b'Complete source/build scripts and codec archives: https://github.com/wu-hongjun/A7C2\nSee camera_worker/LAUNCHER.md, camera_worker/heif_sources and NOTICE.md in that repository.\nGenerated LUT tables remain private; this workspace is not an installer.\n')
    manifest={'schema':1,'host_test_only':host_test,'hardware_tested':False,'installation':False,
        'catalog_sha256':sha(catalog).hex(),'looks':[i for i,_,_ in looks],
        'lenses':['none']+[l['id'] for l in selected_lenses],
        'binary_sha256':{k:sha(v).hex() for k,v in bins.items()},
        'completion_contract':'Trusted producer arms an event before exposure, then submits only after final file close/sync; no native capture hook is established.',
        'selection_snapshot':'arm event; live menu/shutter adapter not implemented',
        'private_profile_tables':True,'redistribution':'See project NOTICE/licenses and profile rights; local generated tables remain private.'}
    write(output/'manifest.json',(json.dumps(manifest,indent=2)+'\n').encode())
    for d in ('bin','looks','jobs','licenses'):sync_directory(output/d)
    sync_directory(output);sync_directory(output.parent)
    return manifest

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    for n in ('output','launcher','jpeg','hif','luts','lenses'):p.add_argument('--'+n,type=Path,required=True)
    p.add_argument('--host-test',action='store_true');a=p.parse_args()
    print(json.dumps(bundle(a.output,launcher=a.launcher,jpeg=a.jpeg,hif=a.hif,luts=a.luts,lenses=a.lenses,host_test=a.host_test),indent=2))
