"""Recheck historical preservation counterexamples without USB or camera data."""
from collections import namedtuple
import copy
import json
from pathlib import Path
import sys
import tempfile
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import app
import core

Property = namedtuple("Property", "data attr resetData")


def main():
    before = [{1: Property(b"same", 0x74, b"original reset")}, {}]
    after = [{1: Property(b"same", 0x74, b"changed reset")}, {}]
    try:
        result = core.verify_preservation(before, after, {})
        reset_gap = result["preserved_properties"] == 1
    except ValueError:reset_gap=False

    cp0, cp1 = core.CP_KEYS
    ap0, ap1 = core.AP_KEYS
    unrelated = 0x02a40002
    originals = {cp0: b"original chroma", cp1: b"original matrix"}
    merged = {cp0: b"new chroma", cp1: b"new matrix"}
    memory_before = [{ap0: Property(b"\0", 0x74, b"\0"),
                      ap1: Property(b"\0", 0x74, b"\0")},
                     {cp0: Property(originals[cp0], 0, None),
                      cp1: Property(originals[cp1], 0, None),
                      unrelated: Property(b"original unrelated unit", 0, None)}]
    memory_after = copy.deepcopy(memory_before)
    for key, data in merged.items():
        memory_after[1][key] = Property(data, 0, None)
    for key, data in [(ap0, b"\x0a"), (ap1, b"\x0b")]:
        memory_after[0][key] = Property(data, 0x74, b"\0")
    stored_after = copy.deepcopy(memory_after)
    stored_after[1][unrelated] = Property(b"changed unrelated unit", 0, None)

    class Camera:
        def __init__(self):
            self.reads = 0
            self.stored_reads=0
            self.ap = {ap0: b"\0", ap1: b"\0"}

        def getBackupPresetData(self, from_memory):
            if not from_memory:
                self.stored_reads+=1
                return b"stored-before" if self.stored_reads==1 else b"stored"
            self.reads += 1
            return b"before" if self.reads == 1 else b"after"

        def readBackup(self, key):
            return self.ap[key]

        def saveBackup(self, subsystem):
            pass

    class Session:
        def __init__(self):
            self.camera = Camera()
            self.cp = originals.copy()

        def call(self, method, *args):
            return method(*args)

        def cp_read(self, key):
            return self.cp[key]

        def cp_write(self, key, data):
            self.cp[key] = data

        def ap_write(self, key, data):
            self.camera.ap[key] = data

    snapshots = {b"before": memory_before, b"stored-before":copy.deepcopy(memory_before),
                 b"after": memory_after, b"stored": stored_after}
    with tempfile.TemporaryDirectory() as folder:
        job = app.Job(folder)
        job.state = {"journal": []}
        job.units = lambda: (originals, merged)
        with patch.object(core, "backup_records", side_effect=lambda raw: snapshots[raw]):
            try:job.settings(Session())
            except ValueError:pass
        stored_gap = any(row["step"] == "settings_storage_verified" for row in job.state["journal"])
    if reset_gap or stored_gap:raise AssertionError('Preservation guard regression')
    print(json.dumps({"camera_accessed": False,
        "reset_data_change_accepted_as_preserved": reset_gap,
        "unrelated_stored_cp_unit_change_accepted_as_storage_verified": stored_gap,
        "unrelated_stored_unit": hex(unrelated),
        "interpretation": "Guard counterexamples, not observations of actual camera corruption"}, indent=2))


if __name__ == "__main__":
    main()
