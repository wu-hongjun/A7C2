# Native MLUT SRAM banks and register payloads

Target: original A7C II 2.01 `cpapp-b.bin`, 18,090,880 bytes, SHA-256 `f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428`.

This is an offline investigation. No camera connection, camera execution, upload, installation, firmware modification, queue operation, DMA or peripheral access occurred. The one host-emulated routine only builds three register-transfer descriptors in ordinary emulated memory. This report and its JSON are **ordinary static/host-emulation evidence, not REA Evidence records**.

Reproduce with `.venv-research/bin/python research/trace_mlut_layout.py`. It writes [mlut-layout.json](evidence/mlut-layout.json), including exact source ranges, instruction bytes, 402 named descriptor records, their source hashes, the 17 matched 31-word register templates, 24,000 exhaustive identity-data comparisons, 12,000 diagnostic-pattern comparisons and 128 isolated descriptor-builder cases. All retained code is Thumb-2; the offline link-model address is file offset plus `0x108000`, as established in [CP architecture](CP_ARCHITECTURE.md). It is not a measured runtime address.

## What is now established

The native 48,000-byte SRAM payload has a concrete banked scalar layout:

- Three output-channel planes in **R, G, B** order.
- Eight parity banks per channel, **2,000 bytes per bank**.
- Each bank contains **1,000 little-endian 16-bit entries**, two entries per 32-bit transfer word.
- Bank number is `4 × R parity + 2 × G parity + B parity`.
- Within a bank, B varies fastest, then G, then R.

This is not inferred merely from factoring 48,000. The firmware includes explicitly named `MLUT_L3D_SRAM_0_R_THRU` through `MLUT_L3D_SRAM_7_B_THRU` descriptors. Their real sample data exactly fits a three-dimensional identity relationship using the twenty values in `MLUT_L3D_REG_THRU`. Every one of their **24,000 halfwords** is checked.

An independent AP producer investigation finds the corresponding writer: [native LUT format](NATIVE_LUT_FORMAT.md). Its packer corroborates this byte ordering from instructions and host execution. That AP work also distinguishes the 17 active coordinates used by ordinary CUBE imports from the storage layout's twenty-coordinate capacity. The CP identity dataset alone does not establish arbitrary 20-point CUBE support or the numerical encoding expected at an image-processing stage.

## Actual descriptors and data, rather than names alone

The relevant named records contain three 32-bit header words followed by an inline name. In the SRAM records, those words identify a file-backed sample pointer, register-window offset `0x3000`, and 500 transfer words. Register payload records instead identify a pointer, offset `0x400`, and 31 words. Other descriptor kinds use different member interpretations, so this is not a universal C structure assigned to every matching string.

The bounded scan recovers:

| Record type | Count | What was checked |
|---|---:|---|
| SRAM payload records | 312 | Each points to 500 words / 2,000 file-backed bytes; all source locations and hashes retained |
| Register payload records | 17 | Each points to 31 words / 124 file-backed bytes; all words retained |
| All named records in this scan | 402 | Includes bank selectors and other matching descriptor candidates |

Useful concrete records include:

| Record | Record VA | Source data VA | Destination / count |
|---|---|---|---|
| `MLUT_L3D_REG_THRU` | `0x1127828` | `0x1118dcc` | `0x400`, 31 words |
| `MLUT_L3D_SRAM_0_R_THRU` | `0x11283a4` | `0x111b5ec` | `0x3000`, 500 words |
| `MLUT_L3D_SRAM_0_G_THRU` | `0x1128804` | `0x111f46c` | `0x3000`, 500 words |
| `MLUT_L3D_SRAM_0_B_THRU` | `0x1128c64` | `0x11232ec` | `0x3000`, 500 words |
| `MLUT_L3D_REG_USER_LUT_17` | `0x11c1c78` | `0x11406a4` | `0x400`, 31 words |

Those addresses are source locations in the recovered CP map; they are not directions to invoke firmware functions.

