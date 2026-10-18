# Completed-JPEG LUT and manual-lens worker (host prototype)

This runs on the computer. It takes an explicitly selected, completed local JPEG, snapshots the image/look/lens selection, and writes a **new JPEG copy**. There is no camera transport, installation command, filesystem watcher, native menu change or printer connection.

The tested environment is the repository's existing `.venv-research`: Python 3.14, Pillow 12.1.0 and OpenColorIO 2.6.0. Dependencies are pinned in [requirements.txt](requirements.txt); no new packages or system configuration were installed for this prototype. Run module commands from the repository root.

## Prepare and run a photo job

```sh
mkdir -p research/build/jpeg-jobs research/build/jpeg-output
.venv-research/bin/python -m photo_worker list-lenses photo_worker/lens-presets.example.json

.venv-research/bin/python -m photo_worker prepare \
  /path/to/completed-photo.jpg research/build/jpeg-jobs/shot-001 \
  --cube /path/to/look-srgb.cube \
  --lut-contract encoded-srgb-to-encoded-srgb \
  --lens-presets photo_worker/lens-presets.example.json \
  --lens example-helios-58 --aperture 4 \
  --completed --assume-srgb

.venv-research/bin/python -m photo_worker run \
  research/build/jpeg-jobs/shot-001 research/build/jpeg-output/shot-001-graded.jpg
```

Use your own lens values; the supplied Helios/Takumar entries are examples. Select another lens ID on the next `prepare` command to switch presets. The saved job contains a full preset snapshot, so subsequent edits or selection changes cannot silently change that job's lens metadata. This snapshot occurs at **job preparation**, not at the camera shutter. Automatic shutter-time association still needs a native capture hook.

Omit `--aperture` if the actual shooting aperture is unknown. For a selected manual lens, the worker then omits FNumber/ApertureValue rather than retaining an unrelated camera-reported value. Maximum aperture remains available in LensInfo and, when representable, MaxApertureValue. To apply only lens EXIF, omit `--cube` and `--lut-contract`. To apply only a LUT, omit the lens options; existing allowlisted lens/aperture fields are retained.

`--completed` is your assertion that the chosen JPEG is closed and complete. The worker decodes the snapshot to validate it, but does not infer capture completion from a quiet file size. `--assume-srgb` asserts the encoding of an **untagged** image; omit it for a valid ICC-tagged input. Do not use it to relabel an Adobe RGB or log image as sRGB.

## Color and metadata contract

- Input: RGB8 JPEG, up to 40 million pixels and 64 MiB compressed. RAW, HEIF, CMYK, grayscale and Adobe XMP/DCP inputs are outside this worker.
- LUT: strict 17³/33³ 3D-only CUBE, red-fastest ordering, finite values, optional domain bounds; bounded by the existing [reference parser](../research/lut_reference.py). The converter's self-contained CLF subset is also supported, preserving transfer tables, matrices, unit clamps and a 32³ tetrahedral table. Both formats require an explicit encoded-sRGB → encoded-sRGB contract. CLF files are bounded to 8 MiB; external references and general CLF programs are rejected. No hidden RAW development or log conversion occurs.
- A valid input RGB ICC profile is converted to sRGB with LittleCMS perceptual intent. Orientation is baked into the pixels. OpenColorIO evaluates float strips: trilinear for CUBE, preserved tetrahedral/matrix/transfer/clamp stages for converter CLF. Final SDR values clip to [0,1] and round to RGB8. Output is JPEG quality 95, 4:4:4 by default; quality can be snapshotted with `prepare --quality`.
- Output EXIF retains an allowlist of capture fields including make/model, dates, exposure time, ISO, exposure program/bias, metering and flash/white-balance data. It sets fresh dimensions, orientation 1, sRGB ColorSpace and the worker's Software marker.
- Lens presets write LensMake, LensModel, FocalLength, four-rational LensInfo and MaxApertureValue. APEX is `2*log2(f-number)`. Negative APEX below f/1 is omitted from unsigned EXIF fields; the actual f-number remains representable in FNumber/LensInfo.
- GPS, MakerNotes, serial numbers, original thumbnails/XMP and non-allowlisted metadata are omitted. This avoids stale Sony look identifiers and unsafe offset-sensitive blobs. Originals remain intact. A JPEG re-encode is lossy; it is not a RAW development result or an in-camera Creative Look.
- EXIF editing does not set IBIS focal length, lens correction or camera exposure controls. This version handles fixed-focal-length presets; zoom metadata needs a per-shot focal-length selection.

`prepare` reserves a new private job directory and writes `job.json` last. File contents, LUT and resolved preset are pinned by SHA-256; `run` rechecks the snapshots and schema. These are integrity checks, not authentication against someone who can rewrite the entire job. Job names and output names are never reused automatically.

