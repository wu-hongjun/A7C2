"""Build the transport-neutral observer core and self-test, entirely offline.

No Sony loader/trampoline or installation tool is produced. The ARM64 self-test
executes only our core; it is not linked to or allowed to call Sony libraries.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]

def build(output, *, codecs=None, host=False, sanitize=False):
    output=Path(output).resolve();output.mkdir(parents=True,exist_ok=False)
    if sanitize and not host:raise ValueError('Host sanitizers only')
    if host:compiler=['clang++']
    else:
        if not codecs:raise ValueError('Supply existing offline ARM64 toolchain directory')
        codecs=Path(codecs).resolve()
        receipt=json.loads((codecs/'build.json').read_text())
        if receipt['host_variant']:raise ValueError('Require Linux ARM64 toolchain')
        compiler=[str(codecs/'cxx')]
    flags=['-O2','-g0','-std=c++20','-D_GNU_SOURCE','-Wall','-Wextra','-Werror','-fno-exceptions','-fno-rtti','-ffp-contract=off']
    if sanitize:flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-g']
    obj=output/'sony_bridge.o'
    subprocess.run(compiler+flags+['-c',str(ROOT/'camera_worker/sony_bridge.cc'),'-o',str(obj)],check=True)
    archive=output/'libsony_bridge.a'
    # Apple's ar can silently omit Linux ELF members. Use the local LLVM
    # archiver, verify the member, and link/run its entry points in the test.
    zig=shutil.which('zig')
    if not zig:raise ValueError('Require existing local Zig/LLVM archiver')
    subprocess.run([zig,'ar','rcsD',str(archive),str(obj)],check=True)
    members=subprocess.check_output([zig,'ar','t',str(archive)],text=True).splitlines()
    if members!=[obj.name] or archive.stat().st_size<obj.stat().st_size:raise ValueError('Archive lost its object member')
    binary=output/('sony-bridge-test-host' if host else 'sony-bridge-test-aarch64')
    subprocess.run(compiler+flags+[str(ROOT/'camera_worker/tests/sony_bridge_test.cc'),str(archive)]+
                   ([] if host else ['-static','-s','-Wl,-z,noexecstack'])+['-o',str(binary)],check=True)
    names=['camera_worker/sony_bridge.h','camera_worker/sony_bridge.cc','camera_worker/sony_identity.h','camera_worker/tests/sony_identity_cases.h','camera_worker/tests/sony_bridge_test.cc','camera_worker/build_sony_bridge.py']
    def digest(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
    doc={'schema':1,'host_variant':host,'sanitizers':sanitize,'network_accessed':False,'camera_accessed':False,
         'target':'native macOS' if host else 'generic ARMv8-A aarch64-linux-musl',
         'scope':'Transport-neutral core/self-test only; no Sony module, UI renderer, hook, loader, callback registration or installation',
         'compiler':subprocess.check_output(compiler+['--version'],text=True).splitlines()[0],
         'archiver':'zig '+subprocess.check_output([zig,'version'],text=True).strip()+' ar rcsD',
         'sources':{n:digest(ROOT/n) for n in names},
         'artifacts':{p.name:{'sha256':digest(p),'bytes':p.stat().st_size} for p in (archive,binary)}}
    (output/'build.json').write_text(json.dumps(doc,indent=2)+'\n');return doc

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--codecs',type=Path);p.add_argument('--host',action='store_true');p.add_argument('--sanitize',action='store_true');a=p.parse_args()
    print(json.dumps(build(a.output,codecs=a.codecs,host=a.host,sanitize=a.sanitize),indent=2))
