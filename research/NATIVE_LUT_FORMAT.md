# Native `.LUT` producer and the ordinary 48,000-byte format

**Later follow-up:** [HQ_LUT_FORMAT.md](HQ_LUT_FORMAT.md) adds original-instruction evidence beyond the bounded pass below. Earlier unresolved statements here describe that earlier evidence scope.

The ordinary application-side LUT conversion path is now recovered at the byte-layout level. Original ARM64 instructions produce exactly the same 48,000 bytes as a separately written Python packer, for both supported input grid sizes. This is an offline result; it does not establish a usable still-photo route or camera-side performance.

[trace_native_lut.py](trace_native_lut.py) reproduces the trace and checks. [native-lut-format.json](evidence/native-lut-format.json) retains exact instruction bytes, source hashes, direct callers, constants and test results. The source is official A7C II 2.01 `appFw.so`, SHA-256 `2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b`. Addresses below are ELF-relative virtual addresses, equal to file offsets for the examined ranges.

These are ordinary Capstone/ELF observations and isolated Unicorn emulation results, **not REA Evidence records**. The emulator never forwards a system call, opens a camera, executes a native target process or writes firmware. All external calls and their limitations are explicit in the evidence.

## 1. Actual import, conversion and persistence chain

The relevant functions are:

| Address | Observed responsibility |
|---|---|
| `0x30201d0` | Extract bounded lines from an in-memory input text descriptor |
| `0x3020308` | Scan CUBE size and `LUT_3D_INPUT_RANGE` |
| `0x30208d4` | Parse RGB triples, choose ordinary or alternate conversion |
| `0x302002c` | Pack the ordinary parity-banked 48,000-byte payload |
| `0x3020e40` | Write the native result and original CUBE text as a pair |
| `0x3020430` | Format `/usr/data/lut/LUT%03d.LUT`, using index + 1 |
| `0x302040c` | Format `/usr/data/lut/LUT%03d.cube`, using index + 1 |
| `0x367f414` | Open, write raw bytes, fsync and close |

Direct calls at `0x3088968` and `0x30896a4` enter the converter; nearby `0x30889a4` and `0x30896e0` call the paired writer. Another caller at `0x30ac41c` explicitly supplies conversion flag 0. The two larger call paths derive a boolean flag from **`import_lut_hq_conversion == 1`**: ordinary conversion is HQ conversion **OFF**, and the alternate vendor path is HQ conversion **ON**. The selected runtime value, product defaults and complete user-interface gating on the connected camera remain unchecked. We do not assume that every imported user LUT uses the ordinary branch.

This identification has three separate source anchors. Both callers load their local value through getter `0x2887b70` (`0x30888a0` and `0x30894ec`). Setter context `0x2b9c168` uses this getter together with model parameter ID **`0x1a5d`**. The parameter-name record at `0x504606c` stores that ID and a relative string offset resolving to **`import_lut_hq_conversion`** in the name pool at `0x5063468`; its neighbor is `is_imported_lut_hq15`. Enum metadata at `0x5867650` and `0x5867660` records **0 = `IMPORT_LUT_HQ_CONVERSION_OFF`**, **1 = `IMPORT_LUT_HQ_CONVERSION_ON`**. Exact metadata bytes and bounded caller instructions are preserved in the JSON.

The paired writer supplies the converted descriptor's **exact length and pointer** directly to the raw file writer. It separately writes the original input text's length/pointer to the `.cube` path. There is no demonstrated extra header prepended by this writer. In-memory emulation of the original writer verified both exact filenames and both byte hashes. All file calls were intercepted; neither the host filesystem nor a camera received those simulated writes.

The raw writer uses Linux `open` flags `0x41` (`O_WRONLY | O_CREAT`) and a single `write`, then calls `fsync` and `close`. It does not request `O_TRUNC` or retry a partial write in this helper. **The two examined import callers remove both old files before writing:** `0x308897c` / `0x30896b4` lead to native-file deletion helper `0x3021338`, then text-file deletion helper `0x302138c`, then the paired writer. Each deletion helper formats its corresponding path and calls libc `remove`. The callers do not inspect those deletion return values, but the normal successful-delete path removes a stale tail when replacing a 48,128-byte file with a 48,000-byte file. Missing `O_TRUNC` in isolation is therefore **not a demonstrated retained-tail bug in this normal replacement flow**. Other callers and failure paths remain a separate question. The writer callers also compare the returned write count with the intended length.

