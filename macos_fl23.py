"""Use MDHan's pinned Mac USB/workflow adapter with this repo's fixed payload.

The downloaded upstream release and adapter stay unchanged and checksum checked.
Only the engine's module-provider seam selects our separately validated app/core/
transport. No firmware fingerprints, state gates or native checks are relaxed.
"""
import argparse
import hashlib
import importlib
import json
import os
from pathlib import Path
import platform
import sys

ROOT = Path(__file__).resolve().parent
ADAPTER = ROOT / 'research/build/a7c2-fl23-macos'
LOCK = ROOT / 'research/toolchains/macos-adapter.json'


def owned_context_backend(backend_module, library):
    """Keep a fresh context alive until its last raw device is released.

    PyUSB's raw _Device normally relies on the singleton backend's lifetime.
    A ResourceManager releases its backend attribute before its raw device
    attribute, so a non-singleton context needs an owner on the raw device too.
    """
    class OwnedContextBackend(backend_module._LibUSB):
        def enumerate_devices(self):
            iterator = super().enumerate_devices()
            try:
                for device in iterator:
                    device._local_context_owner = self
                    yield device
            finally:
                # Release the enumeration list while this context is alive.
                iterator.finalize()

        def get_parent(self, device):
            parent = super().get_parent(device)
            if parent is not None:
                parent._local_context_owner = self
            return parent

    return OwnedContextBackend(library)


def fresh_context_wait(session):
    """Rescan after PID changes without reusing PyUSB's singleton context.

    PyUSB 1.3.1 caches get_backend(); constructing its pinned backend creates a
    new libusb context, whose Darwin initialization explicitly scans IORegistry.
    Devices retain the backend, keeping its context alive through their use.
    """
    def wait_pid(pid, location, seconds=15):
        backend_module = session.usb.backend.libusb1
        original = backend_module.get_backend(find_library=session.transport.libusb_package.find_library)
        if original is None:
            raise RuntimeError('libusb backend unavailable')
        deadline = session.time.monotonic() + seconds
        while True:
            # A device can arrive after the initial scan. Rescan on every poll,
            # rather than depending on the same possibly missed hotplug event.
            backend = owned_context_backend(backend_module, original.lib)
            found = list(session.usb.core.find(find_all=True, idVendor=0x054c, idProduct=pid, backend=backend))
            if len(found) > 1:
                raise RuntimeError('Multiple target USB devices; refusing ambiguous handoff')
            if found:
                device = found[0]
                if (device.bus, tuple(device.port_numbers or ())) != location:
                    session.usb.util.dispose_resources(device)
                    raise RuntimeError('USB physical port changed')
                if not list(device):
                    session.usb.util.dispose_resources(device)
                    raise RuntimeError('Expected USB device has no configuration')
                return device
            if session.time.monotonic() >= deadline:
                raise session.UsbWaitTimeout('Expected USB identity did not appear: %04x' % pid)
            session.time.sleep(0.5)
    return wait_pid


def instrument_session(session):
    """Confirm the firmware before service entry; tolerate slow enumeration.

    These updater calls are INIT and QUERY_VERSION only. They never switch into
    updater mode, send firmware data or complete an update. Existing USB port,
    identity, phase and original-program checks remain mandatory.
    """
    from pmca.usb.sony import SonyUpdaterCamera
    original_identity = session.read_identity
    original_wait = session.wait_pid

    def read_identity(device):
        identity, backend = original_identity(device)
        backend.reset()
        driver = session.transport.MscBbbDriver(backend)
        updater = SonyUpdaterCamera(session.transport.SonyMscExtCmdDevice(driver))
        updater.init()
        version, _ = updater.getFirmwareVersion()
        if version != session.core.VERSION:
            raise ValueError('Unsupported camera firmware: ' + version)
        print('Camera model and firmware confirmed: ' + identity['model'] + ' ' + version, flush=True)
        return identity, backend

    def wait_pid(pid, location, seconds=15):
        if pid == 0x0336:
            seconds = max(seconds, 60)
            print('First authentication finished; waiting up to 60s for service USB 054c:0336.', flush=True)
        device = original_wait(pid, location, seconds)
        print('Observed expected USB identity: 054c:%04x.' % pid, flush=True)
        return device

    session.read_identity = read_identity
    session.wait_pid = wait_pid


def configure():
    sys.dont_write_bytecode = True
    metadata = json.loads(LOCK.read_text())
    for name, expected in metadata['files'].items():
        path = ADAPTER / name
        if path.is_symlink() or hashlib.sha256(path.read_bytes()).hexdigest() != expected:
            raise ValueError('Pinned Mac adapter mismatch: ' + name)
    sys.path.insert(0, str(ADAPTER))
    engine = importlib.import_module('macfl23.engine')
    engine.verify()  # Also verify every unchanged original upstream release file.
    sys.path.insert(0, str(ROOT))
    import app
    import core
    import transport
    core.assets()  # Our fixed native installer, patch table and manifest.
    for name, module in [('app', app), ('core', core), ('transport', transport)]:
        if Path(module.__file__).resolve() != ROOT / (name + '.py'):
            raise ValueError('Wrong local module provider: ' + name)
    engine.load = lambda: (app, core, transport)
    cli = importlib.import_module('macfl23.cli')
    session = importlib.import_module('macfl23.session')
    session.wait_pid = fresh_context_wait(session)
    instrument_session(session)
    cli.ROOT = ROOT  # Keep private camera data outside our public repository too.
    return cli


