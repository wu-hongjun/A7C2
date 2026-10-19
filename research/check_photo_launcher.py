"""Offline native launcher/handoff integration tests in isolated local workspaces."""
from array import array
import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import shutil
import signal
import struct
import subprocess
import time
from PIL import Image
from camera_worker.launcher_bundle import bundle
from photo_worker.clf import parse_clf,processor
from photo_worker.worker import rational
from research.check_jpeg_worker import exiftool
ROOT=Path(__file__).resolve().parents[1]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n in ('launcher','jpeg','hif','fixture','luts','work','report'):p.add_argument('--'+n,type=Path,required=True)
    p.add_argument('--fault-library',type=Path);p.add_argument('--sample',type=Path);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=False)
    binary=a.launcher.resolve();work=a.work.resolve();captures=work/'captures';captures.mkdir();root=work/'workspace'
    bundle(root,launcher=binary,jpeg=a.jpeg,hif=a.hif,luts=a.luts,lenses=ROOT/'photo_worker/lens-presets.example.json',host_test=True)
    rows=[]
    def command(mode,*args,ok=True):
        r=subprocess.run([str(binary),mode,str(root),*[str(x) for x in args]],capture_output=True,text=True,timeout=120)
        assert (r.returncode==0)==ok,(mode,args,r.stdout,r.stderr)
        assert 'AddressSanitizer' not in r.stderr and 'runtime error:' not in r.stderr,r.stderr
        return r
    def select(look='classic-chrome-srgb',lens='example-takumar-50',aperture='2.8'):
        return command('--select',look,lens,aperture)
    def arm(id):command('--arm',id)
    def enqueue(id,path):return command('--enqueue-completed-srgb',captures,path,id,hashlib.sha256((captures/path).read_bytes()).hexdigest())
    def run(id):return command('--run',id)
    def jpeg(name,orientation=1,**save):
        exif=Image.Exif();exif[271]='SONY';exif[272]='Synthetic ILCE-7CM2';exif[274]=orientation
        exif[34665]={0xa001:1,0x9003:'2026:10:18 14:20:00',0x8827:320,0x829d:rational(5.6),0x920a:rational(24),0xa434:'Source Lens',0x927c:b'PRIVATE MAKERNOTE'}
        exif[34853]={1:'N',2:(rational(1),rational(2),rational(3))}
        im=Image.new('RGB',(128,96));im.paste((70,150,210),(0,0,64,96));im.paste((200,80,90),(64,0,128,96));path=captures/name
        im.save(path,quality=98,subsampling=0,exif=exif,**save);return path
    source=jpeg('one.JPG');source_hash=hashlib.sha256(source.read_bytes()).hexdigest()
    r=command('--list');assert 'classic-chrome-srgb' in r.stdout and 'example-helios-58' in r.stdout
    for clf in sorted(a.luts.glob('*.clf')):
        select(clf.stem);id=clf.stem;arm(id);enqueue(id,source.name);run(id);job=root/'jobs'/id
        with Image.open(source) as before,Image.open(job/'result.jpg') as after:
            assert before.size==after.size;actual=[c for point in ((32,48),(96,48)) for c in after.getpixel(point)]
            expected=array('f',(c/255 for point in ((32,48),(96,48)) for c in before.getpixel(point)))
            processor(parse_clf(clf.read_bytes())).applyRGB(expected);err=max(abs(v-min(1,max(0,f))*255) for v,f in zip(actual,expected));assert err<4,err
        tags=exiftool(job/'result.jpg');assert tags['ExifIFD:FocalLength']==50 and tags['ExifIFD:FNumber']==2.8 and tags['ExifIFD:LensModel']=='Super-Takumar 50mm f/1.4'
        assert tags['ExifIFD:ISO']==320 and tags['ExifIFD:DateTimeOriginal']=='2026:10:18 14:20:00'
        assert not any('GPS' in k or 'MakerNote' in k for k in tags)
        assert command('--status',id).stdout=='done\n'
        rows.append({'case':'native-ingest-'+clf.stem,'status':'passed','max_JPEG_RGB8_error':err})
    # A's selection is armed before a later B selection; complete in reverse order.
    select('classic-chrome-srgb','example-takumar-50','2.8');arm('frame-a')
    select('none','example-helios-58','unknown');arm('frame-b')
    enqueue('frame-b',source.name);enqueue('frame-a',source.name);run('frame-a');run('frame-b')
    ta=exiftool(root/'jobs/frame-a/result.jpg');tb=exiftool(root/'jobs/frame-b/result.jpg')
    assert ta['ExifIFD:FocalLength']==50 and ta['ExifIFD:FNumber']==2.8
    assert tb['ExifIFD:FocalLength']==58 and 'ExifIFD:FNumber' not in tb
    assert (root/'jobs/frame-a/look.bin').exists() and not (root/'jobs/frame-b/look.bin').exists()
    rows.append({'case':'arm-frozen-selection-reverse-completion-and-unknown-aperture','status':'passed'})
    for orientation in range(1,9):
        source2=jpeg('oriented-'+str(orientation)+'.JPG',orientation)
        select('none','none','unknown');id='orientation-'+str(orientation);arm(id);enqueue(id,source2.name);run(id)
        with Image.open(root/'jobs'/id/'result.jpg') as after:
            assert after.getexif()[274]==orientation and after.size==(128,96)
        tags=exiftool(root/'jobs'/id/'result.jpg');assert tags['ExifIFD:LensModel']=='Source Lens'
        rows.append({'case':id,'status':'passed'})
    full_jpeg=captures/'full.JPG'
    with Image.open(source) as original:
        image=Image.new('RGB',(7008,4672),(70,150,210));image.save(full_jpeg,quality=98,subsampling=0,exif=original.getexif());image.close()
    select();arm('full-jpeg');enqueue('full-jpeg',full_jpeg.name);run('full-jpeg')
    with Image.open(root/'jobs/full-jpeg/result.jpg') as result:assert result.size==(7008,4672)
    rows.append({'case':'full-resolution-32MP-JPEG-native-ingest-and-child-worker','status':'passed'})
    # Exercise rational bounds and sub-f/1 omission, including big-endian source TIFF.
    extra_lenses=work/'extreme-lens.json';extra_lenses.write_text(json.dumps({'schema':1,'lenses':[{'id':'extreme-lens','name':'Bounds test','make':'Synthetic','model':'5000 mm f0.5','focal_length_mm':5000,'max_aperture':.5}]}))
    original_root=root;root=work/'extreme-workspace'
    bundle(root,launcher=binary,jpeg=a.jpeg,hif=a.hif,luts=a.luts,lenses=extra_lenses,host_test=True)
    select('none','extreme-lens','0.8');arm('extreme');enqueue('extreme',source.name);run('extreme')
    tags=exiftool(root/'jobs/extreme/result.jpg');assert tags['ExifIFD:FocalLength']==5000 and tags['ExifIFD:FNumber']==.8
    assert 'ExifIFD:ApertureValue' not in tags and 'ExifIFD:MaxApertureValue' not in tags
    root=original_root;rows.append({'case':'TIFF-rational-upper-bound-and-sub-f1-aperture','status':'passed'})
    hif=captures/'synthetic.HIF'
    subprocess.run([str(a.fixture.resolve()),str(hif),'8','13','10','422','color'],check=True,capture_output=True)
    select('classic-chrome-srgb','example-helios-58','4');arm('hif');enqueue('hif',hif.name);run('hif')
    tags=exiftool(root/'jobs/hif/result.HIF');assert tags['QuickTime:BitDepthLuma']==10 and tags['QuickTime:BitDepthChroma']==10
    assert tags['ExifIFD:FocalLength']==58 and tags['ExifIFD:FNumber']==4
    subprocess.run(['ffmpeg','-v','error','-i',str(root/'jobs/hif/result.HIF'),'-frames:v','1','-f','null','-'],check=True,capture_output=True)
    rows.append({'case':'HIF-completed-ingest-to-real-child-worker-independent-decode','status':'passed'})
    # Invalid user controls cannot change the durable selection.
    selected=(root/'selection.bin').read_bytes()
    for args in [('missing','none','unknown'),('none','missing','unknown'),('none','none','2'),('none','example-helios-58','nan'),('none','example-helios-58','1.4'),('none','example-helios-58','999')]:
        command('--select',*args,ok=False);assert (root/'selection.bin').read_bytes()==selected
    rows.append({'case':'invalid-selection-aperture-cannot-change-state','status':'passed'})
    command('--enqueue-completed-srgb',captures,source.name,'not-armed',source_hash,ok=False)
    command('--arm','frame-a',ok=False);command('--enqueue-completed-srgb',captures,source.name,'frame-a',source_hash,ok=False)
    old=(root/'jobs/frame-a/result.jpg').read_bytes();command('--run','frame-a',ok=False);assert (root/'jobs/frame-a/result.jpg').read_bytes()==old
    rows.append({'case':'no-implicit-completion-duplicate-id-or-result-overwrite','status':'passed'})
    (captures/'link.JPG').symlink_to(source);(captures/'alias').symlink_to(captures,target_is_directory=True)
    for path in ('../one.JPG','/tmp/one.JPG','link.JPG','alias/one.JPG','one.ARW'):
        id='path-'+str(len(rows));arm(id);command('--enqueue-completed-srgb',captures,path,id,source_hash,ok=False)
        assert not list((root/'jobs'/id).glob('input.*'));rows.append({'case':'confined-source-'+path.replace('/','_'),'status':'rejected'})
    # Corrupt source metadata, color, progressive JPEG and HDR HIF are rejected before readiness.
    progressive=jpeg('progressive.JPG',progressive=True)
    adobe=jpeg('adobe.JPG');im=Image.open(adobe);ex=im.getexif();sub=ex.get_ifd(34665);sub[0xa001]=2;ex[34665]=sub;im.save(adobe,exif=ex,quality=98)
    for path in (progressive,adobe):
        id=path.stem.lower();arm(id);command('--enqueue-completed-srgb',captures,path.name,id,hashlib.sha256(path.read_bytes()).hexdigest(),ok=False)
        assert not (root/'jobs'/id/'job.bin').exists();command('--run',id,ok=False);rows.append({'case':'reject-'+id,'status':'passed'})
    arm('stale-name');command('--enqueue-completed-srgb',captures,source.name,'stale-name','0'*64,ok=False)
    assert not (root/'jobs/stale-name/job.bin').exists();command('--run','stale-name',ok=False)
    rows.append({'case':'producer-digest-rejects-stale-or-reused-filename','status':'passed'})
    # Binding changes are declined even if the new selection has a correct internal checksum.
    select();arm('binding');enqueue('binding',source.name)
    select('none','example-helios-58','unknown');(root/'jobs/binding/selection.bin').write_bytes((root/'selection.bin').read_bytes())
    command('--run','binding',ok=False);assert not (root/'jobs/binding/result.jpg').exists()
    rows.append({'case':'per-job-selection-binding','status':'passed'})
    # Host locks are real: no second control mutator or concurrent encoder.
    with (root/'lock').open('rb') as lock:
        fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB);command('--select','none','none','unknown',ok=False)
    with (root/'worker.lock').open('rb') as lock:
        fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB);select();arm('busy');enqueue('busy',source.name);command('--run','busy',ok=False)
        assert command('--status','busy').stdout=='ready\n'
    rows.append({'case':'real-control-and-worker-locks','status':'passed'})
    # Altered executable is rejected before any worker starts.
    old_binary=(root/'bin/jpeg-worker').read_bytes();(root/'bin/jpeg-worker').write_bytes(b'tampered')
    command('--run','busy',ok=False);assert command('--status','busy').stdout=='ready\n';(root/'bin/jpeg-worker').write_bytes(old_binary)
    rows.append({'case':'worker-hash-before-exec','status':'passed'})
    # Persisted running is deliberately not resumed after an unknown interruption.
    (root/'jobs/busy/state').write_bytes(b'running\n');command('--run','busy',ok=False)
    rows.append({'case':'interrupted-running-needs-manual-review','status':'passed'})
    if a.sample:
        full=captures/'full.HIF';shutil.copyfile(a.sample,full);full_hash=hashlib.sha256(full.read_bytes()).hexdigest()
        select();arm('full');enqueue('full',full.name);t=time.monotonic();run('full');elapsed=time.monotonic()-t
        tags=exiftool(root/'jobs/full/result.HIF');assert tags['QuickTime:BitDepthLuma']==10 and tags['ExifIFD:FocalLength']==50
        assert hashlib.sha256(full.read_bytes()).hexdigest()==full_hash
        rows.append({'case':'real-full-resolution-HIF-native-ingest-child-worker','status':'passed','host_seconds':elapsed})
        select();arm('cancel');enqueue('cancel',full.name)
        child=subprocess.Popen([str(binary),'--run',str(root),'cancel'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        deadline=time.monotonic()+20
        while not list((root/'jobs/cancel').glob('.hif-worker-*.tmp')):
            assert child.poll() is None
            if time.monotonic()>deadline:child.kill();raise AssertionError('Worker never started')
            time.sleep(.01)
        # Switching selection is allowed while the previous encoder owns its worker lease.
        select('none','example-helios-58','unknown');arm('while-busy');enqueue('while-busy',source.name);command('--run','while-busy',ok=False)
        child.send_signal(signal.SIGTERM);out,err=child.communicate(timeout=90)
        assert child.returncode and not (root/'jobs/cancel/result.HIF').exists() and not list((root/'jobs/cancel').glob('.hif-worker-*.tmp'))
        assert command('--status','cancel').stdout in ('running\n','needs-review\n');command('--run','cancel',ok=False)
        rows.append({'case':'concurrent-selector-backpressure-and-parent-to-child-cancellation','status':'passed'})
        # A real child inherits the worker lease across exec and parent SIGKILL.
        select();arm('orphan');enqueue('orphan',full.name)
        parent=subprocess.Popen([str(binary),'--run',str(root),'orphan'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        deadline=time.monotonic()+20;worker_pid=None
        while not list((root/'jobs/orphan').glob('.hif-worker-*.tmp')):
            assert parent.poll() is None
            if time.monotonic()>deadline:parent.kill();raise AssertionError('Orphan worker never started')
            time.sleep(.01)
        processes=subprocess.check_output(['ps','-axo','pid,ppid'],text=True)
        descendants=[int(line.split()[0]) for line in processes.splitlines()[1:] if len(line.split())==2 and int(line.split()[1])==parent.pid]
        assert len(descendants)==1,descendants;worker_pid=descendants[0]
        parent.kill();parent.wait(timeout=10)
        assert b'worker_busy' in command('--run','while-busy',ok=False).stderr.encode()
        select('none','none','unknown')
        os.kill(worker_pid,signal.SIGTERM)
        with (root/'worker.lock').open('rb') as lease:
            deadline=time.monotonic()+60
            while True:
                try:fcntl.flock(lease,fcntl.LOCK_EX|fcntl.LOCK_NB);break
                except BlockingIOError:
                    if time.monotonic()>deadline:raise AssertionError('Orphan lease not released')
                    time.sleep(.05)
        parent.communicate(timeout=10)
        assert command('--status','orphan').stdout=='running\n';command('--run','orphan',ok=False)
        assert not (root/'jobs/orphan/result.HIF').exists() and not list((root/'jobs/orphan').glob('.hif-worker-*.tmp'))
        rows.append({'case':'worker-lease-survives-parent-SIGKILL-and-orphan-cancel','status':'passed'})
    if a.fault_library:
        large=captures/'large.JPG';large.write_bytes(source.read_bytes()+bytes(1024*1024))
        for case in ('snapshot-enospc','source-changed','fork-failed','exec-failed'):
            select();arm(case)
            env=os.environ.copy();env['DYLD_INSERT_LIBRARIES']=str(a.fault_library.resolve());env['A7_LAUNCHER_TEST_FAULT']=case
            if case in ('fork-failed','exec-failed'):enqueue(case,source.name);args=['--run',str(root),case]
            else:args=['--enqueue-completed-srgb',str(root),str(captures),large.name,case,hashlib.sha256(large.read_bytes()).hexdigest()]
            r=subprocess.run([str(binary),*args],env=env,capture_output=True,text=True,timeout=60)
            assert r.returncode and not (root/'jobs'/case/'result.jpg').exists(),(case,r.stdout,r.stderr)
            assert 'AddressSanitizer' not in r.stderr and 'runtime error:' not in r.stderr,r.stderr
            assert command('--status',case).stdout in ('failed\n','needs-review\n')
            command('--run',case,ok=False);rows.append({'case':case,'status':'injected-failure-stops-without-output-or-retry','diagnostic':r.stderr.strip()})
    assert hashlib.sha256(source.read_bytes()).hexdigest()==source_hash
    result={'schema':1,'scope':'Native Mac launcher prepares jobs and execs actual JPEG/HIF workers; tests offline in synthetic local workspace',
        'camera_accessed':False,'camera_installation':False,'network_during_tests':False,'originals_unchanged':True,
        'cases':rows,'source_sha256':{n:hashlib.sha256((ROOT/n).read_bytes()).hexdigest() for n in ('camera_worker/launcher.cc','camera_worker/ingest_exif.h','camera_worker/launcher_bundle.py','camera_worker/heif_io.h','camera_worker/worker.c','research/check_photo_launcher.py')},
        'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),
        'limits':['Actual camera startup/menu/capture callback not implemented or executed','Explicit arm/closed-event contract tested; no inference of firmware completion','ARM64/Linux process semantics require separate target validation']}
    a.report.write_text(json.dumps(result,indent=2)+'\n');print('Launcher checks passed:',len(rows),flush=True)
if __name__=='__main__':main()
