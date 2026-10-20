# Standalone ARM64 JPEG worker — offline-validated candidate

Current [native capture probe findings](../research/NATIVE_CAPTURE_PROBES.md)
correct an earlier FSYS cursor/readable-buffer assumption. Native handoff now
uses bounded vector resolution and a source-bound locked position observation;
the probe remains uninstalled and hardware unvalidated.

This is a **camera-targeted build, not a hardware-validated camera application**.
The 491,656-byte [ARM64 Linux executable](dist/jpeg-worker-aarch64) is statically
linked with musl and libjpeg-turbo. It has no Python, Pillow, OpenColorIO, C++
runtime, Sony library, dynamic loader or service-mode dependency. It processes
an explicitly prepared completed-JPEG job into a separate `result.jpg`.

No camera access, enumeration, installation or execution occurred during these
tests. The existing installer and firmware assets do not use this binary.
There is no installer, launch script, shared-library constructor, capture watcher,
background process or firmware/settings operation in this worker.

A separate [10-bit HIF-to-HIF worker](HEIF.md) now supports the same CLF and lens presets, with its own codec build, job format and offline evidence. The [native launcher](LAUNCHER.md) now prepares completed-photo jobs and selects LUT/lens presets without Python; it preserves valid JPEG EXIF orientations 1–8.

## What is implemented

- **All 11 Fujifilm JPEG adaptations:** the previously validated CLF pipeline is
  exported into a bounded native binary representation. It retains four color
  matrices, two shared 4097-point transfer tables, clipping and the 32³
  blue-fastest tetrahedral table. [Color scope and Adobe reference](../research/FUJIFILM_SRGB_CONVERSION.md)
  remain unchanged: this adapts the RGBTable stage to sRGB JPEG, excluding full
  RAW/DCP development or a measured Fujifilm-camera match.
- **Manual-lens EXIF presets:** the host resolves and freezes a selected preset,
  actual aperture when known, and allowlisted capture metadata. The native runner
  validates and writes that EXIF. Helios and Takumar selections, including an
  unknown actual aperture, pass independent ExifTool checks. Preset selection is
  at job preparation, not at the shutter.
- **Scanline JPEG processing:** one RGB row, bounded codec buffers and a roughly
  416-KiB LUT; no full-image RGB or coefficient allocation for accepted inputs.
  Sequential 8-bit RGB/YCbCr JPEGs, at most 8192 per side / 40 million pixels and
  64 MiB compressed. Progressive, multi-scan, lossless, arithmetic, grayscale and
  CMYK inputs are declined. The native input contract is rendered sRGB; output EXIF may preserve orientation 1–8. The legacy host packer still normalizes non-1 inputs explicitly.
- **Fresh output metadata:** sRGB ICC, sRGB EXIF ColorSpace, validated orientation 1–8,
  dimensions and the frozen lens/capture tags. GPS, MakerNotes, serial numbers,
  old thumbnails and XMP are excluded. JPEG quality is snapshotted, default 95,
  with 4:4:4 sampling and a scalar integer DCT. Recompression is lossy.
- **Bounded execution:** single process; Linux requests 64 MiB address space,
  120 CPU seconds and 128 MiB output-file size limits. Startup fails if limits
  cannot be set. INT/TERM request cleanup at scanline boundaries.
- **Protected publication:** fixed filenames in an explicitly opened job
  directory; no symlink assets or arbitrary output paths. Input/LUT/EXIF/header
  hashes are checked. A private exclusive temporary file is encoded, fsynced
  and decoded before Linux `renameat2(RENAME_NOREPLACE)` publishes `result.jpg`.
  An existing result is never replaced. Directory synchronization follows.

Hashes provide consistency, **not authentication** against an actor who can
rewrite the entire job. Use private job directories. A forced kill/power loss
can leave a private `.jpeg-worker-*.tmp`; it is never a finished result. A failure
after publication can leave a valid `result.jpg` with `published:true` in the
diagnostic. Inspect it rather than retrying or deleting it automatically.

## What the build does not yet supply

