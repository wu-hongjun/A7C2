"""Synthetic host tests only; no camera, mounted card, personal photo or network."""
import copy
import json
from pathlib import Path
import random
import subprocess
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from PIL import Image, ImageCms

from photo_worker import worker as w
from photo_worker.profiles import dcp_info, xmp_info
from research.lut_reference import parse_cube

ROOT = Path(__file__).resolve().parents[2]


def cube_text(n=17, fn=lambda r, g, b: (b, r, g), lo=(0, 0, 0), hi=(1, 1, 1)):
    return (f'LUT_3D_SIZE {n}\nDOMAIN_MIN '+ ' '.join(map(str, lo))+'\nDOMAIN_MAX '+
            ' '.join(map(str, hi))+'\n'+'\n'.join(' '.join(map(str, fn(r/(n-1), g/(n-1), b/(n-1))))
              for b in range(n) for g in range(n) for r in range(n))+'\n')


class WorkerTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.source, self.cube, self.presets = [self.root/n for n in ('input.jpg', 'look.cube', 'lenses.json')]
        self.job, self.output = self.root/'job', self.root/'output.jpg'
        self.cube.write_text(cube_text())
        self.lens = {'id': 'helios', 'name': 'Helios', 'make': 'Helios', 'model': '44-2 58mm f/2',
                     'focal_length_mm': 58, 'max_aperture': 2}
        self.write_presets()
        exif = Image.Exif()
        exif[271] = 'Sony'; exif[272] = 'ILCE-7CM2'; exif[274] = 6
        exif[w.EXIF_IFD] = {0x9003: '2026:10:18 12:34:56', 0x829a: w.rational(1/125),
            0x8827: 320, 0x829d: w.rational(8), 0x9202: w.rational(6), 0x9205: w.rational(4),
            0x920a: w.rational(35), 0xa405: 35, 0xa434: 'Old lens', 0x927c: b'old-makernote',
            0xa431: 'private-body-serial'}
        exif[0x8825] = {1: 'N', 2: (w.rational(40), w.rational(0), w.rational(0))}
        Image.new('RGB', (96, 64), (32, 96, 192)).save(self.source, quality=100, subsampling=0, exif=exif)

    def write_presets(self):
        self.presets.write_text(json.dumps({'schema': 1, 'lenses': [self.lens]}))

    def prepare(self, **overrides):
        kwargs = dict(cube=self.cube, lut_contract=w.SRGB_CONTRACT, lens_presets=self.presets,
                      lens_id='helios', assume_srgb=True, completed=True)
        kwargs.update(overrides)
        return w.prepare(self.source, self.job, **kwargs)

    def test_copy_grade_orientation_exif_and_original_preservation(self):
        hashes = [w.sha(p.read_bytes()) for p in (self.source, self.cube, self.presets)]
        self.prepare(aperture=4)
        report = w.run(self.job, self.output)
        self.assertEqual(report['dimensions'], [64, 96])
        self.assertEqual(hashes, [w.sha(p.read_bytes()) for p in (self.source, self.cube, self.presets)])
        with Image.open(self.output) as out:
            pixel = out.getpixel((20, 20))
            for a, b in zip(pixel, (192, 32, 96)):
                self.assertLessEqual(abs(a-b), 3)
            exif = out.getexif(); p = exif.get_ifd(w.EXIF_IFD)
            self.assertEqual(exif[274], 1); self.assertEqual(exif[272], 'ILCE-7CM2')
            self.assertEqual(p[0x9003], '2026:10:18 12:34:56')
            self.assertAlmostEqual(float(p[0x829a]), 1/125)
            self.assertEqual(p[0x8827], 320)
            self.assertEqual(p[0xa434], self.lens['model']); self.assertEqual(p[0xa433], 'Helios')
            self.assertEqual(float(p[0x920a]), 58); self.assertEqual(float(p[0x829d]), 4)
            self.assertEqual(float(p[0x9205]), 2); self.assertEqual(float(p[0x9202]), 4)
            self.assertEqual(tuple(map(float, p[0xa432])), (58, 58, 2, 2))
            self.assertEqual((p[0xa002], p[0xa003]), out.size)
            self.assertNotIn(0xa405, p); self.assertNotIn(0x927c, p); self.assertNotIn(0xa431, p)
            self.assertNotIn(0x8825, exif); self.assertTrue(out.info['icc_profile'])

    def test_no_invented_aperture_metadata_only(self):
        self.prepare(cube=None, lut_contract=None)
        report = w.run(self.job, self.output)
        self.assertFalse(report['lut_applied'])
        with Image.open(self.output) as out:
            p = out.getexif().get_ifd(w.EXIF_IFD)
            self.assertNotIn(0x829d, p); self.assertNotIn(0x9202, p)
            self.assertEqual(float(p[0x9205]), 2)

    def test_lut_only_retains_original_lens_and_aperture(self):
        self.prepare(lens_presets=None, lens_id=None)
        w.run(self.job, self.output)
        with Image.open(self.output) as out:
            p = out.getexif().get_ifd(w.EXIF_IFD)
            self.assertEqual(p[0xa434], 'Old lens'); self.assertEqual(float(p[0x829d]), 8)

    def test_fast_backend_matches_scalar_reference_both_sizes_domains(self):
        rng = random.Random(8314)
        for n in (17, 33):
            for lo, hi in [((0, 0, 0), (1, 1, 1)), ((-.2, .1, .3), (.7, 1.2, 2))]:
                cube = parse_cube(cube_text(n, lambda r, g, b: (r*r+.3*g*b, g*r-.2, b*b*b), lo, hi))
                colors = [(0, 0, 0), (255, 255, 255), (255, 0, 0)]
                colors += [tuple(rng.randrange(256) for _ in range(3)) for _ in range(253)]
                image = Image.new('RGB', (16, 16)); image.putdata(colors)
                out = w.grade(image, cube, strip_rows=3)
                for actual, rgb in zip(out.get_flattened_data(), colors):
                    expected = cube.apply_rgb8(rgb)
                    self.assertLessEqual(max(abs(a-b) for a, b in zip(actual, expected)), 1)

    def test_snapshot_survives_later_input_look_and_preset_switch(self):
        self.prepare()
        self.lens.update(model='Changed', focal_length_mm=50); self.write_presets()
        self.source.write_bytes(b'changed source after preparation')
        self.cube.write_text('changed look after preparation')
        w.run(self.job, self.output)
        with Image.open(self.output) as out:
            self.assertEqual(out.getexif().get_ifd(w.EXIF_IFD)[0xa434], '44-2 58mm f/2')

    def test_snapshot_and_manifest_tampering_rejected(self):
        self.prepare()
        raw = (self.job/'input.jpg').read_bytes()
        (self.job/'input.jpg').write_bytes(raw+b'changed')
        with self.assertRaisesRegex(ValueError, 'hash mismatch'):
            w.run(self.job, self.output)
        (self.job/'input.jpg').write_bytes(raw)
        p = self.job/'job.json'; doc = json.loads(p.read_text()); doc['payload']['quality'] = 5
        p.write_text(json.dumps(doc))
        with self.assertRaisesRegex(ValueError, 'manifest hash'):
            w.run(self.job, self.output)
        self.assertFalse(self.output.exists())

    def test_job_paths_and_types_rejected_even_with_rehashed_manifest(self):
        self.prepare()
        p = self.job/'job.json'; original = json.loads(p.read_text())
        for edit in (lambda x: x['assets'].update({'../escape': x['assets']['input.jpg']}),
                     lambda x: x.update(quality=True), lambda x: x.update(completed_asserted=False)):
            doc = copy.deepcopy(original); edit(doc['payload']); doc['sha256'] = w.sha(w.canonical(doc['payload']))
            p.write_text(json.dumps(doc))
            with self.assertRaises(ValueError): w.run(self.job, self.output)

    def test_completion_contract_untagged_and_wrong_format_rejected(self):
        for kwargs in ({'completed': False}, {'assume_srgb': False}, {'lut_contract': None}):
            with self.assertRaises(ValueError): self.prepare(**kwargs)
            self.assertFalse(self.job.exists())
        wrong = self.root/'adobe.xmp'; wrong.write_text('<xmp/>')
        with self.assertRaisesRegex(ValueError, 'Adobe RAW'):
            self.prepare(cube=wrong)
        self.source.write_bytes(self.source.read_bytes()[:-100])
        with self.assertRaises(OSError): self.prepare()

    def test_lens_schema_validation_and_explicit_aperture(self):
        original = copy.deepcopy(self.lens)
        for key, val in [('id', '../escape'), ('max_aperture', float('nan')), ('focal_length_mm', True),
                         ('focal_length_mm', 10**1000), ('model', 'a\0b'), ('make', '非ASCII')]:
            lens = dict(original); lens[key] = val
            with self.assertRaises(ValueError): w.validate_lens(lens)
        for aperture in (1.4, float('nan'), True):
            with self.assertRaises(ValueError): self.prepare(aperture=aperture)
        self.presets.write_text('{"schema":1,"schema":1,"lenses":[]}')
        with self.assertRaisesRegex(ValueError, 'Duplicate JSON'): w.load_lenses(self.presets)

    def test_sub_f1_lens_omits_unsigned_negative_apex(self):
        self.lens['max_aperture'] = .95; self.write_presets()
        self.prepare(aperture=.95); w.run(self.job, self.output)
        with Image.open(self.output) as out:
            p = out.getexif().get_ifd(w.EXIF_IFD)
            self.assertAlmostEqual(float(p[0x829d]), .95)
            self.assertNotIn(0x9205, p); self.assertNotIn(0x9202, p)

    def test_existing_outputs_job_names_symlinks_and_retry(self):
        self.prepare()
        with self.assertRaises(FileExistsError): self.prepare()
        self.output.write_bytes(b'existing')
        with self.assertRaises(ValueError): w.run(self.job, self.output)
        self.assertEqual(self.output.read_bytes(), b'existing')
        self.output.unlink(); self.output.symlink_to(self.root/'missing')
        with self.assertRaises(ValueError): w.run(self.job, self.output)
        self.output.unlink(); w.run(self.job, self.output)
        with self.assertRaises(ValueError): w.run(self.job, self.output)
        self.assertEqual(list(self.root.glob('.jpeg-worker-*')), [])

    def test_low_disk_partial_write_and_publish_race_leave_originals(self):
        self.prepare(); before = w.sha(self.source.read_bytes())
        with patch.object(w.shutil, 'disk_usage', return_value=SimpleNamespace(free=0)):
            with self.assertRaisesRegex(ValueError, 'space'): w.run(self.job, self.output)
        def fail_save(image, f, **kwargs):
            f.write(b'partial'); raise OSError('synthetic ENOSPC')
        with patch.object(Image.Image, 'save', fail_save):
            with self.assertRaises(OSError): w.run(self.job, self.output)
        self.assertFalse(self.output.exists()); self.assertEqual(list(self.root.glob('.jpeg-worker-*')), [])
        real_link = w.os.link
        def race(src, dst):
            Path(dst).write_bytes(b'concurrent existing output'); real_link(src, dst)
        with patch.object(w.os, 'link', race):
            with self.assertRaises(FileExistsError): w.run(self.job, self.output)
        self.assertEqual(self.output.read_bytes(), b'concurrent existing output')
        self.assertEqual(before, w.sha(self.source.read_bytes()))
        self.assertEqual(list(self.root.glob('.jpeg-worker-*')), [])

    def test_icc_valid_invalid_and_pixel_bound(self):
        with Image.open(self.source) as im:
            im.save(self.source, icc_profile=ImageCms.ImageCmsProfile(ImageCms.createProfile('sRGB')).tobytes())
        self.prepare(assume_srgb=False); w.run(self.job, self.output)
        shutil = __import__('shutil'); shutil.rmtree(self.job); self.output.unlink()
        with Image.open(self.source) as im: im.save(self.source, icc_profile=b'invalid ICC')
        self.prepare()
        with self.assertRaisesRegex(ValueError, 'ICC'): w.run(self.job, self.output)
        with patch.object(w, 'MAX_IMAGE_PIXELS', 1):
            with self.assertRaises(ValueError): w.jpeg_info(self.source.read_bytes())

    def test_cli_lens_listing_and_prepare_run(self):
        def cli(*args):
            return subprocess.run([sys.executable, '-m', 'photo_worker', *map(str, args)],
                                  cwd=ROOT, capture_output=True, text=True)
        listed = cli('list-lenses', self.presets)
        self.assertEqual(listed.returncode, 0, listed.stderr)
        self.assertEqual(json.loads(listed.stdout)['lenses'][0]['id'], 'helios')
        prepared = cli('prepare', self.source, self.job, '--lens-presets', self.presets,
                       '--lens', 'helios', '--completed', '--assume-srgb')
        self.assertEqual(prepared.returncode, 0, prepared.stderr)
        done = cli('run', self.job, self.output)
        self.assertEqual(done.returncode, 0, done.stderr)
        self.assertTrue(json.loads(done.stdout)['host_only'])

    def test_adobe_profiles_rejected_as_jpeg_luts_and_xmp_bounds(self):
        raw = b'''<x xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:crs="http://ns.adobe.com/camera-raw-settings/1.0/"><rdf:Description crs:SupportsOutputReferred="False" crs:Table_ABC="encoded"/></x>'''
        summary = xmp_info(raw)
        self.assertFalse(summary['jpeg_worker_supported'])
        self.assertEqual(summary['embedded_tables'][0]['encoded_characters'], 7)
        for bad in (b'<!DOCTYPE x><x/>', b'invalid', b'x'*1048577):
            with self.assertRaises(ValueError): xmp_info(bad)
        for bad in (b'', b'II*\0\xff\xff\xff\xff'):
            with self.assertRaises(ValueError): dcp_info(bad)


if __name__ == '__main__': unittest.main()
