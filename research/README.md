# A7C II firmware research baseline

The [native capture-probe follow-up](NATIVE_CAPTURE_PROBES.md) corrects the FSYS
cursor/pointer assumption, resolves bounded stream vectors, identifies the
same-open position field under the native writer lock, and adds fault-contained
self-reads, nested request scopes and an offline ABI-preserving probe veneer.
No hook is installed, and no camera or G HUB access occurs.

The [October 20 completed-photo handoff](COMPLETED_PHOTO_HANDOFF.md) adds native
frozen-frame fan-out and a bounded owned-byte reconstruction core for JPEG/HIF,
with original FSYS instruction tests and offline worker replay. It also reviews
the newly linked native custom-look implementation. No camera or G HUB access
occurred; a real Sony hook and storage/lifetime observations are still required.

Baseline research date: October 17, 2026. Documentation updated October 18, 2026. Starting commit: `370bf0b5b9b23d3d50dd381b562a736035c6690a`.

## October 18 hardware installation

The later controlled Mac trial installed the revised 0.1.1 payload on one A7C II 2.01. Full programs, parameters, WBI completion and six AP runtime regions after battery removal passed verification; the user reported basic FL1/FL2/FL3 and Auto capture. Normal USB return remained unconfirmed. This hardware trial is separate from the offline LUT/firmware investigations below.

- [Mac operating guide](../MACOS.md): phases, commands and failure classification.
- [Installation findings and lessons](MACOS_INSTALLATION_LESSONS.md): tested environment, host lifetime bug, G HUB ownership, parser-overlap proof, reviewed recovery and outstanding work.
- [Sanitized hardware evidence](evidence/macos-installation-review.json) and [current validation](../VALIDATION.json): computer checks versus user-reported capture and inherited results.

## Offline firmware investigation

The [Sony menu/completion pass](SONY_MENU_COMPLETION.md) now traces the actual
per-file FSYS callback, its conditional close/fsync path, the AP singleton
completion delegate and relocation-backed UIBIZ keys. A [native observer/menu
core](../camera_worker/SONY_INTEGRATION.md) is tested offline through the existing
JPEG/10-bit HIF launcher. It does not install a hook or establish the remaining
shutter/file-identity and Sony GUI boundaries. Original instruction tests and
synthetic native replay are explicitly distinguished from hardware validation.

The [capture/file identity follow-up](CAPTURE_FILE_IDENTITY.md) adds a checked
returned-ID → MPR record → write cookie → FSYS open → real-close model,
including deferred close and a conservative PrFILE-only lease. It retains
45 additional original-instruction cases. Early arming across modes, scoped
hooks and owned encoded bytes still gate automatic worker handoff.

The [standalone ARM64 JPEG-worker candidate](../camera_worker/README.md) now
replaces the host runtime for processing exported jobs: static musl/libjpeg,
native 11-look LUT evaluation, frozen lens EXIF and bounded scanline memory.
Two clean builds are byte-identical. Sanitizer tests, full-resolution host
processing and actual ELF execution in a restricted host emulator pass;
[evidence](evidence/camera-worker-checks.json) separates these from unperformed
camera validation. No camera access or installation occurred. Job production,
native selection UI and media registration remain future work.

The subsequent [completed-JPEG host worker](../photo_worker/README.md) implements custom sRGB CUBE processing and manual-lens EXIF presets on separate copies, with snapshotted jobs, an initial 15 host tests (now 26 including RGBTable/CLF coverage), a 7008×4672 synthetic proof and independent ExifTool readback. [Worker evidence](evidence/jpeg-worker-checks.json) records correctness and host-only resource measurements. The user supplied the Fujifilm pack after the initial research: [all 1,500 files are inventoried](evidence/fujifilm-profile-pack.json); the original Adobe RAW XMP/DCP files are not directly applied by the worker. The [new sRGB adaptation](FUJIFILM_SRGB_CONVERSION.md) converts all 11 embedded RGBTable stages to self-contained CLF pipelines, validates them against the supplied Adobe SDK scalar function, and verifies JPEG/manual-lens integration. It excludes full RAW/DCP rendering. No camera access or installation occurred.

