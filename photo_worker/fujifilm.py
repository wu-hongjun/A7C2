"""Explicit sRGB JPEG adaptations of the supplied Adobe RGBTable stage.

Adobe algorithm/matrix attribution: NOTICE.md, licenses/Adobe-DNG-SDK.txt.
This is not the DCP/RAW pipeline or a native Sony/Fujifilm Creative Look.
"""
from array import array
import json
from pathlib import Path
import re

from .adobe_table import MAX_ENCODED_BYTES, decode_xmp
from .profiles import xmp_info
from .worker import read_limited, sha


def multiply(a, b):
    return tuple(tuple(sum(a[i][k]*b[k][j] for k in range(3)) for j in range(3)) for i in range(3))


def inverse(m):
    a, b, c = m[0]; d, e, f = m[1]; g, h, i = m[2]
    z = a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g)
    return (( (e*i-f*h)/z, (c*h-b*i)/z, (b*f-c*e)/z),
            ((f*g-d*i)/z, (a*i-c*g)/z, (c*d-a*f)/z),
            ((d*h-e*g)/z, (b*g-a*h)/z, (a*e-b*d)/z))


def pcs_matrix(rows):
    # dng_color_space::SetMatrixToPCS normalizes rows to the SDK D50 white.
    white = (.3457/.3585, 1., (1-.3457-.3585)/.3585)
    return tuple(tuple(v*white[i]/sum(row) for v in row) for i, row in enumerate(rows))


SRGB = pcs_matrix(((.4361, .3851, .1431), (.2225, .7169, .0606), (.0139, .0971, .7141)))
P3 = pcs_matrix(((.5151, .2920, .1571), (.2412, .6922, .0666), (-.0010, .0419, .7843)))
PROPHOTO = pcs_matrix(((.7977, .1352, .0313), (.2880, .7119, .0001), (0., 0., .8249)))


def encode(x):
    return x*12.92 if x <= .0031308 else 1.055*x**(1/2.4)-.055


def decode(y):
    return y/12.92 if y <= .0031308*12.92 else ((y+.055)/1.055)**2.4


def stage_group(table):
    """Fast CPU implementation; retains native ProPhoto intermediate clipping.

    OCIO lossless optimization prevents replacement by approximate fast LUTs.
    The 4097-point transfer tables match SDK's 4096-interval 1D sampling.
    Input/output sRGB transfer is also sampled; its small approximation is
    measured against analytic transfer in the source-bound native oracle.
    """
    import PyOpenColorIO as o

    def matrix(m):
        return o.MatrixTransform(matrix=[m[i][j] if i < 3 and j < 3 else float(i == j)
                                         for i in range(4) for j in range(4)])

    def gamma(fn):
        lut = o.Lut1DTransform(length=4097, interpolation=o.INTERP_LINEAR)
        for i in range(4097):
            v = fn(i/4096)
            lut.setValue(i, v, v, v)
        return lut

    def clip():
        return o.RangeTransform(minInValue=0, maxInValue=1, minOutValue=0, maxOutValue=1)

    lut = o.Lut3DTransform(gridSize=32, interpolation=o.INTERP_TETRAHEDRAL)
    for i, row in enumerate(table.samples()):
        lut.setValue(i//1024, (i//32) % 32, i % 32, *(x/65535 for x in row))
    # Deliberately don't collapse the ProPhoto round trip across its clamp.
    group = o.GroupTransform([gamma(decode), matrix(multiply(inverse(PROPHOTO), SRGB)),
        matrix(multiply(inverse(P3), PROPHOTO)), clip(), gamma(encode), lut, clip(),
        gamma(decode), matrix(multiply(inverse(PROPHOTO), P3)), clip(),
        matrix(multiply(inverse(SRGB), PROPHOTO)), clip(), gamma(encode)])
    return group


def stage_processor(table):
    import PyOpenColorIO as o
    return o.Config.CreateRaw().getProcessor(stage_group(table)).getOptimizedCPUProcessor(o.OPTIMIZATION_LOSSLESS)


def clf_bytes(table):
    """Self-contained CLF preserves the native grid, transfer tables and clamps."""
    import PyOpenColorIO as o
    return stage_group(table).write('Academy/ASC Common LUT Format', config=o.Config.CreateRaw()).encode()


def flatten_cube_for_diagnostics(table, name, size=33):
    """Rejected delivery format: retain only to reproduce measured clipping errors."""
    if size not in (17, 33):
        raise ValueError('JPEG worker accepts only 17 or 33 point CUBEs')
    if not name or any(c not in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -_' for c in name):
        raise ValueError('Unsafe LUT name')
    values = array('f', (v/(size-1) for b in range(size) for g in range(size)
                         for r in range(size) for v in (r, g, b)))
    stage_processor(table).applyRGB(values)
    lines = ['# sRGB JPEG adaptation of embedded Adobe RGBTable stage only; not RAW/DCP rendering.',
             '# Source RGBTable MD5: '+table.fingerprint,
             '# Source XMP SHA256: '+table.source_sha256,
             '# Adobe DNG SDK algorithm notice: NOTICE.md / licenses/Adobe-DNG-SDK.txt',
             'TITLE "'+name+' - sRGB JPEG adaptation"', f'LUT_3D_SIZE {size}',
             'DOMAIN_MIN 0 0 0', 'DOMAIN_MAX 1 1 1']
    lines.extend(' '.join(f'{x:.9f}' for x in values[i:i+3]) for i in range(0, len(values), 3))
    return ('\n'.join(lines)+'\n').encode()


def convert_directory(source, destination):
    """No overwrites. A manifest is written last; validation is a separate check."""
    source, destination = Path(source), Path(destination)
    inputs = []
    for path in sorted(source.glob('*.xmp')):
        raw = read_limited(path, MAX_ENCODED_BYTES)
        if xmp_info(raw).get('PresetType') == 'Look':
            slug = path.stem.lower().replace(' ', '-')
            if not re.fullmatch('[a-z0-9][a-z0-9-]{0,95}', slug):
                raise ValueError('Unsupported profile filename')
            inputs.append((slug, decode_xmp(raw)))
    if not inputs:
        raise ValueError('No actual look profiles found')
    if len({name for name, _ in inputs}) != len(inputs):
        raise ValueError('Profile filenames collide after normalization')
    # Preflight every name/table before reserving a new directory.
    baked = [(name, table, clf_bytes(table)) for name, table in inputs]
    destination.mkdir(parents=True, exist_ok=False)
    records = []
    for name, table, raw in baked:
        path = destination/(name+'-srgb.clf')
        with path.open('xb') as stream:
            stream.write(raw)
        records.append({'file': path.name, 'bytes': len(raw), 'sha256': sha(raw), **table.summary()})
    manifest = {'schema': 1, 'contract': 'encoded-srgb-to-encoded-srgb',
        'scope': 'Embedded RGBTable stage adapted to rendered sRGB; excludes RAW/DCP calibration and development.',
        'format': 'self-contained CLF', 'native_grid': 32, 'interpolation': 'tetrahedral',
        'validation': 'Run research.check_fujifilm_conversion; this manifest alone is not a validation receipt.',
        'camera_accessed': False, 'network_accessed': False, 'profiles': records}
    (destination/'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
    return manifest
