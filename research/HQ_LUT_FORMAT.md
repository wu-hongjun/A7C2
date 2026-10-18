# HQ native LUT converter: offline execution and byte layout

The stock A7C II 2.01 HQ converter has now completed in a host ARM64 emulator. It emits **48,000 SRAM bytes plus 128 serialized bytes**, for 48,128 bytes total. The AP→CP request examined separately asks for only the first **124** bytes of that second region. This resolves the size discrepancy without inventing a four-byte file header: the last four bytes are explicitly written **32-byte alignment padding** at native-blob offsets 48,124–48,127, as the later section trace proves.

No camera connection was opened and no camera was addressed by this work. No firmware, deployment asset or camera file was changed. These are source-bound static and Unicorn observations, **not REA Evidence records**. They do not demonstrate still-photo routing or a camera-ready converter.

## Reproduction and source binding

Use the existing research environment, with Python 3.13+ (`math.fma`), pyelftools, Capstone and Unicorn:

```sh
.venv-research/bin/python research/trace_hq_lut.py --grid 17 --pattern identity
.venv-research/bin/python research/trace_hq_lut.py --grid 17 --pattern permutation
.venv-research/bin/python research/trace_hq_lut.py --grid 33 --pattern identity
.venv-research/bin/python research/trace_hq_lut.py --summarize
```

The first three commands retain separate compact case records in `research/evidence/hq-lut-cases/`. Synthetic native outputs and captured Float32 quantizer inputs are written only under ignored `research/build/hq-lut/`; they contain generated mathematical data. The final command independently verifies those rebuilt outputs and writes [hq-lut-format.json](evidence/hq-lut-format.json). The aggregate retains output hashes and validation results, so reading the published evidence does not require the ignored synthetic binaries. Regeneration does require the locally retained official firmware, as do the other firmware mappers.

The exact source is:

```text
research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/
  dev/nflasha15_unpacked/lib/appFw.so
SHA-256 2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b
```

All VAs below refer to this ELF at link base zero. The retained instruction ranges are in its first PT_LOAD segment, where file offsets equal VAs. The aggregate includes every instruction byte in four bounded relevant ranges and the exact quantizer constants.

## What actually executed

The original constructor at `0x36f662c` initializes the processor object. The original entry `0x36f6e50` dispatches to orchestrator `0x36f6900`. The synthetic arguments reproduce the AP HQ producer recovered in [native LUT format](NATIVE_LUT_FORMAT.md): byte 1 at config+0, `IPSX` at +1, grid size at +0x23, and a Float4 pointer at +0x28. The Float4 values are RGB plus unused zero, ordered with B fastest, then G, then R. Two output wrappers point to the contiguous SRAM base and base+48,000.

The actual path enters configuration `0x36f7c34`, numerical-stage traversal `0x36f0b8c`, postprocessing `0x36f3d30`, section construction `0x36f9d00`, then serializer traversal `0x36fd0d8`. Section dispatch `0x36fcf94` and byte emission `0x36fc5a4` each run 25 times: 24 SRAM banks plus the final register region. The converter fills the two wrapper counts with 48,000 and 128. All 48,128 destination bytes receive native emulated store instructions; a following 32-byte `0xa5` guard remains intact.

The supplied `IPSX` spelling is significant: original comparison code accepts it. An original diagnostic stream prints `LOSGATOS or IPSX` on this accepted path; it is not by itself evidence of an error. The alternative LosGatos configuration and other format enums were not exercised.

## SRAM order and numerical representation

The emitted SRAM uses the same channel planes and parity banks established independently from CP named THRU tables in [MLUT layout](MLUT_LAYOUT.md). For output channel `c=0,1,2` and coordinates `r,g,b=0..19`:

```text
bank = 4*(r%2) + 2*(g%2) + (b%2)
local = (r//2)*100 + (g//2)*10 + b//2
byte_offset = c*16000 + bank*2000 + local*2
```

Each sample is a little-endian 16-bit integer. The HQ identity case fills all 20×20×20 coordinates per channel. This differs from the ordinary importer's 17 active nodes plus zero padding. Its twenty output levels are:

```text
761,1095,1430,1765,2099,2434,2769,3103,3438,3772,
4107,4442,4776,5111,5446,5780,6115,6450,6784,7119
```

Every one of the 24,000 identity halfwords is checked against the independently addressed coordinate/axis relationship. The non-symmetric transform `(R,G,B)→(B,R,G)` produces exactly the expected full native blob: channel planes rotate B,R,G, while the 128-byte second region stays identical. This catches channel/axis/bank/endian errors across all output bytes. The 17- and 33-point linear identity cases produce identical payloads; that **does not establish** how nonlinear 33-point input is resampled or whether particular odd source nodes contribute.

The final sample quantizer is concrete. Original instructions `0x3702650–0x3702664` load a Float32 scalar, add Float32 `4.999999987376214e-7`, promote to Float64, perform fused multiply-add `value*8192 + 0.5`, narrow to Float32, then convert toward zero to a signed integer. The constant bytes are `bd370635` at `0x51a06d4` and Float64 8192 bytes `000000000000c040` at `0x4bfa180`. Formatting with width four and the serializer produce the observed low-order 16-bit little-endian sample bytes.

