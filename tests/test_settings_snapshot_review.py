"""Counter scope and pointer-proven duplicate reads must fail closed."""
from collections import namedtuple
import copy
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from research import settings_snapshot_review as review
import core

P = namedtuple('Property', 'attr data resetData')


class SnapshotReviewTests(unittest.TestCase):
    def fixture(self):
        old = [{0x02cf1288: P(0xc0, b'a' * 13, bytes(13)),
                0x02cf1289: P(0x60, b'\0', b'\0'),
                0x02770015: P(2, (100).to_bytes(4, 'little'), None),
                7: P(0x74, b'keep', b'reset')}, {}]
        new = copy.deepcopy(old)
        new[0][0x02cf1289] = new[0][0x02cf1289]._replace(data=b'\x0b')
        new[0][0x02cf1288] = new[0][0x02cf1288]._replace(resetData=b'\0\x0b' + bytes(11))
        new[0][0x02770015] = new[0][0x02770015]._replace(data=(160).to_bytes(4, 'little'))
        layout = {0x02cf1288: SimpleNamespace(ptr=100, attr=0xc0, size=13, maxSize=13),
                  0x02cf1289: SimpleNamespace(ptr=114, attr=0x60, size=1, maxSize=1)}
        return old, new, layout

    def run_review(self, before, after, layout, allow=True):
        parser = SimpleNamespace(_readProperty=lambda key: layout[key])
        with patch.object(core, 'backup_records', side_effect=[before, after]), \
                patch.object(review, '_bk4_files', return_value=[parser, object()]):
            return review.review_pair(b'before', b'after', {(0, 0x02cf1289): b'\x0b'},
                                      allow_exact_live_delta=allow)

    def test_exact_alias_and_live_delta_do_not_change_global_allowance(self):
        before, after, layout = self.fixture()
        result = self.run_review(before, after, layout)
        self.assertEqual(len(result['scoped_proofs']), 2)
        self.assertFalse(core.runtime_counter(0, 0x02770015, before[0][0x02770015], after[0][0x02770015]))
        self.assertEqual(before[0][0x02cf1288].resetData, bytes(13))

    def test_stored_review_cannot_use_live_delta(self):
        with self.assertRaises(ValueError):
            self.run_review(*self.fixture(), allow=False)

    def test_rejects_unproven_pointer_reset_bytes_attributes_or_unrelated_change(self):
        for problem in ('pointer', 'reset_byte', 'target_reset', 'attribute', 'other_property', 'delta'):
            with self.subTest(problem=problem):
                before, after, layout = self.fixture()
                if problem == 'pointer': layout[0x02cf1289].ptr += 1
                if problem == 'reset_byte': after[0][0x02cf1288] = after[0][0x02cf1288]._replace(resetData=b'X\x0b' + bytes(11))
                if problem == 'target_reset': after[0][0x02cf1289] = after[0][0x02cf1289]._replace(resetData=b'X')
                if problem == 'attribute': after[0][0x02770015] = after[0][0x02770015]._replace(attr=8)
                if problem == 'other_property': after[0][7] = after[0][7]._replace(resetData=b'changed')
                if problem == 'delta': after[0][0x02770015] = after[0][0x02770015]._replace(data=(161).to_bytes(4, 'little'))
                with self.assertRaises(ValueError):
                    self.run_review(before, after, layout)


if __name__ == '__main__':
    unittest.main()
