"""Offline review of one reported FL2/FL3 settings interruption.

This does not authorize camera I/O or modify core's preservation whitelist.
The two exceptions require independent byte/layout evidence, not just key names.
"""
import io
import re

import core


def _bk4_files(raw):
    from pmca.backup import BackupFile
    files = []
    for match in re.finditer(b'BK4\0', raw):
        try:
            files.append(BackupFile(io.BytesIO(raw[match.start() - 12:])))
        except Exception:
            continue
    if len(files) != 2:
        raise ValueError('Expected two checksum-valid BK4 files')
    return files


def review_pair(before_raw, after_raw, expected, *, allow_exact_live_delta=False):
    """Check every value/attribute/reset value, with two narrowly scoped proofs.

    The vendor parser's 13-byte reset read for array 02cf1288 reaches into
    02cf1289's data. Only that exact duplicate byte can follow the expected FL3
    binding. The separate observed +60 is permitted only when explicitly asked
    for this historical live-view review; it is not a decoded field meaning.
    """
    before = core.backup_records(before_raw)
    after = core.backup_records(after_raw)
    adjusted = [dict(chip) for chip in before]
    proofs = []
    key, owner = 0x02cf1288, 0x02cf1289
    old, new = before[0].get(key), after[0].get(key)
    if old is not None and new is not None and old.resetData != new.resetData:
        a, b = _bk4_files(before_raw)[0], _bk4_files(after_raw)[0]
        p, q = a._readProperty(key), b._readProperty(key)
        x, y = a._readProperty(owner), b._readProperty(owner)
        if (p != q or x != y or p.attr != 0xc0 or p.size != 13 or p.maxSize != 13
                or x.attr != 0x60 or x.size != 1 or x.maxSize != 1
                or x.ptr != p.ptr + p.maxSize + 1
                or old.attr != new.attr or old.data != new.data
                or old.resetData is None or new.resetData is None
                or len(old.resetData) != 13 or len(new.resetData) != 13
                or expected.get((0, owner)) != b'\x0b'
                or before[0][owner].data != b'\0' or after[0][owner].data != b'\x0b'
                or old.resetData[1:2] != before[0][owner].data
                or new.resetData[1:2] != after[0][owner].data
                or old.resetData[:1] != new.resetData[:1]
                or old.resetData[2:] != new.resetData[2:]):
            raise ValueError('Reset-data difference is not the proven FL3 array overread')
        adjusted[0][key] = old._replace(resetData=new.resetData)
        proofs.append({'kind': 'duplicate_parser_byte', 'array_key': hex(key),
                       'owner_key': hex(owner), 'reset_byte_index': 1})
    counter = 0x02770015
    old, new = before[0].get(counter), after[0].get(counter)
    if old is not None and new is not None and old.data != new.data and allow_exact_live_delta:
        if (old.attr != 2 or new.attr != 2 or old.resetData != new.resetData
                or len(old.data) != 4 or len(new.data) != 4
                or int.from_bytes(new.data, 'little') - int.from_bytes(old.data, 'little') != 60):
            raise ValueError('Live difference is not the exact reviewed +60')
        adjusted[0][counter] = old._replace(data=new.data)
        proofs.append({'kind': 'scoped_historical_live_delta', 'key': hex(counter), 'delta': 60})
    result = core.verify_preservation(adjusted, after, expected)
    result['scoped_proofs'] = proofs
    return result