## Bank order and packed entry coordinates

For integer storage coordinates `r`, `g`, `b` from 0 through 19 and output channel `c` equal to 0/1/2 for R/G/B:

```text
bank = ((r & 1) << 2) | ((g & 1) << 1) | (b & 1)
local = ((r // 2) * 10 + (g // 2)) * 10 + (b // 2)
byte_offset = c * 16000 + bank * 2000 + local * 2
```

Read a little-endian halfword at that offset. Consecutive pairs of local entries occupy the low then high half of a transferred 32-bit word. For the THRU data, output R equals the R coordinate's threshold value, output G equals G's threshold value, and output B equals B's threshold value.

The twenty threshold words in its register template are exactly `0xccc × n` for `n = 0..19`. Consequently:

- R banks 0–3 begin at zero, while R banks 4–7 begin at `0xccc`; R values change every 100 halfwords.
- G bank bit 1 selects the even/odd threshold sequence; G values change every ten halfwords.
- B bank bit 0 selects its even/odd sequence; B changes every halfword. For B bank 0, the first packed words are `0x19980000`, `0x4cc83330`, `0x7ff86660`, `0xb3289990`, `0xe658ccc0`.

This independent analytic reconstruction catches channel permutations, parity-bank permutations, axis-stride mistakes and halfword-endian/pair-order mistakes. It is a static file-data comparison, not a hardware image-rendering test. Hardware interpolation, clipping and transfer-function behavior remain unmeasured.

### The transfer-control table agrees with the bank names

The existing SRAM builder at `0xb8336c` uses pointer table `0xe953fc`, with 24 control-word sources in R0–R7, G0–G7, B0–B7 order. Its paired descriptors write one control word at MLUT base `+0x408`, then 500 payload words at `+0x3000`.

For 23 file-backed entries, the control word is exactly:

```text
channel_index | (bank_index << 4)
```

The matching named bank-selector record points to an adjacent copy of the same initialized word; the builder's pointer is four bytes after that named record's source pointer. Both values are checked rather than treating the pointers as identical. R/bank 0 uses pointer `0x1a76524`, outside the file-backed image. Its runtime word is not read or asserted as a measured zero.

The separately traced selector wrapper at `0xb7994a` checks IP index at most 6, channel selector at most 3, and bank below 8, before helper `0xb7cf0c` constructs `channel | bank << 4`. Named data establishes R=0, G=1, B=2. The accepted fourth selector's meaning is not recovered. The channel comparison uses signed `BGT`, so this wrapper alone also does not reject negative 32-bit channel values; it is not a complete safe-input contract. The IP and bank comparisons are unsigned. No invalid selector was invoked on hardware.

## The 124-byte register payload

The register worker at `0xb82e00` constructs a transfer of 31 words beginning at MLUT base `+0x400`. Thus word `i` maps to register-relative offset `0x400 + 4*i`.

Some fields have direct code corroboration:

| Register-relative offset | Established observation | Evidence |
|---|---|---|
| `0x400` | `all_on` parameter stored as a word | Named diagnostic `0xb7a936`, synchronous builder `0xb7ce6a`, queued builder `0xb833f4` |
| `0x404` | Named CPU L3D RAM-access parameter, written as a whole word | Diagnostic `0xb7a364`, wrapper `0xb78ed8`, forwarding `0xb82a66`, register write `0xb7832e` |
| `0x408` | Bank/channel selector encoded as above | Selector wrapper/helper plus 23 file-backed control words |
| `0x410` | `post_black` parameter in bit 0, `pre_black` in bit 1 | Same three on/off construction paths |
| `0x414` | R/G/B on/off parameters at bits 0, 8, 16 | Same three on/off construction paths |
| `0x42c–0x478` | Twenty node/coordinate-threshold candidates | Exact agreement of THRU sample values and register words; named USER_LUT_17 template |

