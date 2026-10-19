"""Offline launcher build against the locally built pinned HEIF codec prefix.

First use build_heif.py (no network) to generate the host/target codec prefix.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
from camera_worker.build_heif import ARCHIVES
ROOT=Path(__file__).resolve().parents[1]

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def build(codecs,output,host=False,sanitize=False):
    codecs=Path(codecs).resolve();output=Path(output).resolve();output.mkdir(parents=True,exist_ok=False)
    receipt=json.loads((codecs/'build.json').read_text())
    if bool(receipt.get('sanitizers',False))!=sanitize:raise ValueError('Launcher and codec sanitizer configurations must match')
    if receipt['host_variant']!=host:raise ValueError('Codec build target mismatch')
    for name,value in ARCHIVES.items():
        if sha(ROOT/'camera_worker/heif_sources'/name)!=value[0]:raise ValueError('Codec source mismatch')
    prefix=codecs/'prefix'
    for name,digest in receipt['static_library_sha256'].items():
        if sha(prefix/'lib'/name)!=digest:raise ValueError('Codec library differs from pinned local build')
    c=['clang'] if host else [str(codecs/'cc')]
    cpp=['clang++'] if host else [str(codecs/'cxx')]
    flags=['-O2','-D_GNU_SOURCE','-ffp-contract=off','-fno-fast-math','-fno-strict-aliasing']
    if sanitize:
        if not host:raise ValueError('Host sanitizers only')
        flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-g']
    obj=output/'core.o'
    subprocess.run(c+flags+['-std=c11','-DA7_HIF_CORE','-c',str(ROOT/'camera_worker/worker.c'),'-o',str(obj)],check=True)
    target=output/('photo-launcher-host' if host else 'photo-launcher-aarch64')
    subprocess.run(cpp+flags+['-std=c++20','-I'+str(prefix/'include'),str(ROOT/'camera_worker/launcher.cc'),str(obj),
        str(prefix/'lib/libheif.a'),str(prefix/'lib/libde265.a'),str(prefix/'lib/libx265.a'),'-lpthread','-lm']+
        ([] if host else ['-static','-s','-Wl,-z,noexecstack'])+['-o',str(target)],check=True)
    names=['camera_worker/launcher.cc','camera_worker/ingest_exif.h','camera_worker/heif_io.h','camera_worker/heif_core.h','camera_worker/worker.c','native/sha256.h','camera_worker/build_launcher.py']
    result={'schema':1,'host_variant':host,'sanitizers':sanitize,'target':'native macOS' if host else 'static generic ARMv8-A aarch64-linux-musl',
        'camera_accessed':False,'network_during_build':False,'binary':{'name':target.name,'bytes':target.stat().st_size,'sha256':sha(target)},
        'sources':{n:sha(ROOT/n) for n in names},'codec_build_receipt_sha256':sha(codecs/'build.json'),
        'static_library_sha256':receipt['static_library_sha256'],'archives':receipt['archives'],
        'compiler':subprocess.check_output(cpp+['--version'],text=True).splitlines()[0],
        'scope':'Native manual CLI and explicit completed-file ingress; no firmware capture hook or camera UI adapter installed',
        'distribution_license':'GPL-3.0-or-later combined artifact; codec sources/notices accompany repository'}
    (output/'build.json').write_text(json.dumps(result,indent=2)+'\n');return result

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--codecs',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--host',action='store_true');p.add_argument('--sanitize',action='store_true');a=p.parse_args()
    print(json.dumps(build(a.codecs,a.output,a.host,a.sanitize),indent=2))
