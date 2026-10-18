"""Snapshot explicit completed JPEG jobs and publish separate graded copies."""
from __future__ import annotations

from array import array
from fractions import Fraction
import hashlib
import io
import json
import math
import os
from pathlib import Path
import re
import shutil
import tempfile

from PIL import Image, ImageCms, ImageOps, TiffImagePlugin

from research.lut_reference import MAX_CUBE_BYTES, MAX_IMAGE_PIXELS, parse_cube
from . import __version__

MAX_JPEG_BYTES = 64 * 1024 * 1024
MAX_JSON_BYTES = 128 * 1024
SRGB_CONTRACT = "encoded-srgb-to-encoded-srgb"
EXIF_IFD = 0x8769
# Deliberately exclude GPS, MakerNotes, thumbnails, offset/pointer tags and serials.
ROOT_TAGS = {0x010f, 0x0110, 0x0132, 0x013b, 0x8298}  # make/model/time/artist/copyright
PHOTO_TAGS = {0x829a, 0x829d, 0x8822, 0x8827, 0x8830, 0x8832, 0x9003, 0x9004,
              0x9010, 0x9011, 0x9012, 0x9201, 0x9202, 0x9204, 0x9205, 0x9207,
              0x9208, 0x9209, 0x920a, 0x9290, 0x9291, 0x9292, 0xa402, 0xa403,
              0xa405, 0xa432, 0xa433, 0xa434}


def canonical(data):
    return json.dumps(data, sort_keys=True, separators=(",", ":"), ensure_ascii=True,
                      allow_nan=False).encode()


def sha(data):
    return hashlib.sha256(data).hexdigest()


def read_limited(path, limit):
    path = Path(path)
    if path.is_symlink() or not path.is_file():
        raise ValueError(f"Expected an ordinary file, not a symlink: {path.name}")
    with path.open("rb") as f:
        raw = f.read(limit + 1)
    if len(raw) > limit:
        raise ValueError(f"File exceeds {limit} bytes: {path.name}")
    return raw


def json_data(raw):
    def unique(pairs):
        out = {}
        for k, v in pairs:
            if k in out:
                raise ValueError(f"Duplicate JSON key: {k}")
            out[k] = v
        return out
    return json.loads(raw, object_pairs_hook=unique,
                      parse_constant=lambda x: (_ for _ in ()).throw(ValueError(f"Nonfinite JSON: {x}")))


def number(value, low, high, label):
    # Bounds first also reject enormous JSON integers without converting to float.
    if type(value) not in (int, float) or not low <= value <= high or not math.isfinite(value):
        raise ValueError(f"{label} must be a finite number in [{low}, {high}]")
    return value


def sync_directory(path):
    fd = os.open(path, os.O_RDONLY)
    try:
        os.fsync(fd)
    finally:
        os.close(fd)


def validate_lens(lens):
    fields = {"id", "name", "make", "model", "focal_length_mm", "max_aperture"}
    if not isinstance(lens, dict) or set(lens) != fields:
        raise ValueError("Each lens needs exactly id, name, make, model, focal_length_mm, max_aperture")
    if not isinstance(lens['id'], str) or not re.fullmatch(r"[a-z0-9][a-z0-9_-]{0,63}", lens['id']):
        raise ValueError("Lens id must be 1-64 lowercase ASCII letters, digits, _ or -")
    for k in ("name", "make", "model"):
        if not isinstance(lens[k], str) or not 1 <= len(lens[k]) <= 128 or any(ord(c) < 32 for c in lens[k]):
            raise ValueError(f"Invalid lens {k}")
        if k != "name" and any(not 32 <= ord(c) <= 126 for c in lens[k]):
            raise ValueError(f"EXIF lens {k} must be printable ASCII")
    number(lens['focal_length_mm'], 1, 5000, 'Focal length')
    number(lens['max_aperture'], 0.5, 128, 'Maximum aperture')
    return dict(lens)


def load_lenses(path):
    data = json_data(read_limited(path, MAX_JSON_BYTES))
    if not isinstance(data, dict) or set(data) != {'schema', 'lenses'} or type(data['schema']) is not int or data['schema'] != 1:
        raise ValueError("Expected lens preset schema 1")
    if not isinstance(data['lenses'], list) or not 1 <= len(data['lenses']) <= 100:
        raise ValueError("Expected 1-100 lens presets")
    lenses = [validate_lens(lens) for lens in data['lenses']]
    if len({lens['id'] for lens in lenses}) != len(lenses):
        raise ValueError("Duplicate lens id")
    return lenses


