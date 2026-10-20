# Native photo launcher and explicit completion handoff

The [static ARM64 launcher](dist/photo-launcher-aarch64) can select LUT/lens
presets, freeze them under an event ID, snapshot a completed JPEG/HIF, prepare
its worker job **without Python**, and start the matching native worker.
It is an offline-tested, hardware-untested candidate. Nothing was installed or
run on the camera, and this task did not access its filesystem or USB service.

**This supplies the native controller and handoff contract, not a Sony menu,
custom-button binding, service-loaded entry module, boot service or recovered
firmware file-close callback.** The CLI is the selector/control interface for a
future adapter. There is no watcher, shell, automatic startup, firmware patch,
settings write, capture interception or DCF/playback registration.

The next [Sony integration candidate](SONY_INTEGRATION.md) adds a native
LUT/lens menu model, source-bound callback decoding, original-handler forwarding
and bounded completion matching. It still supplies no installed Sony hook or
renderer, and releases only candidates pending same-file identity proof. See
the [recovered callback/menu boundaries](../research/SONY_MENU_COMPLETION.md).

## Per-photo association and completion

A trusted future adapter must perform these steps for the same unique event ID:

1. Select a look, lens and known shooting aperture (or `unknown`).
2. **Arm before exposure**, freezing that selection. A later switch affects
   subsequently armed events, not this one. The prototype records the arm
   generation; it cannot prove the call actually occurred at shutter time.
3. After the full original file's final write/close and successful save, submit
   its relative path and expected SHA-256. The adapter must derive this identity
   from the same completed frame; a path or encoder notification alone is not
   sufficient. Supply the sRGB assertion explicitly for JPEG; HIF NCLX is checked.
4. Run that ready event once, when an external scheduling policy permits it.

`--enqueue-completed-srgb` is an assertion at a trusted boundary. Matching size,
mtime, a successful filesystem open or a quiet directory is **not** used to infer
completion. The producer digest rejects a delayed event whose filename now
refers to different bytes. Native ingress confines path components to the
supplied capture root with `openat`/`O_NOFOLLOW`; it copies through one read-only
regular-file descriptor, checks size/device/inode/nanosecond mtime/ctime, hashes
the source again, and compares the private snapshot with the expected digest.
A mutation aborts readiness. Originals are never opened for writing.

The actual firmware producer still needs source-bound proof of the **full
original path, matching frame ID, successful final file close, error/cancel
handling, and shutter-time association** for JPEG and HIF separately. Existing
[encoder/saved-image research](../research/CAPTURE_HANDOFF.md) is not a validated
file-close API. No undocumented callback/address was wired into capture here.

## Interface

These are interface examples for an already staged **local test workspace**,
not camera deployment instructions. Use a native Mac test launcher on this Mac;
the ARM64 ELF is for Linux and was used only in restricted offline emulation.

```sh
photo-launcher --list WORKSPACE
photo-launcher --select WORKSPACE classic-chrome-srgb example-takumar-50 2.8
photo-launcher --arm WORKSPACE frame-0001
# A trusted producer supplies SHA256_OF_COMPLETED_FRAME after final close:
photo-launcher --enqueue-completed-srgb WORKSPACE CAPTURE_ROOT \
  DCIM/example.JPG frame-0001 SHA256_OF_COMPLETED_FRAME
photo-launcher --run WORKSPACE frame-0001
photo-launcher --status WORKSPACE frame-0001
```

`none` means no LUT or preserve source lens. `unknown` omits manual-lens shooting
aperture; maximum aperture remains lens information, not a measured f-number.
Selection IDs are stable ASCII IDs from the catalog. The `--list` diagnostic
prints numeric aperture 0 for unknown; output EXIF never receives f/0.
An aperture is accepted only with a manual lens, between its maximum aperture
and f/128. Focal lengths 1–5000 mm and maximum apertures f/0.5–128 are supported.
EXIF rationals use bounded integer construction, including the upper focal
limit; negative APEX fields below f/1 are omitted.

Native ingestion supports baseline sRGB JPEG with EXIF orientation 1–8 and the
[existing bounded 10-bit sRGB HIF contract](HEIF.md). JPEG retains its original
pixel orientation and writes the matching EXIF orientation without a host
normalization/re-encode. HIF retains container transformations and uses EXIF
orientation 1. Fresh TIFF is built in a bounded 65,527-byte buffer; source
endianness/offsets are validated and only allowlisted capture/lens tags survive.
GPS, MakerNote, old thumbnails and XMP are excluded. JPEG ICC conversion,
progressive JPEG and HLG/PQ HIF are declined rather than guessed.

The result remains `WORKSPACE/jobs/EVENT_ID/result.jpg` or `result.HIF`.
A LUT processes the already-rendered Sony image, including one shot in AUTO;
it does not unlock AUTO Creative Look controls or undo the original Sony look.
Lens EXIF changes apply to the copy only; this does not configure IBIS or ARW.

## Durable states and process lifetime

| State | Meaning / allowed next operation |
| --- | --- |
| `armed` | Immutable selection exists; one explicit completion may be submitted |
| `frame-armed` | Frozen parent selection; derive distinct group/media children, never encode the parent |
| `preparing` | Snapshot/metadata/ready marker construction in progress; never runnable |
| `ready` | Hash-bound completed job is durable; one run may begin |
| `running` | Persisted before child launch; an interrupted/abandoned run requires review |
| `done` | Worker succeeded; result SHA-256 and done state persisted |
| `failed` | Ingress failed; retained for inspection, no automatic retry |
| `needs-review` | Worker/process/final receipt failed; inspect retained files, no automatic retry |

