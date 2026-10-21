"""Offline Astia native-grid experiment; no device or installer APIs.

prepare uses existing OCIO/Adobe host oracle; fit uses existing NumPy/SciPy.
All profile bytes, fitted tables and image artifacts remain in ignored build/.
This defines a rendered Sony-ST-sRGB adaptation, not a full RAW/Fuji renderer.
"""
import argparse
from array import array
import colorsys
import hashlib
import itertools
import json
from pathlib import Path
import random
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
REF = ROOT / 'research/build/qaqleonard-reference-20261020'
WORK = ROOT / 'research/build/astia-native-20261020'
PROFILE = Path('/Users/hongjunwu/Downloads/Camera-Profile-for-Fujifilm-Film-Simulation-main/Fujifilm Simulation LUT/Astia.xmp')
sys.path.insert(0, str(ROOT))


def sha(data):
    return hashlib.sha256(data).hexdigest()


def save(path, value):
    path.write_text(json.dumps(value, indent=2, allow_nan=False) + '\n')


def groups():
    rng = random.Random(20261020)
    random_rows = lambda n: [tuple(rng.random() for _ in range(3)) for _ in range(n)]
    gray = [(i / 511,) * 3 for i in range(512)]
    return {'train': random_rows(12000) + gray * 4,
            'selection': random_rows(2048), 'random_holdout': random_rows(8192),
            'neutral': [(i / 4096,) * 3 for i in range(4097)],
            'synthetic_skin': [colorsys.hsv_to_rgb(h / 360, s, v)
                               for h, s, v in itertools.product(range(10, 51, 5), [.15, .3, .45, .6], [.2, .35, .5, .65, .8, .95])],
            'saturated': [colorsys.hsv_to_rgb(h / 360, s, v)
                          for h, s, v in itertools.product(range(0, 360, 3), [.8, .9, 1.], [.1, .3, .5, .7, .9, 1.])],
            'gamut_faces': [tuple([a / 32, b / 32][:axis] + [fixed] + [a / 32, b / 32][axis:])
                            for fixed, axis, a, b in itertools.product([0., 1.], range(3), range(33), range(33))]}


def prepare(work, profile):
    from photo_worker.adobe_table import decode_xmp
    from photo_worker.fujifilm import stage_processor
    from photo_worker.profiles import CRS, RDF
    from photo_worker.adobe_table import inflate_table
    from xml.etree import ElementTree as ET
    if work.exists():
        raise ValueError('Use a new work directory; preserve previous candidates')
    raw = profile.read_bytes()
    table = decode_xmp(raw)
    info = next(ET.fromstring(raw).iter(RDF + 'Description')).attrib
    payload = inflate_table(info[CRS + 'Table_' + table.fingerprint])
    oracle = ROOT / 'research/build/adobe-rgb-table-oracle'
    previous = json.loads((ROOT / 'research/evidence/fujifilm-srgb-checks.json').read_text())
    # Existing executable has no installation effect. Pin this local run's bytes;
    # validate its output against the independently implemented OCIO path below.
    assert oracle.is_file() and previous['passed']
    prior = next(row for row in previous['profiles'] if row['name'] == 'Astia')
    assert table.source_sha256 == prior['source']['source_sha256'], 'Source differs from reviewed Astia XMP'
    work.mkdir(parents=True)
    (work / 'table.private.bin').write_bytes(payload)
    processor = stage_processor(table)
    summary = {'schema': 1, 'look': 'Astia', 'source': table.summary(),
               'oracle_sha256': sha(oracle.read_bytes()),
               'input_contract': 'encoded Sony ST assumed to be rendered sRGB SDR',
               'target_contract': 'existing SDK-validated rendered-sRGB Astia RGBTable adaptation',
               'scene_referred_inverse_ST': False, 'camera_accessed': False,
               'ghub_accessed': False, 'compiler_invoked': False, 'groups': {}}
    for name, rows in groups().items():
        values = array('f', (v for row in rows for v in row))
        source = work / (name + '.input.f32')
        target = work / (name + '.target.f32')
        source.write_bytes(values.tobytes())
        subprocess.run([str(oracle), str(work / 'table.private.bin'), str(source), str(target)], check=True)
        expected = array('f'); expected.frombytes(target.read_bytes())
        actual = array('f', values); processor.applyRGB(actual)
        assert len(expected) == len(values)
        error = max(abs(a - b) * 255 for a, b in zip(actual, expected))
        assert error < .01, (name, error)
        summary['groups'][name] = {'vectors': len(rows), 'input_sha256': sha(source.read_bytes()),
                                  'target_sha256': sha(target.read_bytes()), 'OCIO_vs_oracle_max_RGB8': error}
    save(work / 'inputs.json', summary)
    print(json.dumps({'prepared': True, 'groups': {k: v['vectors'] for k, v in summary['groups'].items()}}))


