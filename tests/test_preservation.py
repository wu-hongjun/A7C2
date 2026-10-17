"""Regression cases for complete live/storage/reset-data preservation."""
from collections import namedtuple
import copy
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import app
import core

P=namedtuple('Property','data attr resetData')


class PreservationTests(unittest.TestCase):
    def test_unrelated_reset_change_rejected(self):
        with self.assertRaises(ValueError):
            core.verify_preservation([{1:P(b'same',0x74,b'old')},{}],
                                     [{1:P(b'same',0x74,b'new')},{}],{})

    def test_target_reset_change_rejected(self):
        with self.assertRaises(ValueError):
            core.verify_preservation([{1:P(b'old',0x74,b'reset')},{}],
                                     [{1:P(b'new',0x74,b'changed')},{}],{(0,1):b'new'})

    def test_counter_reset_change_rejected(self):
        key=0x01070000
        a=P((1).to_bytes(8,'little'),0x74,b'old')
        b=P((2).to_bytes(8,'little'),0x74,b'new')
        with self.assertRaises(ValueError):core.verify_preservation([{key:a},{}],[{key:b},{}],{})

    def test_existing_counter_allowance_retained(self):
        key=0x01070000
        a=P((1).to_bytes(8,'little'),0x74,b'reset')
        b=P((2).to_bytes(8,'little'),0x74,b'reset')
        self.assertEqual(core.verify_preservation([{key:a},{}],[{key:b},{}],{})['runtime_counters'],[hex(key)])

    def test_missing_target_rejected(self):
        with self.assertRaises(ValueError):core.verify_preservation([{},{}],[{},{}],{(1,7):b'new'})


class SettingsTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup)
        self.job=app.Job(self.temp.name);self.job.state={'journal':[]}
        self.originals=dict(zip(core.CP_KEYS,[b'original chroma',b'original matrix']))
        self.merged=dict(zip(core.CP_KEYS,[b'new chroma',b'new matrix']))
        self.job.units=lambda:(self.originals,self.merged)
        self.unrelated=0x02a40002;self.outside=0x04000001

    def exercise(self,restore=False,change=None):
        start=self.merged if restore else self.originals
        target=self.originals if restore else self.merged
        ap_start=[b'\x0a',b'\x0b'] if restore else [b'\0',b'\0']
        ap_target=[b'\0',b'\0'] if restore else [b'\x0a',b'\x0b']
        before=[{k:P(v,0x60,b'\0') for k,v in zip(core.AP_KEYS,ap_start)},
                {k:P(v,8,None) for k,v in start.items()}]
        before[0][self.outside]=P(b'live pending',0x60,b'reset')
        before[1][self.unrelated]=P(b'keep',0x74,b'keep reset')
        stored_before=copy.deepcopy(before)
        stored_before[0][self.outside]=P(b'previously saved',0x60,b'reset')
        after=copy.deepcopy(before)
        for k,v in target.items():after[1][k]=P(v,8,None)
        for k,v in zip(core.AP_KEYS,ap_target):after[0][k]=P(v,0x60,b'\0')
        stored_after=copy.deepcopy(after)
        stored_after[0][self.outside]=stored_before[0][self.outside]
        views=[before,stored_before,after,stored_after]
        if change:change(*views)
        snapshots={bytes([i]):v for i,v in enumerate(views)}
        owner=self

        class Camera:
            def __init__(self):self.reads={True:0,False:0};self.ap=dict(zip(core.AP_KEYS,ap_start))
            def getBackupPresetData(self,memory):
                index=(0 if memory else 1)+2*self.reads[memory];self.reads[memory]+=1
                return bytes([index])
            def readBackup(self,key):return self.ap[key]
            def saveBackup(self,subsystem):owner.operations.append(('save',subsystem))

        class Session:
            def __init__(self):self.camera=Camera();self.cp=start.copy()
            def call(self,method,*args):return method(*args)
            def cp_read(self,key):return self.cp[key]
            def cp_write(self,key,data):owner.operations.append(('cp',key));self.cp[key]=data
            def ap_write(self,key,data):owner.operations.append(('ap',key));self.camera.ap[key]=data

        self.operations=[]
        with patch.object(core,'backup_records',side_effect=lambda raw:snapshots[raw]):
            self.job.settings(Session(),restore)

    def test_install_preserves_independent_memory_storage_baselines(self):
        self.exercise()
        self.assertEqual(len(self.operations),6)
        self.assertEqual(self.job.state['journal'][-1]['step'],'settings_storage_verified')
        self.assertIn('stored_preservation',self.job.state['journal'][-1])

    def test_restore_preserves_independent_memory_storage_baselines(self):self.exercise(True)

    def test_unrelated_stored_change_rejected_install_and_restore(self):
        for restore in [False,True]:
            with self.subTest(restore=restore):
                def corrupt(a,b,c,d):d[1][self.unrelated]=P(b'corrupted',0x74,b'keep reset')
                with self.assertRaises(ValueError):self.exercise(restore,corrupt)

    def test_unrelated_stored_attribute_change_rejected(self):
        def corrupt(a,b,c,d):d[1][self.unrelated]=d[1][self.unrelated]._replace(attr=0)
        with self.assertRaises(ValueError):self.exercise(change=corrupt)

    def test_target_stored_reset_change_rejected(self):
        def corrupt(a,b,c,d):d[0][core.AP_KEYS[0]]=d[0][core.AP_KEYS[0]]._replace(resetData=b'\xff')
        with self.assertRaises(ValueError):self.exercise(change=corrupt)

    def test_unrelated_pending_saved_subsystem_edit_rejected_before_writes(self):
        def pending(a,b,c,d):b[1][self.unrelated]=b[1][self.unrelated]._replace(data=b'stored old')
        with self.assertRaises(ValueError):self.exercise(change=pending)
        self.assertEqual(self.operations,[])

    def test_pending_reset_difference_rejected_before_writes(self):
        def pending(a,b,c,d):b[1][self.unrelated]=b[1][self.unrelated]._replace(resetData=b'other')
        with self.assertRaises(ValueError):self.exercise(change=pending)
        self.assertEqual(self.operations,[])

    def test_outside_subsystem_storage_change_is_still_rejected(self):
        def corrupt(a,b,c,d):d[0][self.outside]=d[0][self.outside]._replace(data=b'changed saved')
        with self.assertRaises(ValueError):self.exercise(change=corrupt)


if __name__=='__main__':unittest.main()