An independently written Python quantizer compares all 24,000 captured pre-quantization values per completed case with their actual emitted samples. This verifies this final numerical/serialization stage; it does not substitute for recovering the upstream resampling or color-domain transform. Tested values are finite and bounded. Negative, overflow, NaN, HDR clipping, larger output integers and noncanonical input behavior remain unestablished. Do not generalize this signed conversion into the ordinary importer's unsigned/clamping contract.

## The 128-byte second region

The first 124 bytes are 31 little-endian words, matching the CP register payload shape:

```text
word0        1
words1–4     0
word5        0x00010101
words6–10    0
words11–30   760,1095,1430,1764,2099,2433,2768,3103,3437,3772,
             4107,4441,4776,5111,5445,5780,6114,6449,6784,7118
word31       0
```

The independent CP builders identify word0 as `all_on`, word4 as pre/post-black bits and word5 as R/G/B on/off bits at positions 0/8/16. Thus this payload requests all-on and all three channels, with those two black-control bits zero; physical enable polarity has not been measured. The twenty words11–30 occupy the node/threshold area. They are not simply the already rounded SRAM output levels. Exact semantic interpretation and threshold-generation arithmetic still need a recovered evaluator or hardware documentation.

The extra word31 lies beyond the downstream 31-word register transfer and is now established as **four zero alignment bytes** from the native serializer. A separate section trace below observes the original emitter receive 124 content bytes, align that count to 128, explicitly store four zero bytes, and return 128. It is not an additional content field or a leading file header. The AP resource-base/CP transfer-base relationship remains a separate question; these are native-blob-relative offsets, not guessed physical addresses.

The identity output hash is `1b68ec176928ba1dee19210c241427f91f4d641a48f0c582589b9b93db9343fb`; the permutation hash is `6c98f9ec4908bbed1577c9460459036d13cbe7060a03019c1e9f59ce3cb5e5f4`. The 124-byte register prefix hash is `1784239b0a50137e26c6d205ec4c085b055517376beccd2502433218358d697a` for these cases. The machine-readable evidence retains all case hashes and the ordinary/HQ identity byte comparison.

## Emulator assumptions and remaining gaps

The loader maps both PT_LOAD segments and applies 963,299 relative relocations. Imported-function pointer relocations are bound to explicitly hooked PLT entries, including the indirect `vsnprintf` call used by original integer-to-string code. The case records enumerate the actual imported symbols and call counts. No algorithm function inside the HQ converter is replaced.

External stubs cover allocation/deallocation (a bounded 64 MiB arena with no reuse), memory/string primitives, the observed libstdc++ C++11 string ABI operations, bounded integer snprintf/vsnprintf, canonical strtol, errno, character case conversion, C++ initialization guards and diagnostic ostream calls. `pow` and `log10` use host Python `math`; target libm bit identity is not claimed. String and printf stubs cover the actually encountered canonical operations rather than implementing the entire standard libraries. Unknown calls abort. There is no syscall forwarding or native target process execution.

Each invocation allows at most four continuation chunks, each limited to 60 seconds or 150 million instructions. Each successful run remains within its mapped heap and output guard. The elapsed time and millions of emulated library calls are artifacts of this reference investigation, not camera-performance estimates.

The stock HQ importer is no longer an opaque intercepted call. We have complete output bytes, counts, write coverage, storage order and a checked final quantizer for three synthetic cases. The important unresolved work is the complete preceding interpolation/transfer-function contract, nonlinear crosschecks, valid input/output range behavior, runtime HQ selection defaults, and a proven still-capture route. A CUBE text file is still distinct from this native binary payload, and none of this establishes that arbitrary LUTs can safely be loaded onto the connected camera.

## Follow-up: fixed coordinate anchors explain the identity levels

A separate source-bound check now explains the narrower, nearly uniform coordinate sequence for the examined identity cases. It does **not** establish an adaptive grid. Run `.venv-research/bin/python research/trace_hq_coordinates.py` after rebuilding the 17-point identity capture above; [hq-coordinates.json](evidence/hq-coordinates.json) retains the source bytes, constants, twenty actual captured floats, published threshold words and independent calculations.

For the examined format/flag branch, helper `0x36f5344–0x36f5410` selects fixed Float64 endpoints: high `0.8689981698283972` at `0x51a04a0` (bytes `594cf63fd5ceeb3f`) and low `0.09286412512218964` at `0x51a04a8` (bytes `c6175f7cf1c5b73f`). Helper `0x36f5414` constructs an offset and reciprocal range; caller `0x36f5d50–0x36f5d94` reverses those terms for coordinate construction. The loop at `0x36f5ecc–0x36f5f2c` constructs Float32 `i/(N−1)`, then `0x36f5744–0x36f57dc` applies a Float32 affine fused multiply-add independently to RGB.

