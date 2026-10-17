#!/usr/bin/env python3
"""Independent numerical and image checks for the computer-only LUT reference."""
from __future__ import annotations
import hashlib
import itertools
import json
import math
from pathlib import Path
import random
import sys
import unittest

from lut_reference import Cube, MAX_CUBE_BYTES, parse_cube, process_image, read_cube

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "research/build/lut-reference"
COUNTS = {"scalar_oracle_rgb_cases": 0, "analytic_rgb_cases": 0,
          "opencolorio_rgb_cases": 0, "malformed_cases": 0}
ERRORS = {"scalar_oracle_max_abs": 0.0, "analytic_max_abs": 0.0,
          "ocio_max_abs": 0.0}
OCIO = {"performed": False}


def cube_text(n, fn, lo=(0., 0., 0.), hi=(1., 1., 1.)):
    lines = ['TITLE "Synthetic proof fixture"', f'LUT_3D_SIZE {n}',
             'DOMAIN_MIN ' + ' '.join(map(str, lo)),
             'DOMAIN_MAX ' + ' '.join(map(str, hi))]
    for b in range(n):
        for g in range(n):
            for r in range(n):
                row = fn(r / (n - 1), g / (n - 1), b / (n - 1))
                lines.append(' '.join(format(v, '.17g') for v in row))
    return '\n'.join(lines) + '\n'


def oracle(cube, rgb):
    # Independently enumerate/weight all eight corners, rather than nested lerps.
    coords = [min(cube.size - 1., max(0., (rgb[i] - cube.domain_min[i]) /
              (cube.domain_max[i] - cube.domain_min[i]) * (cube.size - 1)))
              for i in range(3)]
    base = [min(int(math.floor(v)), cube.size - 2) for v in coords]
    frac = [coords[i] - base[i] for i in range(3)]
    result = [0., 0., 0.]
    for corner in itertools.product((0, 1), repeat=3):
        weight = math.prod(frac[a] if corner[a] else 1 - frac[a] for a in range(3))
        rr, gg, bb = [base[a] + corner[a] for a in range(3)]
        entry = cube.values[rr + cube.size * (gg + cube.size * bb)]
        for a in range(3):
            result[a] += weight * entry[a]
    return result


def err(a, b):
    return max(abs(x - y) for x, y in zip(a, b))


def fixture_path(name, data):
    p = BUILD / name
    p.write_text(data)
    return p