The native executable consumes a prepared job. It does **not** parse lens JSON,
CLF/XML or source XMPs on-camera; the host packer performs that work. It does not
discover camera photos, freeze settings at shutter time, offer a camera menu,
set IBIS focal length, register a DCF image/playback entry, print, or change AUTO
mode Creative Look restrictions. An autonomous camera job producer and UI are
still needed. The output currently stays in its job directory.

The offline firmware identifies AArch64 Linux **4.9.337-scel+**, glibc **2.25**.
A static musl executable avoids depending on Sony's glibc/plugin namespace.
The generic ARMv8-A target and disabled codec SIMD avoid requiring a newer CPU
extension; ordinary compiler-generated ARMv8/NEON instructions remain possible.
An ELF match and host emulation cannot establish camera execution permissions,
filesystem behavior, available memory, scheduling, thermal impact or coexistence
with capture. Those are required before calling this hardware compatible.

In particular, the actual writable filesystem must support
`renameat2(RENAME_NOREPLACE)` and directory `fsync`. The worker fails closed if
publication is unsupported. The emulator models exclusive publication with a
host APFS operation; it does not prove the camera card driver's implementation.

## Prepare a native job locally

First prepare a completed photo using the existing [host worker](../photo_worker/README.md)
and a converted CLF. Then export its verified snapshot:

```sh
mkdir -p research/build/native-jobs
.venv-research/bin/python -m camera_worker.pack \
  /path/to/existing-host-job research/build/native-jobs/shot-001
```

If that snapshot has an ICC profile, non-1 orientation, or progressive coding,
the packer requires explicit `--normalize-input`. This performs the ICC/orientation
conversion and a quality-100 baseline JPEG re-encode **on the host**, producing a
separate input copy. The original remains unchanged. It is not a native ICC
engine or rotation implementation. Already suitable untagged sRGB snapshots,
with the host job's explicit sRGB assertion, are copied byte-for-byte.

The native interface is `jpeg-worker --check JOB_DIRECTORY` (hashes, metadata,
LUT and JPEG-header checks), or `--run JOB_DIRECTORY` (complete decode, process,
verify and publish). `--check` alone does not verify entropy data through EOI.
The diagnostic `--vectors` mode uses explicit `vectors.in/out` files for offline
float conformance tests; it is not a production photo command. These describe
the interface, not a camera installation procedure. On this Mac, use the host
test build; the ELF runs only inside the offline emulator in this task.

Export format version 1 uses these fixed files:

| File | Content |
| --- | --- |
| `job.bin` | 176-byte ready header: `A7JOBS01`, version, flags, quality, dimensions, asset sizes, input size, three SHA-256s, header SHA-256 |
| `input.jpg` | Closed, snapshotted input JPEG, never modified |
| `metadata.exif` | Bounded fresh output EXIF; native TIFF-offset/type/tag checks apply |
| `look.bin` | Optional 426,152-byte `A7LUT001` header plus 106,534 float32 values |
| `manifest.json` | Human-readable provenance and frozen preset; not parsed by the native runner |

Integers and float32s are little-endian. The header's first 144 bytes are hashed
into its final 32 bytes. The packer writes `job.bin` after the asset files, then
the provenance manifest. The native LUT orders four 3×3 matrices, decode/encode
transfer tables and 32³ RGB triplets. Input and output file bounds are checked
before coding; metadata is capped at one JPEG APP1 payload.

To create the **local, non-installing** bundle of executable, all 11 native LUT
blobs, example host preset definitions and notices:

```sh
.venv-research/bin/python -m camera_worker.bundle \
  --binary camera_worker/dist/jpeg-worker-aarch64 \
  --luts research/build/fujifilm-srgb \
  --output research/build/camera-worker-bundle
```

The generated profile arrays remain in ignored local build storage. The source
repository contains the reproducible converters, executable and validation
hashes. Bundling neither copies files to the camera nor runs the executable.

## Rebuild and verify offline

The reference compiler is the locally installed **Zig 0.16.0**, using its bundled
musl. The explicit locally cached `turbojpeg-sys-1.2.0.crate` supplies unmodified
libjpeg-turbo **3.1.0**. Its pinned SHA-256 is
`49b5a974bc59382d35927e3a07eef2ba9a24fe7227cefe52ec44b508e1b90f86`.
No dependency is fetched and nothing is installed. The build recipe generates
fixed configuration headers, compiles scalar 8/12/16-bit library objects and
restricts the runner to sequential 8-bit input. Arithmetic and explicit codec
SIMD are disabled. No CMake or external target sysroot is needed.

