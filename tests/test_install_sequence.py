"""Installation must stop before later writes on every failed prerequisite."""
import contextlib
import io
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import MagicMock, patch

import app
import core


class InstallSequenceTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.job = app.Job(temp.name)
        self.job.state = {'phase': 'prepared', 'identity': {'model': core.MODEL}}
        self.job.load = MagicMock()
        self.job.save = lambda phase, **values: self.job.state.update(phase=phase, **values)
        originals = {key: b'original-' + str(key).encode() for key in core.CP_KEYS}
        self.job.units = lambda: (originals, {})
        self.records = [{key: SimpleNamespace(data=b'\0') for key in core.AP_KEYS},
                        {key: SimpleNamespace(data=value) for key, value in originals.items()}]
        self.job.module = MagicMock()
        self.job.files = MagicMock()
        self.job.settings = MagicMock()
        self.session = MagicMock()
        self.session.call.return_value = b'synthetic snapshot'
        self.session.__enter__.return_value = self.session
        self.factory = MagicMock(return_value=self.session)
        self.transport = SimpleNamespace(Session=self.factory)

    def install(self):
        with patch.dict(sys.modules, {'transport': self.transport}), \
                patch.object(core, 'backup_records', return_value=self.records), \
                contextlib.redirect_stdout(io.StringIO()):
            self.job.install(82)

    def test_failed_phase_does_not_open_session(self):
        self.job.state['phase'] = 'needs_review'
        with self.assertRaises(ValueError):
            self.install()
        self.factory.assert_not_called()
        self.job.module.assert_not_called()

    def test_changed_target_stops_before_any_native_call(self):
        self.records[0][core.AP_KEYS[0]].data = b'\x01'
        with self.assertRaises(ValueError):
            self.install()
        self.job.module.assert_not_called()
        self.job.settings.assert_not_called()

    def test_wbi_precheck_failure_stops_before_installer(self):
        self.job.module.side_effect = ValueError('WBI precheck failed')
        with self.assertRaisesRegex(ValueError, 'WBI precheck'):
            self.install()
        self.job.module.assert_called_once_with(self.session, 'wbi', 0)
        self.job.files.assert_not_called()
        self.job.settings.assert_not_called()
        self.assertEqual(self.job.state['phase'], 'prepared')

    def test_full_program_readback_failure_stops_settings_and_wbi_finish(self):
        self.job.files.side_effect = ValueError('Full-file mismatch')
        with self.assertRaisesRegex(ValueError, 'Full-file mismatch'):
            self.install()
        self.job.settings.assert_not_called()
        self.assertFalse(any(call.args[1:] == ('wbi', 1) for call in self.job.module.call_args_list))
        self.assertEqual(self.job.state['phase'], 'programs_written')

    def test_preservation_failure_stops_wbi_finish_and_completion(self):
        self.job.settings.side_effect = ValueError('Unrelated stored property changed')
        with self.assertRaisesRegex(ValueError, 'Unrelated stored property'):
            self.install()
        self.assertFalse(any(call.args[1:] == ('wbi', 1) for call in self.job.module.call_args_list))
        self.assertEqual(self.job.state['phase'], 'writing_settings')

    def test_finish_failure_is_not_recorded_as_completed(self):
        def module(session, name, mode):
            if (name, mode) == ('wbi', 1):
                raise TimeoutError('Unknown WBI finish outcome')
        self.job.module.side_effect = module
        with self.assertRaisesRegex(TimeoutError, 'Unknown WBI'):
            self.install()
        self.assertEqual(self.job.state['phase'], 'settings_verified')


if __name__ == '__main__':
    unittest.main()