The output is encoded to a private temporary file, fsynced and decoded before publication. A same-directory hard link publishes the complete file with exclusive creation, preventing overwrite races. Errors before publication clean up the temporary file. A forced process kill can leave an unpublished `.jpeg-worker-*` file; it is not treated as a finished JPEG. The same frozen job can be run to a new destination. If publication succeeds but directory synchronization fails, a valid destination may remain: inspect it rather than overwriting it. The host output filesystem must support hard links (the tested workspace uses APFS).

## The supplied Fujifilm pack

The user supplied a local `Camera-Profile-for-Fujifilm-Film-Simulation-main` folder. Inspect it without modifying or importing it:

```sh
.venv-research/bin/python -m photo_worker inspect-profiles \
  /path/to/Camera-Profile-for-Fujifilm-Film-Simulation-main
```

[The retained inventory](../research/evidence/fujifilm-profile-pack.json) hashes all **1,500 files**: 1,464 DCPs, 22 XMPs, 12 demo JPEGs and two READMEs. The 11 film profiles contain embedded `RGBTable` data and declare `SupportsSceneReferred=True`, `SupportsOutputReferred=False`; the 11 wrappers select those profiles. The pack's README identifies them as RAW/DNG workflows.

The included A7C II DCP identifies `Sony ILCE-7CM2`, profile `Adobe Standard Linear`, identity tone curve `[0,0,1,1]`, and HueSatMap dimensions `[90,30,1]`. Its retained matrix/calibration pipeline is not equivalent to applying an extracted table to a Sony-rendered JPEG. The DCP summary was independently checked with the vendored ExifTool.

**The worker does not load XMP/DCP directly.** The new [converter and validation](../research/FUJIFILM_SRGB_CONVERSION.md) adapt each embedded RGBTable stage to a separately defined sRGB JPEG pipeline. The local Adobe DNG SDK verifies P3 primaries, sRGB transfer, clipping and tetrahedral interpolation. Use the resulting CLF files; a flattened 33-point CUBE failed the accuracy gate near clipping boundaries.

```sh
.venv-research/bin/python -m photo_worker convert-fujifilm \
  '/path/to/Camera-Profile-for-Fujifilm-Film-Simulation-main/Fujifilm Simulation LUT' \
  /path/to/new-lut-directory
```

Then use `prepare --lut /path/to/new-lut-directory/classic-chrome-srgb.clf` with the same color contract and job workflow shown above. `--lut` and `--cube` are aliases. Conversion refuses to overwrite its output directory. The separate [SDK reference checker](../research/check_fujifilm_conversion.py) verifies numerical accuracy and JPEG integration; a conversion manifest alone does not prove validation.

These are JPEG adaptations of the table stage, **not the full Adobe RAW/DCP workflow or a measured Fujifilm camera match**. Existing Sony JPEG processing remains baked into the input. Original profile/demo bytes and generated LUT arrays stay local; the repository retains the reproducible converter, provenance and numerical evidence. The 11 validated local CLFs are under `research/build/fujifilm-srgb/`.

## Offline verification and proof images

```sh
.venv-research/bin/python -m unittest discover -s photo_worker/tests -v
.venv-research/bin/python -m research.check_jpeg_worker --full-resolution \
  --profiles /path/to/Camera-Profile-for-Fujifilm-Film-Simulation-main
```

[jpeg-worker-checks.json](../research/evidence/jpeg-worker-checks.json) records **26 passing tests**, including carrier decoding and CLF conversion/loading checks, 1,024 fast-backend RGB8 samples checked against the independent scalar reference, and a full 7008×4672 synthetic image processed with a deliberately obvious channel permutation. ExifTool independently recovered the expected manual lens, actual aperture, focal length, exposure, ISO and timestamp. The original hash was unchanged. The additional [Fujifilm checks](../research/evidence/fujifilm-srgb-checks.json) cover all 11 converted CLFs against the original SDK scalar function, 11 synthetic JPEG jobs and a full-resolution Classic Chrome job.

Fault checks cover stale preset/input/LUT changes after preparation, asset/manifest tampering, schema/path rejection, missing completion/color assertions, malformed lens values, unknown aperture, sub-f/1 metadata, ICC conversion/errors, size bounds, existing names/symlinks, low disk, partial writes and an output-publication race. Tests use synthetic images and temporary host directories only.

The full-size host worker's time and peak resident memory are recorded in [the evidence](../research/evidence/jpeg-worker-checks.json). Development runs on this Mac took roughly **14–21 seconds**, with **436–444 MiB** peak resident memory. These are synthetic host runs, not camera performance or a representative throughput benchmark. Image iteration uses strips, but Pillow retains full decoded/transformed/output images; it is **not a streaming low-memory codec implementation**. Fixture paths/hashes are under ignored `research/build/jpeg-worker/`.

Before a camera version: implement a compatible isolated ARM64 build/runtime, true streaming JPEG codec, measured resource limits, explicit launch/stop/status handling, shutter/job association, native quick-switch controls and media database/playback integration. [The prototype plan](../research/STILL_PROTOTYPE_PLAN.md) keeps these gates separate from native MLUT capture research. No camera installation or execution occurred in this task.
