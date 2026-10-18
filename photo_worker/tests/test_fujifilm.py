"""Synthetic checks; no supplied profile assets, SDK or camera required."""
from array import array
import json
from pathlib import Path
import tempfile
import unittest

from PIL import Image

from photo_worker.adobe_table import decode_xmp
from photo_worker.clf import parse_clf, processor
from photo_worker.fujifilm import clf_bytes, convert_directory, stage_processor
from photo_worker.tests.test_adobe_table import profile
from photo_worker import worker as w


class FujifilmTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.table = decode_xmp(profile())  # All zero deltas mean an identity RGB table.
        cls.raw = clf_bytes(cls.table)

    def test_delta_prediction_is_identity_and_wraps_uint16(self):
        samples = self.table.samples()
        self.assertEqual(samples[0], (0, 0, 0))
        self.assertEqual(samples[1], (0, 0, 2114))
        self.assertEqual(samples[32], (0, 2114, 0))
        self.assertEqual(samples[1024], (2114, 0, 0))
        self.assertEqual(samples[-1], (65535, 65535, 65535))
        from dataclasses import replace
        words = list(self.table.sample_words); words[-1] = 65535
        self.assertEqual(replace(self.table, sample_words=tuple(words)).samples()[-1][-1], 65534)

    def test_identity_srgb_and_serialized_pipeline(self):
        inputs = array('f', (x/255 for x in range(256) for _ in range(3)))
        direct, loaded = array('f', inputs), array('f', inputs)
        stage_processor(self.table).applyRGB(direct)
        processor(parse_clf(self.raw)).applyRGB(loaded)
        self.assertLess(max(abs(a-b)*255 for a, b in zip(direct, inputs)), .02)
        self.assertLess(max(abs(a-b)*255 for a, b in zip(direct, loaded)), .001)
        self.assertEqual(clf_bytes(self.table), self.raw)  # Deterministic artifact identity.

    def test_clf_rejects_resources_schema_nan_dimensions_and_entities(self):
        bad = [self.raw.replace(b'<LUT3D', b'<Reference path="external.clf"/><LUT3D', 1),
               self.raw.replace(b'dim="32 32 32 3"', b'dim="9999999 9999999 9999999 3"'),
               self.raw.replace(b'interpolation="tetrahedral"', b'interpolation="linear"'),
               self.raw.replace(b'  0.000113378', b'  NaN', 1),
               self.raw.replace(b'<ProcessList', b'<!DOCTYPE x [<!ENTITY y "x">]><ProcessList', 1),
               self.raw.replace(b'compCLFversion="3"', b'compCLFversion="2"'),
               self.raw.replace(b'<Array dim="3 3">', b'<Array dim="3 3" path="remote">', 1),
               b'\0'+self.raw, self.raw[:100], b'x'*(8*1024*1024+1)]
        for raw in bad:
            with self.subTest(prefix=raw[:100]):
                with self.assertRaises(ValueError): parse_clf(raw)

    def test_conversion_preserves_sources_and_refuses_overwrite(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td); source = root/'xmp'; source.mkdir()
            (source/'Identity.xmp').write_bytes(profile())
            result = convert_directory(source, root/'converted')
            self.assertEqual(result['native_grid'], 32)
            self.assertEqual((source/'Identity.xmp').read_bytes(), profile())
            raw = (root/'converted/identity-srgb.clf').read_bytes()
            self.assertEqual(w.sha(raw), result['profiles'][0]['sha256'])
            parse_clf(raw)
            with self.assertRaises(FileExistsError): convert_directory(source, root/'converted')
            with self.assertRaises(ValueError): convert_directory(root/'empty', root/'unused')

    def test_clf_worker_snapshot_metadata_and_tamper_rejection(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td); source = root/'input.jpg'; lut = root/'identity.clf'
            Image.new('RGB', (48, 32), (80, 120, 200)).save(source, quality=100, subsampling=0)
            original = source.read_bytes(); lut.write_bytes(self.raw)
            w.prepare(source, root/'job', cube=lut, lut_contract=w.SRGB_CONTRACT,
                      lens_presets=Path(__file__).parents[1]/'lens-presets.example.json',
                      lens_id='example-helios-58', aperture=4, completed=True, assume_srgb=True)
            lut.write_bytes(b'changed after prepare')
            result = w.run(root/'job', root/'output.jpg')
            self.assertTrue(result['lut_applied']); self.assertIn('tetrahedral', result['interpolation'])
            self.assertEqual(source.read_bytes(), original)
            with Image.open(root/'output.jpg') as im:
                self.assertLessEqual(max(abs(a-b) for a, b in zip(im.getpixel((24,16)), (80,120,200))), 2)
                self.assertEqual(str(im.getexif().get_ifd(w.EXIF_IFD)[0xa434]), '44-2 58mm f/2')
            (root/'job/look.clf').write_bytes(self.raw+b' ')
            with self.assertRaisesRegex(ValueError, 'hash mismatch'): w.run(root/'job', root/'other.jpg')


if __name__ == '__main__': unittest.main()