The event directory is created exclusively at arm time and never recycled.
The last-written worker `job.bin` is still required. A separate 160-byte ready
binding contains magic, selection SHA-256, worker-header SHA-256, catalog SHA-256,
reserved zeros and a full SHA-256 checksum. Switching the global selection does
not affect existing events. The job's selection/binding is rechecked before run.
Hashes detect inconsistency, not an attacker able to rewrite the whole workspace.

A short control `flock` serializes selection/state transactions. A separate
worker lease permits **one encoder process across all events**. The child keeps
that lease descriptor across exec, so killing the launcher cannot release it
while the worker is still alive. Selection and new handoffs remain available
during encoding. The launcher uses fixed worker names, verifies their hashes,
then fork/execs with fixed argv and an `LC_ALL=C` environment; no shell/PATH lookup.
Linux uses the verified executable descriptor with `fexecve`. Mac testing uses
`F_GETPATH`/`execve` inside the private owned workspace.

INT/TERM is forwarded to the owned child and the parent waits for it. Hard
termination can leave `running`, a partial journal or a private worker temporary
file. A result may already exist if termination interrupted acknowledgment.
None of those states is resumed or overwritten automatically. This version has
no recovery override or garbage collector. Inspect retained evidence first.

A workspace must be an owned POSIX directory with no group/other permissions;
FAT/exFAT cards generally cannot provide that contract. Actual storage/exec
permissions, fd-exec, file locking, `renameat2(RENAME_NOREPLACE)` and directory
fsync must be tested before deployment. Free space is conservatively checked
before ingress and execution, but a later ENOSPC still stops safely. Linux caps
the launcher at 512 MiB address space / 1800 CPU seconds; worker limits also apply.
The lease does not coordinate with Sony capture or establish available RAM,
battery/thermal policy, privilege, persistence or capture coexistence.

## Build and local bundle

The [October 20 handoff follow-up](../research/COMPLETED_PHOTO_HANDOFF.md) adds
`--arm-frame ROOT FRAME_ID` and `--derive-file ROOT FRAME_ID GROUP MEDIA`.
The latter creates a separate child event from the parent's frozen selection,
with group 0–1 and media 0–3. It never reads the current global selection.
Frame parents have state `frame-armed` and cannot be encoded directly. This
controller API is for idle preparation/fan-out; no Sony callback is registered.

Build JPEG/HIF candidates as documented in [README](README.md) and [HEIF](HEIF.md).
The launcher reuses that build's pinned local HEIF/de265/x265 static prefix:

```sh
.venv-research/bin/python -m camera_worker.build_launcher \
  --codecs research/build/hif/linux --output research/build/launcher-linux
```

Use a host codec prefix and `--host` for Mac. For sanitizer testing, first build
**all** HEIF codec libraries with `build_heif.py --host --sanitize`, then build the
launcher with `--host --sanitize`. Mixing instrumented and uninstrumented C++
container implementations is rejected by the build script. Worker child tests
use the separately tested release JPEG/HIF binaries.

`camera_worker.launcher_bundle` creates a local private workspace from the three
pinned ARM64 artifacts, all 11 exact SDK-validated CLFs and a lens-preset JSON.
It compiles a bounded binary catalog, initial selection and numeric LUT blobs;
the native process needs no JSON, XML, Python, Pillow or OCIO runtime. It also
copies notices/licenses. Custom lens definitions are imported on the host; the
native selector switches among their catalog IDs. Use `--host-test` only for a
local workspace containing native Mac/sanitizer binaries.

```sh
.venv-research/bin/python -m camera_worker.launcher_bundle \
  --output research/build/photo-workspace \
  --launcher camera_worker/dist/photo-launcher-aarch64 \
  --jpeg camera_worker/dist/jpeg-worker-aarch64 \
  --hif camera_worker/dist/hif-worker-aarch64 \
  --luts research/build/fujifilm-srgb \
  --lenses photo_worker/lens-presets.example.json
```

This performs no installation or device operation. Generated profile tables and
photos stay ignored/private. The three native binaries and corresponding
project/codec sources are public; their licenses are retained in [NOTICE](../NOTICE.md).

## Offline acceptance

[Native handoff tests](../research/evidence/photo-launcher-checks.json) cover
44 cases, including all 11 LUTs, all JPEG orientations, out-of-order completion
with frozen presets, unknown aperture, rational bounds, full-resolution JPEG
and a previously copied real HIF, independent metadata/decode checks, source
hash reuse/mutation guards, actual locks, parent cancellation, parent SIGKILL,
and injected ENOSPC/fork/exec failures. No original was modified.

[37 sanitizer cases](../research/evidence/photo-launcher-sanitizers.json) cover
the launcher **and linked codec/parser libraries** with ASan/UBSan; actual worker
children are release builds. [Five ARM64 scenarios](../research/evidence/photo-launcher-emulation.json)
execute the original target startup/menu/selection instructions under restricted
Unicorn syscall models. Target ingress/fork/exec and HEVC threading are not
executed in that model or on Linux/camera. JPEG and HIF worker regressions,
including full-resolution images and their existing fault cases, also pass.

[Build/provenance receipts](../research/evidence/photo-launcher-builds.json) bind
artifacts and sources. The remaining deployment work is a separately validated
invocation path, Sony menu/button/shutter/completion adapters, idle/power/resource
scheduling, private writable storage, and optional media database/thumbnail
registration. A native controller tested offline does not establish those APIs.
