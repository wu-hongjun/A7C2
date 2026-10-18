"""Offline, source-bound Adobe RGBTable conformance and JPEG integration checks.

Requires the explicitly supplied local DNG SDK zip, local profile directory and
converted CLFs. Compiles a host-only oracle; no camera, network or SDK install.
"""
from array import array
import argparse
from datetime import datetime, timezone
import hashlib
import importlib.metadata
import itertools
import json
from pathlib import Path
import platform
import random
import struct
import subprocess
import sys
import tempfile
from xml.etree import ElementTree as ET
import zipfile

from PIL import Image, ImageDraw
from photo_worker.adobe_table import decode_xmp, inflate_table, inspect_decoded_profiles
from photo_worker.clf import parse_clf, processor
from photo_worker.fujifilm import flatten_cube_for_diagnostics, clf_bytes, stage_processor
from photo_worker.profiles import CRS, RDF
from photo_worker.worker import EXIF_IFD, SRGB_CONTRACT, prepare, processor_for, rational, sha
from research.check_jpeg_worker import exiftool
from research.lut_reference import parse_cube

ROOT = Path(__file__).resolve().parents[1]
SDK_SHA = 'c1d3a545ed45b150e59af2e6aa0695c1551f0e2bed34ad87436898a05194d2b1'
REF_SHA = '3ec8fe2c9cfc76cc4d7b28270b21d670086aae3f0aa89a8ef3900f829eaa13e4'


def build_oracle(archive, work):
    with archive.open('rb') as stream:
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    if digest != SDK_SHA:
        raise ValueError('SDK archive differs from the reviewed local snapshot')
    names = ['dng_reference.cpp', 'dng_big_table.cpp', 'dng_big_table.h', 'dng_color_space.cpp',
             'dng_1d_table.cpp', 'dng_1d_table.h', 'dng_xy_coord.cpp', 'dng_xy_coord.h']
    with zipfile.ZipFile(archive) as z:
        files = {name: z.read('dng_sdk_1_7_1/dng_sdk/source/'+name) for name in names}
        license_bytes = z.read('dng_sdk_1_7_1/LICENSE.txt')
    if license_bytes != (ROOT/'licenses/Adobe-DNG-SDK.txt').read_bytes():
        raise ValueError('SDK license mismatch')
    source = files['dng_reference.cpp'].decode()
    start = source.index('void RefRGBtoRGBTable3D (')
    end = source.index('/*****************************************************************************/', start)
    function = source[start:end].strip()+'\n'
    if sha(function.encode()) != REF_SHA:
        raise ValueError('Original reference function extraction changed')
    notice = source[:source.index('#include')]
    (work/'adobe_ref.inc').write_text(notice+function)
    exe = work/'adobe-oracle'
    command = ['clang++', '-std=c++17', '-O2', '-ffp-contract=off', '-I', str(work),
               str(ROOT/'research/adobe_rgb_table_oracle.cpp'), '-o', str(exe)]
    subprocess.run(command, check=True, capture_output=True)
    return exe, {'archive_basename': archive.name, 'archive_sha256': digest,
        'identity': 'User-supplied 1.7.1 snapshot, not independently authenticated against Adobe distribution',
        'original_reference_function': 'RefRGBtoRGBTable3D', 'function_sha256': REF_SHA,
        'source_files': [{'file': k, 'sha256': sha(v)} for k, v in files.items()],
        'license_sha256': sha(license_bytes),
        'compiler': subprocess.check_output(['clang++', '--version'], text=True).splitlines()[0],
        'compiler_flags': ['-std=c++17', '-O2', '-ffp-contract=off'],
        'scope': 'Original SDK scalar function in a minimal host harness, not a full Adobe RAW renderer; wrappers implement the documented sRGB adaptation.'}


def vectors():
    rng = random.Random(6432)
    rows = [tuple(rng.random() for _ in range(3)) for _ in range(20000)]
    rows += list(itertools.product((x/16 for x in range(17)), repeat=3))
    rows += [(x/4096,)*3 for x in range(4097)]
    # Dense faces/edges and dark/toe-region vectors catch clipping and transfer errors.
    for fixed in (0., 1., 1/255, 254/255):
        for axis in range(3):
            for a, b in itertools.product(range(33), repeat=2):
                row = [a/32, b/32]; row.insert(axis, fixed); rows.append(tuple(row))
    rows += list(itertools.permutations((.0404498, .0404500, .0031308)))
    return array('f', (v for rgb in rows for v in rgb))


def native(exe, work, payload, inputs):
    (work/'table.bin').write_bytes(payload)
    (work/'vectors.bin').write_bytes(inputs.tobytes())
    subprocess.run([str(exe), str(work/'table.bin'), str(work/'vectors.bin'), str(work/'native.bin')], check=True)
    out = array('f'); out.frombytes((work/'native.bin').read_bytes())
    assert len(out) == len(inputs)
    return out


