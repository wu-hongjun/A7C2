# CP port review before the next A7C II trial

**Resolution:** all three numbered findings below are fixed in local revision 0.1.1-preview. [Fix details and rerun evidence](FIXES.md) supersede their unresolved status. The original review and counterexample results remain below for context. The remaining trial gaps still apply.

**October 18 follow-up:** the revised candidate was installed and passed full program/parameter and six-region AP runtime checks after battery removal. The user reported FL1/FL2/FL3 and Auto capture working. [The Mac case study](MACOS_INSTALLATION_LESSONS.md) records the host faults, scoped preservation review and WBI recovery; it does not close the CP runtime, reset/import, controlled color or full-format gaps below.

Reviewed October 17, 2026, against the pinned A7C II 2.01 original and existing candidate. **The modeled still-photo processing path is coherent, but the current installer should not be used for the next trial until the preservation checks and movie fallback are addressed.** This review did not change deployment assets or enter a camera service session.

## Findings to fix

### 1. [P1] Unrelated changes in saved CP storage can pass verification

Location: `app.py`, `Job.settings`, lines 149–158.

The routine compares all live properties before and after writing. It then retrieves a storage snapshot, but checks only the four intended target properties in that snapshot. It does not compare the other saved properties against a storage snapshot taken before the operation.

The offline counterexample keeps live properties intact, saves the expected four target values, and changes unrelated saved CP unit `0x02a40002`. The routine still records `settings_storage_verified`. That unit is particularly relevant because the port intends to preserve units 00/02 and per-body data.

This demonstrates a missing detector, not an observed camera-side write to unit02.

Fix: capture a separate storage baseline before the first settings write, compare the complete stored property set afterward, and permit only explicitly expected persisted changes. Account explicitly for memory-only edits that subsystem SAVE may also persist; comparing the previous live snapshot to later storage is not sufficient. Apply the same checks to restoration. Add a regression case for an unrelated stored-unit change and a legitimate preexisting live-versus-storage difference.

### 2. [P2] FL2/FL3 movie fallback mixes two existing looks

Locations in `assets/patches.json`: CP offsets `0x1384a4`, `0x1384a6`, and helper `0x138832`.

For CP modes19/20, the movie color dispatcher branches to the existing FL path and selects color parameter45. The gamma helper instead normalizes those modes to CP mode0 when column0 is selected. In a neutral modeled state this produces gamma0, while ordinary FL uses gamma37.

| Movie request | Selected color parameter | Gamma pair |
|---|---:|---|
| Existing FL, mode15 | 45 | 37/37 |
| Patched FL2, mode19 | 45 | 0/0 |
| Patched FL3, mode20 | 45 | 0/0 |

Thus the fallback is neither the complete original FL pipeline nor the complete original Standard pipeline. This may produce an unintended rendering when the new look survives a still-to-movie transition. It is not evidence of a crash or hardware damage.

Fix: define one fallback policy and make color and gamma dispatch agree. Using FL would retain the current movie color branch and normalize the movie gamma request to mode15 instead of mode0. Alternatively block the new looks in movies through a verified mode restriction. Re-test transitions, both gamma call sites, overrides, and metadata. Any binary fix must update the JSON/C patch tables, full candidate hash, native installer, and asset manifest together; editing one host JSON value is insufficient.

### 3. [P2] Reset-data changes are counted as preserved

Location: `core.py`, `verify_preservation`, lines 103–114.

The parsed backup property contains `data`, `attr`, and `resetData`. This comparison checks only `data` and `attr`, including for intended targets. A property whose current value and attributes remain equal but whose reset data changes is counted as preserved.

The standalone counterexample changes only `resetData` and is accepted. This could hide changes that surface later when settings are reset. It is not evidence that the current native writes actually change reset data.

Fix: include reset data in preservation checks, including narrow telemetry exceptions and intended targets unless a specific reset-data change is explicitly part of the plan. Add regression tests for unrelated and target-property reset-data changes.

## Processing checks completed

Reproduction script: [review_cp.py](review_cp.py). Full machine-readable observations: [evidence/cp-review.json](evidence/cp-review.json).

| Family | Cases | Modeled result |
|---|---:|---|
| Still/movie parameter dispatch | 4,096 | All 256 byte values, four variants, two context-flag values, two dispatchers; differences restricted to modes19/20 |
| Gamma selection | 336 | Modes0..20, two columns, eight override combinations; existing modes0..18 unchanged |
| Both photo-name writers | 512 | All 256 byte inputs in each writer; only19/20 change, to FL2/FL3 |
| MakerNote ColorMode | 256 | All byte inputs preserve previous behavior except19→22 and20→23 |
| Primary/secondary chroma | 171 | Parameters0..56; existing outputs unchanged in neutral context |
| Matrix selection and blending | 3,648 | Parameters0..56, sixteen selectors, three weight basis vectors and a mixture; existing outputs unchanged |
| Original-code record oracle | 134 | New chroma/matrix records produce the same modeled results when routed through an existing parameter in the original code |
| **Total** | **9,153** | Isolated instruction and control/table tests |

