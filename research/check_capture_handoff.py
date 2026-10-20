"""Offline owned-byte/close-gate replay and frozen frame-to-file fan-out.

Only generated local files under research/build are created/accessed as photos.
The caller supplies prebuilt local workers, a synthetic fixture generator and LUTs.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
import subprocess
from PIL import Image
from camera_worker.launcher_bundle import bundle
from research.check_jpeg_worker import exiftool

ROOT=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n in ('bridge','launcher','jpeg','hif','fixture','luts','work','report'):p.add_argument('--'+n,type=Path,required=True)
    a=p.parse_args();a.work=a.work.resolve()
    if not a.work.is_relative_to(ROOT/'research/build'):raise ValueError('Generated local build workspace required')
    for binary in [a.bridge,a.launcher]:
        doc=json.loads((binary.parent/'build.json').read_text())
        for n,value in doc['sources'].items():assert sha(ROOT/n)==value,('stale build',n)
        recorded=doc['artifacts'][binary.name]['sha256'] if 'artifacts' in doc else doc['binary']['sha256']
        assert sha(binary)==recorded,('changed binary',binary)
    a.work.mkdir(parents=True,exist_ok=False);fixtures=a.work/'fixtures';fixtures.mkdir();owned=a.work/'owned';owned.mkdir()
    rows=[]
    def call(args,ok=True):
        r=subprocess.run([str(x) for x in args],capture_output=True,text=True,timeout=180)
        assert (r.returncode==0)==ok,(args,r.stdout,r.stderr)
        assert 'AddressSanitizer' not in r.stderr and 'runtime error:' not in r.stderr,r.stderr
        return r
    image=Image.new('RGB',(256,192),(70,150,210));exif=Image.Exif();exif[271]='SONY';exif[272]='Synthetic offline source';exif[274]=6;exif[34665]={0xa001:1,0x8827:320}
    jpg=fixtures/'source.JPG';image.save(jpg,quality=98,subsampling=0,exif=exif)
    hif=fixtures/'source.HIF';call([a.fixture.resolve(),hif,'8','13','10','422','color'])
    original_hashes={f.name:sha(f) for f in [jpg,hif]}
    root=a.work/'workspace';manifest=bundle(root,launcher=a.launcher,jpeg=a.jpeg,hif=a.hif,luts=a.luts,
        lenses=ROOT/'photo_worker/lens-presets.example.json',host_test=True)
    launcher=a.launcher.resolve()
    def command(mode,*args,ok=True):return call([launcher,mode,root,*args],ok)
    command('--select','classic-chrome-srgb','example-helios-58','unknown')
    command('--arm-frame','frame-73');frozen=(root/'jobs/frame-73/selection.bin').read_bytes()
    generation=struct.unpack_from('<Q',frozen,32)[0]
    replay=json.loads(call([a.bridge.resolve(),'--owned-replay',jpg,hif,owned,generation]).stdout)
    assert replay['scratch_destroyed_before_completion'] and replay['padding_removed']
    assert [(r['group'],r['media']) for r in replay['photos']]==[(0,0),(1,3)]
    for row in replay['photos']:
        assert row['frame']==73 and row['token']==7 and row['generation']==generation
        path=owned/row['path'];source=hif if path.suffix=='.HIF' else jpg
        assert sha(path)==original_hashes[source.name] and path.stat().st_size==row['bytes']
    rows.append({'case':'source-bound-ledger-gates-to-owned-JPEG-HIF-bytes','status':'passed','replay':replay})
    # Discard the generated source names before worker ingress. Only owned bytes
    # survive; no source pathname is ever passed to the launcher after this point.
    jpg.unlink();hif.unlink();assert not jpg.exists() and not hif.exists()
    command('--select','none','example-takumar-50','2.8')
    assert (root/'selection.bin').read_bytes()!=frozen
    children=[]
    for group in range(2):
      for media in range(4):
        child=command('--derive-file','frame-73',group,media).stdout.strip()
        assert child==f'frame-73-g{group}-m{media}'
        assert (root/'jobs'/child/'selection.bin').read_bytes()==frozen
        command('--derive-file','frame-73',group,media,ok=False)
        children.append((child,group,media))
    rows.append({'case':'eight-file-fanout-preserves-frame-selection-after-global-switch','status':'passed','files':len(children)})
    for child,group,media in reversed(children):
        path=owned/('owned-hif.HIF' if group else 'owned-jpeg.JPG');expected=sha(path)
        command('--enqueue-completed-srgb',owned,path.name,child,expected)
        command('--run',child);assert command('--status',child).stdout=='done\n'
        output=root/'jobs'/child/('result.HIF' if group else 'result.jpg');tags=exiftool(output)
        assert tags['ExifIFD:FocalLength']==58 and 'ExifIFD:FNumber' not in tags
        assert (root/'jobs'/child/'look.bin').exists() and sha(path)==expected
        if group:
            assert tags['QuickTime:BitDepthLuma']==10 and tags['QuickTime:BitDepthChroma']==10
            call(['ffmpeg','-v','error','-i',output,'-frames:v','1','-f','null','-'])
        else:
            with Image.open(output) as result:assert result.size==(256,192) and result.getexif()[274]==6
            assert tags['ExifIFD:ISO']==320
        rows.append({'case':'owned-snapshot-to-'+('HIF10' if group else 'JPEG')+f'-media-{media}',
            'status':'passed','frozen_lens':'example-helios-58','actual_aperture':'unknown','output_sha256':sha(output)})
    command('--enqueue-completed-srgb',owned,'owned-jpeg.JPG','frame-73',sha(owned/'owned-jpeg.JPG'),ok=False)
    command('--run','frame-73',ok=False)
    for group,media in [('2','0'),('-1','0'),('00','0'),('0','4'),('0','-1'),('0','x')]:
        command('--derive-file','frame-73',group,media,ok=False)
    command('--arm','ordinary');command('--derive-file','ordinary','0','0',ok=False)
    command('--arm-frame','x'*58,ok=False)
    command('--arm-frame','x'*57);child=command('--derive-file','x'*57,'0','0').stdout.strip();assert len(child)==63
    command('--arm-frame','../bad',ok=False)
    command('--arm-frame','bad-selection');selection=root/'jobs/bad-selection/selection.bin';raw=bytearray(selection.read_bytes());raw[12]^=1;selection.write_bytes(raw)
    command('--derive-file','bad-selection','0','0',ok=False);assert not (root/'jobs/bad-selection-g0-m0').exists()
    rows.append({'case':'duplicates-nonframes-bounds-malformed-parent-and-parent-ingress-declined','status':'passed'})
    command('--arm','stale');call([launcher,'--enqueue-completed-srgb',root,owned,'owned-jpeg.JPG','stale','0'*64],ok=False)
    assert not (root/'jobs/stale/job.bin').exists()
    rows.append({'case':'incorrect-owned-snapshot-digest-never-ready','status':'passed'})
    names=['camera_worker/launcher.cc','camera_worker/sony_owned_image.h','camera_worker/tests/sony_owned_image_cases.h',
        'research/check_capture_handoff.py','research/trace_encoded_write.py']
    report={'schema':1,'camera_accessed':False,'ghub_accessed':False,'network_during_tests':False,'installed':False,
        'cases':rows,'artifacts':{n:sha(getattr(a,n)) for n in ['bridge','launcher','jpeg','hif','fixture']},
        'sources':{n:sha(ROOT/n) for n in names},'encoded_write_evidence_sha256':sha(ROOT/'research/evidence/encoded-write.json'),
        'limits':['Generated photos and synthetic Sony identity events; no runtime hook or readable-memory proof.',
                  'sRGB is an explicit fixture contract; HLG/PQ and full DCP reproduction are outside this test.',
                  'HEIF decode/encode is real on the host; ARM64 tests are separate, not camera execution.']}
    a.report.write_text(json.dumps(report,indent=2)+'\n');print(f'{len(rows)} offline owned-byte/fan-out integration groups passed')
if __name__=='__main__':main()
