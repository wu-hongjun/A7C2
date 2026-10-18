"""Host-only worker checks and optional 7008x4672 synthetic JPEG proof.

Uses local Python/Pillow/OCIO and the vendored ExifTool as an independent reader.
No device enumeration, camera/card access, installation or network operations.
"""
from pathlib import Path
import argparse
import importlib.metadata
import json
import platform
import subprocess
import sys
import tempfile
import unittest

from PIL import Image, ImageDraw
from photo_worker.worker import EXIF_IFD, SRGB_CONTRACT, prepare, rational, sha
from photo_worker.profiles import inspect_pack
from photo_worker.tests.test_worker import cube_text
from research.lut_reference import parse_cube

ROOT = Path(__file__).resolve().parents[1]


def exiftool(path):
    command = ['perl', str(ROOT/'research/upstream/exiftool/exiftool'), '-j', '-G1', '-n', str(path)]
    return json.loads(subprocess.check_output(command))[0]


def full_resolution():
    build = ROOT/'research/build/jpeg-worker'
    build.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='proof-', dir=build))
    source, look, output, job = [work/name for name in ('synthetic-input.jpg', 'channel-permutation.cube', 'graded-lens-copy.jpg', 'job')]
    width, height = 7008, 4672
    image = Image.new('RGB', (width, height))
    draw = ImageDraw.Draw(image)
    points = []
    for y in range(8):
        for x in range(8):
            box = (x*width//8, y*height//8, (x+1)*width//8-1, (y+1)*height//8-1)
            color = (16+x*30, 16+y*30, 16+((x+3*y) % 8)*30)
            draw.rectangle(box, fill=color)
            points.append(((box[0]+box[2])//2, (box[1]+box[3])//2))
    exif = Image.Exif(); exif[271] = 'Sony'; exif[272] = 'Synthetic ILCE-7CM2 test fixture'
    exif[EXIF_IFD] = {0x9003: '2026:10:18 12:34:56', 0x8827: 320, 0x829a: rational(1/125)}
    image.save(source, quality=100, subsampling=0, exif=exif)
    del draw, image
    look.write_text(cube_text())
    original_hash = sha(source.read_bytes())
    prepare(source, job, cube=look, lut_contract=SRGB_CONTRACT,
            lens_presets=ROOT/'photo_worker/lens-presets.example.json', lens_id='example-helios-58',
            aperture=4, assume_srgb=True, completed=True)
    # Fresh host child measures this worker rather than the parent test harness.
    program = '''import json,resource,sys,time
from photo_worker.worker import run
t=time.monotonic();r=run(sys.argv[1],sys.argv[2]);r['elapsed_seconds']=time.monotonic()-t
r['peak_rss_raw']=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
r['peak_rss_units']='bytes' if sys.platform=='darwin' else 'KiB'
print(json.dumps(r))'''
    result = json.loads(subprocess.check_output([sys.executable, '-c', program, str(job), str(output)], cwd=ROOT))
    assert sha(source.read_bytes()) == original_hash
    cube = parse_cube(look.read_text()); error = 0
    with Image.open(source) as before, Image.open(output) as after:
        assert after.size == (width, height)
        for point in points:
            expected = cube.apply_rgb8(before.getpixel(point)); actual = after.getpixel(point)
            error = max(error, max(abs(a-b) for a, b in zip(expected, actual)))
        assert error <= 3, error
    independent = exiftool(output)
    assert independent['ExifIFD:LensModel'] == '44-2 58mm f/2'
    assert independent['ExifIFD:LensMake'] == 'Helios'
    assert independent['ExifIFD:FocalLength'] == 58 and independent['ExifIFD:FNumber'] == 4
    assert independent['ExifIFD:LensInfo'] == '58 58 2 2'
    assert independent['ExifIFD:ISO'] == 320
    assert independent['IFD0:Orientation'] == 1
    assert not any(key.startswith(('GPS:', 'Sony:')) for key in independent)
    keep = ['ExifIFD:LensModel', 'ExifIFD:LensMake', 'ExifIFD:FocalLength', 'ExifIFD:FNumber',
            'ExifIFD:MaxApertureValue', 'ExifIFD:LensInfo', 'ExifIFD:ISO', 'ExifIFD:DateTimeOriginal',
            'ExifIFD:ExposureTime', 'IFD0:Orientation']
    return {'dimensions': [width, height], 'sampled_flat_patch_centers': len(points),
            'max_RGB8_error_including_JPEG_recompression': error,
            'source_unchanged': True, 'independent_reader': 'pinned vendored ExifTool via local Perl',
            'independently_read_EXIF': {k: independent[k] for k in keep},
            'elapsed_seconds': result['elapsed_seconds'], 'peak_rss_raw': result['peak_rss_raw'],
            'peak_rss_units': result['peak_rss_units'], 'camera_measurement': False,
            'fixture_directory': str(work.relative_to(ROOT)),
            'fixture_files': [{'path': str(p.relative_to(ROOT)), 'bytes': p.stat().st_size,
                               'sha256': sha(p.read_bytes())} for p in (source, look, output)]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--full-resolution', action='store_true')
    parser.add_argument('--profiles', type=Path)
    args = parser.parse_args()
    suite = unittest.defaultTestLoader.discover(str(ROOT/'photo_worker/tests'))
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    if not result.wasSuccessful():
        return 1
    evidence = {'schema': 1, 'host_only': True, 'camera_accessed': False, 'network_accessed': False,
                'camera_installation': False, 'worker_tests': result.testsRun,
                'failures': len(result.failures), 'errors': len(result.errors), 'skipped': len(result.skipped),
                'fast_backend_vs_scalar_RGB8_samples': 1024,
                'limitations': 'Host prototype; Pillow retains full images. No camera ABI/build, native menu, shutter hook, streaming JPEG codec or Adobe RAW renderer.',
                'sources': []}
    evidence['environment'] = {'python': platform.python_version(), 'platform': platform.platform(),
                               'machine': platform.machine(),
                               'Pillow': importlib.metadata.version('Pillow'),
                               'OpenColorIO': importlib.metadata.version('opencolorio')}
    reader = ROOT/'research/upstream/exiftool/exiftool'
    evidence['independent_reader_source'] = {'path': str(reader.relative_to(ROOT)), 'sha256': sha(reader.read_bytes())}
    if args.full_resolution:
        evidence['full_resolution_proof'] = full_resolution()
    if args.profiles:
        pack = inspect_pack(args.profiles)
        (ROOT/'research/evidence/fujifilm-profile-pack.json').write_text(json.dumps(pack, indent=2)+'\n')
        dcp = args.profiles/pack['a7c2_linear_dcp']['path']
        decoded = exiftool(dcp)
        assert decoded['IFD0:ProfileName'] == 'Adobe Standard Linear'
        assert decoded['IFD0:UniqueCameraModel'] == 'Sony ILCE-7CM2'
        assert decoded['IFD0:ProfileToneCurve'] == '0 0 1 1'
        profiles = pack['xmp_profiles_and_wrappers']
        evidence['Fujifilm_pack'] = {'inventory': 'research/evidence/fujifilm-profile-pack.json',
            'files_hashed': len(pack['files']), 'XMP_profiles': sum(x.get('PresetType') == 'Look' for x in profiles),
            'XMP_wrappers': sum(x.get('PresetType') == 'Normal' for x in profiles),
            'DCP_summary_independently_crosschecked': True, 'original_XMPs_directly_JPEG_compatible': 0,
            'converted_RGBTable_sRGB_CLF_validation': 'research/evidence/fujifilm-srgb-checks.json',
            'source_profile_bytes_republished': False}
    for path in sorted((ROOT/'photo_worker').rglob('*')):
        if path.is_file() and '__pycache__' not in path.parts:
            evidence['sources'].append({'path': str(path.relative_to(ROOT)), 'sha256': sha(path.read_bytes())})
    evidence['sources'].append({'path': 'research/check_jpeg_worker.py', 'sha256': sha(Path(__file__).read_bytes())})
    (ROOT/'research/evidence/jpeg-worker-checks.json').write_text(json.dumps(evidence, indent=2)+'\n')
    print(json.dumps({'worker_tests': result.testsRun, 'passed': True,
                      'full_resolution': evidence.get('full_resolution_proof'),
                      'evidence': 'research/evidence/jpeg-worker-checks.json'}, indent=2))
    return 0


if __name__ == '__main__': sys.exit(main())