The emulator loads the CP image at relocation base `0x108000`, maps synthetic request structures and the extracted factory parameter units, and checks that each function reaches its expected return/handoff. It stubs diagnostic/logging calls and the optional chroma adjustment helper. It does not emulate CP startup, the sensor, image processor, DMA, actual exposure/white-balance policy, or a complete photo capture. Passing these checks does not prove photographic or video correctness.

Additional static checks:

- All 47 original gamma pointer rows are copied byte-for-byte. Rows48/49 select the new records in all seven columns.
- The extended initialized-data range ends at runtime address `0x1209aa0`, leaving 1,376 bytes before the next zero-initialized region at `0x120a000`. No overlap was found at that boundary.
- Six existing records with identical curve prefixes exhibit the same donor selector3→target selector5 adjustment at record offset `0x21c`. This supports the FL2/FL3 adaptation rather than treating that changed byte as arbitrary. The exact full hardware meaning remains to be established.
- The 50-row range also admits row47, which is empty. Normal modeled requests do not select it. Add an explicit rejection/fallback test for that hole across all gamma consumers before treating arbitrary gamma-index input as supported.

The preservation counterexamples are in [review_host_guards.py](review_host_guards.py) and [evidence/host-guard-review.json](evidence/host-guard-review.json). They use synthetic properties and mocked sessions; they do not import the USB transport.

## Remaining trial gaps

The post-restart worker reads six AP runtime regions, while CP verification currently reads the CP file on disk and native parameters. There is no direct verification here of the CP executing those changed instructions. Fresh photo metadata and controlled rendering comparisons must be part of the first trial; they cannot be replaced by the AP runtime check.

Failure recovery is not a single atomic transaction. Program rollback does not roll back all native parameter saves and WBI handling. Before a new write trial, have an inspectable process for retrieving an already-produced native result after a timeout and classifying program/setting states without repeating writes. Recreate the missing native-worker fault-injection tests. Do not rely on normal memory-card updater retry instructions as proof of arbitrary-damage recovery.

Full camera reset and settings import/restore also need their own scenario. In the official A7C II template the two reserved AP bindings have attribute `0x60` and reset byte00. The installer changes their current values to0a/0b and the AP initial tables, but does not establish what every full-reset path will do to the saved bindings. The two CP color units have attribute `0x08` and no reset-data payload in that template. These are factory-template facts, not reads from this particular camera.

After the fixes and offline failure tests, start with read-only identity/program readback and two verified copies of this body's own backup. Then use a narrowly controlled still-photo trial: ST/FL2/FL3 with fixed scene, exposure and white balance; reset defaults; fresh numeric metadata and names; camera playback; and all existing looks. RAW/JPEG/HEIF, burst modes, still-to-movie transitions, and video need separate coverage.

## Does this enable arbitrary LUTs for still photos?

**No.** This implementation adds two specific native Creative Looks using tone-curve records, chroma records, illuminant-dependent matrices, dispatch changes and identifiers. It contains no arbitrary `.cube` import or still-photo 3D-LUT application path.

Sony's A7C II documentation supports user LUT registration using 17-point or 33-point CUBE files, and documents PPLUT1–4 recording as movie-only. [Manage User LUTs](https://helpguide.sony.net/ilc/2360/v1/en/contents/211h_manage_lut.html), [Picture Profile](https://helpguide.sony.net/ilc/2360/v1/en/contents/0412D_picture_profile.html).

A future custom-photo-profile project could investigate native curves/matrices or trace whether the existing 3D-LUT hardware path can be used for still-image rendering. Those are separate implementations. An arbitrary nonlinear 3D transform cannot generally be reproduced exactly by substituting a few tone curves and matrices. Input color space/gamma, output format, preview-versus-recorded rendering, and ISP resource limits would need validation. The current port does not establish that possibility.

## Historical connected-camera observation on October 17

macOS USB inventory reports `ILCE-7CM2`, vendor/product `054c:0e8a`, link speed480 Mb/s. [Inventory evidence](evidence/usb-presence.json) omits serial numbers. This confirms USB visibility; it does not confirm the installed firmware revision or its program hashes. No service authentication, diagnostic upload, program/settings write, or installation was performed.

That paragraph describes the initial observation, not the later completed installation. The subsequent normal protocol version, complete original/candidate fingerprints, native operations and final runtime result are recorded in [the hardware evidence](evidence/macos-installation-review.json).

Run the new offline checks from the repository root:

```sh
.venv-research/bin/python research/review_cp.py
python3 research/review_host_guards.py
```