def jpeg_info(raw):
    with Image.open(io.BytesIO(raw)) as image:
        if image.format != 'JPEG' or image.mode != 'RGB':
            raise ValueError("Worker accepts completed RGB8 JPEGs only")
        if image.width * image.height > MAX_IMAGE_PIXELS:
            raise ValueError("Image exceeds 40 million pixels")
        if image.getexif().get(274, 1) not in range(1, 9):
            raise ValueError("Invalid EXIF orientation")
        info = {'width': image.width, 'height': image.height,
                'orientation': image.getexif().get(274, 1),
                'has_icc': bool(image.info.get('icc_profile'))}
        # JPEG verify() alone does not decode entropy data or reject all truncation.
        image.load()
    return info


def prepare(source, job, *, cube=None, lut_contract=None, lens_presets=None, lens_id=None,
            aperture=None, assume_srgb=False, completed=False, quality=95):
    """Snapshot file bytes and preset now; this is NOT a shutter-time camera hook."""
    if not completed:
        raise ValueError("Explicit --completed assertion is required; no file watcher guesses completion")
    if type(quality) is not int or not 1 <= quality <= 100:
        raise ValueError("JPEG quality must be an integer from 1 to 100")
    if bool(lens_presets) != bool(lens_id):
        raise ValueError("Provide both --lens-presets and --lens")
    lens = None
    if lens_id:
        lens = next((x for x in load_lenses(lens_presets) if x['id'] == lens_id), None)
        if lens is None:
            raise ValueError(f"Unknown lens id: {lens_id}")
    if aperture is not None:
        if not lens:
            raise ValueError("Actual aperture requires a selected lens")
        number(aperture, lens['max_aperture'], 128, 'Actual shooting aperture')
    if cube is None and lut_contract is not None:
        raise ValueError("A LUT contract requires a CUBE file")
    assets = {'input.jpg': read_limited(source, MAX_JPEG_BYTES)}
    info = jpeg_info(assets['input.jpg'])
    if not info['has_icc'] and not assume_srgb:
        raise ValueError("Untagged JPEG requires explicit --assume-srgb")
    if cube is not None:
        if lut_contract != SRGB_CONTRACT:
            raise ValueError(f"LUT requires explicit --lut-contract {SRGB_CONTRACT}; XMP/DCP are not JPEG LUTs")
        if Path(cube).suffix.lower() != '.cube':
            raise ValueError("Only 17/33-point 3D CUBE accepted; Adobe RAW XMP/DCP need a separate validated bake")
        assets['look.cube'] = read_limited(cube, MAX_CUBE_BYTES)
        parse_cube(assets['look.cube'].decode('utf-8-sig'))
    payload = {'schema': 1, 'completed_asserted': True, 'input': info,
               'assets': {name: {'bytes': len(raw), 'sha256': sha(raw)} for name, raw in assets.items()},
               'lut_contract': lut_contract, 'lens': lens, 'actual_aperture': aperture,
               'assume_srgb': bool(assume_srgb), 'quality': quality,
               'preset_snapshot_time': 'job preparation; no camera shutter hook'}
    manifest = {'payload': payload, 'sha256': sha(canonical(payload))}
    job = Path(job)
    # Reserve the job name exclusively; job.json is the last, ready marker.
    job.mkdir(mode=0o700)
    try:
        for name, raw in assets.items():
            with (job/name).open('xb') as f:
                f.write(raw); f.flush(); os.fsync(f.fileno())
        with (job/'job.json').open('xb') as f:
            f.write(json.dumps(manifest, indent=2, allow_nan=False).encode()+b'\n')
            f.flush(); os.fsync(f.fileno())
        sync_directory(job)
        sync_directory(job.parent)
    except BaseException:
        # Only this newly reserved directory is ours; originals are never rewritten.
        shutil.rmtree(job)
        raise
    return {'job': str(job.resolve()), 'job_sha256': manifest['sha256'], 'input': info,
            'lens': lens, 'has_lut': cube is not None}