```sh
.venv-research/bin/python camera_worker/build.py \
  --codec-archive /path/to/turbojpeg-sys-1.2.0.crate \
  --output research/build/new-linux-build
# Repeat with --host for a host build, or --host --sanitize for ASan/UBSan.

.venv-research/bin/python -m unittest discover -s camera_worker/tests -v
.venv-research/bin/python -m research.check_camera_worker \
  --linux-build research/build/new-linux-build \
  --host-build research/build/new-host-build \
  --asan-build research/build/new-sanitizer-build \
  --luts research/build/fujifilm-srgb --full-resolution
```

Use new build/output directory names. The retained [build record](dist/build.json)
pins the compiler executable, target, flags, codec tree, generated headers,
worker source hashes and final ELF. [The earlier two-clean-build receipt](../research/evidence/camera-worker-reproducibility.json) applies to the original orientation-1 build. The current artifact was rebuilt for native orientation-preserving handoff; its current hash is in `dist/build.json` and its regression checks were rerun.

## Results and next gates

[Offline evidence](../research/evidence/camera-worker-checks.json) records:

- All 11 native LUTs: 42,084 RGB vectors each against the previously
  SDK-validated OCIO CLF route; worst float difference **0.002161 RGB8 level**.
  A rounded result can differ by one level at a rounding boundary.
- All 11 JPEG jobs under ASan/UBSan; independent ExifTool lens and capture-tag
  checks, fresh ICC, and unchanged originals. Additional malformed-job,
  unsupported-input, hash, symlink, publication-race and interruption checks.
- The **actual cross-built ELF** executes startup, JPEG encoding, a rejected
  input and 1,024 color vectors in Unicorn. Explicitly injected output-space,
  unsupported-publication and post-publication sync failures exercise cleanup.
  Syscalls and requested resource limits are recorded. Only generated host job
  files are visible; no firmware executable, device or camera runs.
- A 7008×4672 native host JPEG test stays within **one RGB8 level** at 64 flat
  patch centers, including recompression. One run took about **2.2 seconds** and
  **3 MiB peak RSS** on this Mac, with a 21,024-byte RGB row. These are host-only
  synthetic measurements. Peak guest mappings in emulation include an 8-MiB
  synthetic stack; they are not camera resident-memory measurements.

Before an independently authorized camera experiment: establish an isolated
launch/stop/status path, verify permissions and writable filesystem operations,
measure resources and suspend/capture interaction, then provide a completed-file
job producer and shutter-time preset association. Native menu selection,
media/playback registration and broader JPEG/format coverage follow separately.
The native MLUT allocation-to-encoder research remains independent and unresolved.

This software is based in part on the work of the Independent JPEG Group.
[Third-party notices and licenses](../NOTICE.md) cover libjpeg-turbo, musl, Zig
and applicable Adobe algorithm portions. No Sony executable or firmware data is
included in the worker binary.

## Sony integration follow-up

The [native Sony adapter core](SONY_INTEGRATION.md) adds a two-row LUT/lens
selector and bounded per-file callback observation, with original-firmware
instruction tests and JPEG/10-bit HIF launcher replay. It remains an offline
candidate: physical button routing, Sony rendering, shutter/request identity,
same-file completion proof and a loadable hook are still required before use.


The [capture/file identity follow-up](../research/CAPTURE_FILE_IDENTITY.md)
adds a checked recording-object/cookie/open/real-close ledger for JPEG/HIF.
It remains an offline model: early arming across modes, scoped Sony hooks,
owned encoded bytes and resource-safe dispatch still block automatic capture
processing. No installation is included.

The [owned-byte handoff follow-up](../research/COMPLETED_PHOTO_HANDOFF.md) traces
the synchronous FSYS scratch-write boundary, adds a bounded encoded-image
collector, and implements frozen frame-to-file fan-out in the launcher. Offline
JPEG/10-bit HIF replay survives destruction of scratch/source names. The native
adapter, same-open initial position and readable-memory proof remain required.