class ReferenceChecks(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        BUILD.mkdir(parents=True, exist_ok=True)
        cls.identity_text = cube_text(17, lambda r, g, b: (r, g, b))
        cls.identity = parse_cube(cls.identity_text)

    def test_identity_and_channel_order_both_grid_sizes(self):
        rng = random.Random(170033)
        for n in (17, 33):
            identity = parse_cube(cube_text(n, lambda r, g, b: (r, g, b)))
            permutation = parse_cube(cube_text(n, lambda r, g, b: (b, r, g)))
            cases = list(itertools.product((0., 1.), repeat=3))
            cases += [(rng.random(), rng.random(), rng.random()) for _ in range(1000)]
            cases += [(0.1, 0.4, 0.8), (-2., 0.2, 9.)]
            for rgb in cases:
                clipped = tuple(min(1., max(0., v)) for v in rgb)
                for cube, expected in [(identity, clipped), (permutation, (clipped[2], clipped[0], clipped[1]))]:
                    delta = err(cube.apply(rgb), expected)
                    ERRORS['analytic_max_abs'] = max(ERRORS['analytic_max_abs'], delta)
                    COUNTS['analytic_rgb_cases'] += 1
                    self.assertLess(delta, 2e-14)

    def test_independent_oracle_nonlinear_and_domains(self):
        rng = random.Random(420)
        fn = lambda r, g, b: (math.sin(r * 2) + .2 * g * b, g * g - .1 * b, b ** 3 + .1 * r * g)
        for n in (17, 33):
            for lo, hi in [((0., 0., 0.), (1., 1., 1.)), ((-.2, .1, .3), (.7, 1.2, 2.))]:
                cube = parse_cube(cube_text(n, fn, lo, hi))
                cases = [lo, hi, (-5., -5., -5.), (5., 5., 5.)]
                cases += [tuple(rng.uniform(-1, 3) for _ in range(3)) for _ in range(1000)]
                for rgb in cases:
                    delta = err(cube.apply(rgb), oracle(cube, rgb))
                    ERRORS['scalar_oracle_max_abs'] = max(ERRORS['scalar_oracle_max_abs'], delta)
                    COUNTS['scalar_oracle_rgb_cases'] += 1
                    self.assertLess(delta, 2e-14)

    def test_nondefault_domain_identity_analytic(self):
        lo, hi = (-1., .1, 2.), (2., .9, 4.)
        cube = parse_cube(cube_text(17, lambda r, g, b: (r, g, b), lo, hi))
        for rgb in [lo, hi, (.5, .5, 3.), (-5., 4., 3.2)]:
            expected = tuple(min(1., max(0., (rgb[i] - lo[i]) / (hi[i] - lo[i]))) for i in range(3))
            self.assertLess(err(cube.apply(rgb), expected), 2e-14)

    def test_clipping_and_rgb8_rounding(self):
        cube = parse_cube(cube_text(17, lambda r, g, b: (-.5, 1.5, .5)))
        self.assertEqual(cube.apply((.3, .4, .5)), (-.5, 1.5, .5))
        self.assertEqual(cube.apply_rgb8((0, 128, 255)), (0, 255, 128))
        for pixel in [(True, 0, 0), (256, 0, 0), (1.0, 0, 0), (1, 2)]:
            with self.assertRaises(ValueError):
                cube.apply_rgb8(pixel)

    def test_comment_title_bom_reading(self):
        text = self.identity_text.replace('TITLE "Synthetic proof fixture"', 'TITLE "A # in a title" # after title')
        text = '# before\n\n' + text.replace('LUT_3D_SIZE 17', 'LUT_3D_SIZE 17 # inline comment')
        cube = parse_cube(text)
        self.assertEqual(cube.title, 'A # in a title')
        p = BUILD / 'identity-17.cube'
        p.write_bytes(b'\xef\xbb\xbf' + text.encode())
        self.assertEqual(read_cube(p), cube)

    def test_reject_malformed_or_unsupported(self):
        t = self.identity_text
        variants = [
            '', t.replace('LUT_3D_SIZE 17', 'LUT_3D_SIZE 2'),
            t.replace('LUT_3D_SIZE 17', 'LUT_1D_SIZE 17'),
            t.replace('LUT_3D_SIZE 17', 'LUT_3D_SIZE 17\nLUT_1D_SIZE 17'),
            t.replace('LUT_3D_SIZE 17', 'LUT_3D_SIZE 17\nLUT_3D_SIZE 17'),
            t.replace('DOMAIN_MIN 0.0 0.0 0.0', 'DOMAIN_MIN 0 0 0\nDOMAIN_MIN 0 0 0'),
            t.replace('DOMAIN_MAX 1.0 1.0 1.0', 'DOMAIN_MAX 0 1 1'),
            t.replace('DOMAIN_MAX 1.0 1.0 1.0', 'DOMAIN_MAX -1 1 1'),
            t.replace('DOMAIN_MIN 0.0 0.0 0.0', 'DOMAIN_MIN nan 0 0'),
            t.replace('DOMAIN_MAX 1.0 1.0 1.0', 'DOMAIN_MAX 1e309 1 1'),
            t.replace('DOMAIN_MAX 1.0 1.0 1.0', 'DOMAIN_MAX 1 1'),
            t.replace('0 0 0\n', 'nan 0 0\n', 1),
            t.replace('0 0 0\n', 'inf 0 0\n', 1),
            t.replace('0 0 0\n', '0 0 0 1\n', 1),
            t.replace('0 0 0\n', '0 invalid 0\n', 1),
            t.rsplit('\n', 2)[0] + '\n', t + '0 0 0\n',
            t + 'DOMAIN_MAX 1 1 1\n', t.replace('TITLE "Synthetic proof fixture"', 'TITLE "unclosed'),
            'LUT_3D_SIZE 17 extra\n', '# ' + 'a' * MAX_CUBE_BYTES,
            'DOMAIN_MIN -1e308 0 0\nDOMAIN_MAX 1e308 1 1\n' + '\n'.join(t.splitlines()[1:2] + t.splitlines()[4:]),
        ]
        for i, bad in enumerate(variants):
            with self.subTest(case=i), self.assertRaises(ValueError):
                parse_cube(bad)
            COUNTS['malformed_cases'] += 1
        for rgb in [(math.nan, 0, 0), (0, math.inf, 0), (0, 0), (0, 0, 0, 0)]:
            with self.assertRaises(ValueError):
                self.identity.apply(rgb)

    def test_crosscheck_opencolorio(self):
        try:
            import PyOpenColorIO as ocio
        except ImportError:
            OCIO.update(performed=False, reason='PyOpenColorIO unavailable')
            self.skipTest('PyOpenColorIO unavailable; cross-check not performed')
        OCIO.update(performed=True, version=ocio.__version__, interpolation='INTERP_LINEAR',
                    processor='Config.CreateRaw + FileTransform, default CPU processor')
        rng = random.Random(1949)
        fn = lambda r, g, b: (math.sin(r * 1.4) + .12 * g * b, .1 + .7 * g * r, -.1 + b * b + .13 * r * g)
        for n in (17, 33):
            for lo, hi in [((0., 0., 0.), (1., 1., 1.)), ((-.2, .1, .3), (.7, 1.2, 2.))]:
                p = fixture_path(f'ocio-{n}-domain-{lo[0]}.cube', cube_text(n, fn, lo, hi))
                cube = read_cube(p)
                ft = ocio.FileTransform(src=str(p.resolve()), interpolation=ocio.INTERP_LINEAR)
                cpu = ocio.Config.CreateRaw().getProcessor(ft).getDefaultCPUProcessor()
                cases = [lo, hi, (-3., -3., -3.), (3., 3., 3.)]
                cases += [tuple(rng.uniform(-.8, 2.3) for _ in range(3)) for _ in range(2000)]
                for rgb in cases:
                    delta = err(cube.apply(rgb), cpu.applyRGB(rgb))
                    ERRORS['ocio_max_abs'] = max(ERRORS['ocio_max_abs'], delta)
                    COUNTS['opencolorio_rgb_cases'] += 1
                    self.assertLess(delta, 2e-6)

    def test_synthetic_image_outputs_and_guards(self):
        from PIL import Image, ImageCms
        width, height = 96, 64
        pixels = [(round(x / (width - 1) * 255), round(y / (height - 1) * 255),
                   round((x + y) / (width + height - 2) * 255)) for y in range(height) for x in range(width)]
        source = BUILD / 'input-srgb.png'
        image = Image.new('RGB', (width, height))
        image.putdata(pixels)
        image.save(source)
        before = hashlib.sha256(source.read_bytes()).hexdigest()
        for name in ['identity.png', 'permuted.png', 'graded.jpg', 'alpha-output.png', 'oriented.png', 'tagged-output.png']:
            (BUILD / name).unlink(missing_ok=True)
        with self.assertRaises(ValueError):
            process_image(self.identity, source, BUILD / 'identity.png')
        process_image(self.identity, source, BUILD / 'identity.png', assume_srgb=True)
        perm = parse_cube(cube_text(33, lambda r, g, b: (b, r, g)))
        process_image(perm, source, BUILD / 'permuted.png', assume_srgb=True)
        process_image(perm, source, BUILD / 'graded.jpg', assume_srgb=True)
        with Image.open(BUILD / 'identity.png') as out:
            self.assertEqual(list(out.get_flattened_data()), pixels)
            self.assertTrue(out.info.get('icc_profile'))
        with Image.open(BUILD / 'permuted.png') as out:
            self.assertEqual(list(out.get_flattened_data()), [(b, r, g) for r, g, b in pixels])
        with Image.open(BUILD / 'graded.jpg') as out:
            self.assertEqual(out.size, (width, height))
            self.assertTrue(out.info.get('icc_profile'))
        with self.assertRaises(ValueError):
            process_image(perm, source, BUILD / 'identity.png', assume_srgb=True)
        with self.assertRaises(ValueError):
            process_image(perm, source, source, assume_srgb=True)
        self.assertEqual(before, hashlib.sha256(source.read_bytes()).hexdigest())
        rgba = image.convert('RGBA')
        alphas = [i % 256 for i in range(width * height)]
        rgba.putalpha(Image.frombytes('L', image.size, bytes(alphas)))
        rgba.save(BUILD / 'alpha-input.png')
        process_image(perm, BUILD / 'alpha-input.png', BUILD / 'alpha-output.png', assume_srgb=True)
        with Image.open(BUILD / 'alpha-output.png') as out:
            self.assertEqual(list(out.getchannel('A').get_flattened_data()), alphas)
        with self.assertRaises(ValueError):
            process_image(perm, BUILD / 'alpha-input.png', BUILD / 'alpha-output.jpg', assume_srgb=True)
        exif = Image.Exif()
        exif[274] = 6
        image.save(BUILD / 'oriented-input.jpg', exif=exif)
        process_image(self.identity, BUILD / 'oriented-input.jpg', BUILD / 'oriented.png', assume_srgb=True)
        with Image.open(BUILD / 'oriented.png') as out:
            self.assertEqual(out.size, (height, width))
            self.assertFalse(out.getexif())
        profile = ImageCms.ImageCmsProfile(ImageCms.createProfile('sRGB')).tobytes()
        image.save(BUILD / 'tagged.png', icc_profile=profile)
        result = process_image(self.identity, BUILD / 'tagged.png', BUILD / 'tagged-output.png')
        self.assertTrue(result['input_icc_converted'])
        with Image.open(BUILD / 'tagged-output.png') as out:
            self.assertEqual(list(out.get_flattened_data()), pixels)
        image.save(BUILD / 'invalid-icc.png', icc_profile=b'not an ICC profile')
        with self.assertRaises(ValueError):
            process_image(self.identity, BUILD / 'invalid-icc.png', BUILD / 'never-written.png')
        self.assertFalse((BUILD / 'never-written.png').exists())


def main():
    suite = unittest.defaultTestLoader.loadTestsFromTestCase(ReferenceChecks)
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    from PIL import __version__ as pillow_version
    evidence = {"schema": 1, "computer_only": True, "camera_access": False,
                "target_execution": False, "tests_run": result.testsRun,
                "failures": len(result.failures), "errors": len(result.errors),
                "skipped": len(result.skipped), "passed": result.wasSuccessful(),
                "counts": COUNTS, "max_absolute_errors": ERRORS,
                "opencolorio": OCIO, "pillow_version": pillow_version,
                "limits": "Host math/reference checks only; no camera performance, loader integration, still-pipeline routing or photo-quality equivalence established.",
                "sources": [], "synthetic_outputs": []}
    for p in [Path(__file__), ROOT / 'research/lut_reference.py']:
        evidence['sources'].append({"path": str(p.relative_to(ROOT)),
                                   "sha256": hashlib.sha256(p.read_bytes()).hexdigest()})
    for p in sorted(BUILD.iterdir()):
        if p.is_file():
            evidence['synthetic_outputs'].append({"path": str(p.relative_to(ROOT)),
                "bytes": p.stat().st_size, "sha256": hashlib.sha256(p.read_bytes()).hexdigest()})
    out = ROOT / 'research/evidence/lut-reference-checks.json'
    out.write_text(json.dumps(evidence, indent=2) + '\n')
    print(json.dumps({"evidence": str(out), "passed": result.wasSuccessful(),
                      "counts": COUNTS, "max_absolute_errors": ERRORS}, indent=2))
    return 0 if result.wasSuccessful() else 1


if __name__ == '__main__':
    sys.exit(main())