def load_job(job):
    job = Path(job)
    if job.is_symlink() or not job.is_dir():
        raise ValueError("Job must be an ordinary directory")
    doc = json_data(read_limited(job/'job.json', MAX_JSON_BYTES))
    if not isinstance(doc, dict) or set(doc) != {'payload', 'sha256'}:
        raise ValueError("Invalid job envelope")
    p = doc['payload']
    if not isinstance(p, dict) or set(p) != {'schema', 'completed_asserted', 'input', 'assets',
            'lut_contract', 'lens', 'actual_aperture', 'assume_srgb', 'quality', 'preset_snapshot_time'}:
        raise ValueError("Invalid job fields")
    if sha(canonical(p)) != doc['sha256']:
        raise ValueError("Job manifest hash mismatch")
    if type(p['schema']) is not int or p['schema'] != 1 or p['completed_asserted'] is not True:
        raise ValueError("Unsupported/incomplete job")
    if type(p['assume_srgb']) is not bool or type(p['quality']) is not int or not 1 <= p['quality'] <= 100:
        raise ValueError("Invalid job output options")
    if not isinstance(p['assets'], dict) or set(p['assets']) not in ({'input.jpg'}, {'input.jpg', 'look.cube'}):
        raise ValueError("Invalid job asset names")
    assets = {}
    for name, identity in p['assets'].items():
        if not isinstance(identity, dict) or set(identity) != {'bytes', 'sha256'}:
            raise ValueError("Invalid asset identity")
        raw = read_limited(job/name, MAX_JPEG_BYTES if name == 'input.jpg' else MAX_CUBE_BYTES)
        if len(raw) != identity['bytes'] or sha(raw) != identity['sha256']:
            raise ValueError(f"Snapshot hash mismatch: {name}")
        assets[name] = raw
    if jpeg_info(assets['input.jpg']) != p['input']:
        raise ValueError("JPEG header differs from job")
    if p['lens'] is not None:
        validate_lens(p['lens'])
    if p['actual_aperture'] is not None:
        if not p['lens']:
            raise ValueError("Actual aperture has no lens snapshot")
        number(p['actual_aperture'], p['lens']['max_aperture'], 128, 'Actual shooting aperture')
    if ('look.cube' in assets and p['lut_contract'] != SRGB_CONTRACT) or ('look.cube' not in assets and p['lut_contract'] is not None):
        raise ValueError("Invalid LUT color contract")
    return doc, assets