For the tested twenty-coordinate lattice, the independent calculation is:

```text
coordinate[i] = f32(fma(f32(i/19), f32(1/(1/(high-low))), f32(low)))
```

The reciprocal-then-reciprocal form preserves the original Float64 setup sequence instead of assuming algebraic simplification. All twenty captured identity axis floats match exactly. Checking the full parity-banked identity capture gives **24,000 exact Float32 matches**, with maximum error zero. Analytic truncation `int(coordinate[i]*8192)` also exactly matches all twenty published register words11–30; the precise register-generator instructions have not yet been identified by this separate check.

These constants are **coordinate anchors for the examined converter configuration**. They have not been established as camera black/white clipping levels, legal RGB bounds, a color standard, or the actual image-domain limits of the hardware LUT stage. Hardware interpolation or extrapolation beyond the anchors remains unknown. In particular, the positive first SRAM value does not establish that loading an identity LUT would lift photographic blacks: interpreting it requires the matching input thresholds and the complete surrounding pipeline. The general interpolation and color-domain contract, nonlinear transforms, alternate flags/formats and actual camera rendering remain unproven.


## Separate REA corroboration

The later [HQ REA recipe](rea-jobs/hq-lut.json) completed ten native Ghidra pseudocode queries, exported its Evidence bundle, and closed successfully. The actual records are retained under [evidence/rea/hq-lut](evidence/rea/hq-lut/). They are distinct from the Capstone/Unicorn results above. Ghidra addresses add `0x100000` to ELF addresses; for example `FUN_037f6900` is the original `0x36f6900` orchestrator.

The completed job explicitly disabled automatic `Decompiler Switch Analysis`, with analysis-profile digest `8ef2c2edbec129968ba4fc374262df4e0dc09e396cd0a868926a4e105ca804ba`. This changes analysis coverage and is not interchangeable with the earlier default-profile application investigation. Preliminary attempts were cancelled before producing successful function results; local diagnostics are excluded from successful Evidence counts.

| Observation | Actual REA Evidence |
|---|---|
| Format selection, lattice copying, conversion stages, serializer call and output wrapper lengths | `ev_0aa1c369bc9807ba3e4e1121118a2c3824d91604af6e9c8560908c7683aa9917` |
| Byte emitter, hex conversion and alignment/zero-fill branch | `ev_4afcf105b8223d8a526e2d1e49cd046320e6a8d66b95247512023cb6283dc887` |
| Float-grid bank formatting and final quantization | `ev_4494cd5e027c7d2daf1c2e1c351b746add3e1bdc02013fde3a1de06b0eadc4dd` |
| Intermediate record dispatch to the type-2 lattice copier | `ev_4fcdff2ccac82926b7d3dd61c04a9d8b81ccb8c63da315024359b3f4e9274630` |

These records corroborate specific control/data relationships; their generic decompiler types are not recovered vendor source or a complete numerical model. The input hashes, raw results, profiles and successful close receipt are included in the [REA index](evidence/rea/index.json).


## Follow-up: the last four bytes are serializer alignment padding

The original byte-emitter control flow seen in REA is now independently verified by bounded emulation. Run `.venv-research/bin/python research/trace_hq_sections.py`; [hq-sections.json](evidence/hq-sections.json) retains the original instruction bytes, all 25 actual emitter calls, 28 isolated original alignment-helper checks and the four zero-store events. It imports the unchanged HQ harness rather than replacing any numerical routine.

The actual **first** emitter invocation is the register section: input length `x2=124`, format flag `x5=0`, destination `native_base+48000`, destination offset `x4=0`, and returned length `x0=128`. The following 24 SRAM calls each receive `x2=2000`, flag `x5=1`, and return 2000. Thus native-blob position is distinct from emission order: the register region is at the end of the native blob but is emitted before the SRAM banks on this tested path.

The original helper `0x36fe634–0x36fe650` rounds the tested nonnegative lengths to a multiple of 32. All 28 isolated cases match independent `(length+31)&~31`, including `124→128`, `2000→2016` and `48000→48000`. This helper's potential alignment of 2000 does not enlarge the SRAM sections: original emitter flag 1 bypasses its final padding step.

For the register call, the original branch `0x36fc934–0x36fc948` takes flag 0, aligns 124 to 128, and computes the four-byte difference. The loop store at **`0x36fc964`** writes zero separately to native-blob offsets **48124, 48125, 48126 and 48127**, then the original function returns 128. The full 48,128-byte output matches the previously published identity hash exactly, and the following 32-byte guard remains intact. This supplies a concrete causal explanation for the four bytes rather than inferring padding from zeros alone.

The earlier `hq-lut-format.json` aggregate intentionally retains its initial statement that the last-word semantics were unknown; its generator and case records remain immutable. The new section evidence and this report supersede that earlier uncertainty. The padding conclusion applies to the examined IPSX identity path. Negative/overflow alignment inputs and other converter formats were not tested, and this observation makes no claim about the image pipeline or hardware interpolation.
