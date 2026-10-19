"""Offline HIF regression/conformance tests. No transport, card writes or install."""
from array import array
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import signal
import struct
import subprocess
import sys
import time

from camera_worker.heif_pack import pack
from photo_worker.clf import parse_clf, processor
from research.check_fujifilm_conversion import vectors, stats
from research.check_jpeg_worker import exiftool
from elftools.elf.elffile import ELFFile

ROOT=Path(__file__).resolve().parents[1]


def command(args, **kwargs):
    return subprocess.run([str(x) for x in args],capture_output=True,timeout=180,**kwargs)


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    for name in ('binary','fixture','luts','work','elf'):ap.add_argument('--'+name,type=Path,required=True)
    ap.add_argument('--sample',type=Path);ap.add_argument('--report',type=Path,required=True)
    a=ap.parse_args();a.work.mkdir(parents=True,exist_ok=False)
    binary=a.binary.resolve();fixture=a.fixture.resolve();work=a.work.resolve();records=[]
    def call(mode,path):return command([binary,mode,path])
    def job(source,name,clf=None,aperture=2.8):
        p=work/name;pack(source,p,inspector=binary,completed=True,clf=clf,
            lens_presets=ROOT/'photo_worker/lens-presets.example.json',lens_id='example-takumar-50',aperture=aperture)
        return p
    def inspect(path):
        r=call('--inspect',path);assert r.returncode==0,r.stderr;return json.loads(r.stdout)
    def dump(path):
        r=command([fixture,'dump',path]);assert r.returncode==0,r.stderr
        values=array('H');values.frombytes(r.stdout);return values
    def generate(name,orient=1,transfer=13,chroma='422',mode='color'):
        path=work/(name+'.HIF');r=command([fixture,path,orient,transfer,10,chroma,mode]);assert r.returncode==0,r.stderr;return path
    baseline=json.loads((ROOT/'research/evidence/fujifilm-srgb-checks.json').read_text());assert baseline['passed']
    known={r['CLF_file']:r['CLF_sha256'] for r in baseline['profiles']}
    color=generate('color');gray=generate('gray',mode='gray')
    original=dump(color);inputs=vectors();look_records=[]
    for clf in sorted(a.luts.glob('*.clf')):
        assert hashlib.sha256(clf.read_bytes()).hexdigest()==known[clf.name],clf.name
        p=job(color,clf.stem,clf);r=call('--run',p);assert r.returncode==0,r.stderr
        info=inspect(p/'result.HIF');assert info['bits']==10 and info['nclx']==[1,13,6,1]
        expected=array('f',inputs);processor(parse_clf(clf.read_bytes())).applyRGB(expected)
        r=command([binary,'--vectors',p],input=inputs.tobytes());assert r.returncode==0,r.stderr
        actual=array('f');actual.frombytes(r.stdout);numeric=stats(actual,expected);assert numeric['max_RGB8_scale']<.01,numeric
        expected=array('f',(v/1023 for v in original));processor(parse_clf(clf.read_bytes())).applyRGB(expected)
        actual=dump(p/'result.HIF');errors=sorted(abs(v-min(1,max(0,f))*1023) for v,f in zip(actual,expected))
        assert len(actual)==len(expected)
        codec={'mean_RGB10_error':sum(errors)/len(errors),'p99_RGB10_error':errors[int(.99*len(errors))],'max_RGB10_error':errors[-1]}
        assert codec['mean_RGB10_error']<15 and codec['p99_RGB10_error']<80,codec
        tags=exiftool(p/'result.HIF');assert tags['ExifIFD:LensModel']=='Super-Takumar 50mm f/1.4' and tags['ExifIFD:FNumber']==2.8
        assert tags['QuickTime:BitDepthLuma']==10 and tags['QuickTime:BitDepthChroma']==10
        look_records.append({'look':clf.stem,'CLF_sha256':known[clf.name],'packed_look_sha256':hashlib.sha256((p/'look.bin').read_bytes()).hexdigest(),'vectors':len(inputs)//3,'numeric_OCIO':numeric,'codec_roundtrip':codec})
        print('Passed '+clf.stem,flush=True)
    assert len(look_records)==11
    for orientation in range(1,9):
        source=generate('orientation-'+str(orientation),orient=orientation,chroma='420' if orientation%2 else '422')
        p=job(source,'oriented-'+str(orientation),aperture=None);r=call('--run',p);assert r.returncode==0,r.stderr
        before,after=inspect(source),inspect(p/'result.HIF')
        assert before['orientation']==after['orientation']==orientation
        assert [before[x] for x in ('width','height')]==[after[x] for x in ('width','height')]
        tags=exiftool(p/'result.HIF');assert 'ExifIFD:FNumber' not in tags
        assert tags['ExifIFD:ExifImageWidth']==after['width'] and tags['ExifIFD:ExifImageHeight']==after['height']
        before_pixels=dump(source);after_pixels=dump(p/'result.HIF')
        assert len(before_pixels)==len(after_pixels)
        mean=sum(abs(x-y) for x,y in zip(before_pixels,after_pixels))/len(before_pixels);assert mean<15,mean
        independent=[]
        for path in (source,p/'result.HIF'):
            result=command(['ffmpeg','-v','error','-i',path,'-frames:v','1','-pix_fmt','rgb48le','-f','rawvideo','pipe:1'])
            assert result.returncode==0 and len(result.stdout)==1024*128*6,result.stderr
            pixels=array('H');pixels.frombytes(result.stdout);independent.append(pixels)
        display_mean=sum(abs(x-y)/64 for x,y in zip(*independent))/len(independent[0]);assert display_mean<15,display_mean
        records.append({'raw_pixel_mean_RGB10_error':mean,'independent_transformed_pixel_mean_RGB10_error':display_mean,'orientation':orientation,'input_chroma':'420' if orientation%2 else '422','status':'passed'})
    p=job(gray,'gray-identity');r=call('--run',p);assert r.returncode==0,r.stderr
    levels=len(set(dump(p/'result.HIF')[::3]));assert levels>900,levels
    # Independent decoder: no linked libheif/libde265 worker code is reused here.
    r=command(['ffmpeg','-v','error','-i',p/'result.HIF','-frames:v','1','-pix_fmt','rgb48le','-f','rawvideo','pipe:1'])
    assert r.returncode==0 and len(r.stdout)==1024*128*6,r.stderr
    independent=array('H');independent.frombytes(r.stdout);assert len(set(independent[::3]))>900
    template=work/look_records[0]['look'];faults=[]
    def fault(name,mutate):
        p=work/('fault-'+name);shutil.copytree(template,p,ignore=shutil.ignore_patterns('result.HIF'));mutate(p)
        r=call('--run',p);assert r.returncode and not (p/'result.HIF').exists(),(name,r.stdout,r.stderr)
        assert not list(p.glob('.hif-worker-*.tmp')),name
        faults.append({'case':name,'status':'rejected_without_publication','diagnostic':r.stderr.decode().strip()})
    def rehash(p,name):
        h=bytearray((p/'job.bin').read_bytes());raw=(p/name).read_bytes();off={'input.HIF':48,'metadata.exif':80,'look.bin':112}[name]
        h[off:off+32]=hashlib.sha256(raw).digest()
        if name=='input.HIF':struct.pack_into('<Q',h,40,len(raw))
        h[144:]=hashlib.sha256(h[:144]).digest();(p/'job.bin').write_bytes(h)
    fault('header-hash',lambda p:(p/'job.bin').write_bytes(b'X'*176))
    fault('input-hash',lambda p:(p/'input.HIF').write_bytes(b'X'*(p/'input.HIF').stat().st_size))
    fault('look-hash',lambda p:(p/'look.bin').write_bytes(b'X'*(p/'look.bin').stat().st_size))
    def broken(p):
        (p/'input.HIF').write_bytes(b'not a HEIF');rehash(p,'input.HIF')
    fault('malformed-container',broken)
    def symlink(p):
        (p/'input.HIF').unlink();(p/'input.HIF').symlink_to(color)
    fault('symlink-input',symlink)
    def wrong_magic(p):
        h=bytearray((p/'job.bin').read_bytes());h[:8]=b'A7JOBS01';h[144:]=hashlib.sha256(h[:144]).digest();(p/'job.bin').write_bytes(h)
    fault('jpeg-job',wrong_magic)
    def nonfinite(p):
        raw=bytearray((p/'look.bin').read_bytes());struct.pack_into('<f',raw,16,float('nan'));(p/'look.bin').write_bytes(raw);rehash(p,'look.bin')
    fault('nonfinite-lut',nonfinite)
    for name in ('existing-output','symlink-output'):
        p=work/('fault-'+name);shutil.copytree(template,p,ignore=shutil.ignore_patterns('result.HIF'))
        target=p/'result.HIF'
        if name=='existing-output':target.write_bytes(b'preserve')
        else:target.symlink_to(color)
        r=call('--run',p);assert r.returncode
        assert target.read_bytes()==(b'preserve' if name=='existing-output' else color.read_bytes())
        faults.append({'case':name,'status':'preserved'})
    for transfer in (16,18):
        source=generate('hdr-'+str(transfer),transfer=transfer);r=call('--inspect',source);assert r.returncode and b'requires_sRGB_SDR' in r.stderr
        faults.append({'case':'reject-transfer-'+str(transfer),'status':'rejected'})
    # Codec-independent header mutations verify guards before pixel conversion.
    for case,offset,value in (('non-sRGB-transfer',6,1),('non-709-primaries',4,9),('unsupported-matrix',8,9)):
        raw=bytearray(color.read_bytes());i=raw.index(b'nclx');struct.pack_into('>H',raw,i+offset,value)
        path=work/(case+'.HIF');path.write_bytes(raw);r=call('--inspect',path);assert r.returncode,r.stdout
        faults.append({'case':case,'status':'rejected'})
    raw=bytearray(color.read_bytes());pos=0
    while True:
        i=raw.find(b'hvcC',pos)
        if i<0:break
        raw[i+4+17]&=0xf8;raw[i+4+18]&=0xf8;pos=i+4
    pos=0
    while True:
        i=raw.find(b'pixi',pos)
        if i<0:break
        count=raw[i+8];raw[i+9:i+9+count]=bytes([8])*count;pos=i+4
    path=work/'declared-8bit.HIF';path.write_bytes(raw);r=call('--inspect',path)
    assert r.returncode and b'requires_10_bit' in r.stderr,r.stderr
    faults.append({'case':'declared-8bit','status':'rejected'})
    full=None
    if a.sample:
        sample_hash=hashlib.sha256(a.sample.read_bytes()).hexdigest();p=job(a.sample,'private-full-resolution',a.luts/'classic-chrome-srgb.clf')
        # getrusage is process-local; time supplies this child's peak RSS in bytes on macOS.
        t=time.monotonic();r=command(['/usr/bin/time','-l',binary,'--run',p]);elapsed=time.monotonic()-t
        assert r.returncode==0,r.stderr
        rss=int(next(line.split()[0] for line in r.stderr.decode().splitlines() if 'maximum resident set size' in line))
        assert rss<384*1024*1024,rss
        before,after=inspect(a.sample),inspect(p/'result.HIF');assert before['bits']==after['bits']==10
        assert (before['width'],before['height'],before['orientation'])==(after['width'],after['height'],after['orientation'])
        tags=exiftool(p/'result.HIF');assert tags['ExifIFD:FocalLength']==50 and tags['ExifIFD:FNumber']==2.8
        # Decode the entire 32MP result with independent FFmpeg, discard pixels.
        r=command(['ffmpeg','-v','error','-i',p/'result.HIF','-frames:v','1','-f','null','-']);assert r.returncode==0,r.stderr
        assert hashlib.sha256(a.sample.read_bytes()).hexdigest()==sample_hash
        full={'input':before,'output':after,'host_seconds':elapsed,'host_peak_RSS_bytes':rss,'input_unchanged':True,
            'independent_full_FFmpeg_decode':'passed','ExifTool_lens_and_depth':'passed','private_photo_not_published':True}
        p=job(a.sample,'private-interrupt',a.luts/'classic-chrome-srgb.clf');child=subprocess.Popen([str(binary),'--run',str(p)],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        deadline=time.monotonic()+20
        while not list(p.glob('.hif-worker-*.tmp')):
            assert child.poll() is None
            if time.monotonic()>deadline:child.kill();raise AssertionError('No temporary output')
            time.sleep(.01)
        child.send_signal(signal.SIGTERM);out,err=child.communicate(timeout=60)
        assert child.returncode and not (p/'result.HIF').exists() and not list(p.glob('.hif-worker-*.tmp'))
        faults.append({'case':'SIGTERM-after-output-reservation','status':'cancelled_cleanly'})
    with a.elf.open('rb') as f:
        elf=ELFFile(f);assert elf.header['e_machine']=='EM_AARCH64'
        assert not any(s.header.p_type in ('PT_INTERP','PT_DYNAMIC') for s in elf.iter_segments())
        stacks=[s for s in elf.iter_segments() if s.header.p_type=='PT_GNU_STACK'];assert stacks and not stacks[0].header.p_flags&1
    report={'schema':1,'scope':'offline Mac HEVC execution and structural static ARM64 validation; no camera execution',
        'sample_copied_read_only':bool(a.sample),'camera_transport_or_installation':False,'network_during_tests':False,
        'sources':{name:hashlib.sha256((ROOT/name).read_bytes()).hexdigest() for name in ('camera_worker/worker.c','camera_worker/heif_worker.cc','camera_worker/heif_core.h','camera_worker/heif_pack.py','camera_worker/build_heif.py','camera_worker/tests/heif_fixture.cc','research/check_heif_worker.py')},
        'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'target_sha256':hashlib.sha256(a.elf.read_bytes()).hexdigest(),
        'looks':look_records,'geometry_and_unknown_aperture':records,'grayscale_output_levels_of_1024':levels,
        'independent_FFmpeg_RGB16_levels':len(set(independent[::3])),'faults':faults,'full_resolution':full,
        'ARM64_validation':'ELF64 AArch64, static, non-executable stack. Full HEVC path not executed on Linux/camera.',
        'limits':'sRGB SDR 10-bit YCbCr 420/422 only; crop/ICC/HDR unsupported. Lossy re-encode, host-selected presets; no shutter hook/playback claim.'}
    a.report.write_text(json.dumps(report,indent=2)+'\n');print('HIF checks passed',flush=True)


if __name__=='__main__':main()
