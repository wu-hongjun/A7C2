# Supplied Fujifilm profiles: validated sRGB JPEG adaptations

The host JPEG worker now accepts **11 self-contained CLF LUT pipelines** converted
from the supplied profiles: Astia, Bleach Bypass, Classic Chrome, Classic Neg,
Eterna, Nostalgic Neg, Pro Neg Hi, Pro Neg Std, Provia, Reala Ace and Velvia.
The converter, numerical reference checks and worker integration run entirely
on the computer. No camera enumeration, card access, installation or execution
is involved.

These are **rendered-sRGB adaptations of the embedded RGBTable stage**. They
exclude Adobe Standard Linear DCP calibration, RAW exposure/white balance,
demosaicing, HueSatMap, the rest of Adobe's development pipeline and any inverse
of Sony's existing JPEG processing. An sRGB input that already has FL1/FL2/FL3
applied receives another color transform. This work does not establish a match
to a Fujifilm camera, Lightroom RAW output or native Sony Creative Looks.

## What the source actually contains

The [pack inventory](evidence/fujifilm-profile-pack.json) covers 1,500 local
files, including 11 actual look XMPs and 11 wrappers. The look files declare
`SupportsSceneReferred=True`, `SupportsOutputReferred=False` and
`CameraProfile=Adobe Standard Linear`. The originals are not treated as
output-referred profiles; the converter creates a separately defined JPEG
adaptation.

The [byte decoder](../photo_worker/adobe_table.py) verifies all 11 XMP table
fingerprints. Each table's little-endian base85 carrier starts with the expected
uncompressed length, followed by a bounded zlib stream. The decompressed
196,652-byte payload has this layout:

| Offset | Content | Verified interpretation |
| --- | --- | --- |
| 0 | Four little-endian uint32s: `1,1,3,32` | RGB table type, version 1, three dimensions, 32 divisions |
| 16 | 98,304 little-endian uint16s | 32³ RGB triplets, **blue fastest**, delta-predicted |
| 196624 | Three uint32s: `3,1,0` | **Display P3 primaries, sRGB transfer, gamut clipping** |
| 196636 | Two float64s: `1,1` | Minimum and maximum amount are both 1 |

The format fingerprint is MD5 of the entire decompressed payload. It is a
format integrity check, not authentication of the source. SHA-256 records the
source XMP and payload identities in [decoded-table evidence](evidence/fujifilm-decoded-tables.json).

The predictor for coordinate `i` is `(i*65535 + 16)//31`. Add it to the stored
channel word and wrap modulo 65,536. Do not interpret the words directly as
absolute RGB samples, transpose red/blue ordering, or assume an arbitrary
32-point table is directly encoded sRGB.

## Adobe reference and color contract

The user supplied `dng_sdk_1_7_1_2768_20261013.zip`. Its measured SHA-256 is
`c1d3a545ed45b150e59af2e6aa0695c1551f0e2bed34ad87436898a05194d2b1`.
This pins the supplied SDK snapshot; it is not an independent authentication
against Adobe's published distribution. No online references or downloads were
needed after the local archive became available.

The relevant SDK sources are:

- `dng_big_table.h`: serialized `dng_rgb_table` primary/gamma/gamut enums.
- `dng_big_table.cpp`: `dng_rgb_table::GetStream` delta predictor and
  `dng_rgb_to_rgb_table_data` color-matrix/gamma setup.
- `dng_reference.cpp`: `RefRGBtoRGBTable3D`, including tetrahedral sampling and
  clipping before gamma, after table output and after the ProPhoto return.
- `dng_color_space.cpp`: sRGB/Display P3/ProPhoto PCS matrices, row normalization
  in `SetMatrixToPCS`, and the sRGB transfer pair.
- `dng_1d_table.cpp/.h`: 4096-interval float transfer tables and linear sampling.
- `dng_xy_coord.cpp/.h`: PCS D50 white at xy `(0.3457,0.3585)`.

The serialized-table enum differs from an unrelated RGBTables export enum in
the same source. The serialized footer's **3 means P3**, not ProPhoto. The
matrices are Adobe's rounded D50 PCS matrices, normalized to the SDK white;
substituting generic D65 matrices changes the result.

The explicitly chosen adaptation is:

1. Decode rendered sRGB to linear RGB and map it to linear ProPhoto PCS.
2. Follow SDK setup: ProPhoto → P3 matrix, clip, sRGB encode, native 32³
   tetrahedral table at amount 1, clip, sRGB decode, P3 → ProPhoto, clip.
3. Map linear ProPhoto to linear sRGB, clip for SDR, then sRGB encode.

The native stage's transfers use SDK-style 4097 samples / 4096 intervals. The
fast converter also samples the outer sRGB transfer pair; the independent
oracle uses analytic outer transfer functions and measures this small extra
approximation. The ProPhoto clamp is preserved; matrices are not collapsed
across it. OCIO uses lossless optimization, avoiding approximate fast LUT
replacement. The resulting file has an explicit encoded-sRGB → encoded-sRGB
contract despite its internal P3/ProPhoto stages.

Applicable Adobe notices/license are retained in [NOTICE.md](../NOTICE.md) and
[Adobe-DNG-SDK.txt](../licenses/Adobe-DNG-SDK.txt). The original SDK, source
profile bytes, demo photos, generated LUT arrays and JPEG jobs remain local.
The committed converter reproduces the outputs from the user's assets; evidence
pins their hashes. New CLFs are about 1.6 MB each and are under ignored
`research/build/fujifilm-srgb/` in this workspace.

## Why CLF instead of a flattened 33-point CUBE

