# Computer-only still-image LUT reference

[lut_reference.py](lut_reference.py) implements the mathematical reference and a host JPEG/PNG command. It has no camera transport imports or deployment path. No camera upload, execution or installation was used.

This completes the first host prototype proposed in [STILL_LUT_FEASIBILITY.md](STILL_LUT_FEASIBILITY.md). It does not yet supply an ARM64 camera worker, camera JPEG codecs, automatic capture detection or native still-pipeline integration.

The later [completed-JPEG worker](../photo_worker/README.md) adds job snapshots, a faster OCIO backend, selected capture-EXIF preservation and manual-lens preset tags, including a full-resolution host proof. This smaller scalar reference remains the independent numerical oracle. Neither implementation installs or executes on the camera.

## Color contract

The LUT must expect **encoded sRGB input and produce encoded sRGB output**. It operates directly on those encoded RGB channel values; it does not silently linearize, apply a display transform or convert to S-Log3. The CUBE file does not reliably identify the intended color spaces, so the operator must know that the selected LUT satisfies this contract.

For image files, a valid embedded RGB ICC profile is converted to sRGB using Pillow/LittleCMS with perceptual rendering intent before applying the LUT. An untagged input requires the explicit `--assume-srgb` assertion. Invalid/unsupported profiles fail. A fresh sRGB ICC profile is attached to the output. This input-profile conversion is separate from the LUT's required sRGB-to-sRGB meaning.

The floating-point core retains LUT values outside [0,1]. The RGB8 image exporter clips only the final result to [0,1] and rounds `floor(value * 255 + 0.5)`. This is an SDR output policy, not an HDR or wide-gamut preservation claim.

## Supported CUBE subset

The standard-library core supports a deliberately bounded subset:

- UTF-8 text, optional BOM when read from a file; blank lines and `#` comments.
- Optional `TITLE`, `DOMAIN_MIN`, `DOMAIN_MAX`; one required `LUT_3D_SIZE` of **17 or 33**.
- Exactly `size³` finite RGB triples, with **red changing fastest**, then green, then blue.
- Headers before data; no duplicate headers; defaults of domain [0,1] per channel.
- Strictly increasing finite domain bounds, input clamping to those bounds, and trilinear interpolation between the eight surrounding grid entries.
- An 8 MiB text limit. Unknown directives, 1D/shaper/combined LUTs, unsupported grid sizes, non-finite values and incorrect row counts fail.

The ordering, domain convention and possibility of out-of-range LUT outputs agree with the reference reader in [OpenColorIO 2.6.0's Iridas CUBE implementation](https://github.com/AcademySoftwareFoundation/OpenColorIO/blob/v2.6.0/src/OpenColorIO/fileformats/FileFormatIridasCube.cpp). Our accepted syntax is narrower; successful processing here does not establish that Sony's importer accepts the same file.

## Run it locally

From `/Users/hongjunwu/Git/A7C2`:

```sh
.venv-research/bin/python research/check_lut_reference.py
.venv-research/bin/python research/lut_reference.py look-srgb.cube photo.jpg graded-copy.png --assume-srgb
```

The optional image and independent-check dependencies are pinned in [requirements-lut-reference.txt](requirements-lut-reference.txt). To reproduce them in a local environment, run `.venv-research/bin/python -m pip install -r research/requirements-lut-reference.txt`.

Use an already-tagged input without `--assume-srgb`; its RGB ICC profile is converted before grading. The flag matters only when no profile is embedded. Input must be RGB8/RGBA8 JPEG or PNG, at most 40 million pixels. Output must have a new PNG/JPEG filename. The command refuses existing outputs and input/output aliases. RGBA input requires PNG output and retains its alpha channel unchanged.

The original file is never rewritten. Orientation is baked into the output pixels. The new file deliberately omits original EXIF, XMP, thumbnails and capture metadata rather than copying stale orientation/preview information. A future camera worker needs a proper metadata-preservation design. JPEG export uses quality 95 and 4:4:4 and introduces another lossy encoding; PNG is the clean reference for numerical checks.

The evaluator itself requires only the Python standard library. Image processing uses **Pillow 12.1.0**, installed in `.venv-research`. The independent cross-check uses **OpenColorIO 2.6.0**, also installed only in that environment. No global dependencies or system configuration were changed. An attempted older OpenColorIO source installation was stopped before building; the successful installation used its macOS ARM64 Python 3.14 wheel.

## Verification performed

[check_lut_reference.py](check_lut_reference.py) passed **eight test groups**, with the detailed source identities, versions, counts, error bounds and generated-file hashes in [lut-reference-checks.json](evidence/lut-reference-checks.json).

| Check | Coverage | Largest observed absolute channel error |
|---|---:|---:|
| Identity and channel permutation, both grid sizes | 4,040 RGB cases | `1.12e-16` or less |
| Independent scalar eight-corner oracle, nonlinear tables, ordinary and nondefault domains | 4,016 RGB cases | `4.45e-16` or less |
| OpenColorIO `INTERP_LINEAR` file processors, both sizes and both domains, including input boundaries and out-of-range values | **8,016 RGB cases** | `2.34e-7` or less |
| Malformed/unsupported CUBE inputs | 22 rejection cases | All rejected |
| File/image behavior | Identity PNG, channel-permuted PNG, JPEG output, embedded ICC conversion, invalid ICC rejection, untagged-input assertion, alpha, orientation, overwrite guards, unchanged original hash | Passed |

The scalar oracle uses a separately written sum of eight corner weights, whereas the implementation uses nested linear interpolation. Analytic identity/permutation tests additionally establish expected values without calling that oracle. OpenColorIO is an independent external implementation, with its own float precision. Its explicit linear interpolation was selected; a tetrahedral result would be a different algorithm.

The deterministic synthetic fixtures and proof images are confined to `research/build/lut-reference/`, including `input-srgb.png`, `identity.png`, `permuted.png` and `graded.jpg`. No personal photograph was needed.

## Practical limits and next steps

This Python implementation is a **numerical reference**, not a proposed camera performance design. It visits image rows in strips, but Pillow still allocates full decoded and output images, and ICC/orientation processing may allocate additional full frames. Strip iteration does not establish bounded low-memory decoding or encoding. Large images will also incur substantial per-pixel Python overhead. Camera CPU time, peak memory and codec integration remain unmeasured.

Its purpose is to provide expected outputs and validation fixtures for a later native implementation. Before any camera execution, that implementation still needs an isolated ARM64 worker, a known ABI/build target, streaming codec and memory strategy, completed-file selection, metadata handling, output placement and interruption/failure behavior. Native capture-stage LUT routing is a separate reverse-engineering question; this host processor does not establish or modify it.
