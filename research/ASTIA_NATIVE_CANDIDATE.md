# One offline Astia native-look candidate

October 20, 2026. This experiment produced one **1,215-byte native correction
table** from the user's supplied Astia XMP, measured fitting loss, audited
composition with our revised FL2/FL3 patches, and traced a separate HIF gamma
command. The result is a reproducible model candidate, **not an installable or
photographically validated look**. No camera, card, USB, G HUB, network download
or Apple developer tool was used. Git publication is the only network action.

The table is local at
`research/build/astia-native-20261020/slot1-astia.raw-s8.bin`, SHA-256
`a27dc63c70a2ab2bad2a45bf16947ea563cf1ed6229c65af21281feb18b44487`.
It is a slot-1, base-ST experiment. Original profile bytes, fitted coefficients,
synthetic images and full firmware remain outside Git under ignored storage.

## What was fitted

The input is the previously inventoried **TingfengLuo pack's `Astia.xmp`**,
not the reference project's MTriton DCP. The embedded Display-P3/sRGB RGBTable
is decoded with the existing fingerprint checks. Its target is our
[SDK-validated rendered-sRGB adaptation](FUJIFILM_SRGB_CONVERSION.md).

This experiment explicitly **assumes the input to the native predictor is
encoded Sony ST sRGB SDR**. It does not apply a measured inverse-ST RAW bridge,
donor sensor calibration, or full Adobe development. It does not make a HLG/PQ
or wide-gamut output contract. A real native stage may operate at a different
point/domain; that uncertainty remains outside the numerical fit.

The existing host Adobe reference executable supplied targets for 37,295
synthetic vectors. The independent OCIO adaptation agreed within 0.01 of an
8-bit code value for every tested channel. Nothing was recompiled. The fit
uses 14,048 training vectors, 2,048 selection vectors, and 21,199 final test
vectors; test groups are disjoint by role, not guaranteed unique RGB values.
There are no independently captured scene or chart measurements.

The fitter from the pinned reference project solves a 5×9×9 correction grid
under its provisional BT.601 limited-range coordinate/ISP response model.
Three smoothing strengths were compared on selection data; 0.1 was selected.
The serialized table has 405 coefficients in each of three signed planes,
all within −64…63. **138 of 1,215 coefficients (11.4%) are at a bound.**

## Measured approximation loss

All channel errors below use a 0–255 scale. The p95 column is over individual
absolute channel errors, not pixels. ΔE76 is calculated from the synthetic
sRGB outputs in D65 Lab; it is not a measurement of real camera color accuracy.

| Held-out group | Vectors | Mean channel error | p95 channel error | Maximum channel error | Mean ΔE76 |
|---|---:|---:|---:|---:|---:|
| Random RGB | 8,192 | 4.21 | 15.03 | 62.16 | 3.09 |
| Neutral ramp | 4,097 | 3.58 | 8.45 | 9.64 | 1.48 |
| Synthetic skin-like HSV samples | 216 | 3.41 | 9.07 | 12.53 | 2.56 |
| Saturated HSV samples | 2,160 | 4.74 | 15.63 | 65.71 | 3.89 |
| RGB gamut faces | 6,534 | 4.06 | 15.83 | 71.91 | 3.46 |

For comparison, leaving random RGB unchanged has mean error 37.12 against
this target. The fitted table captures much of the requested transform but
still has substantial errors, especially at saturation/gamut boundaries.
It should not be labeled a faithful Astia match.

A diagnostic solve removes coefficient bounds and integer quantization while
keeping the same grid and chosen regularizer. Its random mean error is still
2.92 and p95 is 10.63; 146 coefficients exceed the native range. This is not a
precision-only comparison or a proof of the optimal achievable fit. It shows
that merely increasing coefficient precision would not make this particular
model/fit exact. A separate scalar evaluator agrees with the vectorized native
predictor on 128 held-out inputs to better than 1e-12.

[Fitting evidence](evidence/astia-native-fit.json) retains source/model hashes,
selection trials, all group metrics, clipping fractions and the diagnostic.
The local comparison image `research/build/astia-native-20261020/astia-native-comparison.png`
shows input, SDK target and native-model prediction on synthetic swatches.
It is not a camera preview or a photo comparison.

## Compatibility with our installed FL23 revision

The audit reconstructs our exact 0.1.1 AP/CP candidates from official local
2.01 inputs and the checked deployment patch table. It composes reference
runtime windows and Astia **only in host memory**. No installer, firmware file,
name resource or saved recipe is written.

- Our AP hash matches the upstream FL23 source identity. Our revised CP hash
  `60b9dc9d…` differs from its accepted historical `60a9de1a…`: the stock
  upstream importer would reject this pair. No identity guard was changed.
- All ten proposed CP source windows nevertheless match byte-for-byte.
  Three overlap our modifications: initialized-data length at file `0x20141`
  and gamma-call instructions at `0x323172` / `0x323298`.
