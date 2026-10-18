"""Bounded, source-fingerprinted decoding of the supplied XMP RGBTable blobs.

Format semantics are checked against the user-supplied Adobe DNG SDK 1.7.1.
See research/FUJIFILM_SRGB_CONVERSION.md and the Adobe notice in NOTICE.md.
"""
from dataclasses import dataclass
import hashlib
from pathlib import Path
import re
import struct
from xml.etree import ElementTree as ET
import zlib

from .profiles import CRS, RDF, xmp_info
from .worker import read_limited, sha

# Recovered from this pack: little-endian base85, with the three XML-unsafe Z85
# characters replaced. Every supplied blob validates zlib checksum AND MD5.
ALPHABET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?`'|()[]{}@%$#"
REVERSE = {c: i for i, c in enumerate(ALPHABET)}
MAX_ENCODED_BYTES = 1024*1024
MAX_DECODED_BYTES = 1024*1024


def encode85(raw):
    out = []
    for i in range(0, len(raw), 4):
        block = raw[i:i+4]
        value = int.from_bytes(block, 'little')
        chars = []
        for _ in range(len(block)+1):
            value, digit = divmod(value, 85)
            chars.append(ALPHABET[digit])
        out.extend(chars)
    return ''.join(out)


def decode85(text):
    if not isinstance(text, str) or not 2 <= len(text) <= MAX_ENCODED_BYTES or len(text) % 5 == 1:
        raise ValueError('Invalid base85 length')
    out = bytearray()
    for i in range(0, len(text), 5):
        block = text[i:i+5]
        try:
            value = sum(REVERSE[c]*85**j for j, c in enumerate(block))
        except KeyError as exc:
            raise ValueError('Invalid Adobe base85 character') from exc
        if value >= 1 << (8*(len(block)-1)):
            raise ValueError('Noncanonical/overflowing base85 block')
        out.extend(value.to_bytes(len(block)-1, 'little'))
    raw = bytes(out)
    if encode85(raw) != text:
        raise ValueError('Noncanonical base85 data')
    return raw


def inflate_table(encoded):
    packed = decode85(encoded)
    if len(packed) < 6:
        raise ValueError('Truncated compressed table')
    expected, = struct.unpack_from('<I', packed)
    if not 1 <= expected <= MAX_DECODED_BYTES:
        raise ValueError('Table decompressed length exceeds bound')
    decoder = zlib.decompressobj()
    try:
        raw = decoder.decompress(packed[4:], expected+1)
    except zlib.error as exc:
        raise ValueError('Invalid zlib table') from exc
    if len(raw) != expected or not decoder.eof or decoder.unused_data or decoder.unconsumed_tail:
        raise ValueError('Table length/termination mismatch')
    return raw


@dataclass(frozen=True)
class DecodedTable:
    source_sha256: str
    fingerprint: str
    payload_sha256: str
    header: tuple
    footer_u32: tuple
    footer_f64: tuple
    sample_words: tuple

    def samples(self):
        """Undo uint16 delta prediction; SDK order is blue-fastest RGB triplets."""
        n = self.header[3]
        predictor = [(i*65535+(n >> 1))//(n-1) for i in range(n)]
        return tuple(tuple((self.sample_words[3*((r*n+g)*n+b)+c]+predictor[k]) & 65535
                           for c, k in enumerate((r, g, b)))
                     for r in range(n) for g in range(n) for b in range(n))

    def summary(self):
        return {'source_sha256': self.source_sha256, 'XMP_fingerprint_MD5': self.fingerprint,
                'payload_sha256': self.payload_sha256, 'header_u32': list(self.header),
                'footer_u32': list(self.footer_u32), 'footer_f64': list(self.footer_f64),
                'stored_sample_words': len(self.sample_words),
                'primaries': 'Display P3', 'transfer': 'sRGB', 'gamut': 'clip',
                'amount': 1.0, 'sample_order': 'blue-fastest, uint16 delta-predicted',
                'color_space_ID_mapping_verified': True,
                'Adobe_interpolation_semantics_verified': True,
                'native_interpolation': 'tetrahedral',
                'decoding_alone_validates_sRGB_conversion': False}


def decode_xmp(raw):
    info = xmp_info(raw)  # Shared XML size/DTD/entity guards.
    tree = ET.fromstring(raw)
    attrs = next(tree.iter(RDF+'Description')).attrib
    if attrs.get(CRS+'PresetType') != 'Look':
        raise ValueError('Expected an actual look profile, not a wrapper')
    fingerprint = attrs.get(CRS+'RGBTable', '')
    if not re.fullmatch('[0-9A-F]{32}', fingerprint):
        raise ValueError('Invalid RGBTable fingerprint')
    encoded = attrs.get(CRS+'Table_'+fingerprint)
    if encoded is None or len(info['embedded_tables']) != 1:
        raise ValueError('Expected one matching embedded RGB table')
    payload = inflate_table(encoded)
    # MD5 is the supplied format fingerprint, not a cryptographic trust claim.
    if hashlib.md5(payload).hexdigest().upper() != fingerprint:
        raise ValueError('RGBTable payload does not match XMP fingerprint')
    if len(payload) != 16 + 32**3*6 + 28:
        raise ValueError('Unsupported RGBTable payload size')
    header = struct.unpack_from('<4I', payload)
    if header != (1, 1, 3, 32):
        raise ValueError('Unsupported RGBTable schema/dimensions')
    footer_u32 = struct.unpack_from('<3I', payload, len(payload)-28)
    footer_f64 = struct.unpack_from('<2d', payload, len(payload)-16)
    if footer_u32 != (3, 1, 0) or footer_f64 != (1., 1.):
        raise ValueError('Unsupported RGBTable settings')
    return DecodedTable(sha(raw), fingerprint, sha(payload), header, footer_u32, footer_f64,
                        struct.unpack_from('<98304H', payload, 16))


def inspect_decoded_profiles(directory):
    directory = Path(directory)
    result = []
    for path in sorted(directory.glob('*.xmp')):
        raw = read_limited(path, MAX_ENCODED_BYTES)
        info = xmp_info(raw)
        if info.get('PresetType') == 'Look':
            result.append({'file': path.name, **decode_xmp(raw).summary()})
    if not result:
        raise ValueError('No embedded look tables found')
    return {'profiles': result, 'camera_accessed': False, 'network_accessed': False,
            'scope': 'Byte/fingerprint and SDK format semantics; bake checks are recorded separately.'}
