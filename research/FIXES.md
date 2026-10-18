# Three fixes before a new camera trial

Local revision: `0.1.1-preview`, October 17, 2026. A subsequent October 18 hardware trial wrote and verified the revised AP/CP programs and saved target settings, then stopped during preservation checking. A reviewed finish-only procedure subsequently verified WBI completion. After confirmed battery removal/cold start, both complete programs, target parameters and all six runtime memory/disk regions passed verification. The user reports FL2/FL3 visible and basic capture working with FL1, FL2, FL3 and Auto. Playback, capture format and detailed photographic behavior were not separately confirmed. Only final MSC return was unconfirmed after successful service exit. See [the current Mac attempt](../MACOS.md) and [sanitized review](evidence/macos-installation-review.json). The earlier complete one-body reports belong to the original candidate.

## Changes

1. `app.Job.settings` captures separate full live and saved baselines before writes, verifies every property in each view afterward, and journals both preservation results. Before writing, it rejects unrelated pending edits in any subsystem that the operation would SAVE. Preexisting live/saved differences elsewhere remain allowed. Installation and restoration use the same checks.
2. The CP gamma helper maps movie FL2/FL3 requests to existing FL (mode15), matching the existing movie color fallback. The instruction at file offset `0x13883e` changes from `movls r0,#0` to `movls r0,#15`. Still requests continue selecting gamma48/49. JSON, generated C table, candidate fingerprints, native installer and deployment manifest agree.
3. Preservation now compares `resetData` as well as current data and attributes, including intended targets and the narrow runtime-counter exceptions. Missing intended properties are rejected.

## Offline results

| Check | Result | Evidence |
|---|---|---|
| Host tests | 24 passed: original guards, preservation/install/restore regressions, backup-only workflow | [log](evidence/offline-tests-fixed.log) |
| CP instruction/model checks | 9,185 passed, including 32 explicit complete-FL movie fallback/override comparisons | [report](evidence/cp-review-fixed.json) |
| AP converter | 259 cases passed | [asset/native/AP audit](evidence/baseline-audit-fixed.json) |
| Historical preservation counterexamples | Both rejected | [report](evidence/host-guard-review-fixed.json) |
| Official firmware/candidate checks | Complete forward and reverse hashes, donor records, all 12 template merges passed | [report](evidence/firmware-audit-fixed.json) |
| Pinned upstream source verification | Nine repositories verified | [log](evidence/source-verification-fixed.log) |
| Native rebuild | Zig0.13.0; executable `.text` unchanged, updated patch data/hashes; all 40 binary patch regions match JSON/C | [build report](evidence/fix-build.json) |

The AP candidate remains `1aaeb8fbe0297950cab454d498d1a5ba874f91542aa2737aae27a73971e324d7`.

The revised CP candidate is `60b9dc9d974cf1e32ddc7e72099fda80c328a84a5e6bd89790d62e36f8ed58ab`. The earlier `60a9de…` candidate does not include the movie gamma fix.

The rebuilt installer is `575e9d34418ef47c8e6cb2202d249737f928aa18f4ea752409fc4da70016a423`. No complete firmware update container or replacement historical ZIP was produced.

Run from the repository root:

```sh
.venv-research/bin/python -m unittest discover -s tests -v
.venv-research/bin/python app.py check-package
.venv-research/bin/python research/review_cp.py
.venv-research/bin/python research/review_host_guards.py
.venv-research/bin/python research/audit_baseline.py --allow-baseline-changes
.venv-research/bin/python research/audit_firmware.py
.venv-research/bin/python research/verify_sources.py
```

These checks do not emulate a complete camera or validate photographs, reset/import behavior, CP runtime loading, power loss, or all failure recovery. The remaining gaps in [the CP review](CP_REVIEW.md#remaining-trial-gaps) still apply.

## Connected camera and backups

The camera enumerates as `054c:0e8a`, with interface class08/subclass06/protocol50 (Mass Storage, SCSI, Bulk-Only). A normal identity/version query stalled inside macOS IOKit while libusb claimed the interface, before any protocol command was sent. The process was terminated. USB descriptor `bcdDevice=0x0200` is not treated as a confirmed firmware version. No service authentication or camera program/settings writes occurred in that attempt.

`backup_camera.py` provides a separate backup-only workflow. Unlike installer preparation, it does not upload diagnostic/native modules or create installation state. After model/version checks, it uses service authentication only to read full live/saved settings, complete AP/CP program files, two CP units, and two AP bindings. It writes eight files and a private manifest into each of two independent local directories, verifies SHA-256 readback of both copies, and records whether the directories reside on distinct filesystems. An unexpected firmware revision stops before service access. An incomplete backup is marked incomplete rather than usable for installation.

The paragraph above describes the earlier backup-only workflow and initial USB failure. The later Mac-adapter preparation confirmed firmware 2.01, completed eight hash-verified original backups and all three native prechecks, and verified reversible candidates. The subsequent partial installation and preservation stop are documented in [MACOS.md](../MACOS.md); those private backups remain outside Git.