- The new gamma wrapper normalizes custom markers to ST, then branches to our
  existing helper at VA `0x240832`. The corrected FL2/FL3 movie fallback body
  is preserved. All 4,279 other CP patch bytes and 22 other AP patch bytes
  remain unchanged. The shared AP converter is deliberately replaced by the
  slot-mask implementation; that is not byte preservation of that converter.
- 336 existing gamma cases and 336 existing color-dispatch cases give identical
  outputs before/after composition. Another 96 custom-marker gamma cases match
  ST. These are isolated instruction tests, not imaging hardware tests.
- 36 actual composed-helper/checksum cases select Astia only in eligible
  synthetic contexts, reject corruption, and preserve the fallback and stack.
  Another 24 cases feed the actual Astia bytes through the original stock
  packer, matching all 1,620 bank bytes against an independent packer.

The resulting descriptor extents leave **96 bytes** between initialized-data
end `0x1209fa0` and BSS start `0x120a000`. The executable extent reaches
`0xc2ffb0`, covering the helper/checker. These bounds do not prove loader
activation or ownership of every storage range. The table prefix at file
`0x8000` is outside the ordinary code/data descriptor ranges; the reference
project relies on bootstrap preload behavior. We did not replay that bootstrap
or prove this prefix's running content/lifetime for our composed build.

[Integration evidence](evidence/astia-native-integration.json) records hashes,
overlaps, descriptor extents and test results. Compatibility is **promising at
the examined byte/control boundaries**, not established for installation,
boot, resource transactions, retained-image reload or long-term operation.

## HIF investigation

New source anchors establish a separate HIF gamma control:

| Boundary | Original source evidence |
|---|---|
| AP command identity | Text `PID_SET_HEIF_GAMMA` at AP `0x4c7c4a7`; command `0x4141` |
| AP request builder | `0x354182c…0x35418bc` constructs a 12-byte request, invokes converter `0x3528564`, and supplies it to the sender |
| AP converter | For tested first input 0/1 and second input 0…29: output is 1 only for first=0 and second≤11; otherwise 0. Numeric meanings are not inferred |
| CP dispatch row | File `0xff3f64`: command `0x4141`, handler `0x356341`, validator `0x3642fd` |
| CP setter | Original entry `0x356340` reads message byte +8 and stores it as a u32 at `0x165a720+0xfc` |
| CP validator | `0x3642fc` compares that field against requested byte +8 under its validation flag |

The converter passed 60 original-instruction cases, and the setter passed all
256 byte values with logging disabled. The AP-to-CP transport and validation
callback were not executed. A separate bounded builder slice copies a source
`+0xfc` value to a destination byte `+0x2cd`; its base-pointer identity is not
established here, so it is **not** presented as the missing image-path link.

The native CC parameter orchestration at CP `0x74cb3a` calls controller
`0x23d380`; its call at `0x23d4f6` reaches the bank builder `0x23dc2c`.
The existing [HEIF capture trace](HEIF_OWNERSHIP.md) follows the separate
`av-cam` stage through encoder input submission, output ownership and still-info
publication. We still have **no source-bound join** from the custom CC bank's
processing context to that HIF encoder's actual full-resolution input pixels.

Our previously copied HIF fixture was SDR sRGB, NCLX primaries 1 / transfer 13,
as recorded by the [HIF worker](../camera_worker/HEIF.md). That historical
sample proves neither the custom native-table path nor HLG behavior. We did
not reread the sample or access its source card in this pass. The new candidate
cannot be assumed correct for HLG/PQ merely because the container is 10-bit.

## Reproduction and decision

[build_astia_native.py](build_astia_native.py) uses existing local interpreters
and the previously built SDK oracle. Use a new directory for preparation;
fitting an existing directory verifies that its table is unchanged before
refreshing generated reports. [check_astia_integration.py](check_astia_integration.py)
only composes in RAM and runs isolated emulation.

```sh
.venv-research/bin/python research/build_astia_native.py prepare
/opt/anaconda3/bin/python research/build_astia_native.py fit
.venv-research/bin/python research/build_astia_native.py preview
.venv-research/bin/python research/check_astia_integration.py
.venv-research/bin/python research/check_native_looks.py
.venv-research/bin/python -m unittest discover -s tests -v
```

The original selector/packing checks still pass (3,520 + 120 cases), and all
41 root host tests pass. A fresh preparation/fit reproduces the exact table;
a tampered training target is rejected before a candidate is written.
[Validation receipt](evidence/astia-native-validation.json) pins the scripts
and reports. No compiler or installation command was run.

**Decision:** retain Astia as an offline research candidate; do not install it
or retire the worker. The next useful improvements are reducing the tonal/color
fit error under a better characterized ST response, proving prefix loading and
the HIF CC-to-encoder connection, and reviewing a compatible adapter without
discarding our gamma fix. AUTO, menu resources and manual-lens EXIF remain
separate work. No claim of a complete Fujifilm simulation follows from this fit.
