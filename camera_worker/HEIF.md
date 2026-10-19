# 10-bit HIF-to-HIF worker

A working offline prototype now applies the 11 validated Fujifilm sRGB CLFs and
manual-lens EXIF presets to a **separate 10-bit HIF copy**. The static
[ARM64 Linux candidate](dist/hif-worker-aarch64) is built for the firmware's
AArch64/Linux environment. It has **not run on the camera or under real Linux**.
Its ARM64 startup runs in Unicorn; complete codec processing runs on this Mac.
There is no installer, camera transport, capture watcher or firmware hook.

The user authorized reading a sample from EXTREME. One 9,715,712-byte existing
A7C2 HIF was copied to ignored private test storage; source-before, source-after
and copy hashes matched. Only the local copy was processed. No original photo,
profile table, card path, personal EXIF or camera backup is published.

## Supported path

| Stage | Implementation |
| --- | --- |
| Input | One primary image, rendered SDR sRGB: NCLX primaries 1 / transfer 13, supported YCbCr matrix 1/5/6, declared full or limited range, 10-bit 4:2:0 or 4:2:2 |
| Decode | Pinned libheif 1.23.6 + libde265 1.1.3; strict decode; RGB in little-endian 16-bit storage with **10 significant bits** |
| LUT | Normalize by 1023, existing float32 CLF matrix/transfer/tetrahedral pipeline, round to 0–1023; no JPEG or RGB8 intermediary |
| Encode | Pinned 10-bit-only x265 4.2; quality 95 by default, ultrafast, one frame thread, no worker pool/WPP; output HEVC 10-bit **4:2:2**, sRGB transfer, BT.601 matrix 6, full range |
| Geometry | Decode untransformed source tiles; preserve ordered rotations/mirroring in the output grid's container properties. Grid dimensions retain right/bottom edge crops. Nontrivial `clap` crops are declined |
| EXIF | Fresh allowlisted capture metadata and host-selected manual-lens tags; unknown shooting aperture omitted; no GPS, serials, MakerNote, XMP or stale thumbnail |
| Publication | Hash-bound private job; exclusive temporary file, fsync, reopen, verify geometry/color/exact EXIF and decode **every output tile**, then exclusive publication as `result.HIF` |

The input's real layout is 7008×4672 raw pixels, displayed as 4672×7008 using
orientation 8. Its six source tiles are 3520×1600. The worker divides those into
880×800 encoder tiles (48 output tiles) to lower x265 memory use, while decoding
each source tile once. It does not need a 32MP RGB frame allocation.

Input is capped at 64 MiB, 8192 pixels per side / 40MP, 256 tiles and 6MP per
independently decoded source tile. Encoder tiles are at most 1MP. Linux requests
512 MiB address space, 1800 CPU seconds and 128 MiB output-file limits; libheif
also has explicit item/tile/allocation limits. These limits do **not** establish
that the camera has sufficient free memory. The final full-resolution test used
about 232 MiB peak RSS and 35 seconds on the Mac while other checks ran.

HLG/PQ, other primaries/transfers, ICC conversion, 8/12-bit input, alpha,
multiple primary images, large untiled photos and arbitrary crops are
unsupported. Unsupported color is rejected, never relabeled as sRGB. Sony
range autocorrection is disabled; a decoding warning stops processing rather
than silently guessing. This sample's container and HEVC range flags agree.

Re-encoding is lossy. Retaining 10-bit precision does not preserve every original
pixel. The Fujifilm profiles adapt the RGBTable stage to already-rendered sRGB;
they do not reproduce full Fujifilm RAW development or install native Creative
Looks. Source metadata/lens settings are frozen at host preparation, not shutter
time. Camera AUTO behavior, IBIS, quick switching, Sony playback/DCF registration
and autonomous image discovery remain separate integration work.

## Prepare and run locally

