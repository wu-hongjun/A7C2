"""Host-only native-job format and preparation checks with synthetic assets."""
import hashlib
import io
import json
from pathlib import Path
import struct
import tempfile
import unittest
from xml.etree import ElementTree as ET

from PIL import Image, ImageCms
from camera_worker.pack import lut_binary, pack_job
from photo_worker.adobe_table import decode_xmp
from photo_worker.fujifilm import clf_bytes
from photo_worker.tests.test_adobe_table import profile
from photo_worker.tests.test_worker import cube_text
from photo_worker.worker import prepare,SRGB_CONTRACT,EXIF_IFD

ROOT=Path(__file__).resolve().parents[2]


class NativePackingTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup)
        self.root=Path(self.temp.name);self.input=self.root/'photo.jpg';self.clf=self.root/'identity.clf'
        Image.new('RGB',(48,32),(80,120,200)).save(self.input,quality=100,subsampling=0)
        self.clf.write_bytes(clf_bytes(decode_xmp(profile())))

    def host(self,**changes):
        options=dict(cube=self.clf,lut_contract=SRGB_CONTRACT,completed=True,assume_srgb=True,
                     lens_presets=ROOT/'photo_worker/lens-presets.example.json',lens_id='example-helios-58',aperture=4)
        options.update(changes);prepare(self.input,self.root/'host',**options)
        return self.root/'host'

    def test_header_hashes_layout_and_immutable_lens_snapshot(self):
        before=self.input.read_bytes();host=self.host();native=self.root/'native'
        record=pack_job(host,native);header=(native/'job.bin').read_bytes()
        self.assertEqual(len(header),176);self.assertEqual(header[:8],b'A7JOBS01')
        self.assertEqual(header[144:],hashlib.sha256(header[:144]).digest())
        self.assertEqual(struct.unpack_from('<IIIII',header,8),(1,1,95,48,32))
        self.assertEqual((native/'look.bin').stat().st_size,426152)
        for name,offset in [('input.jpg',48),('metadata.exif',80),('look.bin',112)]:
            self.assertEqual(hashlib.sha256((native/name).read_bytes()).digest(),header[offset:offset+32])
        exif=Image.Exif();exif.load((native/'metadata.exif').read_bytes())
        self.assertEqual(exif.get_ifd(EXIF_IFD)[0xa434],'44-2 58mm f/2')
        self.assertEqual(record['lens']['focal_length_mm'],58)
        self.assertEqual(self.input.read_bytes(),before)

    def test_lens_only_unknown_aperture(self):
        native=self.root/'native';pack_job(self.host(cube=None,lut_contract=None,aperture=None),native)
        self.assertFalse((native/'look.bin').exists());header=(native/'job.bin').read_bytes()
        self.assertEqual(struct.unpack_from('<I',header,12)[0],0)
        self.assertEqual(header[112:144],hashlib.sha256(b'').digest())
        exif=Image.Exif();exif.load((native/'metadata.exif').read_bytes())
        self.assertNotIn(0x829d,exif.get_ifd(EXIF_IFD))

    def test_cannot_reuse_directory_or_export_tampered_job(self):
        host=self.host();native=self.root/'native';pack_job(host,native)
        with self.assertRaises(FileExistsError):pack_job(host,native)
        (host/'look.clf').write_bytes(b'changed')
        with self.assertRaises(ValueError):pack_job(host,self.root/'bad')
        self.assertFalse((self.root/'bad').exists())

    def test_cube_declined_before_reservation(self):
        cube=self.root/'look.cube';cube.write_text(cube_text())
        with self.assertRaisesRegex(ValueError,'CUBE'):
            pack_job(self.host(cube=cube),self.root/'native')
        self.assertFalse((self.root/'native').exists())

    def test_explicit_host_normalization_orientation_and_profile(self):
        exif=Image.Exif();exif[274]=6
        srgb=ImageCms.ImageCmsProfile(ImageCms.createProfile('sRGB')).tobytes()
        Image.new('RGB',(48,32),(80,120,200)).save(self.input,exif=exif,icc_profile=srgb,progressive=True)
        host=self.host();native=self.root/'native'
        with self.assertRaisesRegex(ValueError,'normalize'):pack_job(host,native)
        self.assertFalse(native.exists())
        record=pack_job(host,native,normalize=True)
        self.assertTrue(record['extra_JPEG_reencode']);self.assertEqual(record['dimensions'],[32,48])
        with Image.open(native/'input.jpg') as im:
            self.assertEqual(im.size,(32,48));self.assertFalse(im.info.get('progressive'))
        out=Image.Exif();out.load((native/'metadata.exif').read_bytes());self.assertEqual(out[274],1)

    def test_native_rejects_distinct_transfer_stages(self):
        raw=self.clf.read_bytes();root=ET.fromstring(raw)
        root[7][0].text=root[7][0].text.replace('0\n','0.1\n',1)
        with self.assertRaisesRegex(ValueError,'shared'):lut_binary(ET.tostring(root))


if __name__=='__main__':unittest.main()