def fit(work):
    import numpy as np
    sys.path.insert(0, str(REF / 'src/looks'))
    from looktool import native
    model_path = REF / 'src/looks/assets/rendering-model-v1.json'
    model = json.loads(model_path.read_text())
    response = np.asarray(model['response_matrix'])
    inputs = json.loads((work / 'inputs.json').read_text())
    data = {}
    for name, record in inputs['groups'].items():
        files = [(work / (name + suffix)).read_bytes() for suffix in ['.input.f32', '.target.f32']]
        assert [sha(b) for b in files] == [record['input_sha256'], record['target_sha256']]
        data[name] = [np.frombuffer(b, dtype='<f4').reshape(-1, 3).astype(float) for b in files]
    h, rhs = native.equations(*data['train'], response)
    trials = []
    candidates = []
    for strength in [.1, 1., 10.]:
        coefficients, convergence = native.constrained_fit(h, rhs, strength)
        assert convergence['converged'], convergence
        actual = native.predict(data['selection'][0], coefficients, response)
        score = native.metrics(actual, data['selection'][1])
        trials.append({'smoothness': strength, **score, **convergence})
        candidates.append(coefficients)
    choice = min(range(3), key=lambda i: trials[i]['rgb_mae'])
    coefficients = candidates[choice]
    raw = native.encode(coefficients)
    assert len(raw) == 1215 and np.array_equal(native.decode(raw), coefficients)
    out = work / 'slot1-astia.raw-s8.bin'
    if out.exists():
        assert out.read_bytes() == raw, 'Existing candidate differs; use a new directory'
    else:
        out.write_bytes(raw)

    def lab(rgb):
        linear = np.where(rgb <= .04045, rgb / 12.92, ((rgb + .055) / 1.055) ** 2.4)
        xyz = linear @ np.array([[.4124564, .3575761, .1804375], [.2126729, .7151522, .0721750], [.0193339, .1191920, .9503041]]).T
        xyz /= [.95047, 1., 1.08883]
        f = np.where(xyz > (6 / 29) ** 3, np.cbrt(xyz), xyz / (3 * (6 / 29) ** 2) + 4 / 29)
        return np.column_stack([116 * f[:, 1] - 16, 500 * (f[:, 0] - f[:, 1]), 200 * (f[:, 1] - f[:, 2])])

    def metrics(actual, target):
        error = np.abs(actual - target) * 255
        de = np.linalg.norm(lab(actual) - lab(target), axis=1)
        return {'RGB8_MAE': float(error.mean()), 'RGB8_p95': float(np.quantile(error, .95)),
                'RGB8_p99': float(np.quantile(error, .99)), 'RGB8_max': float(error.max()),
                'deltaE76_mean': float(de.mean()), 'deltaE76_p95': float(np.quantile(de, .95)),
                'deltaE76_max': float(de.max())}

    scores = {}
    predictions = {}
    for name, (source, target) in data.items():
        if name in ['train', 'selection']:
            continue
        prediction = native.predict(source, coefficients, response)
        predictions[name] = prediction
        scores[name] = {'vectors': len(source), 'native_fit': metrics(prediction, target),
                        'no_effect_baseline': metrics(source, target),
                        'fraction_pixels_clipped': float(np.mean(np.any((prediction <= 0) | (prediction >= 1), axis=1)))}
    # Diagnostic only, not another deployable candidate: remove the signed-7
    # constraint while keeping the chosen regularizer and 405-node topology.
    unrestricted = np.linalg.solve(h + trials[choice]['smoothness'] * native.smoothness() + np.eye(405) * .001, rhs)
    unrestricted_prediction = native.predict(data['random_holdout'][0], unrestricted, response)
    diagnostic = {'description': 'Same grid/regularizer without integer or coefficient-range limits; not camera data',
                  'coefficients_outside_signed7': int(np.count_nonzero((unrestricted < -64) | (unrestricted > 63))),
                  'random_holdout': metrics(unrestricted_prediction, data['random_holdout'][1]),
                  'not_a_precision_only_comparison': True}
    # Tiny private visual companion. Swatches are synthetic, not user photos.
    swatches = []
    for name, indices in [('neutral', [round(i * 4096 / 7) for i in range(8)]),
                          ('synthetic_skin', [round(i * 215 / 23) for i in range(24)]),
                          ('saturated', [round(i * 2159 / 31) for i in range(32)])]:
        source, target = data[name]
        for i in indices:
            swatches.append([source[i].tolist(), target[i].tolist(), predictions[name][i].tolist()])
    save(work / 'swatches.private.json', swatches)
    # Independent evaluator: scalar corner accumulation, signed-plane decoding
    # and explicit row-vector response. Do not call native.interpolation/predict.
    scalar_errors = []
    for rgb, expected in zip(data['random_holdout'][0][:128], predictions['random_holdout'][:128]):
        r, g, b = rgb
        y = .299 * r + .587 * g + .114 * b
        cb = (b - y) / (2 * (1 - .114))
        cr = (r - y) / (2 * (1 - .299))
        coord = np.clip([(y - 16 / 255) * 255 / 219 * 4,
                         cb * 255 / 224 * 8 + 4, cr * 255 / 224 * 8 + 4], [0, 0, 0], [4, 8, 8])
        lo = np.minimum(np.floor(coord).astype(int), [3, 7, 7]); frac = coord - lo
        delta = [0., 0., 0.]
        for corner in itertools.product([0, 1], repeat=3):
            ix = (lo[0] + corner[0]) * 81 + (lo[1] + corner[1]) * 9 + lo[2] + corner[2]
            weight = 1.
            for axis in range(3): weight *= frac[axis] if corner[axis] else 1 - frac[axis]
            for channel in range(3):
                value = raw[channel * 405 + ix]
                delta[channel] += (value - 256 if value >= 128 else value) * weight / 256
        result = np.clip([rgb[c] + sum(delta[j] * response[j, c] for j in range(3)) for c in range(3)], 0, 1)
        scalar_errors.append(float(np.max(np.abs(result - expected))))
    assert max(scalar_errors) < 1e-12
    report = {'schema': 1, 'look': 'Astia', 'slot': 1, 'required_base': 'ST',
              'status': 'OFFLINE_MODEL_CANDIDATE_NOT_INSTALLABLE', 'camera_accessed': False,
              'ghub_accessed': False, 'inputs': inputs, 'model_sha256': sha(model_path.read_bytes()),
              'fitter_sha256': sha((REF / 'src/looks/looktool/native.py').read_bytes()),
              'coordinate_model': model['coordinate_model'], 'response_matrix': model['response_matrix'],
              'model_measured_on_our_body': False, 'full_RAW_or_Fujifilm_match': False,
              'raw_sha256': sha(raw), 'raw_bytes': len(raw), 'selected_trial': choice,
              'trials': trials, 'coefficient_min': int(coefficients.min()), 'coefficient_max': int(coefficients.max()),
              'coefficients_at_bounds': int(np.count_nonzero((coefficients == -64) | (coefficients == 63))),
              'coefficient_count': 1215, 'metrics': scores,
              'unrestricted_fit_diagnostic': diagnostic,
              'independent_scalar_cases': len(scalar_errors), 'independent_scalar_max_error': max(scalar_errors),
              'metric_scope': 'Synthetic encoded-sRGB input/target through provisional native ISP predictor; not photo/camera accuracy',
              'HIF_color_contract_validated': False, 'generator_sha256': sha(Path(__file__).read_bytes())}
    save(work / 'candidate.private.json', report)
    save(ROOT / 'research/evidence/astia-native-fit.json', report)
    print(json.dumps({k: report[k] for k in ['raw_sha256', 'coefficients_at_bounds', 'metrics']}, indent=2))


