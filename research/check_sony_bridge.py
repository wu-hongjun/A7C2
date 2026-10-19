"""Offline Sony-adapter replay, original-instruction evidence and native workers.

Only generated research/build workspaces are used. Synthetic frame/request/
close/identity bindings are explicit; this does not establish a camera hook.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
from PIL import Image
from camera_worker.launcher_bundle import bundle
from research.check_jpeg_worker import exiftool
from research.check_camera_worker import elf_check
from research.emulate_camera_worker import execute

ROOT=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()

def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n in ('host','asan','linux','launcher','jpeg','hif','fixture','luts','work','report'):
        p.add_argument('--'+n,type=Path,required=True)
    a=p.parse_args();a.work=a.work.resolve()
    if not a.work.is_relative_to(ROOT/'research/build'):raise ValueError('Only local generated build workspace allowed')
    a.work.mkdir(parents=True,exist_ok=False);rows=[]
    def call(argv):
        r=subprocess.run([str(x) for x in argv],capture_output=True,text=True,timeout=180)
        assert r.returncode==0,(argv,r.stdout,r.stderr)
        assert 'AddressSanitizer' not in r.stderr and 'runtime error:' not in r.stderr,r.stderr
        return r
    builds={}
    for label,base in [('host',a.host),('asan',a.asan),('linux',a.linux)]:
        doc=json.loads((base/'build.json').read_text())
        for name,value in doc['artifacts'].items():assert sha(base/name)==value['sha256']
        for name,value in doc['sources'].items():assert sha(ROOT/name)==value
        builds[label]=doc
        if label!='linux':
            r=json.loads(call([base/'sony-bridge-test-host']).stdout);assert r['checks']>=160
            rows.append({'case':label+'-core-and-linked-archive-selftest',**r})
    # Includes initialization of multiple 128-slot fixed mailboxes in the test;
    # a bounded instruction budget, not a camera callback latency measurement.
    target=a.linux/'sony-bridge-test-aarch64';emulated=execute(target,count=20000000)
    assert emulated['exit_code']==0 and '"status":"passed"' in emulated['stdout'];rows.append({'case':'ARM64-core-and-linked-archive-selftest',**emulated,'ELF':elf_check(target)})
    # Stress the mailbox independently on actual host threads with sanitizers.
    threads=a.work/'threads'
    call(['clang++','-std=c++20','-O1','-fsanitize=address,undefined','-g','-fno-omit-frame-pointer',
          ROOT/'camera_worker/tests/sony_bridge_threads.cc','-o',threads])
    thread_result=json.loads(call([threads]).stdout);rows.append({'case':'host-MPSC-forwarding-ASan-UBSan',**thread_result})
    scenario=json.loads(call([a.host/'sony-bridge-test-host','--scenario']).stdout)
    # Use real native launcher/worker processes, with preexisting local codecs
    # and private validated LUTs. No polling, camera, mounted card or network.
    captures=a.work/'captures';photo_dir=captures/'DCIM/100MSDCF';photo_dir.mkdir(parents=True)
    exif=Image.Exif();exif[271]='SONY';exif[272]='Synthetic offline source';exif[274]=1;exif[34665]={0xa001:1,0x8827:320}
    before=Image.new('RGB',(256,192),(75,155,210));before.save(photo_dir/'DSC00001.JPG',quality=98,subsampling=0,exif=exif)
    call([a.fixture.resolve(),photo_dir/'DSC00002.HIF','8','13','10','422','color'])
    identities={path:sha(captures/path) for path in scenario['paths']} # known synthetic bytes before callback replay
    root=a.work/'workspace';manifest=bundle(root,launcher=a.launcher,jpeg=a.jpeg,hif=a.hif,luts=a.luts,
                                           lenses=ROOT/'photo_worker/lens-presets.example.json',host_test=True)
    launcher=a.launcher.resolve();choices=scenario['menu_choices'];ids=['sony-jpeg','sony-hif']
    for ident,(look,lens) in zip(ids,choices):
        call([launcher,'--select',root,manifest['looks'][look],manifest['lenses'][lens],'unknown'])
        call([launcher,'--arm',root,ident])
    for ident in scenario['completion_order']:
        index=ids.index(ident);path=scenario['paths'][index]
        call([launcher,'--enqueue-completed-srgb',root,captures,path,ident,identities[path]])
        call([launcher,'--run',root,ident]);assert call([launcher,'--status',root,ident]).stdout=='done\n'
        output=root/'jobs'/ident/('result.jpg' if index==0 else 'result.HIF');tags=exiftool(output)
        expected=58 if manifest['lenses'][choices[index][1]]=='example-helios-58' else 50
        assert tags['ExifIFD:FocalLength']==expected and 'ExifIFD:FNumber' not in tags
        if index:
            assert tags['QuickTime:BitDepthLuma']==10 and tags['QuickTime:BitDepthChroma']==10
            call(['ffmpeg','-v','error','-i',output,'-frames:v','1','-f','null','-'])
        else:
            with Image.open(output) as im:assert im.size==(256,192)
        assert sha(captures/path)==identities[path]
        rows.append({'case':'synthetic-callback-replay-to-native-'+('HIF10' if index else 'JPEG'),
          'status':'passed','look':manifest['looks'][choices[index][0]],'lens':manifest['lenses'][choices[index][1]],
          'shooting_aperture':'unknown','source_unchanged':True,'identity_proof':'synthetic fixture digest known before replay; not inferred from a late pathname',
          'output_sha256':sha(output),'focal_length_mm':expected})
    # A recycled pathname cannot be accepted using the bound earlier digest.
    stale=photo_dir/'STALE.JPG';shutil.copyfile(photo_dir/'DSC00001.JPG',stale);digest=sha(stale)
    call([launcher,'--arm',root,'stale']);before=Image.new('RGB',(256,192),(200,20,60));before.save(stale,exif=exif)
    failed=subprocess.run([str(launcher),'--enqueue-completed-srgb',str(root),str(captures),'DCIM/100MSDCF/STALE.JPG','stale',digest],capture_output=True,text=True)
    assert failed.returncode!=0 and not (root/'jobs/stale/job.bin').exists()
    rows.append({'case':'stale-path-with-earlier-identity-declined','status':'passed'})
    proof=json.loads((ROOT/'research/evidence/sony-integration.json').read_text())
    assert len(proof['isolated_original_instruction_cases'])==34
    names=['camera_worker/sony_bridge.h','camera_worker/sony_bridge.cc','camera_worker/sony_identity.h','camera_worker/tests/sony_identity_cases.h','camera_worker/build_sony_bridge.py',
           'camera_worker/tests/sony_bridge_test.cc','camera_worker/tests/sony_bridge_threads.cc','research/check_sony_bridge.py','research/trace_sony_integration.py']
    report={'schema':1,'camera_accessed':False,'network_accessed':False,'installed':False,
      'cases':rows,'builds':builds,'original_firmware_instruction_cases':34,
      'original_firmware_evidence_sha256':sha(ROOT/'research/evidence/sony-integration.json'),
      'sources':{n:sha(ROOT/n) for n in names},
      'capture_identity_original_instruction_cases':len(json.loads((ROOT/'research/evidence/capture-identity.json').read_text())['original_instruction_cases']),
      'capture_identity_evidence_sha256':sha(ROOT/'research/evidence/capture-identity.json'),
      'limits':['No Sony input hook, GUI renderer, shutter association or filesystem close hook installed or validated on hardware.',
                'Native worker replay uses explicitly synthetic request/frame/close and prebound file identities.',
                'ARM64 syscall emulation is not actual Linux/camera execution; only stdout/startup syscalls occur.',
                'ASan/UBSan are memory/undefined-behavior checks, not a thread race detector.']}
    a.report.write_text(json.dumps(report,indent=2)+'\n');print(f'{len(rows)} integration groups passed; 34 original-firmware instruction cases retained')

if __name__=='__main__':main()