def processor_for(cube):
    import PyOpenColorIO as ocio
    lut = ocio.Lut3DTransform(gridSize=cube.size, interpolation=ocio.INTERP_LINEAR)
    n = cube.size
    for i, row in enumerate(cube.values):
        lut.setValue(i % n, (i//n) % n, i//(n*n), *row)
    matrix = [0.0]*16
    offset = [0.0]*4
    for i, (lo, hi) in enumerate(zip(cube.domain_min, cube.domain_max)):
        matrix[i*5] = 1/(hi-lo); offset[i] = -lo/(hi-lo)
    matrix[15] = 1
    group = ocio.GroupTransform([ocio.MatrixTransform(matrix=matrix, offset=offset), lut])
    return ocio.Config.CreateRaw().getProcessor(group).getDefaultCPUProcessor()


def grade(image, cube, strip_rows=16):
    """Native OCIO interpolation on float strips; Pillow still holds full images."""
    if type(strip_rows) is not int or not 1 <= strip_rows <= 256:
        raise ValueError("strip_rows must be an integer in [1,256]")
    processor = processor_for(cube)
    output = Image.new('RGB', image.size)
    for y in range(0, image.height, strip_rows):
        box = (0, y, image.width, min(y+strip_rows, image.height))
        data = array('f', (x/255 for x in image.crop(box).tobytes()))
        processor.applyRGB(data)  # Buffer-protocol path; NumPy is not required.
        if not all(math.isfinite(x) for x in data):
            raise ValueError("LUT produced nonfinite output")
        raw = bytes(math.floor(min(1.0, max(0.0, x))*255+0.5) for x in data)
        output.paste(Image.frombytes('RGB', (box[2], box[3]-box[1]), raw), box)
    return output


def rational(value):
    f = Fraction(str(value)).limit_denominator(1_000_000)
    return TiffImagePlugin.IFDRational(f.numerator, f.denominator)


def output_exif(original, lens, aperture, size):
    exif = Image.Exif()
    for tag in ROOT_TAGS:
        if tag in original:
            exif[tag] = original[tag]
    source = original.get_ifd(EXIF_IFD)
    photo = {tag: value for tag, value in source.items() if tag in PHOTO_TAGS}
    if lens:
        # Camera-reported aperture/focal tags belong to the previous optical state.
        for tag in (0x829d, 0x9202, 0x9205, 0x920a, 0xa405, 0xa432, 0xa433, 0xa434):
            photo.pop(tag, None)
        focal, maximum = lens['focal_length_mm'], lens['max_aperture']
        photo.update({0x920a: rational(focal), 0xa432: tuple(rational(v) for v in (focal, focal, maximum, maximum)),
                      0xa433: lens['make'], 0xa434: lens['model']})
        # APEX can be negative below f/1, but these EXIF tags are unsigned rationals.
        if maximum >= 1:
            photo[0x9205] = rational(2*math.log2(maximum))
        if aperture is not None:
            photo[0x829d] = rational(aperture)
            if aperture >= 1:
                photo[0x9202] = rational(2*math.log2(aperture))
    photo.update({0xa001: 1, 0xa002: size[0], 0xa003: size[1]})
    exif[EXIF_IFD] = photo
    exif[274] = 1
    exif[305] = f'A7C2 JPEG Worker {__version__}'
    return exif


def run(job, destination, *, strip_rows=16):
    doc, assets = load_job(job)
    p = doc['payload']
    destination = Path(destination)
    if destination.suffix.lower() not in {'.jpg', '.jpeg'}:
        raise ValueError("Output must be a separate .jpg/.jpeg file")
    if destination.exists() or destination.is_symlink():
        raise ValueError("Output exists; refusing overwrite")
    # Fail early on obviously inadequate space, and still handle actual ENOSPC at write time.
    required = p['input']['width'] * p['input']['height'] * 3 + 1024*1024
    if shutil.disk_usage(destination.parent).free < required:
        raise ValueError("Insufficient output space for conservative JPEG reservation")
    srgb = ImageCms.ImageCmsProfile(ImageCms.createProfile('sRGB'))
    with Image.open(io.BytesIO(assets['input.jpg'])) as opened:
        original = opened.getexif()
        icc = opened.info.get('icc_profile')
        if not icc and not p['assume_srgb']:
            raise ValueError("Untagged image has no sRGB assertion")
        rgb = ImageOps.exif_transpose(opened)
        if icc:
            try:
                rgb = ImageCms.profileToProfile(rgb, ImageCms.ImageCmsProfile(io.BytesIO(icc)), srgb,
                                               renderingIntent=0, outputMode='RGB')
            except (OSError, ValueError, ImageCms.PyCMSError) as exc:
                raise ValueError("Invalid/unsupported RGB ICC profile") from exc
        if 'look.cube' in assets:
            rgb = grade(rgb, parse_cube(assets['look.cube'].decode('utf-8-sig')), strip_rows)
        exif = output_exif(original, p['lens'], p['actual_aperture'], rgb.size)
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=destination.parent, prefix='.jpeg-worker-', suffix='.jpg', delete=False) as f:
                temporary = Path(f.name)
                rgb.save(f, format='JPEG', quality=p['quality'], subsampling=0,
                         exif=exif, icc_profile=srgb.tobytes())
                f.flush(); os.fsync(f.fileno())
            with Image.open(temporary) as check:
                if check.size != rgb.size:
                    raise ValueError("Encoded JPEG dimensions do not match")
                check.load()
            # Same-directory hard link publishes the complete inode atomically and
            # refuses concurrent outputs. There is no overwrite/replace window.
            os.link(temporary, destination)
            sync_directory(destination.parent)
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
    return {'output': str(destination.resolve()), 'output_sha256': sha(destination.read_bytes()),
            'job_sha256': doc['sha256'], 'dimensions': list(rgb.size), 'lens': p['lens'],
            'actual_aperture': p['actual_aperture'], 'lut_applied': 'look.cube' in assets,
            'color_contract': SRGB_CONTRACT, 'interpolation': 'trilinear via OpenColorIO',
            'metadata_policy': 'allowlisted capture EXIF; baked orientation; fresh sRGB ICC; no GPS/MakerNotes/thumbnail',
            'host_only': True}