## 2. Ordinary parser: output quantization and retained input nodes

With flag 0, `0x30208d4` accepts grid sizes **17 or 33** and reads the source RGB triples in file order. For each finite channel, the observed instruction sequence is:

1. Float64 fused multiply-add: `value * 8192.0 + 0.5`.
2. Clamp that result to `[0, 65535]`.
3. Convert to unsigned integer with `FCVTZU`, then store its low 16 bits.

The constants are stored at `0x4bfa180` (8192.0) and `0x4bfa188` (65535.0). A scalar value of 1.0 therefore becomes **8192**, not 65535. Tests also confirm negative values clip to 0, 0.5 becomes 4096 and sufficiently large values clip to 65535.

This is the **16-bit output sample representation**. It does not identify the precision of CP hardware input thresholds, nor does it establish the input/output color encoding expected by the look. No implicit sRGB/log conversion is visible in this ordinary parsing/packing sequence. The host image reference's explicit sRGB contract is a separate software design.

`quantize_sample` supplies a finite-input Python reference using `math.fma`, avoiding a second rounding operation in the multiply/add. `pack_reference` deliberately accepts already quantized integer samples; it is not a replacement for all of Sony's import validation.

## 3. Exact ordinary payload layout

There are **24,000 little-endian unsigned 16-bit words**, arranged as three channel planes:

| Byte range | Channel |
|---|---|
| 0–15,999 | Red output |
| 16,000–31,999 | Green output |
| 32,000–47,999 | Blue output |

Each channel has **eight banks of 1,000 words**. For retained grid coordinate `(r,g,b)` with each coordinate in `0..16`:

```text
bank  = 4*(r % 2) + 2*(g % 2) + (b % 2)
local = 100*(r // 2) + 10*(g // 2) + (b // 2)
word  = 8000*channel + 1000*bank + local
byte  = 2*word
```

Channel is 0 for red output, 1 for green and 2 for blue. The eight parity seeds at source `0x4bfa190` are `000, 001, 010, 011, 100, 101, 110, 111`, matching that bank formula.

The source loop traverses each bank's coordinate parity in increments of two through coordinate 19. It writes zero whenever any coordinate exceeds 16. Thus the allocated address space corresponds to 20 positions per axis split by parity, but **only 17³ = 4,913 grid nodes contain source samples**. There are 8,000 allocated slots per channel and **3,087 zero-padding slots per channel**. The 48,000-byte length is not evidence of a dense 20³ look with 20 meaningful source nodes.

For a 17-point input, retained coordinates map directly to the source grid. For a 33-point input, the packer uses source coordinates **`(2r,2g,2b)`**: odd source nodes are discarded. This branch reduces a 33-point source to the same 17-point retained grid by **decimation**, without a resampling interpolation step in this packer.

The Python `pack_reference` uses the direct destination formula above rather than reproducing the original nested parity loops. `unpack_reference` recovers the retained red-fastest RGB17 grid. It deliberately does not invent the discarded odd nodes of a 33-point source. For the input flat RGB array, source index is `3 * ((b*N + g)*N + r) + channel`, so red changes fastest in the CUBE stream.

Independent CP-side investigation of the stock `MLUT_L3D_SRAM_*_THRU` arrays found the same channel-plane, parity-bank and local-address formula across all 24,000 halfwords. That corroborates the layout from the producer and consumer sides. Register meanings and hardware input thresholds are documented separately; the AP output quantizer must not be confused with those thresholds.

## 4. The alternate branch has a different conversion contract

With flag nonzero, the parser does not invoke the ordinary uint16 parity packer. It instead stores Float32 records with four components: parsed R, G, B and a zero fourth component. Its intermediate index is:

```text
float4_index = (r*N + g)*N + b
```

Thus **blue changes fastest in the intermediate**, unlike the red-fastest input CUBE stream. Original-instruction boundary captures for both N=17 and N=33 verified selected non-symmetric coordinates and their actual float4 contents.

The parser initializes a configuration containing `IPSX`, records N at offset `+0x23`, and supplies the float4 pointer at `+0x28`. It creates two output wrappers: one points to a prezeroed output buffer, and the other points to **that buffer + 48,000**. It invokes `0x36f6e50`, which branches to the larger conversion orchestrator at `0x36f6900`. That orchestrator contains `IPSX`/`LosGatos` string selection, multiple conversion stages and a serializer, and stores reported output lengths back into the wrappers. Full alternate conversion semantics are not recovered by this pass.

The surrounding parser returns a **48,128-byte** descriptor on its successful alternate path. This is now established from producer instructions and boundary emulation, rather than merely inferred from a read capacity. **The vendor converter was intercepted at entry and not executed** in these boundary tests, so the captured result is evidence of its input/output contracts, not a representative generated alternate payload.

The application message builder independently requests a 48,000-byte SRAM range at resource offset 0 and a 124-byte register range at offset 48,000 in its flagged path. Those are offsets relative to the **CP-resolved resource base**, which has not been proved identical to the AP mapping's copy base in every route. The producer's explicit second output pointer makes a leading four-byte header unsupported by this evidence. A possible trailing four bytes in the returned 48,128-byte region remain unassigned until the vendor serializer's actual emitted lengths are proved.

## 5. Parser compatibility findings

Actual parser instructions, with explicit libc stubs for canonical text, showed:

- The observed `LUT_3D_INPUT_RANGE` accepts **0 and 1**; a nondefault range is rejected.
- Prefixed `DOMAIN_MIN -1 -1 -1` / `DOMAIN_MAX 2 2 2` lines do **not** change the ordinary result. These tested lines are ignored by this path; they do not produce OpenColorIO-style domain normalization.
- A source with one missing RGB row is rejected.
- A source with an additional complete RGB row is accepted and yields the same payload as the expected-size source. This path stops after the expected grid content rather than enforcing the strict exact-row policy used by our host reference.

These findings are scoped to the examined in-memory parser and synthetic inputs. The `sscanf` stub implements only its three observed formats and canonical whitespace-separated data; it is not a complete emulation of libc parsing, malformed Unicode, locale, overflow or every CUBE dialect. Do not present the host reference parser's broader `DOMAIN_MIN/MAX` behavior as Sony-importer compatibility.

## 6. Validation and remaining boundaries

Run:

```sh
.venv-research/bin/python research/trace_native_lut.py
```

The quantization reference requires Python 3.13 or later for `math.fma`; this run used Python 3.14.6 with the pinned research dependencies.

The checks include:

- **Ten original-packer cases**: 17/33 grids with identity ramps, channel permutation, nonlinear values, random full-range uint16 values and coordinate-coded samples. Every output byte is compared, totaling **480,000 bytes**. The inverse mapping additionally checks every retained sample.
- Original parser cases for identity, domain handling, row counts and clipping, comparing the entire ordinary payload with the independent reference.
- Both paired native/text writer calls with in-memory path/hash stubs and no actual file writes.
- Alternate-branch boundary captures proving float4 order, the 48,000-byte output-pointer separation and 48,128-byte returned descriptor, explicitly without executing its vendor converter.

The emulator maps only selected original instruction/data regions. `memset`, `memcpy`, `strncmp`, the three observed `sscanf` formats, allocation/deallocation, diagnostics and simulated file calls are stubs. Unknown external calls abort rather than being forwarded. Instruction/time limits are enforced.

This establishes one native payload format and its producer, and substantially narrows the alternate format boundary. It still does not establish the alternate serializer's full bit layout, the selected branch for a particular real import, CP resource-base translation, a full-resolution still-image LUT route or photographic correctness. Those require additional offline tracing and later separately authorized device validation.