The on/off meanings are parameter names recovered from firmware diagnostic help. The builder confirms bit packing; the camera's resulting physical behavior and polarity have not been tested.

The isolated host execution covers all 64 Boolean combinations of `all_on, R, G, B, pre_black, post_black`, for IP indices 0 and 6: **128 cases**. The real `0xb833f4–0xb8346e` code constructs three descriptors targeting `0x400`, `0x410`, and `0x414`. Every source pointer, word count and resulting value is checked. Execution is bounded to that function; it calls no external functions and accesses no hardware registers.

### The USER_LUT_17 template provides a specific node configuration

Its 31 words are:

```text
word 0:       1
words 1–10:   0
words 11–27:  0, 0x200, 0x400, …, 0x1e00, 0x1fff
words 28–30:  0x10001, 0x10001, 0x10001
```

That is a 17-value scale followed by three larger padding values in a twenty-word node area. The AP producer independently establishes 17 active source coordinates and padding beyond coordinate 16. Together these are concrete dimensional constraints, rather than a guess from a convenient byte count.

Do not assign a dedicated "disable bit" meaning to bit 16 solely because the final three words are `0x10001`. The examined firmware moves those words as an opaque register block. Likewise, 13-bit-scale values in USER_LUT_17 do not establish a universal 13-bit input format: the THRU template uses values up to `0xf324`, other templates use nonuniform thresholds, and the diagnostic TEST template contains `0x1ffff` node values. Register hardware documentation or a recovered numerical evaluator is needed to name those details precisely.

Words for `0x40c`, `0x418–0x41c`, and `0x420–0x428` remain incompletely interpreted here. The first three are zero in all 17 retained register templates, a bounded observation rather than proof that the registers are reserved. Repeated paired-halfword values such as `0x02000200` in the latter group should not be assigned a black-offset or precision meaning without a corresponding code path.

## A diagnostic pattern that would mislead a format guess

Diagnostic SRAM command 31 at `0xb7acac` copies 24 blocks of 2,000 bytes from source beginning at `0x112b93c`, then submits them through the normal loader. Every 32-bit word follows:

```text
0x3d000000 | (channel << 20) | (bank << 16) | (0x3000 + word_index * 4)
```

All **12,000 words** match. For example, the first bank begins `0x3d003000, 0x3d003004, …`. These are address/bank-tagged test words, not ordinary numerical LUT samples. Treating them as float32 or using their apparent bit ranges to infer LUT precision would produce a false conclusion.

Register diagnostic command 43 at `0xb7b524` similarly copies its TEST payload from `0x112a68c`; it does not use the USER_LUT_17 template. Separating diagnostic patterns from named transform data is necessary before comparing numerical conventions.

## Finding ledger and remaining limits

| Question | Status |
|---|---|
| RGB plane and bank order | Established by named descriptors plus all 24,000 THRU data halfwords; independently matched to AP writer |
| Scalar storage packing and endianness | Two little-endian 16-bit entries per word established for the native SRAM layout |
| Storage coordinate capacity | Twenty-coordinate bank layout established for THRU data; ordinary user import uses 17 active coordinates per companion AP trace |
| Register payload size/address | 31 words starting at MLUT base `+0x400` |
| Enable/black-control bit construction | Three register words recovered and 128 descriptor-builder cases passed |
| Remaining register fields and invalid-node encoding | Partially known; several templates retained, hardware interpretation unresolved |
| Exact input/output color domain and silicon precision | Not established by CP storage alone; AP quantizer provides a producer representation, not full pipeline color semantics |
| Image-frame routing, DMA/coherency and payload lifetime | Outside this pass and unresolved |

The inspected CP movers do not numerically transform samples before submission. This bounded negative observation applies only to the retained copy/descriptor routines; it does not claim the entire CP lacks a converter. A safe still-LUT implementation still needs a proven still-frame route, color contract, context lifecycle and completion/ownership contract. No installation or camera experiment is implied by this recovered layout.