The October 18 [capture handoff](CAPTURE_HANDOFF.md) traces an incoming source handle through `ChipTransfer`/PCIe DMA request construction into a JPEG input slot, and follows the CP shared-handle globals into a type-`0x108` export record. No writer connects that export or MLUT allocation to the incoming request yet. Six host fixtures include a transfer-failure control; no native pixel processing occurs. The [prototype plan](STILL_PROTOTYPE_PLAN.md) covers the user's four priorities: Creative Looks in AUTO, imported Fujifilm profiles, camera-native InstantLink printing and manual-lens EXIF. Additional evidence maps native BLE request producers and establishes that two named EXIF APIs are stubs. The referenced Fujifilm repository was unavailable locally and was not fetched under the offline instruction.

[Validation receipt](evidence/still-prototype-followup-checks.json): three new generators, 25 regions / 1,755 independently byte-verified instruction records, six host handoff cases, 41 root host tests and eight LUT-reference tests. No camera or printer operations occurred.

The [firmware architecture atlas](A7C2_FIRMWARE_ATLAS.md) adds a deeper REA/Ghidra investigation: container and boot structure, original filesystem metadata, application initialization, AP/CP interfaces, recording LUT metadata and MLUT control paths. Its [REA index](evidence/rea/index.json) retains actual tool Evidence separately from the reproducible Python/Capstone maps. The camera was not accessed for this work.

The next pass recovers the [ordinary native LUT format and producer](NATIVE_LUT_FORMAT.md), independently matches [CP SRAM/register layout](MLUT_LAYOUT.md), and deepens [still-sequence routing](STILL_MLUT_ROUTE.md) plus the [AP recording boundary](STILL_RECORDING_BOUNDARY.md). Full-resolution saved-still processing through MLUT remains unproven; all new execution tests use isolated host emulation.

A further offline pass executes the [HQ converter](HQ_LUT_FORMAT.md), maps the [two-resource LUT transfer](LUT_TRANSFER.md), and resolves both a [CP JPEG request boundary](STILL_ENCODER_ROUTE.md) and the [AP JPEG/HEIF file-write sink](STILL_SINK.md). The MLUT-to-encoder buffer join remains unresolved. Research findings are committed and published separately from ignored official firmware and private backups; publication is the only network activity in this pass.

The preceding pass traces [MLUT-context resource addresses into pixel-port records](MLUT_ENCODER_LINK.md), recovers [AP JPEG commands and the dataflow envelope](AVBBC_ENCODER_BRIDGE.md), and identifies the [HEIF MC encode sequence](HEIF_ENCODER.md). It closes the [AP still-info notification-to-RecordStill descriptor link](ENCODER_SAVE_LINK.md). At that point the MLUT-to-encoder backing-buffer join and codec-to-notification producer remained missing; a local HEIF command-conversion gap also prevented treating the recovered class as a callable API. Its four mappers passed 2,984 bounded host cases without camera access.