def preview(work):
    from PIL import Image, ImageDraw, ImageFont
    swatches = json.loads((work / 'swatches.private.json').read_text())
    image = Image.new('RGB', (1440, 600), '#181b20')
    draw = ImageDraw.Draw(image)
    title = ImageFont.load_default(size=26); label = ImageFont.load_default(size=20)
    draw.text((24, 18), 'Astia native-grid experiment: synthetic colors, provisional ISP model', fill='white', font=title)
    for panel, text in enumerate(['Input assumed Sony ST sRGB', 'SDK Astia sRGB target', 'Fitted native-grid prediction']):
        left = 24 + panel * 472
        draw.text((left, 68), text, fill='white', font=label)
        for i, values in enumerate(swatches):
            color = tuple(round(max(0, min(1, x)) * 255) for x in values[panel])
            x, y = left + (i % 8) * 55, 105 + (i // 8) * 55
            draw.rectangle((x, y, x + 52, y + 52), fill=color)
    draw.text((24, 557), 'Rows: neutrals / synthetic skin-like colors / saturated colors. These are not camera output or measured skin patches.', fill='#c1c7d0', font=label)
    image.save(work / 'astia-native-comparison.png')
    print(str(work / 'astia-native-comparison.png'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stage', choices=['prepare', 'fit', 'preview'])
    parser.add_argument('--work', type=Path, default=WORK)
    parser.add_argument('--profile', type=Path, default=PROFILE)
    args = parser.parse_args()
    if args.stage == 'prepare': prepare(args.work, args.profile)
    elif args.stage == 'fit': fit(args.work)
    else: preview(args.work)


if __name__ == '__main__':
    main()
