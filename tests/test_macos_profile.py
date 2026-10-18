"""Offline checks for the local Mac profile; no USB imports or commands."""
import contextlib
import io
from types import ModuleType, SimpleNamespace
import json
from pathlib import Path
import tempfile
import unittest
import gc
import weakref
from unittest.mock import MagicMock, patch

import macos_fl23


class MacProfileTests(unittest.TestCase):
    def test_context_outlives_raw_devices_and_parents_during_cleanup(self):
        events = []

        class RawDevice:
            def __del__(self):
                events.append('device_released')

        class Iterator:
            def __init__(self):
                self.devices = [RawDevice()]

            def __iter__(self):
                return iter(self.devices)

            def finalize(self):
                events.append('list_released')
                self.devices.clear()

        class Backend:
            def __init__(self, library):
                self.library = library

            def enumerate_devices(self):
                return Iterator()

            def get_parent(self, device):
                return RawDevice()

            def __del__(self):
                events.append('context_released')

        backend = macos_fl23.owned_context_backend(SimpleNamespace(_LibUSB=Backend), 'fixture')
        raw = list(backend.enumerate_devices())[0]
        parent = backend.get_parent(raw)
        self.assertEqual(events, ['list_released'])
        ref = weakref.ref(backend)
        del backend
        gc.collect()
        self.assertIsNotNone(ref())
        del raw
        gc.collect()
        self.assertIsNotNone(ref())
        del parent
        gc.collect()
        self.assertIsNone(ref())
        self.assertEqual(events, ['list_released', 'device_released', 'device_released', 'context_released'])

    def test_late_service_check_rejects_postwrite_failure_before_usb_import(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / 'state.json').write_text(json.dumps({
                'phase': 'needs_review', 'failed_action': 'install',
                'failed_during': 'writing_settings', 'backup_files': {}, 'journal': []}))
            cli = SimpleNamespace(private_path=MagicMock(return_value=root), camera_lock=MagicMock())
            with patch.object(macos_fl23.platform, 'system', return_value='Darwin'), \
                    patch.object(macos_fl23.os, 'geteuid', return_value=0):
                with self.assertRaisesRegex(ValueError, 'pre-backup entry failure'):
                    macos_fl23.check_late_service(cli, ['--failed-prepare', str(root), '--bus', '2', '--ports', '1'])
            cli.camera_lock.assert_not_called()

    def wait_fixture(self, devices):
        factory = patch.object(macos_fl23, 'owned_context_backend',
                               side_effect=lambda module, library: module._LibUSB(library))
        factory.start()
        self.addCleanup(factory.stop)
        original, fresh = MagicMock(), MagicMock()
        module = SimpleNamespace(get_backend=MagicMock(return_value=original),
                                 _LibUSB=MagicMock(return_value=fresh))
        find = MagicMock(return_value=devices)
        dispose = MagicMock()
        session = SimpleNamespace(
            usb=SimpleNamespace(backend=SimpleNamespace(libusb1=module),
                                core=SimpleNamespace(find=find),
                                util=SimpleNamespace(dispose_resources=dispose)),
            transport=SimpleNamespace(libusb_package=SimpleNamespace(find_library=MagicMock())),
            time=SimpleNamespace(monotonic=MagicMock(side_effect=[0, 61]), sleep=MagicMock()),
            UsbWaitTimeout=TimeoutError)
        return session, module, original, fresh, find, dispose

    def test_discovery_uses_new_context_and_retains_physical_port_check(self):
        device = MagicMock(bus=2, port_numbers=(1,))
        device.__iter__.return_value = iter([object()])
        session, module, original, fresh, find, dispose = self.wait_fixture([device])
        self.assertIs(macos_fl23.fresh_context_wait(session)(0x0336, (2, (1,)), 60), device)
        module._LibUSB.assert_called_once_with(original.lib)
        self.assertIs(find.call_args.kwargs['backend'], fresh)
        dispose.assert_not_called()

    def test_discovery_refuses_wrong_port_and_multiple_devices(self):
        for devices, error in [([MagicMock(bus=2, port_numbers=(9,))], 'physical port'),
                               ([MagicMock(), MagicMock()], 'Multiple')]:
            session, *_ = self.wait_fixture(devices)
            with self.assertRaisesRegex(RuntimeError, error):
                macos_fl23.fresh_context_wait(session)(0x0336, (2, (1,)), 60)

    def test_discovery_timeout_has_no_device_commands_or_reset(self):
        session, _, _, _, _, dispose = self.wait_fixture([])
        with self.assertRaisesRegex(TimeoutError, '0336'):
            macos_fl23.fresh_context_wait(session)(0x0336, (2, (1,)), 60)
        dispose.assert_not_called()
        session.time.sleep.assert_not_called()

    def test_late_device_is_found_by_another_scan_without_hotplug_notification(self):
        device = MagicMock(bus=2, port_numbers=(1,))
        device.__iter__.return_value = iter([object()])
        session, module, _, first_context, find, _ = self.wait_fixture([])
        second_context = MagicMock()
        module._LibUSB.side_effect = [first_context, second_context]
        find.side_effect = [[], [device]]
        session.time.monotonic.side_effect = [0, 0]
        self.assertIs(macos_fl23.fresh_context_wait(session)(0x0336, (2, (1,)), 60), device)
        self.assertEqual(module._LibUSB.call_count, 2)
        self.assertIs(find.call_args_list[0].kwargs['backend'], first_context)
        self.assertIs(find.call_args_list[1].kwargs['backend'], second_context)

    def instrument(self, version):
        backend = MagicMock()
        original_wait = MagicMock(return_value='synthetic-device')
        session = SimpleNamespace(
            read_identity=MagicMock(return_value=({'model': 'ILCE-7CM2'}, backend)),
            wait_pid=original_wait,
            transport=SimpleNamespace(MscBbbDriver=MagicMock(), SonyMscExtCmdDevice=MagicMock()),
            core=SimpleNamespace(VERSION='2.01'),
        )
        updater = MagicMock()
        updater.getFirmwareVersion.return_value = (version, None)
        fake_sony = ModuleType('pmca.usb.sony')
        fake_sony.SonyUpdaterCamera = MagicMock(return_value=updater)
        with patch.dict('sys.modules', {'pmca.usb.sony': fake_sony}):
            macos_fl23.instrument_session(session)
        return session, updater, original_wait

    def test_wrong_version_stops_before_service_wait(self):
        session, updater, wait = self.instrument('3.00')
        with self.assertRaisesRegex(ValueError, 'Unsupported camera firmware'):
            session.read_identity('synthetic-device')
        self.assertEqual([call[0] for call in updater.method_calls], ['init', 'getFirmwareVersion'])
        wait.assert_not_called()

    def test_supported_version_preserves_identity_and_same_port_wait(self):
        session, updater, wait = self.instrument('2.01')
        with contextlib.redirect_stdout(io.StringIO()):
            identity, backend = session.read_identity('synthetic-device')
            self.assertEqual(identity, {'model': 'ILCE-7CM2'})
            backend.reset.assert_called_once()
            session.wait_pid(0x0336, ('original-port',))
            wait.assert_called_once_with(0x0336, ('original-port',), 60)
            wait.reset_mock()
            session.wait_pid(0x0e8a, ('original-port',), 60)
            wait.assert_called_once_with(0x0e8a, ('original-port',), 60)
        self.assertEqual([call[0] for call in updater.method_calls], ['init', 'getFirmwareVersion'])


if __name__ == '__main__':
    unittest.main()
