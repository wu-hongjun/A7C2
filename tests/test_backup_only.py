"""The backup workflow must remain usable without any camera-side writes."""
import copy
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
from collections import namedtuple
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import backup_camera
import core


class BackupTests(unittest.TestCase):
    def test_backup_workflow_uses_only_reads_and_verifies_two_copies(self):
        P=namedtuple('Property','data attr resetData')
        records=[{k:P(bytes([i]),0,None) for i,k in enumerate(core.AP_KEYS)},
                 {k:P(bytes([i+4]),0,None) for i,k in enumerate(core.CP_KEYS)}]
        operations=[]
        class Camera:
            def getBackupPresetData(self,memory):operations.append(('backup',memory));return b'memory' if memory else b'storage'
            def readBackup(self,key):operations.append(('read_binding',key));return records[0][key].data
        class Session:
            def __init__(self,expected):self.camera=Camera()
            def __enter__(self):return self
            def __exit__(self,*args):pass
            def call(self,method,*args):return method(*args)
            def read(self,path):operations.append(('read_program',path));return path.encode()
            def cp_read(self,key):operations.append(('read_cp',key));return records[1][key].data
        identity={'model':core.MODEL,'firmware_version':'2.01','serial_sha256':'not a raw serial'}
        with tempfile.TemporaryDirectory() as folder,patch.object(core,'backup_records',return_value=records):
            copies=backup_camera.backup(Path(folder)/'one',Path(folder)/'two',identity,Session)
            self.assertEqual(copies.manifest['phase'],'backups_verified_no_installation')
            self.assertEqual(len(copies.manifest['files']),8)
            for name in copies.manifest['files']:
                self.assertEqual((copies.root/name).read_bytes(),(copies.second/name).read_bytes())
            path=copies.second/'settings-storage.private.bin';path.write_bytes(b'corruption')
            with self.assertRaises(ValueError):copies.verify()
        self.assertEqual(len(operations),8)
        self.assertTrue(all(row[0] in ['backup','read_binding','read_program','read_cp'] for row in operations))

    def test_wrong_firmware_does_not_open_service(self):
        identity={'model':core.MODEL,'firmware_version':'9.99','serial_sha256':'hash'}
        def forbidden(*args):raise AssertionError('Must not open USB')
        with self.assertRaises(ValueError):backup_camera.backup('unused-one','unused-two',identity,forbidden)

    def test_backup_paths_are_separate_and_not_overwritten(self):
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder)/'one'
            with self.assertRaises(ValueError):backup_camera.Copies(root,root/'child',{})
            root.mkdir();(root/'existing').write_text('keep')
            with self.assertRaises(ValueError):backup_camera.Copies(root,Path(folder)/'two',{})


if __name__=='__main__':unittest.main()
