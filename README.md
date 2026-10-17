# A7C II 2.01 FL2/FL3 Port: Research and Installation Guide

English | [繁體中文](./README.zh-TW.md)

This repository documents an experimental port of the FL2 and FL3 still-photo Creative Looks to the **Sony ILCE-7CM2 (A7C II), firmware 2.01**. It covers the implementation, the observed results on one camera, and installation without an AI assistant.

**This is an experimental preview with basic functionality confirmed on one camera. It is not a fully validated stable release or an official Sony memory-card firmware update.**

Updated: October 17, 2026.

## Current validation status

| Item | Evidence so far |
|---|---|
| Camera and version | One ILCE-7CM2 displaying version 2.01; complete AP/CP readbacks matched the pinned official 2.01 program hashes |
| Program installation | Fixed AP/CP regions written in place, preserving file lengths and inodes; fsync and complete on-camera SHA-256 verification completed |
| Independent program readback | Both complete program files read back over USB and matched the candidates |
| Native parameters | New records merged into this camera's own backup; native writes, readbacks, saves, and storage readbacks completed |
| FL2/FL3 menu | The user confirmed that both looks appeared after normal shutdown, removal of the battery for about 10 seconds, and power-on |
| Basic photography and playback | The user confirmed that FL2, FL3, and the existing ST look could each capture new photos and play them back on the camera |
| Packaged portable installer | Offline guard tests and simulated workflows passed; this reorganized portable workflow has not been tested on a second camera |

User-reported successful photography is not an independent measurement of every image format, setting combination, MakerNote field, or color characteristic. The following remain unvalidated:

- Long-term operation, additional bodies, regional variants, and a second-camera installation.
- Comprehensive regression testing across RAW, JPEG, HEIF, bursts, video, and other settings.
- Complete verification of metadata, reset values, and default-value markers in newly captured photos.
- Color matching against a physical Alpha 1 II.
- Recovery after power loss during writing or a partially damaged image.

Firmware modification can prevent a camera from booting. Backups, readbacks, and emulation help detect errors, but **do not guarantee safety or recovery**. Do not describe this work as safe for every A7C II or fully stable.

## Download and package contents

[Download the A7C II 2.01 preview toolkit, version 0.1.0](./a7c2-fl23-2.01-preview-0.1.0.zip)

The ZIP contains the installer and small patch assets. **It does not include complete Sony firmware, a complete camera backup, or private calibration units.** The tool reads the target camera's original files, checks their exact fingerprints, and builds candidates locally. Each camera uses its own backup.

| Contents | Purpose |
|---|---|
| `Start.cmd`, `Start.ps1` | Create a dedicated local Python environment on Windows and launch the interface; initial dependency installation requires Internet access |
| `gui.py` | Graphical interface for preparation, installation, verification after restart, and restoration |
| `app.py`, `core.py`, `transport.py` | Installation workflow, per-body data merging, version/hash checks, and USB communication with bounded timeouts |
| `assets/patches.json`, `colors.json` | Fixed AP/CP patch regions and added look records, rather than an entire donor backup |
| `assets/installer.so`, `probe.so`, `runtime.so`, `wbi.so` | Native camera-side workers for installation/restoration, diagnostics, runtime checks, and the existing boot-snapshot interfaces |
| `assets/manifest.json` | Complete SHA-256 hashes of deployment assets |
| `native/` | Source code for native workers and the fixed patch-region table |
| `vendor/pmca/` | Required communication and backup-parsing source from a pinned Sony-PMCA-RE revision |
| `tests/`, `VALIDATION.json` | Offline tests and the known validation scope |
| `README.zh-TW.md`, `BUILD.md`, `NOTICE.md`, license files | Usage, build, attribution, and licensing information |

The preview GUI and some documents inside the ZIP are currently in Traditional Chinese. The instructions below include translations of the relevant button labels. This English README does not change the toolkit binaries or its interface language.

ZIP SHA-256:

```text
2bbbf3942f2e1b8439242ec7fae703009645a4533473bafa7b41f542e13c6c75
```

Verify the download in PowerShell:

```powershell
Get-FileHash .\a7c2-fl23-2.01-preview-0.1.0.zip -Algorithm SHA256
```

Share this prepared ZIP and the documentation. **Do not publish the per-camera working directory or backups created during installation.**