Build with local tools and the supplied source archives; neither command uses
the network. `--host` creates the Mac test executable. Omit it for the static
ARM64 Linux candidate. An installed CMake or a locally unpacked CMake binary
must be supplied as an absolute path. The tested versions are CMake 4.2.0,
Apple clang 21.0.0 for Mac and Zig 0.16.0 for Linux ARM64.

```sh
.venv-research/bin/python camera_worker/build_heif.py \
  --downloads camera_worker/heif_sources --output research/build/hif-host \
  --cmake /absolute/path/to/cmake --host

.venv-research/bin/python -m camera_worker.heif_pack \
  /path/to/local-copy.HIF research/build/hif-job \
  --inspector research/build/hif-host/hif-worker-host --completed \
  --clf research/build/fujifilm-srgb/classic-chrome-srgb.clf \
  --lens-presets photo_worker/lens-presets.example.json \
  --lens example-takumar-50 --aperture 2.8

research/build/hif-host/hif-worker-host --check research/build/hif-job
research/build/hif-host/hif-worker-host --run research/build/hif-job
```

The result stays in the local job directory. Omit `--clf` for lens-only handling,
or `--aperture` when the actual f-number is unknown. The existing job directory
must not be reused. `--check` verifies job bindings and supported headers, not
all input entropy; `--run` fully decodes and verifies the produced copy.

HIF jobs use a distinct `A7HIFS01` magic, version 1, with the existing 176-byte
little-endian header layout and SHA-256 bindings. Fixed assets are `input.HIF`,
`metadata.exif`, optional `look.bin` and the last-written `job.bin` ready marker.
The shared C core catches its own `longjmp` failures before returning to C++;
C++ object lifetimes are not crossed by a C error jump. The compressed input
snapshot is rehashed before decoding, closing the hash/read gap.

An existing result or symlink is never overwritten. SIGINT/TERM request cleanup;
forced termination or power loss may leave an unpublished `.hif-worker-*.tmp`.
If publication succeeds but directory fsync fails, the error reports
`published=true`; inspect that result rather than blindly retrying. The actual
camera filesystem's `renameat2(RENAME_NOREPLACE)` and directory fsync behavior
still need controlled hardware validation. No such validation was attempted.

## Offline evidence

[HIF checks](../research/evidence/heif-worker-checks.json) record:

- 11 exact previously SDK-validated CLFs × 42,084 float vectors: maximum error
  below 0.01 on an RGB8 scale against OCIO; separate lossy HIF round-trip error
  measurements for each profile.
- All eight orientations, both input chroma formats, pixel/geometry comparisons
  and independent FFmpeg display-transformed decoding; unknown aperture omitted.
- All **1024** gray levels retained in the HIF result and independent RGB16
  FFmpeg decoding, proving this path is not an 8-bit pipeline in a 10-bit wrapper.
- Full-resolution real A7C2 input → LUT/lens HIF, every output tile verified,
  independent full FFmpeg decode and ExifTool depth/lens checks.
- Malformed/hash/tampered-LUT/symlink/wrong-job/color/depth guards, existing-result
  preservation and interruption cleanup.

[Sanitizer evidence](../research/evidence/heif-worker-sanitizers.json) covers
29 cases with address/undefined instrumentation on the C/C++ worker wrappers;
the linked release codec libraries are not sanitizer-instrumented.
[Build records](../research/evidence/heif-worker-builds.json) bind source,
archives, libraries, tools and artifacts. [ARM64 startup evidence](../research/evidence/heif-worker-startup.json)
only proves actual target startup/version instructions in a restricted Unicorn
syscall model; it does not exercise threaded HEVC encoding or Linux scheduling.
The unchanged JPEG executable was rebuilt byte-for-byte and its own full
regression/emulator checks rerun after the shared-core changes.

The HIF artifact includes GPL/LGPL codec components. Complete pinned codec
source archives, project sources/build script and notices accompany the binary;
see [sources](heif_sources/README.md) and [NOTICE](../NOTICE.md).
