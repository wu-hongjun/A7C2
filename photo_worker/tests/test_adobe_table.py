"""Independent synthetic carrier/fingerprint and malformed-input checks."""
import hashlib
import random
import struct
import unittest
import zlib

from photo_worker.adobe_table import decode85, encode85, inflate_table, decode_xmp


def profile(payload=None, *, footer=(3, 1, 0, 1., 1.)):
    if payload is None:
        payload = struct.pack('<4I', 1, 1, 3, 32)+bytes(32**3*6)+struct.pack('<III2d', *footer)
    fingerprint = hashlib.md5(payload).hexdigest().upper()
    encoded = encode85(struct.pack('<I', len(payload))+zlib.compress(payload))
    return (f'<x xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" '
            f'xmlns:crs="http://ns.adobe.com/camera-raw-settings/1.0/">'
            f'<rdf:Description crs:PresetType="Look" crs:RGBTable="{fingerprint}" '
            f'crs:Table_{fingerprint}="{encoded}"/></x>').encode()


class AdobeCarrierTests(unittest.TestCase):
    def test_hand_calculated_base85_vectors(self):
        # Arithmetic independent of encode85's implementation: digits increase
        # in significance left-to-right, and decoded words are little-endian.
        for text, expected in [('00', b'\0'), ('10', b'\1'), ('010', b'\x55\0'),
                               ('0010', b'\x39\x1c\0'), ('Lir00', b'\x2c\0\x03\0')]:
            self.assertEqual(decode85(text), expected)
            self.assertEqual(encode85(expected), text)

    def test_base85_roundtrip_partial_words(self):
        rng = random.Random(7321)
        for length in range(1, 129):
            raw = bytes(rng.randrange(256) for _ in range(length))
            self.assertEqual(decode85(encode85(raw)), raw)

    def test_invalid_base85_inputs(self):
        for text in ('', '0', '000000', '00 ', '00~', '#####', '##', '000\n'):
            with self.assertRaises(ValueError): decode85(text)

    def test_compression_size_trailing_bytes_and_bomb_bounds(self):
        raw = b'one explicitly bounded table'
        self.assertEqual(inflate_table(encode85(struct.pack('<I', len(raw))+zlib.compress(raw))), raw)
        for packed in (struct.pack('<I', 2**32-1)+zlib.compress(raw),
                       struct.pack('<I', 1)+zlib.compress(raw),
                       struct.pack('<I', len(raw))+zlib.compress(raw)+b'extra',
                       struct.pack('<I', len(raw))+zlib.compress(raw)[:-1], b'bad'):
            with self.assertRaises(ValueError): inflate_table(encode85(packed))

    def test_payload_schema_and_fingerprint(self):
        table = decode_xmp(profile())
        self.assertEqual(table.header, (1, 1, 3, 32))
        self.assertEqual(len(table.sample_words), 98304)
        self.assertFalse(table.summary()['decoding_alone_validates_sRGB_conversion'])
        self.assertTrue(table.summary()['color_space_ID_mapping_verified'])
        bad = profile().replace(b'crs:RGBTable="', b'crs:RGBTable="A', 1)
        with self.assertRaises(ValueError): decode_xmp(bad)
        bad = profile().replace(b'crs:PresetType="Look"', b'crs:PresetType="Normal"')
        with self.assertRaises(ValueError): decode_xmp(bad)

    def test_unsupported_settings_and_shapes(self):
        for footer in ((2, 1, 0, 1., 1.), (3, 0, 0, 1., 1.), (3, 1, 0, 0., 1.)):
            with self.assertRaises(ValueError): decode_xmp(profile(footer=footer))
        for payload in (b'bad', struct.pack('<4I', 1, 2, 3, 32)+bytes(32**3*6)+struct.pack('<III2d', 3, 1, 0, 1., 1.)):
            with self.assertRaises(ValueError): decode_xmp(profile(payload))


if __name__ == '__main__': unittest.main()
