# FL2/FL3 installation on macOS

The fixed `0.1.1-preview` payload is installed and verified on one ILCE-7CM2 running firmware 2.01. After confirmed battery removal, both complete program files, target parameters and all six AP runtime regions matched. The user reported FL2/FL3 visible and basic capture working with FL1, FL2, FL3 and Auto. The final phase is `verified_after_cold_start_usb_unconfirmed`: verification and service exit completed, but normal USB return was not observed before its deadline.

The installation required reviewed recovery from USB and settings-check interruptions. This is a tested result on one body, not a reliable out-of-the-box Mac installer. An already-installed body should not be prepared or installed again.

| Read next | Purpose |
|---|---|
| [Installation findings and lessons](research/MACOS_INSTALLATION_LESSONS.md) | Environment, implementation, failures, parser proof, recovery guards and remaining work |
| [Sanitized trial record](research/evidence/macos-installation-review.json) | Exact outcome and limits of the hardware evidence |
| [VALIDATION.json](VALIDATION.json) | Current revision versus inherited 0.1.0 results |
| [Fixes before the trial](research/FIXES.md) | Preservation and CP movie-fallback changes in this payload |

## Supported profile

`macos_fl23.py` combines MDHan's Mac adapter at commit `d9eac5a3f805a65a894722d9a4b8800ac6b8df15` with this repository's local `app.py`, `core.py`, `transport.py` and fixed deployment assets. The [adapter lock](research/toolchains/macos-adapter.json) verifies the imported source. Its original downloaded upstream release remains unchanged and checksum verified; the module-provider seam selects our corrected payload.

New preparation requires the supported original A7C II 2.01 files; verification/restoration use the corresponding validated workspace and candidates. This profile supports FL2/FL3 only. It rejects the optional NTSC feature and generic `diagnose` through this entry point. The private reviewed continuations used in this trial are not published CLI commands. In particular, the pinned local CLI has no `finish-install` action. Do not invoke the adapter's CLI directly as a substitute: that selects its own upstream provider.