The existing worker's trilinear CUBE route is still supported. However, baking
this entire piecewise pipeline into a 33³ CUBE loses accuracy around clipping
surfaces, even when its average error is small. The failed flattening experiment
is retained per look in [the numerical report](evidence/fujifilm-srgb-checks.json).
Those CUBEs are not delivered as validated conversions.

CLF preserves the original 32³ tetrahedral grid, 1D transfer tables, matrices
and clipping operations. The worker's [bounded CLF loader](../photo_worker/clf.py)
accepts only this specific operator layout and numeric array sizes. It constructs
OCIO operators directly from finite, bounded numbers. It rejects external
references, file paths, unknown operators/attributes, DTD/entities, unsupported
dimensions and nonfinite values. It never asks OCIO to resolve a CLF file or
external resource. Existing CUBE jobs retain their schema and behavior.

## Reproduce locally

Run from the repository root, using a **new** conversion output directory:

```sh
.venv-research/bin/python -m photo_worker convert-fujifilm \
  '/Users/hongjunwu/Downloads/Camera-Profile-for-Fujifilm-Film-Simulation-main/Fujifilm Simulation LUT' \
  research/build/fujifilm-srgb

.venv-research/bin/python -m research.check_fujifilm_conversion \
  --sdk /Users/hongjunwu/Downloads/dng_sdk_1_7_1_2768_20261013.zip \
  --profiles '/Users/hongjunwu/Downloads/Camera-Profile-for-Fujifilm-Film-Simulation-main/Fujifilm Simulation LUT' \
  --luts research/build/fujifilm-srgb --full-resolution
```

Conversion refuses to overwrite its output directory. `manifest.json` records
all source/output fingerprints. It is a conversion manifest, **not a validation
receipt**. A success receipt is produced only after the separate checker passes.

To grade an explicitly selected completed host JPEG, use the worker normally:

```sh
.venv-research/bin/python -m photo_worker prepare \
  /path/to/completed-photo.jpg /path/to/new-private-job \
  --lut research/build/fujifilm-srgb/classic-chrome-srgb.clf \
  --lut-contract encoded-srgb-to-encoded-srgb \
  --lens-presets photo_worker/lens-presets.example.json \
  --lens example-helios-58 --aperture 4 --completed --assume-srgb
.venv-research/bin/python -m photo_worker run \
  /path/to/new-private-job /path/to/new-graded-photo.jpg
```

Only use `--assume-srgb` to assert an untagged input's actual encoding. Tagged
inputs are converted through ICC as documented in the [worker README](../photo_worker/README.md).
`--lut` is an alias for the existing `--cube` argument; suffix selects the
strict CUBE or limited CLF parser. Job snapshots and no-overwrite publication
apply to both formats.

## Validation and limits

The completed run passed **42,084 vectors per look** (462,924 across 11 looks),
plus three synthetic reference tables. The worst CLF float difference from the
native oracle was **0.004724 of one RGB8 level**; serialization differences stayed
under **0.000231 level** on these vectors. Flattened 33³ CUBEs reached **33.20 levels** of
error. All 11 small JPEG proofs and the full-resolution Classic Chrome proof
were within **one level** at the sampled flat-patch centers, including JPEG
recompression. The full-size CLF run took about **14 seconds** and **444 MiB**
peak RSS on this Mac; later run measurements remain in the evidence. The
repository's 41 regression tests, 26 worker/conversion tests and eight LUT
reference groups pass (75 total).

The checker extracts the original `RefRGBtoRGBTable3D` function **verbatim** from
the pinned archive and compiles it with local clang++, `-O2 -ffp-contract=off`.
The extracted function SHA-256 is
`3ec8fe2c9cfc76cc4d7b28270b21d670086aae3f0aa89a8ef3900f829eaa13e4`.
The [host harness](adobe_rgb_table_oracle.cpp) independently decodes delta words
and uses Gauss-Jordan matrix inversion; the Python converter uses cofactors.
It implements only the required SDK types/transfer-table wrappers and the
explicit outer sRGB adaptation. This is an original SDK scalar-function oracle,
not a full SDK build or Lightroom renderer.

The numerical suite checks every look against seeded random holdouts, an RGB
grid, a dense gray ramp, dense cube faces/near-faces and transfer-toe boundaries.
Identity, channel permutation and nonlinear synthetic tables independently
exercise ordering, delta wrap and interpolation. The acceptance limit is
**0.01 of one RGB8 code value** for float output versus the native oracle, with
a separate 0.001-level serialization limit. These are sampled numerical checks,
not an exhaustive test of all 16,777,216 RGB8 inputs or a perceptual color-match
certification. Float differences can straddle a rounding boundary by one code.

Every CLF also processes a synthetic JPEG with manual-lens presets and independent
ExifTool readback. Classic Chrome additionally processes a full 7008×4672 JPEG.
The JPEG gate allows at most three RGB8 levels at 64 flat-patch centers, including
lossy recompression; originals must retain their hashes. Timings/RSS are host
measurements only. Current exact results, source identities and output hashes
are in [fujifilm-srgb-checks.json](evidence/fujifilm-srgb-checks.json).

The supplied Adobe-rendered demo photographs are useful visual references but
are not an exact input/output oracle: `Original.jpg` uses Camera ST, whereas the
film examples use Adobe Standard Linear. Their other development stages and
JPEG encoding also differ from this explicit adaptation. No profile equivalence
is inferred from a visually pleasing result.

A [separate static ARM64 worker](../camera_worker/README.md) now implements the
native LUT evaluator, streaming JPEG codec and frozen lens EXIF, with offline
host/sanitizer/emulator checks. Camera execution, target filesystem and resource
validation, a capture completion hook, job/preset selection and media
database/playback integration remain separate gates. This result
does not resolve the native MLUT allocation-to-encoder link, enable Creative
Looks in AUTO, add a native profile-import menu or install any program.
