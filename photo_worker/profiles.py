"""Read-only inventory of the user-supplied Adobe Fujifilm profile pack.

No table decoding or JPEG compatibility is inferred from an embedded table name.
"""
from collections import Counter
from pathlib import Path
import struct
from xml.etree import ElementTree as ET

from .worker import read_limited, sha

CRS = '{http://ns.adobe.com/camera-raw-settings/1.0/}'
RDF = '{http://www.w3.org/1999/02/22-rdf-syntax-ns#}'
MAX_PROFILE_BYTES = 16 * 1024 * 1024


def xmp_info(raw):
    if len(raw) > 1024*1024 or b'<!DOCTYPE' in raw or b'<!ENTITY' in raw:
        raise ValueError('Unsupported/oversized XMP')
    try:
        tree = ET.fromstring(raw)
    except ET.ParseError as exc:
        raise ValueError('Invalid XMP XML') from exc
    descriptions = list(tree.iter(RDF+'Description'))
    if not descriptions:
        raise ValueError('XMP has no RDF description')
    attrs = descriptions[0].attrib
    fields = ['PresetType', 'UUID', 'CameraProfile', 'CameraProfileDigest', 'RGBTable',
              'SupportsSceneReferred', 'SupportsOutputReferred', 'ProcessVersion']
    result = {key: attrs[CRS+key] for key in fields if CRS+key in attrs}
    result['embedded_tables'] = [{'key': key[len(CRS):], 'encoded_characters': len(value),
                                  'encoded_sha256': sha(value.encode())}
                                 for key, value in attrs.items() if key.startswith(CRS+'Table_')]
    result['jpeg_worker_supported'] = False
    result['reason'] = ('Adobe scene-referred RAW profile; output-referred disabled; not an sRGB CUBE'
                        if attrs.get(CRS+'SupportsOutputReferred', '').lower() == 'false'
                        else 'Adobe wrapper/profile is not a validated sRGB CUBE')
    return result


def dcp_info(raw):
    """Bounded first-IFD summary, not a DCP renderer or camera calibration converter."""
    if len(raw) < 8 or raw[:2] not in (b'II', b'MM'):
        raise ValueError('Not a TIFF-style DCP')
    endian = '<' if raw[:2] == b'II' else '>'
    magic, off = struct.unpack_from(endian+'HI', raw, 2)
    if magic not in (42, 0x4352) or off+2 > len(raw):
        raise ValueError('Invalid DCP header')
    count, = struct.unpack_from(endian+'H', raw, off)
    if count > 512 or off+2+count*12+4 > len(raw):
        raise ValueError('Invalid DCP IFD bounds')
    names = {50708: 'UniqueCameraModel', 50936: 'ProfileName', 50937: 'ProfileHueSatMapDims',
             50940: 'ProfileToneCurve', 50981: 'ProfileLookTableDims', 50982: 'ProfileLookTableData',
             51107: 'ProfileHueSatMapEncoding', 51108: 'ProfileLookTableEncoding'}
    values = {}; tags = []
    sizes = {1: 1, 2: 1, 3: 2, 4: 4, 5: 8, 7: 1, 8: 2, 9: 4, 10: 8, 11: 4, 12: 8}
    for i in range(count):
        at = off+2+i*12
        tag, kind, length, pointer = struct.unpack_from(endian+'HHII', raw, at)
        if kind not in sizes or length*sizes[kind] > len(raw):
            raise ValueError('Invalid DCP field size')
        n = length*sizes[kind]; pos = at+8 if n <= 4 else pointer
        if pos+n > len(raw):
            raise ValueError('Invalid DCP field bounds')
        tags.append(tag)
        if tag in names:
            data = raw[pos:pos+n]
            if kind == 2:
                value = data.rstrip(b'\0').decode('utf-8')
            elif kind in (3, 4, 11) and length <= 16:
                value = list(struct.unpack(endian+{3: 'H', 4: 'I', 11: 'f'}[kind]*length, data))
            else:
                value = {'bytes': n, 'sha256': sha(data)}
            values[names[tag]] = value
    return {'magic': hex(magic), 'first_ifd_tags': tags, 'selected_values': values,
            'jpeg_worker_supported': False}


def inspect_pack(directory):
    directory = Path(directory)
    if directory.is_symlink() or not directory.is_dir():
        raise ValueError('Profile pack must be an ordinary directory')
    files = sorted(p for p in directory.rglob('*') if p.is_file())
    if len(files) > 5000:
        raise ValueError('Pack exceeds 5000-file inspection bound')
    inventory = []
    looks = []; camera_dcp = None
    for path in files:
        raw = read_limited(path, MAX_PROFILE_BYTES)
        name = str(path.relative_to(directory))
        inventory.append({'path': name, 'bytes': len(raw), 'sha256': sha(raw)})
        if path.suffix.lower() == '.xmp':
            looks.append({'path': name, **xmp_info(raw)})
        if path.name == 'Sony ILCE-7CM2 Adobe Standard Linear.dcp':
            camera_dcp = {'path': name, 'sha256': sha(raw), **dcp_info(raw)}
    return {'schema': 1, 'pack_directory_name': directory.name,
            'file_counts_by_suffix': dict(sorted(Counter(p.suffix.lower() for p in files).items())),
            'files': inventory, 'xmp_profiles_and_wrappers': looks, 'a7c2_linear_dcp': camera_dcp,
            'camera_accessed': False, 'network_accessed': False,
            'conclusion': 'Supplied pack uses Adobe RAW DCP + scene-referred XMP tables. No direct JPEG import or film-rendering equivalence claimed.',
            'needed_for_JPEG': 'A separately validated output-referred sRGB-to-sRGB CUBE bake, including the intended RAW/base-profile pipeline; extracting or renaming a table is insufficient.',
            'redistribution': 'Only metadata, names and measured digests emitted; no supplied profile or demo-image bytes copied.'}