def stats(actual, expected):
    errors = sorted(abs(a-b)*255 for a, b in zip(actual, expected))
    assert len(actual) == len(expected) and errors
    return {'mean_RGB8_scale': sum(errors)/len(errors), 'p95_RGB8_scale': errors[int(.95*len(errors))],
        'p99_RGB8_scale': errors[int(.99*len(errors))], 'max_RGB8_scale': errors[-1],
        'max_rounded_RGB8_difference': max(abs(round(min(1,max(0,a))*255)-round(min(1,max(0,b))*255))
                                           for a, b in zip(actual, expected))}


def payload_for(raw, table):
    attrs = next(ET.fromstring(raw).iter(RDF+'Description')).attrib
    return inflate_table(attrs[CRS+'Table_'+table.fingerprint])


def synthetic(kind):
    words = []
    for r, g, b in itertools.product(range(32), repeat=3):
        v = (r/31, g/31, b/31)
        output = v if kind == 'identity' else (v[2], v[0], v[1]) if kind == 'permutation' else (v[0]**2, v[1]*v[2], v[2]**.5)
        words.extend((round(x*65535)-(k*65535+16)//31) & 65535 for x, k in zip(output, (r,g,b)))
    payload = struct.pack('<4I', 1,1,3,32)+struct.pack('<98304H', *words)+struct.pack('<III2d', 3,1,0,1.,1.)
    from photo_worker.tests.test_adobe_table import profile
    return payload, decode_xmp(profile(payload))


def jpeg_proof(exe, work, payload, clf, name, full=False):
    directory = work/name; directory.mkdir()
    source, output, job = (directory/n for n in ('synthetic.jpg', 'graded.jpg', 'job'))
    width, height = (7008, 4672) if full else (256, 256)
    image = Image.new('RGB', (width, height)); draw = ImageDraw.Draw(image); centers = []
    for y in range(8):
        for x in range(8):
            box = (x*width//8, y*height//8, (x+1)*width//8-1, (y+1)*height//8-1)
            color = (16+x*30, 16+y*30, 16+((x+3*y) % 8)*30)
            draw.rectangle(box, fill=color); centers.append(((box[0]+box[2])//2, (box[1]+box[3])//2))
    exif = Image.Exif(); exif[271] = 'Sony'; exif[272] = 'Synthetic ILCE-7CM2 test fixture'
    exif[EXIF_IFD] = {0x9003: '2026:10:18 12:34:56', 0x8827: 320, 0x829a: rational(1/125)}
    image.save(source, quality=100, subsampling=0, exif=exif)
    del draw, image
    original = sha(source.read_bytes())
    prepare(source, job, cube=clf, lut_contract=SRGB_CONTRACT,
            lens_presets=ROOT/'photo_worker/lens-presets.example.json', lens_id='example-helios-58',
            aperture=4, assume_srgb=True, completed=True)
    script = '''import json,resource,sys,time
from photo_worker.worker import run
t=time.monotonic();r=run(sys.argv[1],sys.argv[2]);r['elapsed_seconds']=time.monotonic()-t
r['peak_rss_raw']=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
r['peak_rss_units']='bytes' if sys.platform=='darwin' else 'KiB'
print(json.dumps(r))'''
    result = json.loads(subprocess.check_output([sys.executable, '-c', script, str(job), str(output)], cwd=ROOT))
    with Image.open(source) as before, Image.open(output) as after:
        values = array('f', (c/255 for p in centers for c in before.getpixel(p)))
        actual = array('f', (c/255 for p in centers for c in after.getpixel(p)))
        assert after.size == (width, height)
    expected = native(exe, work, payload, values)
    error = max(abs(a-round(min(1,max(0,b))*255)) for a, b in zip((x*255 for x in actual), expected))
    assert error <= 3, (name, error)
    assert sha(source.read_bytes()) == original
    tags = exiftool(output)
    assert tags['ExifIFD:LensModel'] == '44-2 58mm f/2' and tags['ExifIFD:FocalLength'] == 58
    assert tags['ExifIFD:FNumber'] == 4 and tags['ExifIFD:ISO'] == 320 and tags['IFD0:Orientation'] == 1
    assert tags['ExifIFD:DateTimeOriginal'] == '2026:10:18 12:34:56'
    assert not any(k.startswith(('GPS:', 'Sony:')) for k in tags)
    return {'look': name, 'dimensions': [width, height], 'flat_patch_centers': 64,
        'max_RGB8_error_including_JPEG_recompression': round(error), 'original_unchanged': True,
        'independent_ExifTool_capture_and_manual_lens_tags': 'passed',
        'elapsed_seconds': result['elapsed_seconds'], 'peak_rss_raw': result['peak_rss_raw'],
        'peak_rss_units': result['peak_rss_units'], 'camera_performance_measurement': False,
        'files': [{'path': str(p.relative_to(ROOT)), 'bytes': p.stat().st_size, 'sha256': sha(p.read_bytes())}
                  for p in (source, clf, output)]}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--sdk', type=Path, required=True)
    p.add_argument('--profiles', type=Path, required=True, help='Actual look XMP directory')
    p.add_argument('--luts', type=Path, required=True)
    p.add_argument('--full-resolution', action='store_true')
    args = p.parse_args()
    args.sdk = args.sdk.resolve(); args.profiles = args.profiles.resolve(); args.luts = args.luts.resolve()
    base = ROOT/'research/build'; base.mkdir(exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='fuji-validation-', dir=base))
    exe, sdk = build_oracle(args.sdk, work)
    values = vectors()
    evidence = {'schema': 1, 'generated_at_utc': datetime.now(timezone.utc).isoformat(),
        'camera_accessed': False, 'camera_installation': False, 'network_accessed': False,
        'SDK_reference': sdk, 'vectors_per_profile': len(values)//3,
        'vector_design': 'Seeded random holdouts, 17^3 grid, 4097 gray ramp, dense RGB faces/near-faces and sRGB toe boundaries',
        'acceptance_limits': {'max_RGB8_scale_error_vs_native_reference': .01,
            'max_CLF_serialization_RGB8_scale_error': .001, 'max_RGB8_error_including_JPEG': 3},
        'scope': 'sRGB JPEG adaptation of embedded RGBTable stage; not full DCP/Adobe RAW rendering or native Fujifilm/Sony matching.',
        'profiles': [], 'synthetic_reference_checks': [], 'JPEG_proofs': [], 'sources': []}
    for kind in ('identity', 'permutation', 'nonlinear'):
        payload, table = synthetic(kind)
        actual = array('f', values); stage_processor(table).applyRGB(actual)
        score = stats(actual, native(exe, work, payload, values))
        assert score['max_RGB8_scale'] <= .01, (kind, score)
        evidence['synthetic_reference_checks'].append({'fixture': kind, **score})
    manifest = json.loads((args.luts/'manifest.json').read_text())
    expected_names = set()
    for record in inspect_decoded_profiles(args.profiles)['profiles']:
        source = args.profiles/record['file']; raw = source.read_bytes(); table = decode_xmp(raw)
        name = source.stem; slug = name.lower().replace(' ', '-')+'-srgb.clf'; expected_names.add(slug)
        clf = args.luts/slug; clf_raw = clf.read_bytes()
        assert clf_raw == clf_bytes(table), 'Regenerate stale or modified LUT: '+slug
        rows = [r for r in manifest['profiles'] if r['file'] == slug]
        assert len(rows) == 1 and rows[0]['sha256'] == sha(clf_raw)
        payload = payload_for(raw, table); expected = native(exe, work, payload, values)
        direct = array('f', values); stage_processor(table).applyRGB(direct)
        loaded = array('f', values); processor(parse_clf(clf_raw)).applyRGB(loaded)
        score = stats(loaded, expected); serial = stats(loaded, direct)
        assert score['max_RGB8_scale'] <= .01, (name, score)
        assert serial['max_RGB8_scale'] <= .001, (name, serial)
        # Deliberately retain the failed flattening experiment, rather than label it validated.
        flat = array('f', values); processor_for(parse_cube(flatten_cube_for_diagnostics(table, name).decode())).applyRGB(flat)
        flattening = stats(flat, expected)
        evidence['profiles'].append({'name': name, 'source': record, 'CLF_file': slug,
            'CLF_sha256': sha(clf_raw), 'CLF_bytes': len(clf_raw), 'validated': True,
            'vs_original_SDK_function': score, 'CLF_serialization_error': serial,
            '33_cube_flattening_diagnostic': {**flattening, 'accepted_for_delivery': False}})
        evidence['JPEG_proofs'].append(jpeg_proof(exe, work, payload, clf, name.lower().replace(' ', '-')))
        if args.full_resolution and name == 'Classic Chrome':
            evidence['full_resolution_JPEG_proof'] = jpeg_proof(exe, work, payload, clf, 'classic-chrome-full', True)
        print('Validated '+name, flush=True)
    assert len(expected_names) == 11 and {r['file'] for r in manifest['profiles']} == expected_names
    for path in sorted((ROOT/'photo_worker').rglob('*')):
        if path.is_file() and '__pycache__' not in path.parts:
            evidence['sources'].append({'path': str(path.relative_to(ROOT)), 'sha256': sha(path.read_bytes())})
    for name in ('research/check_fujifilm_conversion.py', 'research/adobe_rgb_table_oracle.cpp',
                 'licenses/Adobe-DNG-SDK.txt', 'NOTICE.md'):
        evidence['sources'].append({'path': name, 'sha256': sha((ROOT/name).read_bytes())})
    evidence['passed'] = True
    (ROOT/'research/evidence/fujifilm-srgb-checks.json').write_text(json.dumps(evidence, indent=2)+'\n')
    (ROOT/'research/evidence/fujifilm-decoded-tables.json').write_text(json.dumps(inspect_decoded_profiles(args.profiles), indent=2)+'\n')
    print(json.dumps({'passed': True, 'profiles': 11, 'vectors_per_profile': len(values)//3,
                      'evidence': 'research/evidence/fujifilm-srgb-checks.json'}, indent=2))


if __name__ == '__main__': main()