def check_late_service(cli, arguments):
    """Test only the already-observed late service device from failed prepare."""
    parser = argparse.ArgumentParser(description='Read-only late service authentication check')
    parser.add_argument('--failed-prepare', type=Path, required=True)
    parser.add_argument('--bus', type=int, required=True)
    parser.add_argument('--ports', required=True)
    args = parser.parse_args(arguments)
    if platform.system() != 'Darwin' or os.geteuid() != 0:
        raise RuntimeError('Run service-check with sudo on macOS.')
    state_path = cli.private_path(args.failed_prepare, existing=True) / 'state.json'
    state = json.loads(state_path.read_text())
    if (state.get('phase') != 'needs_review' or state.get('failed_action') != 'prepare'
            or state.get('failed_during') != 'preparing' or state.get('backup_files') != {}
            or state.get('last_error') != 'UsbWaitTimeout: Expected USB identity did not appear: 0336'
            or any(row.get('step') != 'mac_usb_session_exit' for row in state.get('journal', []))):
        raise ValueError('This service check applies only to the reviewed pre-backup entry failure.')
    expected_port = args.bus, tuple(int(part) for part in args.ports.split(','))
    if not expected_port[1]:
        raise ValueError('Missing known physical port')
    from macfl23 import session
    with cli.camera_lock():
        backend = session.usb.backend.libusb1.get_backend(
            find_library=session.transport.libusb_package.find_library)
        if list(session.usb.core.find(find_all=True, idVendor=0x054c, idProduct=0x0e8a, backend=backend)):
            raise RuntimeError('Normal camera identity is present; this check requires the observed late service device.')
        device = session.transport.get_device(0x0336)
        if (device.bus, tuple(device.port_numbers or ())) != expected_port:
            raise RuntimeError('Observed service device does not match the known physical port.')
        interface = device.get_active_configuration()[(0, 0)]
        if interface.bInterfaceClass != 0xff or device.is_kernel_driver_active(0):
            raise RuntimeError('Unexpected service interface or driver; no authentication sent.')
        active = session.MacSession()
        active.dev = device
        active.location = expected_port
        print('Late-service check only: no modules, firmware, settings or WBI writes.', flush=True)
        try:
            session.usb.util.claim_interface(device, 0)
            active.backend = session.BorrowedBackend(device, 30)
            driver = session.transport.GenericUsbDriver(active.backend)
            active.auth = session.transport.SonySenserAuthDevice(driver)
            active.auth.start()
            active.auth.authenticate()
            active.authenticated = True
            active.channel = session.transport.SonySenserDevice(driver)
            active.camera = session.transport.SonySenserCamera(active.channel)
            if len(active.call(active.camera.readHasp)) != 88:
                raise RuntimeError('Unexpected ProductInfo response')
            print('Second authentication and 88-byte ProductInfo read succeeded.', flush=True)
        except BaseException:
            active.healthy = False
            active._close()
            raise
        try:
            active.__exit__(None, None, None)
        except session.NormalReturnTimeout:
            print(json.dumps({'service_check': 'authentication_passed_usb_return_unconfirmed',
                              'normal_usb_returned': False, 'modules_uploaded': 0,
                              'firmware_writes': 0, 'settings_writes': 0, 'wbi_writes': 0}), flush=True)
            print('Select Image Transfer (MSC) on the camera. No automatic retry or installation.', flush=True)
            return 2
        print(json.dumps({'service_check': 'passed', 'normal_usb_returned': active.exit_report.get('normal_usb_returned', False),
                          'modules_uploaded': 0, 'firmware_writes': 0, 'settings_writes': 0, 'wbi_writes': 0}), flush=True)
    return 0


def main(argv=None):
    cli = configure()
    effective = sys.argv[1:] if argv is None else argv
    if effective and effective[0] == 'service-check' and len(effective) > 1:
        return check_late_service(cli, effective[1:])
    if effective == ['service-check']:
        if platform.system() != 'Darwin' or os.geteuid() != 0:
            raise RuntimeError('Run service-check with sudo on macOS.')
        from macfl23.session import MacSession, NormalReturnTimeout
        with cli.camera_lock():
            print('Service check only: no native modules, backup writes, firmware, settings or WBI operations.', flush=True)
            active = MacSession()
            try:
                with active:
                    print('Second authentication and 88-byte ProductInfo read succeeded.', flush=True)
            except NormalReturnTimeout:
                print(json.dumps({'service_check': 'authentication_passed_usb_return_unconfirmed',
                                  'normal_usb_returned': False, 'modules_uploaded': 0,
                                  'firmware_writes': 0, 'settings_writes': 0, 'wbi_writes': 0}), flush=True)
                print('Select Image Transfer (MSC) on the camera. No automatic retry or installation.', flush=True)
                return 2
            print(json.dumps({'service_check': 'passed', 'normal_usb_returned': active.exit_report.get('normal_usb_returned', False),
                              'modules_uploaded': 0, 'firmware_writes': 0, 'settings_writes': 0, 'wbi_writes': 0}), flush=True)
        return 0
    args = cli.parser().parse_args(argv)
    if args.action == 'setup':
        raise ValueError('Run the pinned adapter setup.sh; this entry point uses the fixed local payload.')
    if getattr(args, 'disable_ntsc_warning', False):
        raise ValueError('This local profile supports FL2/FL3 only.')
    if args.action in ('install', 'verify', 'restore', 'diagnose', 'status'):
        state = json.loads((args.workspace.expanduser() / 'state.json').read_text())
        if state.get('features', ['fl23']) != ['fl23']:
            raise ValueError('This local profile supports FL2/FL3 only.')
    if args.action == 'diagnose':
        raise ValueError('Interrupted-install diagnostics require separate review; do not auto-resume.')
    return cli.main(argv)


if __name__ == '__main__':
    sys.exit(main())