## Installation without an AI assistant

An AI assistant is not required to run the installer. Installation is performed by local software. Windows, Python, and manual USB-driver preparation are still required; the toolkit is not yet a standalone EXE that runs without Python.

### Prerequisites

1. An A7C II / ILCE-7CM2 displaying firmware 2.01. The tool also verifies the actual program fingerprints; a matching model name or displayed version does not bypass this check.
2. Python 3.10 or newer, including the Windows Python Launcher (`py`) and Tkinter. The first launch must be able to install pinned dependencies from PyPI.
3. A USB cable that supports data transfer, with the camera set to **Mass Storage** mode.
4. Two separate empty directories for the camera workspace and a second backup copy, preferably on different drives.
5. The actual battery percentage entered into the tool, at least 50%. This is an operational threshold in the tool, not an experimentally established safety guarantee. Keep the camera connected and do not power it off during writing.

### USB drivers: the main manual setup step

Use the tool from the [official Zadig website](https://zadig.akeo.ie/) to prepare libusbK for this camera's two USB identities:

| Identity | VID:PID | Used when |
|---|---|---|
| Normal Mass Storage | `054C:0E8A` | Before the first service-authentication exchange |
| Service identity | `054C:0336` | After re-enumeration following the first exchange, before the second exchange |

The service identity normally appears only after the first authentication. Both identities will not be visible at the same time on a camera that has not entered service mode. Install the Mass Storage identity's driver first, then start the tool. If the first transition to `0336` stops because its driver is missing, install the driver for the currently enumerated `0336` device and reconnect as appropriate for the reported state.

Preserve any workspace created by a failed first attempt. The requirement to use an empty directory is not a reason to overwrite or discard earlier evidence.

Identify the camera by VID/PID. Do not replace drivers for unrelated disks or the camera's PC Remote identity. There is no automatic driver-setup wizard yet; this preview is not a setup-free, one-click update for users unfamiliar with Zadig.

### Graphical interface workflow

1. Extract the ZIP and double-click `Start.cmd`. The first launch creates a local Python environment and installs dependencies. Opening the interface does not automatically write to the camera.
2. Select an empty camera workspace and a separate empty backup directory.
3. Click **「1 備份與檢查」 — Back up and check**. The tool reads the original AP, CP, and camera settings; checks the model and fingerprints; creates two verified backup copies; merges the new color records into this body's own data; and runs native diagnostics and verification-only checks.
4. After preparation succeeds, enter the actual battery percentage and click **「2 安裝預覽版」 — Install preview**. This is the step that writes fixed program regions, performs complete readback, saves four target settings, and handles the old boot snapshot.
5. Wait for all operations to finish, then follow the complete power-cycle procedure below. **Do not simply click Install again after an error or an unknown result.**

Preparation does not modify program files or save shooting settings. It does upload, execute, and remove designated temporary diagnostic modules in Work storage. Verification-only does not mean that no temporary files are written over USB.

### After completion: shut down, then remove the battery

Only after every operation has completed and no reads or writes are running:

1. Disconnect USB.
2. Shut the camera down normally and wait for its access lamp to turn off.
3. Remove the battery for about 10 seconds, reinstall it, and power on.
4. Check that FL2 and FL3 appear and can be selected.

This step comes from the observed installation. Switching the power off and on initially did not reveal the new looks. A subsequent read-only diagnostic showed that the two menu flags were updated on disk but still held their original zero values in the running process. After battery removal, the user confirmed that the new menu entries appeared and all three tested looks could capture and play back photos. No program or settings writes were repeated to obtain that result.

The observation supports the need to reload the camera's runtime state. **It does not establish the single cause of retained values on every body.** Battery removal is not a way to safely interrupt an ongoing write.

Select FL2 and FL3 separately and use the camera's built-in reset for each look. The expected values, in the order Contrast, Highlights, Shadows, Fade, Saturation, Sharpness, Sharpness Range, and Clarity, are:

```text
0, 0, 0, 1, 0, 4, 3, 1
```

Static sources support these defaults, but all eight values and their default markers should still be checked on the camera. The installer does not overwrite the user's current adjustments in bulk.

With white balance and exposure fixed, capture new ST, FL2, and FL3 photos and check the images and camera playback. For computer-side checks, reconnect and click **「3 重開後核對」 — Verify after restart**. This checks the complete program files on disk, saved parameters, and six fixed runtime regions. It does not replace image-quality or shooting-combination testing.

### Command-line usage

The paths and 82% battery level below are examples. Use your own empty directories and actual battery percentage:

```powershell
python app.py check-package
python app.py prepare --workspace X:\MyCamera --backup-copy Y:\MyCameraBackup
python app.py install --workspace X:\MyCamera --battery-percent 82
python app.py verify --workspace X:\MyCamera
python app.py restore --workspace X:\MyCamera --battery-percent 82
```

`check-package` and the offline tests do not access USB. The other commands require the dependencies and camera USB drivers to be ready.

## Why this cannot be installed like a normal Sony update

| Item | Official update | This preview toolkit |
|---|---|---|
| Distribution | Official update files and the procedure specified by Sony | A Windows USB service tool, fixed patches, and native workers |
| Direct memory-card installation | Depends on Sony's procedure for the model | Not supported |
| These USB service drivers required | Not required by the ordinary official workflow | Required |
| Update verification/signature acceptance | Accepted by the official update mechanism | No modified update container accepted by that mechanism has been produced |
| Supported versions | Defined by the official update package | Only original A7C II 2.01 program files with the pinned fingerprints |

The implemented path uses USB service access to modify identified program regions and native parameters. **One-shot code execution inside the camera does not permanently disable official update verification or produce an update container that the official mechanism will accept.**

A memory-card update would require further work on the update container, model/version checks, verification or signature acceptance, and failure handling. This research has not completed that work. Renaming a ZIP or patch to `BODYDATA.DAT` does not solve it.

Packaging the computer-side tool as a single EXE could simplify setup, but would not turn it into an official memory-card firmware update.

## Implementation: AP, CP, and native parameters

Displaying FL2/FL3 names alone does not prove that their image processing has been ported. The implementation separately handles menu visibility, selection and saved bindings, AP-to-CP conversion, CP curves and color parameters, photo identifiers, and the old boot snapshot.

### AP: A7C II locations and product column

The reference A7CR work uses menu product column 8. On the A7C II, the live diagnostic returned product-table index 16 and model enum 9. Combined with static analysis of the consumer, this identified **column 9**. The A7CR column was not copied directly to the A7C II.

The following are **file offsets** in A7C II 2.01 `appFw.so`, not addresses to use directly in camera memory:

| File offset | Length | Purpose |
|---|---:|---|
| `0x3528804` | 112 B | Bounded AP-look-to-CP-mode conversion |
| `0x4b2ed19` | 2 B | Two selection capabilities |
| `0x59892ca` | 2 B | FL2 menu flag, product column 9 |
| `0x5989382` | 2 B | FL3 menu flag, product column 9 |
| `0x4b4a5e4`, `0x4b4a5ec` | 4 B each | Initial bindings of reserved slots 16/17 to looks 10/11 |
| `0x4b3b544`, `0x4b3b54c` | 4 B each | Factory Fade default of 1 for FL2/FL3 |

There are eight fixed regions covering 134 bytes, with 111 bytes actually changed. The conversion is:

```text
AP look 0..11 -> CP mode 0,3,2,1,16,15,17,18,5,13,19,20
```

The ten existing looks, special-value path, and invalid-value handling are preserved. Isolated function-block emulation passed 518 cases; it is not complete AP-process emulation.

### CP: processing, curves, and photo identifiers

Dispatch and branch code were reassembled for A7C II locations. The changes add four adapted donor gamma records, extend mode tables and bounds, update chroma whitelists, and handle two photo-name writers plus ColorMode 22/23. For still photos, FL2/FL3 use CP modes 19/20 and gamma rows 48/49.

The donor is Alpha 1 II firmware 4.00. Its gamma records are 580 bytes; the target records are 576 bytes. After adapting the layout and target fields, 22 comparable pairs of existing Creative Look records match byte-for-byte. Existing records containing body-specific runtime pointers were excluded from direct copying, and the added records were checked for those pointers.

The added data starts at CP file offset `0x1100b80`, where the original 3,872 bytes were zero. Processing and metadata changes together occupy 32 non-overlapping regions covering 4,288 bytes, with 2,047 bytes actually changed. Existing valid gamma rows are preserved. Video modes have fallback logic, but **complete video behavior has not been tested on hardware**.

Processing-block emulation passed 1,284 cases; photo-name/metadata-block emulation passed 748 cases. These are isolated instruction tests, not CP startup or image-processor hardware emulation. Expected numeric photo tags and names still require further checks using newly captured photos.

### Color units: merge into the target body's own data

Only added mappings and records in these two CP chip-1 units are updated:

| BKID | Length | Purpose |
|---|---:|---|
| `0x02a40001` | 8,192 B | Chroma and mappings |
| `0x02a4000d` | 28,672 B | Matrices for three illuminants and mappings |

Target parameters 55/56 correspond to donor still parameters 56/57 and use previously unused record slots 23/25. Each chroma record is 64 bytes and each added matrix record contains 576 bytes. The remaining padding in each matrix slot is preserved.

The merge rejects an already-bound parameter, a slot referenced by another parameter, or a destination record containing existing data. It verifies that the mappings and referenced records for all 55 existing parameters remain unchanged.

CP units 00/02 are not written, and a whole backup is not replaced with another camera's calibration data. Existing AP saved values are migrated separately through native operations: the one-byte reserved bindings `0x02cf1287` and `0x02cf1289` become `0a` and `0b`, then are saved. Changing factory initial values alone does not migrate existing saved bindings.

During the observed transaction, another 53,239 property values and attributes were unchanged. Two internal AP records also advance without settings writes; they were independently observed and checked using narrowly defined change patterns, rather than restored or written back. The property count belongs to this installation, not a requirement that every body contain the same number.

## Native service path and operation boundaries

This section documents protocol and implementation findings. **It is not a procedure for sending arbitrary write commands.** Use the toolkit's complete identity, original-file, backup, state, and readback checks.

1. Complete Senser authentication on normal identity `054C:0E8A`.
2. Reopen the camera after it re-enumerates as `054C:0336`, and complete the second authentication.
3. Jiritsu uses `pFunc=0x40` with a little-endian `<HH category, command>` prefix; the category here is `0x1001`. Both outer response 0 and inner 32-bit status 0 must be checked.
4. Command 15 selects a path, 17 reads a file, 16 writes a file, and 18 deletes the selected file. The older `0xff01` file interface was rejected on the tested body and was not used for the backups.
5. After SET_FILE2 writes the exact path `/tmp/jiritsu/libJit.so`, the native handler loads and unloads that module. Restricted workers use this entry point, return versioned results, and remove their temporary files; the host verifies cleanup.

The observed installation did not use `/etc/ld.so.preload`, trigger the DEBUG command that changes the clock, or write raw flash blocks directly.

Backup category `0x603` uses READ1 for AP. CP READ40/WRITE41 refer to **hexadecimal commands `0x40` and `0x41`**, with a BKID and chip selector 1 in the payload. After complete CP-unit readback, SAVE3 saves subsystem `02a4`. After complete readback of the two AP bindings, subsystem `02cf` is saved. The formats were checked against native handlers in the official version's `backup.ko`.

The native installer requires matching complete original SHA-256 hashes, original bytes at each fixed region, and file lengths. It writes in place while preserving inodes, calls fsync, and verifies the complete candidate hashes. On failure it attempts to restore the original regions and verify them. `/usr`, originally read-only, is restored to read-only. After native success, the host independently reads both complete program files back over USB.

## Pinned program fingerprints

| Program | State | SHA-256 |
|---|---|---|
| `appFw.so` | Original 2.01 | `2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b` |
| `appFw.so` | Preview candidate | `1aaeb8fbe0297950cab454d498d1a5ba874f91542aa2737aae27a73971e324d7` |
| `cpapp-b.bin` | Original 2.01 | `f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428` |
| `cpapp-b.bin` | Preview candidate | `60a9de1a0636c7f6b5b98607a4d25eeb48634e25294f632743a6d256e583b370` |

These identify the two program files, not an entire official update container. A matching model or displayed version is not a substitute for matching original-file hashes.

## Old boot snapshots: WBI

An on-disk modification does not guarantee that the next power-switch cycle runs the new contents. The observed procedure uses the existing sysfs interfaces for this version. It first requires `image_mode` to read `2\n` and `image_dev` to read `*wbi4\n`, then performs:

```text
image_drop: write ASCII 1 once, one byte
image_mode: immediately write ASCII 0 once, one byte
```

The order is **drop1 -> mode0**. Check both write lengths, close results, mode 0, the unchanged backend, and a fresh `dropped wbi` message from that operation. The log does not replace restart and photographic validation, and the procedure does not write raw sectors.

In this kernel version, related function file offsets are `0x63e28` -> `0x63dc0` -> `0x63ce8`; the mode-store function is at `0x63150`. These are analysis locations, not kernel memory addresses to overwrite. Identically named interfaces on another version are not automatically validated.

## Errors and restoration

- Keep the original programs, settings, and second backup copy. Settings backups contain private information; do not publish them.
- After a timeout, disconnection, `needs_review` state, or unknown result, stop further writes. Establish the existing outcome and current file state before retrying or rebooting.
- Fixed-region restoration does not guarantee recovery after arbitrary power loss, partial file damage, or loss of service access.
- **「回復原版」 — Restore original** uses this body's own original data to restore two programs and four target settings. It requires complete program files matching the known candidate and rejects unknown partial images. Select ST and perform a complete power cycle before reconnecting for restoration.
- After successful restoration, perform the complete power cycle and check normal photography again. An official procedure for retrying a failed update is not evidence that arbitrary damage is recoverable.

During this work, a five-second host-side read timeout interrupted a verification-only session. The existing successful result was later retrieved, confirming zero program-region writes for that operation, before cleanup. The unknown operation was not simply repeated. The reorganized transport uses bounded transaction budgets and sends no further cleanup commands after a session becomes unknown.

## Tests and their limits

| Test | Result | What it does not establish |
|---|---:|---|
| Isolated AP/CP instruction blocks | 2,550 cases passed | Complete AP, CP, or imaging-hardware emulation |
| Original-file and patch-region rejection guards | 5 cases passed | Recovery from every invalid image |
| Native diagnostic ELF syscall emulation | 13 cases passed | Actual camera privileges or loader behavior by itself |
| Native installer fault injection | 16 cases passed | Power-loss recovery; the tests use smaller synthetic files |
| WBI ordering and failure emulation | 11 cases passed | Independent readback of snapshot state on the storage medium |
| Portable toolkit offline guard tests | 8 tests passed | Validation on a second camera |
| Portable host installation/restoration simulation | 8 cases passed | USB behavior or real native execution within that simulation |
| Physical cameras | One body | Other bodies, regional variants, or long-term stability |

Program-file writes and complete readbacks, native parameter persistence, user-reported menu/photography results, and portable host simulations answer different questions. They should be reported separately.

## Sources and acknowledgments

The following public work informed this A7C II-specific implementation and analysis. The upstream researchers have not endorsed the safety of this preview or its compatibility with other bodies.

- [lzh2016zzz/a7cr-fl23-research](https://github.com/lzh2016zzz/a7cr-fl23-research/tree/43ba74441c8f10459ade41e84a379ad8005e6ef4): reference for the layered A7CR FL2/FL3 port, native parameters, metadata, and WBI handling.
- [YuuLuo/a7c2-service-preload](https://github.com/YuuLuo/a7c2-service-preload/tree/1619f938663a36d41e72302a8cfee89a0f6acd1a): A7C II 2.01 service authentication and execution-chain research; its documentation is CC BY-SA 4.0. This implementation uses the native libJit loading entry point rather than preload configuration.
- [ma1co/Sony-PMCA-RE](https://github.com/ma1co/Sony-PMCA-RE/tree/a82f5baaa8e9c3d9f28f94699e860fb2e48cc8e0): USB/Senser communication, native Backup operations, and backup parsing, under MIT. The bundled source retains the full license.
- [ma1co/fwtool.py PR52](https://github.com/ma1co/fwtool.py/pull/52), pinned commit `3955d7890fc38daf942bb52827599bfe35de4a14`: reference tooling for offline analysis of official firmware.
- Official Sony A7C II 2.01 and Alpha 1 II 4.00 firmware: sources for program and color-data analysis. Sony trademarks and firmware-data rights remain with their respective rights holders.

This README is licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). See `LICENSE`, `NOTICE.md`, and `licenses/` inside the ZIP for the preview toolkit's new code and upstream licenses. These licenses do not claim to cover Sony firmware data. This repository does not publish the tested camera's serial number, complete settings backup, private calibration data, private login credentials, or personal computer paths.
