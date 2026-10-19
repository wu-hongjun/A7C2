"""Offline native-worker checks: sanitizer faults, LUT conformance, ELF execution.

All jobs are synthetic, private host files. Actual ARM64 instructions run in
Unicorn with a limited syscall model, never on the camera.
"""
from array import array
import argparse
from datetime import datetime, timezone
import hashlib
import io
import json
import os
from pathlib import Path
import shutil
import signal
import struct
import subprocess
import sys
import tempfile
import time

from elftools.elf.elffile import ELFFile
from PIL import Image, ImageDraw, ImageCms
from camera_worker.pack import pack_job
from photo_worker.clf import parse_clf, processor
from photo_worker.worker import EXIF_IFD, SRGB_CONTRACT, prepare, rational, sha
from research.check_fujifilm_conversion import vectors, stats
from research.check_jpeg_worker import exiftool
from research.emulate_camera_worker import execute

ROOT=Path(__file__).resolve().parents[1]


def call(binary,mode,job):
    result=subprocess.run([str(binary),mode,str(job)],capture_output=True,text=True,timeout=60)
    assert 'AddressSanitizer' not in result.stderr and 'runtime error:' not in result.stderr,result.stderr
    return result


def rehash(job,asset):
    head=bytearray((job/'job.bin').read_bytes());raw=(job/asset).read_bytes()
    offset={'input.jpg':48,'metadata.exif':80,'look.bin':112}[asset]
    head[offset:offset+32]=hashlib.sha256(raw).digest()
    if asset=='input.jpg':struct.pack_into('<Q',head,40,len(raw))
    head[144:]=hashlib.sha256(head[:144]).digest();(job/'job.bin').write_bytes(head)


def make_job(root,source,lut=None,lens='example-helios-58',aperture=4,normalize=False):
    root.mkdir();host=root/'host';native=root/'native'
    prepare(source,host,cube=lut,lut_contract=SRGB_CONTRACT if lut else None,
            lens_presets=ROOT/'photo_worker/lens-presets.example.json',lens_id=lens,
            aperture=aperture,completed=True,assume_srgb=True)
    pack_job(host,native,normalize=normalize);return native


