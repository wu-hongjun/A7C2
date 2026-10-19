"""Reject incompatible cached codec builds before compiling/linking anything."""
import hashlib
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
from camera_worker.build_heif import build

ROOT=Path(__file__).resolve().parents[2]
class CacheGuard(unittest.TestCase):
    def test_sanitizer_mode_cannot_relabel_release_libraries(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp);(p/'build.json').write_text(json.dumps({'host_variant':True,'sanitizers':False}))
            with patch('camera_worker.build_heif.subprocess.run') as run:
                with self.assertRaisesRegex(ValueError,'configuration mismatch'):
                    build(ROOT/'camera_worker/heif_sources',p,'/unused/cmake',host=True,worker_only=True,sanitize=True)
                run.assert_not_called()
    def test_changed_static_library_cannot_be_reused(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp);(p/'prefix/lib').mkdir(parents=True);(p/'prefix/lib/libheif.a').write_bytes(b'changed')
            (p/'build.json').write_text(json.dumps({'host_variant':True,'sanitizers':False,'static_library_sha256':{'libheif.a':hashlib.sha256(b'original').hexdigest()}}))
            with patch('camera_worker.build_heif.subprocess.run') as run:
                with self.assertRaisesRegex(ValueError,'library hash mismatch'):
                    build(ROOT/'camera_worker/heif_sources',p,'/unused/cmake',host=True,worker_only=True)
                run.assert_not_called()
if __name__=='__main__':unittest.main()