The trial host was Apple M3 Max, macOS 27.0.1, Python 3.14.6, PyUSB 1.3.1 and libusb-package 1.0.30.0. Full versions, payload hashes and upstream finish provenance are recorded in the [case study](research/MACOS_INSTALLATION_LESSONS.md#tested-configuration). Other environments remain separate compatibility questions.

## Prepare the host

Run these setup commands from the repository root only when the adapter directory does not already exist:

```sh
git clone https://github.com/MDHan/a7c2-fl23-macos.git research/build/a7c2-fl23-macos
git -C research/build/a7c2-fl23-macos checkout d9eac5a3f805a65a894722d9a4b8800ac6b8df15
(cd research/build/a7c2-fl23-macos && ./setup.sh)
```

For an existing checkout, inspect it and retain its state. Do not reset it, overwrite a camera workspace or update pins to make a failed hash check pass. Setup may download dependencies and the pinned release; subsequent offline package checks do not need the camera:

```sh
research/build/a7c2-fl23-macos/.venv/bin/python -B macos_fl23.py check
research/build/a7c2-fl23-macos/.venv/bin/python -B -m unittest discover -s tests -v
```

Close Finder windows browsing the camera cards. Keep G HUB closed during camera-utility operations: in this trial its agent owned the Sony service interface. Reopen it afterward. Use a direct data connection and choose **Image Transfer (MSC)**, the second option when the connection menu appears. Do not use Windows Zadig instructions on macOS or disable SIP.

The read-only host inventory below queries macOS USB/disk information without service authentication:

```sh
research/build/a7c2-fl23-macos/.venv/bin/python -B macos_fl23.py usb-status
```

## Inspect an existing workspace first

Private camera workspaces belong outside Git. The examples use a placeholder path, not the workspace or battery reading from this trial:

```sh
research/build/a7c2-fl23-macos/.venv/bin/python -B macos_fl23.py status --workspace "$HOME/Pictures/A7C2-FL23-work"
```

| Recorded phase | Meaning and next decision |
|---|---|
| `prepared` | Original backups/candidates/prechecks completed; explicit installation may proceed with a current battery reading |
| `prepared_usb_unconfirmed` | Same verified preflight body, service exit sent, final MSC wait timed out; establish normal MSC before explicit installation |
| `installed_pending_cold_start` or its `_usb_unconfirmed` form | Installation body verified; do not reinstall; follow the cold-start step when error-free and idle |
| `verified_after_cold_start` or its `_usb_unconfirmed` form | Program/runtime checks completed; retain the actual USB-return result and separately record manual shooting observations |
| `needs_review` | Inspect `failed_action`, `failed_during`, the journal and raw reports; no general resume, phase-reset or reboot instruction follows from this state |

## New preparation and installation

Use a new private workspace whose parent exists. The operation checks model, normal protocol version and serial-derived identity, then verifies complete original AP/CP fingerprints in service mode. Preparation saves eight original files, validates per-body color slots, builds reversible candidates and runs the native probe, installer mode 0 and WBI mode 0.

```sh
sudo research/build/a7c2-fl23-macos/.venv/bin/python -B macos_fl23.py prepare --workspace "$HOME/Pictures/A7C2-FL23-work"
```

Preparation runs temporary native modules; it does not write program files or save shooting settings. The Mac adapter defaults to one verified original backup location. `--backup-copy` is optional and explicit. The standalone two-copy `backup_camera.py` workflow is different; do not infer that the Mac preparation created a second copy.

After a genuinely completed preflight, supply the **current displayed battery percentage**, at least 50. Replace the placeholder below with that actual integer:

```sh
sudo research/build/a7c2-fl23-macos/.venv/bin/python -B macos_fl23.py install --workspace "$HOME/Pictures/A7C2-FL23-work" --battery-percent ACTUAL_PERCENT
```

Installation rechecks targets and WBI preconditions, writes the fixed program windows, independently reads back the complete files, writes/saves the four target parameters, checks preservation and completes WBI handling. It is not atomic across all these stages. Keep power and USB unchanged during the command. Enter administrator credentials only in the user's Terminal; never in chat. A new Terminal session may need authentication again.

On completion, inspect the process exit code **and** saved phase/journal. A progress message or completed program write does not establish that settings and WBI finished. Do not automatically chain a failed stage into another camera action.

## Classify failures before another device command

| Symptom | What it establishes | Required response |
|---|---|---|
| Timeout waiting for `0336` after first authentication | This process did not acquire the service identity; it says nothing by itself about whether macOS sees it | Preserve state/logs; inspect OS identity, port and ownership. Establish whether the operation body ever began |
| Unexpected driver on service interface | Another owner may hold that interface; G HUB was the confirmed owner here | Identify the exact user client; keep the guard and avoid broad process/driver changes |
| Exit 139 after apparently completed preparation | Host crashed; completed evidence may exist | Independently validate the saved backups/candidates/native reports and exit journal; do not repeat preparation to repair host ownership or cleanup |
| Extra setting change at `chip0/0x2770015` | Preservation rejected an unexplained live change after possible program/settings writes | Read all live/stored before/after evidence. This trial's +60 and parser overlap are narrowly reviewed facts, not global allowances |
| Existing service device after a failed transition | A service object is visible now | A late continuation needs the reviewed prior normal identity, same physical port/object, unchanged connection/power and a known pre-write outcome; do not treat it as a generic entry shortcut |
| `_usb_unconfirmed` with exit 2 | Only a specifically verified completed body and successful service exit may receive this status | Follow the action-specific next step; no repeat writes. Ordinary entry/body timeouts use the incomplete path instead |
| Existing one-shot result or finish marker | An operation has already produced evidence or may have begun mutation | Preserve it. Do not delete logs/markers or edit `state.json` to force another attempt |

Fresh libusb contexts and a 60-second service wait did not make handoff reliable. The context-lifetime crash was fixed separately. [The case study](research/MACOS_INSTALLATION_LESSONS.md#usb-discovery-and-ownership) distinguishes those findings from the unresolved discovery cause.

The reviewed WBI-only recovery used [upstream finish code at b625c78](https://github.com/MDHan/a7c2-fl23-macos/blob/b625c78f43eca9208e6c00f564cebbf353031891/macfl23/finish.py), plus stricter local preservation review and body/session-specific guards. It required explicit informed opt-in. That private procedure is recorded as a case study, not offered here as a command to run on arbitrary failed installations.

## Cold start and final verification

Only after the install/WBI body is verified, the command has finished, and the camera has no error or ongoing access activity:

1. Unplug USB and switch the camera off normally.
2. Wait for the access lamp to go out.
3. Remove the battery for about ten seconds, reinstall it and switch on.
4. Check that FL2/FL3 appear, reconnect and choose MSC.
5. Confirm that the physical battery-removal step actually happened before invoking verification.

```sh
sudo research/build/a7c2-fl23-macos/.venv/bin/python -B macos_fl23.py verify --workspace "$HOME/Pictures/A7C2-FL23-work" --cold-start-confirmed
```

Verification reads both complete programs, checks target parameters and uploads a temporary reader for six AP runtime regions. It does not reinstall programs, save parameters or repeat WBI completion. The six regions are AP checks; they do not directly inspect executing CP instructions.

Then use each look's own reset option and take new comparison photographs with fixed scene, exposure and white balance. Check capture, saved files and playback separately. Record exactly which looks, modes and formats were tested. This trial's user report covers FL1/FL2/FL3 and Auto capture; separate playback and format confirmation were not supplied.

## Preserve the completed result

Keep the original backups, candidates, state, native reports and full readbacks private and intact. Do not repeat preparation/install on this already-modified body, replay the trial's one-off helper scripts or change its final phase to erase the MSC timeout. A later need to restore original firmware is a separate operation requiring this body's validated originals and the existing restore prerequisites.

The public repository retains the common program fingerprints, test results, sanitized hardware record, [technical lessons](research/MACOS_INSTALLATION_LESSONS.md) and [remaining LUT research](research/STILL_LUT_FEASIBILITY.md). Serial hashes, original settings/calibration, full firmware, administrator credentials and private operation logs remain outside the published evidence.