def chart(path,width=256,height=256,**save):
    image=Image.new('RGB',(width,height));draw=ImageDraw.Draw(image);points=[]
    for y in range(8):
        for x in range(8):
            box=(x*width//8,y*height//8,(x+1)*width//8-1,(y+1)*height//8-1)
            draw.rectangle(box,fill=(16+x*30,16+y*30,16+((x+3*y)%8)*30))
            points.append(((box[0]+box[2])//2,(box[1]+box[3])//2))
    exif=Image.Exif();exif[271]='Sony';exif[272]='Synthetic ILCE-7CM2 fixture'
    exif[EXIF_IFD]={0x9003:'2026:10:18 12:34:56',0x8827:320,0x829a:rational(1/125)}
    image.save(path,quality=100,subsampling=0,exif=exif,**save)
    return points


def image_check(job,points,reference,lens='44-2 58mm f/2',aperture=4):
    with Image.open(job/'input.jpg') as before,Image.open(job/'result.jpg') as after:
        assert before.size==after.size
        expected=array('f',(c/255 for p in points for c in before.getpixel(p)))
        if reference:processor(parse_clf(reference.read_bytes())).applyRGB(expected)
        actual=[c for p in points for c in after.getpixel(p)]
        error=max(abs(a-int(min(1,max(0,b))*255+.5)) for a,b in zip(actual,expected))
        assert error<=3,error
        assert ImageCms.getProfileDescription(ImageCms.ImageCmsProfile(io.BytesIO(after.info['icc_profile']))).strip()=='sRGB built-in'
    tags=exiftool(job/'result.jpg')
    assert tags['ExifIFD:LensModel']==lens and tags['IFD0:Orientation']==1
    assert tags['ExifIFD:ISO']==320 and tags['ExifIFD:DateTimeOriginal']=='2026:10:18 12:34:56'
    assert tags['ExifIFD:ColorSpace']==1 and tags['ExifIFD:ExifImageWidth']>0
    if aperture is None:assert 'ExifIFD:FNumber' not in tags
    else:assert tags['ExifIFD:FNumber']==aperture
    assert not any(k.startswith(('GPS:','Sony:')) for k in tags)
    return {'sampled_flat_patch_centers':len(points),'max_RGB8_error_including_JPEG':error,
            'independent_ExifTool_capture_and_lens_tags':'passed','output_has_sRGB_ICC':True}


def fault_checks(binary,template,work):
    records=[]
    def check(name,edit,mode='--run'):
        job=work/name;shutil.copytree(template,job);edit(job)
        source=sha((job/'input.jpg').read_bytes()) if not (job/'input.jpg').is_symlink() else None
        result=call(binary,mode,job)
        assert result.returncode!=0,(name,result.stdout)
        assert not (job/'result.jpg').exists(),name
        assert not list(job.glob('.jpeg-worker-*.tmp')),name
        if source:assert sha((job/'input.jpg').read_bytes())==source
        records.append({'name':name,'exit_code':result.returncode,'diagnostic':result.stderr.strip()})
    check('bad-header-hash',lambda p:(p/'job.bin').write_bytes((p/'job.bin').read_bytes()[:-1]+b'x'))
    check('bad-input-hash',lambda p:(p/'input.jpg').write_bytes((p/'input.jpg').read_bytes()+b'x'))
    check('bad-lut-hash',lambda p:(p/'look.bin').write_bytes((p/'look.bin').read_bytes()[:-4]+bytes(4)))
    def truncated(p):
        (p/'input.jpg').write_bytes((p/'input.jpg').read_bytes()[:-12]);rehash(p,'input.jpg')
    check('truncated-entropy',truncated)
    def trailing(p):
        with (p/'input.jpg').open('ab') as stream:stream.write(b'not another image')
        rehash(p,'input.jpg')
    check('trailing-image-bytes',trailing)
    def nan(p):
        raw=bytearray((p/'look.bin').read_bytes());struct.pack_into('<f',raw,16,float('nan'))
        (p/'look.bin').write_bytes(raw);rehash(p,'look.bin')
    check('nonfinite-lut',nan)
    def exif(p):
        raw=bytearray((p/'metadata.exif').read_bytes());raw[10:14]=bytes([255])*4
        (p/'metadata.exif').write_bytes(raw);rehash(p,'metadata.exif')
    check('malformed-exif-offset',exif)
    def bounds(p):
        raw=bytearray((p/'job.bin').read_bytes());struct.pack_into('<I',raw,20,90000)
        raw[144:]=hashlib.sha256(raw[:144]).digest();(p/'job.bin').write_bytes(raw)
    check('oversized-dimensions',bounds)
    def progressive(p):
        with Image.open(p/'input.jpg') as im:im.save(p/'progressive.jpg',progressive=True)
        (p/'input.jpg').write_bytes((p/'progressive.jpg').read_bytes());rehash(p,'input.jpg')
    check('progressive-jpeg',progressive)
    def grayscale(p):
        Image.new('L',(256,256),128).save(p/'input.jpg');rehash(p,'input.jpg')
    check('grayscale-jpeg',grayscale)
    def symlink(p):
        (p/'input.jpg').rename(p/'actual.jpg');(p/'input.jpg').symlink_to('actual.jpg')
    check('symlink-input',symlink)
    def output_link(p):(p/'result.jpg').symlink_to('nonexistent')
    check('symlink-output',output_link)
    # Existing regular output must retain its exact bytes.
    p=work/'existing-output';shutil.copytree(template,p);(p/'result.jpg').write_bytes(b'keep this output')
    result=call(binary,'--run',p);assert result.returncode!=0 and (p/'result.jpg').read_bytes()==b'keep this output'
    records.append({'name':'existing-output-preserved','exit_code':result.returncode,'diagnostic':result.stderr.strip()})
    # Both processes may precheck before publication; only one may publish.
    p=work/'publication-race';shutil.copytree(template,p)
    children=[subprocess.Popen([str(binary),'--run',str(p)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True) for _ in range(2)]
    messages=[c.communicate(timeout=60) for c in children]
    assert sorted(c.returncode for c in children)==[0,1],messages
    with Image.open(p/'result.jpg') as im:im.load()
    assert not list(p.glob('.jpeg-worker-*.tmp'))
    records.append({'name':'concurrent-publication-one-winner','exit_codes':[c.returncode for c in children]})
    return records


def elf_check(path):
    with path.open('rb') as stream:
        elf=ELFFile(stream);segments=list(elf.iter_segments())
        assert elf['e_machine']=='EM_AARCH64' and elf['e_type']=='ET_EXEC' and elf.little_endian and elf.elfclass==64
        assert not any(s['p_type'] in ('PT_INTERP','PT_DYNAMIC') for s in segments)
        assert all(not(s['p_flags']&1 and s['p_flags']&2) for s in segments if s['p_type']=='PT_LOAD')
        assert any(s['p_type']=='PT_GNU_STACK' and not s['p_flags']&1 for s in segments)
        return {'machine':elf['e_machine'],'type':elf['e_type'],'ELF_bits':64,'little_endian':True,
            'interpreter':None,'dynamic_dependencies':[],'writable_executable_load_segments':False,
            'executable_stack':False,'bytes':path.stat().st_size,'sha256':sha(path.read_bytes())}


def main():
    p=argparse.ArgumentParser(description=__doc__)
    for name in ('linux-build','host-build','asan-build','luts'):p.add_argument('--'+name,type=Path,required=True)
    p.add_argument('--full-resolution',action='store_true');a=p.parse_args()
    linux=(a.linux_build/'jpeg-worker-aarch64').resolve();host=(a.host_build/'jpeg-worker-host').resolve();asan=(a.asan_build/'jpeg-worker-host').resolve()
    a.luts=a.luts.resolve();work=Path(tempfile.mkdtemp(prefix='camera-worker-check-',dir=ROOT/'research/build'))
    input=work/'chart.jpg';points=chart(input);source_sha=sha(input.read_bytes())
    result={'schema':1,'generated_at_utc':datetime.now(timezone.utc).isoformat(),'camera_accessed':False,
        'camera_installation':False,'network_accessed':False,'ELF':elf_check(linux),'LUT_checks':[],
        'JPEG_checks':[],'emulation':[],'sources':[],'limitations':['Hardware untested; Unicorn syscall models are not the camera kernel/filesystem.',
        'Host prepares immutable jobs and presets; no on-camera job producer, shutter hook, menu or media registration.',
        'Native JPEG runner preserves EXIF orientations 1-8; legacy host packer still normalizes non-1 input explicitly. Sequential 8-bit sRGB RGB/YCbCr only.',
        'Linux renameat2(RENAME_NOREPLACE) and directory fsync need actual target-filesystem verification before installation.']}
    builds=[]
    for directory in (a.linux_build,a.host_build,a.asan_build):
        record=json.loads((directory/'build.json').read_text());builds.append(record)
        for source in record['worker_sources']:
            assert sha((ROOT/source['path']).read_bytes())==source['sha256'],source
    result['builds']=builds
    baseline_path=ROOT/'research/evidence/fujifilm-srgb-checks.json'
    baseline=json.loads(baseline_path.read_text());assert baseline['passed']
    known_CLFs={r['CLF_file']:r['CLF_sha256'] for r in baseline['profiles']}
    result['reference_provenance']={'receipt':str(baseline_path.relative_to(ROOT)),'sha256':sha(baseline_path.read_bytes()),
        'scope':'Native implementation compared with the exact previously SDK-validated CLF files; not full RAW development.'}
    firmware=json.loads((ROOT/'research/evidence/firmware-structure.json').read_text())
    kernel=firmware['kernel'];assert sha((ROOT/kernel['source']['path']).read_bytes())==kernel['source']['sha256']
    libc=ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha7_unpacked/lib/libc-2.25.so'
    assert sha(libc.read_bytes())=='f9104526eb7e5d2ea533c1a209bb0b83e69367f569e4c966895c8e106e6382c9'
    result['offline_target_ABI']={'kernel':kernel['source'],'kernel_banner':kernel['banner'][0]['text'],
        'libc':{'path':str(libc.relative_to(ROOT)),'sha256':sha(libc.read_bytes()),'version':'2.25'},
        'strategy':'Standalone static musl executable avoids the firmware glibc loader/symbol namespace; ARMv8-A generic scalar codec.'}
    values=vectors();template=None
    for clf in sorted(a.luts.glob('*.clf')):
        assert sha(clf.read_bytes())==known_CLFs[clf.name],clf.name
        name=clf.stem;job=make_job(work/name,input,clf);template=template or job
        (job/'vectors.in').write_bytes(values.tobytes());r=call(asan,'--vectors',job);assert r.returncode==0,r.stderr
        actual=array('f');actual.frombytes((job/'vectors.out').read_bytes());reference=array('f',values)
        processor(parse_clf(clf.read_bytes())).applyRGB(reference)
        score=stats(actual,reference);assert score['max_RGB8_scale']<.01,(name,score)
        result['LUT_checks'].append({'name':name,'source_CLF_sha256':sha(clf.read_bytes()),'RGB_vectors':len(values)//3,'packed_lut_sha256':sha((job/'look.bin').read_bytes()),**score})
        r=call(asan,'--run',job);assert r.returncode==0,r.stderr
        result['JPEG_checks'].append({'name':name,**image_check(job,points,clf)})
        print('Native scalar/sanitizer JPEG passed: '+name,flush=True)
    assert len(result['LUT_checks'])==11
    # Use a clean template so error cases must not leave or replace a result.
    template=make_job(work/'fault-template',input,a.luts/'classic-chrome-srgb.clf')
    result['fault_checks']=fault_checks(asan,template,work)
    # Manual lens-only selection and unknown aperture are also native jobs.
    lensjob=make_job(work/'lens-only',input,None,lens='example-takumar-50',aperture=None)
    r=call(asan,'--run',lensjob);assert r.returncode==0,r.stderr
    result['lens_only']=image_check(lensjob,points,None,lens='Super-Takumar 50mm f/1.4',aperture=None)
    # Deliberate progressive/orientation/ICC input needs explicit host normalization.
    tagged=work/'tagged-progressive.jpg'
    chart(tagged,progressive=True,icc_profile=ImageCms.ImageCmsProfile(ImageCms.createProfile('sRGB')).tobytes())
    rejected=work/'normalization-required';rejected.mkdir()
    prepare(tagged,rejected/'host',completed=True,assume_srgb=False)
    try:pack_job(rejected/'host',rejected/'native')
    except ValueError:pass
    else:raise AssertionError('Normalization must be explicit')
    pack_job(rejected/'host',rejected/'native',normalize=True)
    r=call(asan,'--run',rejected/'native');assert r.returncode==0,r.stderr
    result['normalization_gate']='explicit host opt-in required, normalized progressive/ICC input passes'
    # Original ELF: tiny image + failure + sampled color vectors. No firmware code loaded.
    result['emulation'].append(execute(linux))
    elfjob=make_job(work/'elf-jpeg',input,a.luts/'classic-chrome-srgb.clf')
    em=execute(linux,elfjob,'--run');assert em['exit_code']==0,em
    result['emulation'].append(em);result['ELF_JPEG']=image_check(elfjob,points,a.luts/'classic-chrome-srgb.clf')
    for fault in ('output_ENOSPC','publication_unsupported','directory_sync'):
        faultjob=make_job(work/('elf-'+fault),input,a.luts/'classic-chrome-srgb.clf')
        em=execute(linux,faultjob,'--run',fault=fault);assert em['exit_code']==1,em
        assert not list(faultjob.glob('.jpeg-worker-*.tmp'))
        if fault=='directory_sync':
            assert json.loads(em['stderr'])['published'] is True
            image_check(faultjob,points,a.luts/'classic-chrome-srgb.clf')
        else:assert not (faultjob/'result.jpg').exists()
        result['emulation'].append(em)
    failjob=make_job(work/'elf-failure',input,a.luts/'classic-chrome-srgb.clf')
    (failjob/'input.jpg').write_bytes(b'bad');em=execute(linux,failjob,'--run');assert em['exit_code']==1,em
    assert not (failjob/'result.jpg').exists();result['emulation'].append(em)
    vectorjob=make_job(work/'elf-vectors',input,a.luts/'classic-chrome-srgb.clf')
    vv=array('f',values[:1024*3]);(vectorjob/'vectors.in').write_bytes(vv.tobytes())
    em=execute(linux,vectorjob,'--vectors');assert em['exit_code']==0,em;result['emulation'].append(em)
    av=array('f');av.frombytes((vectorjob/'vectors.out').read_bytes());processor(parse_clf((a.luts/'classic-chrome-srgb.clf').read_bytes())).applyRGB(vv)
    score=stats(av,vv);assert score['max_RGB8_scale']<.01;result['ELF_vectors']={'RGB_vectors':1024,**score}
    if a.full_resolution:
        source=work/'full.jpg';full_points=chart(source,7008,4672);job=make_job(work/'full-native',source,a.luts/'classic-chrome-srgb.clf')
        script='''import json,resource,subprocess,sys,time
t=time.monotonic();r=subprocess.run(sys.argv[1:],capture_output=True,text=True,timeout=120)
print(json.dumps({'returncode':r.returncode,'stdout':r.stdout,'stderr':r.stderr,'seconds':time.monotonic()-t,'peak_RSS_raw':resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss,'RSS_units':'bytes' if sys.platform=='darwin' else 'KiB'}))'''
        perf=json.loads(subprocess.check_output([sys.executable,'-c',script,str(host),'--run',str(job)],text=True))
        assert perf['returncode']==0,perf
        result['full_resolution']={'dimensions':[7008,4672],**image_check(job,full_points,a.luts/'classic-chrome-srgb.clf'),
            'performance':perf,'host_performance_only':True,'native_RGB_row_bytes':7008*3,'files':[
                {'path':str(p.relative_to(ROOT)),'bytes':p.stat().st_size,'sha256':sha(p.read_bytes())} for p in (job/'input.jpg',job/'result.jpg')]}
        print('Full-resolution native JPEG passed',flush=True)
        # Interrupt a process we own while its private output is in progress.
        interrupted=make_job(work/'interrupt',source,a.luts/'classic-chrome-srgb.clf')
        child=subprocess.Popen([str(host),'--run',str(interrupted)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
        deadline=time.monotonic()+10
        while not list(interrupted.glob('.jpeg-worker-*.tmp')):
            assert child.poll() is None,'Worker finished before interruption fixture'
            if time.monotonic()>deadline:child.kill();raise AssertionError('No temporary output before deadline')
            time.sleep(.005)
        child.send_signal(signal.SIGTERM);out,err=child.communicate(timeout=15)
        assert child.returncode==1 and 'cancelled' in err,(out,err)
        assert not (interrupted/'result.jpg').exists() and not list(interrupted.glob('.jpeg-worker-*.tmp'))
        result['interruption']={'owned_child_SIGTERM':'handled','published':False,'temporary_removed':True}
    assert sha(input.read_bytes())==source_sha
    result['originals_unchanged']=True;result['passed']=True
    for p in sorted((ROOT/'camera_worker').rglob('*')):
        if p.is_file() and '__pycache__' not in p.parts:result['sources'].append({'path':str(p.relative_to(ROOT)),'sha256':sha(p.read_bytes())})
    for name in ('research/check_camera_worker.py','research/emulate_camera_worker.py','photo_worker/clf.py','photo_worker/worker.py','native/sha256.h'):
        result['sources'].append({'path':name,'sha256':sha((ROOT/name).read_bytes())})
    (ROOT/'research/evidence/camera-worker-checks.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'passed':True,'looks':11,'vectors_per_look':len(values)//3,'fault_cases':len(result['fault_checks']),'evidence':'research/evidence/camera-worker-checks.json'},indent=2))


if __name__=='__main__':main()