The October 18 follow-up recovers [SDYC's outer metadata and nested pixel descriptors](SDYC_METADATA.md), [CP segmented backing resolution](SDYC_MAPPING.md), the [actual still-info producer in av-cam](STILL_INFO_PRODUCER.md), and [HEIF native receiver/binding details](HEIF_BINDING.md). Four original-AP metadata fixtures decode correctly through original CP instructions. The capture JPEG callback's adjusted length now reaches the notified JPEG area. Specific live MLUT allocation identity and pixel/output ownership still need tracing; static HEIF translation remains incomplete in both directions. Its four mappers pass 577 bounded host cases. This pass accesses only local firmware and synthetic host memory; network access is reserved for Git publication.

The latest allocation/ownership pass adds [conditional CP MLUT allocation and sharing](MLUT_ALLOCATION.md), [av-cam resize-to-encoder pixel inputs](AV_CAM_PIXELS.md), [native HEIF completion and output ownership](HEIF_OWNERSHIP.md), [av-cam's MLUT software boundary](AV_CAM_MLUT.md), and [AP recording share/free order](RECORDING_BUFFER_OWNERSHIP.md). Its five mappers pass **302 bounded host cases** across 67 instruction regions. The capture HEIF path constructs the native MC request directly, bypassing the separate AP conversion-table gap. HEIF output areas survive publication and participate in AP share-before-free; final shared-reference reclamation remains unobserved. The CP allocation-to-av-cam encoder lineage is still missing, so no saved-still LUT effect is claimed. This pass uses local firmware only; Git publication is its only network activity. [Verification receipt](evidence/allocation-ownership-followup-checks.json).

Follow-up: [CP review before the next hardware trial](CP_REVIEW.md) records 9,153 additional isolated CP checks, an inconsistent movie fallback, two host preservation-check counterexamples, and the distinction between this port and arbitrary LUT support for still photos.

Those three findings are now fixed in local revision 0.1.1-preview. [Fixes and rerun evidence](FIXES.md) records the original 24-test fix check, 9,185 CP cases, 259 AP converter cases and synchronized native assets. The expanded checkout now passes 41 host tests, and the separate hardware trial is linked above. The baseline hashes and results below describe the original candidate unless explicitly marked otherwise.

[Still photo LUT feasibility](STILL_LUT_FEASIBILITY.md) examines a custom post-capture JPEG worker and reuse of the existing Sony LUT engine during still capture. It includes new offline AP/libmpr/CP evidence and identifies the unresolved buffer, color-space and execution contracts. No camera access was used for that investigation.

**FL2/FL3 is implemented experimentally on A7C II 2.01.** The baseline README's original selection/capture/playback results are inherited 0.1.0 reports. The later 0.1.1 hardware trial has its own scoped evidence above. The collection and native LUT investigations below remain offline work and do not establish arbitrary still-photo LUT support.

The [native Fuji-style look implementation review](NATIVE_FUJIFILM_LOOKS.md) explains the alternative ST correction-table route, including source conversion, AP selectors/names, CP packing and preview retries. New offline checks cover 3,520 AP selector cases and 120 original-CP bank cases. This route can avoid the worker handoff for color, while AUTO, HIF and measured color fidelity remain separate questions.

## What was collected

The 47 starting files are inventoried, with sizes and SHA-256 hashes, in [baseline-inventory.json](baseline-inventory.json). They comprise 13 root files, seven deployment assets, six native source/header files, 19 vendored PMCA files, one license, and one test file. Their original revision remains in Git; the follow-up fixes intentionally modify several working files, as recorded by the revised baseline audit.

Every published file at each selected source revision was downloaded, including hidden files and binary fixtures. Original GitHub tarballs, API metadata, and the PR52 discussion are in [downloads](downloads/). Unpacked snapshots are in [upstream](upstream/). [sources.lock.json](sources.lock.json) records the exact revisions, archive hashes, and every snapshot file's size and hash: **nine repositories, 4,132 files**.

| Local snapshot | Revision | Purpose |
|---|---|---|
| [a7cr-fl23-research](upstream/a7cr-fl23-research/README.md) | `43ba74441c8f10459ade41e84a379ad8005e6ef4` | A7CR port research; one documentation file, no complete payload or implementation source |
| [a7c2-service-preload](upstream/a7c2-service-preload/README.md) | `1619f938663a36d41e72302a8cfee89a0f6acd1a` | Five documentation/license files describing A7C II 2.01 service access |
| [Sony-PMCA-RE](upstream/Sony-PMCA-RE/README.md) | `a82f5baaa8e9c3d9f28f94699e860fb2e48cc8e0` | Complete upstream communication and backup sources |
| [fwtool.py](upstream/fwtool.py/README.md) | `3955d7890fc38daf942bb52827599bfe35de4a14` | PR52 revision with CXD90057 firmware unpacking; the old packaged releases do not represent this revision |
| [llr](upstream/llr/README.md) | `07d1a531554d1fe82d071e924f38c243cd40fef2` | Creative Look and RAW processing research |
| [pysonycam](upstream/pysonycam/README.md) | `115102f9288f5055c27241849172b70f4509c359` | PTP enum/property reference, SDK reference material |
| [exiftool](upstream/exiftool/README) | `2200871d9cef988051d2a99d67df3bda6cbb30a8` | Sony metadata decoding and test fixtures |
| [OpenMemories-Platform](upstream/OpenMemories-Platform/) | `85bcb5454bc68a971b674d3b9e93eeedbf35cf3c` | PMCA's exact `updatershell/platform` gitlink |
| [stlport](upstream/stlport/) | `dde1a07889a1da3189b06768d0e66fbb44dcf6f8` | Platform's nested gitlink; recovered at the exact commit from `vincenthsu/stlport` after the old Gitorious remote timed out |

The first four revisions were pinned in the starting README. The next three were resolved from the projects' default branches on the research date, then pinned. Submodule snapshots are stored separately; [sources.lock.json](sources.lock.json) records their original mount points. The archives are source snapshots, without Git history. We did not download every historical executable release or recursively follow unrelated links.

Sony's official inputs, **A7C II 2.01** and **Alpha 1 II 4.00**, have both been downloaded and unpacked. The user explicitly approved accepting Sony's download EULA. Downloads are under `research/firmware/<model>-<version>/BODYDATA.DAT`; extracted files are under `research/extracted/`. These directories stay in this local repository workspace and are excluded from Git. [firmware-inputs.json](firmware-inputs.json) records their URLs, sizes, and measured hashes. A locally measured digest is not a Sony-published authenticity signature.

| Official input | Download bytes | Files produced by recursive unpacking |
|---|---:|---:|
| [A7C II 2.01](https://www.sony.com/electronics/support/e-mount-body-ilce-7-series/ilce-7cm2/software/00301105) | 1,139,929,560 | 1,369 |
| [Alpha 1 II 4.00](https://www.sony.com/electronics/support/e-mount-body-ilce-1-series/ilce-1m2/software/00349486) | 1,194,332,648 | 1,295 |

Both use the unpacker's `CXD90057_k8` branch. The file counts include retained container/filesystem images and extracted members. They do not mean every proprietary inner data format has been decoded.

The starting README's `a7c2-fl23-2.01-preview-0.1.0.zip` link returns HTTP 404 at the pinned repository revision, and the repository has no GitHub releases. It was not available to download. The existing installer files and assets are present individually. Historical private camera backups, logs, and unpublished upstream payloads are not available in these public repositories.

## How the existing implementation works

The starting code connects several independent layers. The following describes the A7C II payloads and source, not an instruction to install them.

```mermaid
flowchart LR
    A[AP menu and selection] --> B[Saved slot binding]
    B --> C[AP look to CP mode]
    C --> D[CP dispatch and gamma]
    D --> E[Body-specific color units]
    E --> F[Photo and metadata]
    G[Boot snapshot handling] --> A
```

1. **AP menu and selection.** `assets/patches.json` enables the two product flags and selection capabilities. A7C II uses product column **9**; the A7CR reference uses **8**. These offsets must not be copied across models. `appFw.so` has eight fixed regions covering 134 bytes, of which 111 differ.
2. **Bindings and conversion.** Reserved ordinary slots 16/17 are initialized to AP looks 10/11. Existing saved bindings also need migration through AP backup IDs `0x02cf1287` and `0x02cf1289`; changing factory initialization alone does not update an existing body. The converter maps the twelve ordinary AP looks to CP modes `0,3,2,1,16,15,17,18,5,13,19,20`.
3. **CP image processing.** `cpapp-b.bin` receives dispatch changes, table bounds and addresses, movie fallback handling, and a 3,872-byte added data region. The complete CP patch has 32 regions covering 4,288 bytes, of which 2,047 differ. The four added gamma records were independently located in the donor. Each retains the donor's first 512 curve bytes, removes one four-byte zero layout word, and changes one control byte. Those control bytes still require semantic verification in CP dispatch.
4. **Color data.** `core.merge_colors` merges two chroma and matrix records into this body's own CP units `0x02a40001` and `0x02a4000d`. Parameters 55/56 use free record slots 23/25. It rejects occupied mappings and records, and checks preservation of the existing 55 parameters. A complete donor backup is not an input to the installer.
5. **Photo identifiers and defaults.** Separate CP patches write names and playback ColorMode values. AP factory Fade changes to +1. The expected adjustment vector is `0,0,0,1,0,4,3,1`; actual reset values and markers need photographic/menu validation.
6. **Persistence and loading.** `app.py` reads and saves native parameters, then uses the version-specific WBI interfaces. Disk contents and process memory can differ until a full reload. The starting camera report required normal shutdown followed by battery removal after all operations completed.

The identifiers belong to different interfaces:

| Field | FL2 | FL3 |
|---|---:|---:|
| Ordinary AP slot | 16 | 17 |
| AP look value | 10 | 11 |
| CP mode / Tag9416 CreativeStyle | 19 | 20 |
| CP still color parameter | 55 | 56 |
| CP still gamma row | 48 | 49 |
| MakerNote `0xb029` ColorMode | 22 | 23 |
| PTP Creative Look enum in pysonycam | `0x000b` | `0x000c` |

The downloaded ExifTool source independently recognizes CreativeStyle 19/20 and ColorMode 22/23 as FL2/FL3. That decoder agreement does not prove that this port writes correct photos. A remote-control enum also does not establish that stock A7C II firmware implements that look's processing.

### Service execution and installation

`transport.py` performs Senser authentication at normal USB identity `054c:0e8a`, reopens the re-enumerated `054c:0336`, and authenticates again. It uses Jiritsu category `0x1001` through function `0x40` for file access. Its path convention reaches the AP and CP files for readback.

The current toolkit uploads a native worker to the exact `libJit.so` loading path. The worker's ELF initialization function executes its bounded task and writes a versioned result. This differs from the downloaded YuuLuo study's preload/DEBUG child process route. This session checked the host code and ELF structure; it did not demonstrate the camera loader behavior independently.

`native/installer.c` verifies complete program hashes, lengths, and fixed original regions; remounts `/usr`; writes in place; calls fsync; verifies complete candidate hashes; and restores read-only mounting. Its failure path attempts program-region rollback. The host subsequently reads both complete files independently. Settings writes, settings saves, and WBI changes are separate operations, so program rollback is not an atomic rollback of the entire installation.

This is a USB service installation path. The presence of firmware extraction/packing code does not establish that a modified memory-card update will pass Sony's acceptance checks.

## What was independently checked here

Results are in [evidence/baseline-audit.json](evidence/baseline-audit.json) and the verification logs.

| Check | Result and scope |
|---|---|
| Starting content | All 47 starting files retain their original SHA-256 |
| Snapshot acquisition | Nine archives and all 4,132 files match the lock |
| Bundled deployment assets | Six asset hashes match `assets/manifest.json` |
| Vendored PMCA | All 19 files match the pinned upstream after newline normalization; 18 have only CRLF/LF differences |
| Patch consistency | All 40 native ELF patch entries match JSON and C arrays, including before/after bytes, chip, offset, and length |
| Native structure | Four AArch64 ELF modules, no `DT_NEEDED` libraries; installer has 80 relative relocations, other workers have none |
| AP converter | 259 fresh Unicorn cases: ordinary looks 0..11, remaining byte values, and three large unsigned values; correct bounded handoff |
| Included offline tests | Eight tests pass; asset, merge, rejection, preservation, and unknown-outcome checks |
| Package checker | Passes without USB |
| Native compilation | All four sources compile with installed Zig 0.16.0; output differs from bundled assets, whose documented reference compiler is 0.13.0 |
| Official program reproduction | Extracted A7C II AP and CP match the pinned originals; forward patches produce both pinned candidates, and reverse patches restore originals byte-for-byte |
| Donor gamma | Four unique donor records found at CP file offsets `0xdbeef0`, `0xdbf800`, `0xdbf378`, `0xdbfa44`; all differences accounted for as described above |
| Donor color records | Both chroma and matrix payloads match all 12 donor regional factory templates; donor parameters 56/57 map to slots 23/25 |
| Factory-template merge | All 12 A7C II regional templates accept the merge; every byte outside added mappings/records is preserved |

The complete firmware audit is [evidence/firmware-audit.json](evidence/firmware-audit.json). Original A7C II program hashes and reproduced candidates are:

| Program | Official original SHA-256 | Reproduced candidate SHA-256 |
|---|---|---|
| `appFw.so` | `2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b` | `1aaeb8fbe0297950cab454d498d1a5ba874f91542aa2737aae27a73971e324d7` |
| `cpapp-b.bin` | `f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428` | `60a9de1a0636c7f6b5b98607a4d25eeb48634e25294f632743a6d256e583b370` |

The new AP check executes only the published 112-byte window. It does not execute the original special-value path, original out-of-range handler, full AP, full CP, or imaging hardware. The binary patch-table check does not establish complete source-to-binary equivalence. The reported historical 2,550 instruction cases, installer fault injection, WBI tests, and host workflow simulations are not included as reproducible suites in the starting checkout; they have not been silently counted as tests run here.

## Work to do before a new hardware trial

1. Preserve the reproduced offline baseline and distinguish it from hardware results. The official files, candidate hashes, and exact reverse restoration have now been checked.
2. Complete semantic analysis of the gamma control-byte changes, CP tables, factory defaults, and ownership of code/data padding. The curve/chroma/matrix provenance has now been checked against the downloaded donor.
3. Rebuild CP instruction tests and native-worker failure tests from inspectable scripts. Include wrong image, short read/write, save failure, and unknown USB outcome cases.
4. Establish the actual target body's firmware, complete original hashes, and its own two backup copies before planning writes. No camera has been accessed in this research session.
5. Validate fresh ST/FL2/FL3 photos with fixed scene, exposure, and white balance; inspect numeric metadata with `-n`, names, reset markers, and camera playback. Extend to existing looks, RAW/JPEG/HEIF, bursts, and video separately. Exact Alpha 1 II color matching and power-loss recovery remain open.

## Reproduce the local checks

From the repository root:

```sh
python3 research/verify_sources.py
python3 research/fetch_sources.py  # restores missing snapshots from pinned archives
python3 -m venv .venv-research
.venv-research/bin/python -m pip install -r research/requirements.txt
.venv-research/bin/python research/audit_baseline.py
.venv-research/bin/python research/audit_firmware.py
python3 -m unittest discover -s tests -v
python3 app.py check-package
```

The download helper `research/fetch_firmware.py` retrieves only the two approved official inputs and checks their sizes and recorded hashes; it does not access a camera. The firmware unpacking revision is in `research/upstream/fwtool.py/`. Keep source snapshots unchanged so their checksums remain useful.

The extraction commands used were:

```sh
.venv-research/bin/python research/upstream/fwtool.py/fwtool.py unpack \
  -f research/firmware/ILCE-7CM2-2.01/BODYDATA.DAT \
  -o research/extracted/ILCE-7CM2-2.01
.venv-research/bin/python research/upstream/fwtool.py/fwtool.py unpack \
  -f research/firmware/ILCE-1M2-4.00/BODYDATA.DAT \
  -o research/extracted/ILCE-1M2-4.00
```

Upstream files retain their original notices and licenses. The starting MIT license does not relicense the snapshots or Sony data. Some research snapshots have no repository-wide license file. Firmware installation, driver changes and camera access were not performed by this research work. Findings and reproducible evidence are committed and pushed when requested; official firmware and per-camera data remain excluded.
