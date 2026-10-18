# HEIF MC encoder request, results and sequence construction

The official A7C II 2.01 AP library `libInfraAvbbc.so` contains a concrete **MC still-encoder request path**, distinct from the previously traced CP JPEG API and AP HEIF container finalizer. Original code constructs a 96-byte frame request from an uncompressed-image descriptor and an output unit, submits it asynchronously, and consumes a later status/length response. This pass recovers those fields and the surrounding resize/encode command graph. It does **not** connect an MLUT output frame to this encoder, identify a live codec receiver, or join the returned stream to the saved HEIF grid/area descriptors.

All work is offline. No camera, USB, network, download, installation, firmware process, codec, resource service or physical memory is accessed. The host emulator substitutes named services and never sends a message. This report's byte and host results are **ordinary evidence, not REA Evidence**; separately identified actual REA records corroborate selected functions below.

## Reproduction and source identity

Run:

```sh
.venv-research/bin/python research/trace_heif_encoder.py
```

The generator writes [heif-encoder.json](evidence/heif-encoder.json): six original instruction regions containing **1,430 decoded AArch64 instructions**, typed RTTI/vtable relocations, five exhaustive byte-input conversion tests, parameter transport, request submission, response branches, open/close boundaries and command-graph construction. It passes **2,653 bounded host cases**: 2,304 enum cases, 288 parameter builders, seven aspect branches, three encode submissions, 24 encode responses, 16 sequence configurations, two open/close submissions and nine open/close responses. Synthetic data is embedded as metadata and contains no user photos.

Source:

```text
research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/
  dev/nflasha15_unpacked/lib/libInfraAvbbc.so
SHA-256 676197567dbbefa54e08f528968efc6ca2aaffe8bee3ee3eae8164382ed5a7e5
```

Addresses below are original ELF link addresses, not measured camera addresses. Executable ranges are in its first PT_LOAD segment; JSON retains the actual file offsets. The original CP image is separately inventoried with SHA-256 `f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428`: its finite case-insensitive HEIF/HEVC/H265 string inventory contains five matches, chiefly gamma diagnostics, and no matching encoder class names. This bounded negative result does not prove CP has no HEIF role.

## A real encoder class, not a nearby string

Defined ELF symbols identify:

| Function/data | ELF VA | Role |
|---|---:|---|
| `ExecCmd_MC_EncodeHeif::MakeMcEncodeFrameParam` | `0x120150` | Builds `MC::MCODEC_STILL_ENC_ENCODE_FRAME` |
| `ExecCmd_MC_EncodeHeif::ExecCmd` | `0x1205b4` | Builds and submits request |
| `ExecCmd_MC_EncodeHeif::ResCmd` | `0x11fdcc` | Handles status/length response |
| `ExecCmd_MC_EncodeHeif` vtable | `0x1af690` | Symbol relocations bind ExecCmd, ResCmd and destructors |
| `ExecCmd_MC_StillEncOpenClose::ExecCmd` / `ResCmd` | `0x1206a4` / `0x120864` | Codec-session open/close boundary |
| `InfraStillEncoderSeqResizeEncForHeif::SetParameter` | `0x1241fc` | Constructs resize/encode sequence |

The vtable is zero-filled in the ELF file and obtains its contents through symbol relocations. Treating raw zeros as missing methods would be wrong. Its `+0x10/+0x18` symbol relocations identify ExecCmd/ResCmd and its `+8` relocation identifies the corresponding RTTI. The generator retains the raw bytes and symbolic relocations without assuming that runtime symbol interposition cannot occur.

## The 96-byte frame request

`ExecCmd` obtains input parameters through ParameterControl virtual slot `+0xb0` with key `0x104a`, zeroes a 96-byte stack structure, calls the original builder, then invokes `OsalSndMsg(0x8057060a, request, 0x60)`. Return zero means this method completed its submission path; there is no synchronous encode/wait here. The host intercepts that send and records its destination and bytes.

The builder's original loads and stores establish:

| Request offset | Source or behavior |
|---|---|
| `+0/+2` u16 | `ConvertCid(5)` and `ConvertCmdMid(5,0x534)` |
| `+4/+6` u16 | Length `0x60`, count `1` |
| `+8` u64 | Session handle obtained through ParameterControl `+0xa8`, key `0x1060` |
| `+0x10` u64, `+0x18` u32 | Message-manager OSAL ID, transported twice |
| `+0x1c` u32 | Mode 3→0, 4→2, 5→1 |
| `+0x20` u32 | Quality byte at input `+0x70`: 0→`0x100`, 1→`0x101`, 2→`0x102`, 3→`0x103`; other tested byte values→`0x102` |
| `+0x24` byte | Zero for mode 4; otherwise input `+0x74` nonzero→0, zero→2 |
| `+0x25/+0x26` bytes | Numeric Y-offset/colorspace conversion results |
| `+0x28` u32, `+0x30` u64 | Input unit/address-like fields, from input `+0x34/+0x40` or a resolved Yc descriptor `+4/+0x10` |
| `+0x38..+0x3b` bytes | Input `+0x62`, converted depth from `+0x63`, converted gamma and colorimetry from `+0x61` |
| `+0x3c..+0x4e` | Ten geometry halfwords, branch-dependent below |
| `+0x50` u64, `+0x58` u32 | Supplied `dmm_comp_open_unit` fields at `+0x18/+0x20` |

Mode 3 uses the direct input unit/address-like pair when object byte `+0x2c` is zero; otherwise it uses the resolved descriptor. Modes 4 and 5 use the resolved descriptor. These are pointer/unit transports proven with distinct synthetic 32/64-bit markers; the underlying physical/virtual translation, allocation bounds, stride, chroma layout, cache operations and lifetime remain unknown. A two-byte pixel-size inference would not supply those missing contracts.

The geometry tests establish mode 3's first two halfwords from input `+0x54/+0x56`, followed by zero offsets and repeated input `+0x68/+0x6a` extents. Mode 4 uses a 640×480 canvas and an aspect branch yielding 320×320, 320×240, 320×212 or 320×180; seven representative aspect/fallback cases are checked. Mode 5 uses a 1920×1088 canvas and repeated extents from input `+0x6c/+0x6e`. Field roles beyond their source loads/repetition are not assigned from dimensions alone.

The tiny conversion helpers execute original instructions for every byte input. Depth yields 1 only for input 1; gamma yields `0x57` for input 4 and `0x58` otherwise; colorimetry yields 1 only for input 4. Y-offset yields 1 only when its first byte is 1 and its second byte zero. Colorspace yields 1 only when the first byte is 1 or 2 and the second byte zero. Those numeric mappings do not alone establish color standards, 10-bit precision or an sRGB/HDR contract.

## Completion and error handling

`ResCmd` accepts selector **`0x8565`**, reads u32 status at response `+0x10`, and looks up a **separate output parameter block** under key `0x105f`. Successful status stores the low 32 bits of the u64 at response `+0x18` into that block at:

| Object mode | Output-block offset | Actual diagnostic label |
|---|---:|---|
| 3 | `+0x5c` | `MainJpg` |
| 4 | `+0x68` | `ThmJpg` |
| 5 | `+0x74` | `screenNail` |

These labels are reused inside the HEIF class; they do not establish that this request necessarily emits JPEG bitstreams. The response diagnostic explicitly calls the scalar `Encode Size`. It is therefore stronger evidence for an encoded-length role than an unnamed scalar, but still does not identify a saved-file area or HEIF grid item. Input key `0x104a` and output key `0x105f` must not be assumed to alias because some offsets coincide.

A nonzero encode status zeroes the applicable length word, calls error conversion for component 5, publishes the converted error through ParameterControl virtual `+0x48` with the object's error-key field, and returns `0x201`. Unknown selector returns 1. Twenty-four synthetic cases check modes 3/4/5 plus an unsupported mode, accepted/wrong selectors, and success/two failure values. Error mapping itself is an explicit stub, so its sentinel is not a vendor error code.

Open logical command `0x532` emits 16 bytes; close `0x533` emits 24 bytes to the same destination. The open request transports the constructor's u32 parameter and sender ID. Close transports the u64 session handle under key `0x1060` and sender ID. Response selector `0x8563` with zero status sets key `0x1060` from response `+8` through virtual `+0x50`; `0x8564` with zero status clears it through `+0x40`. The examined open/close handler returns zero for these selectors even with nonzero status, and then does not update the handle. This is a bounded handler observation, **not a proven ignored-error bug**: surrounding sequence/global response classification must be traced before judging failure behavior.

## The original sequence graph is conditional

The original sequence method creates 27 command objects and registers their numbered slots. Tests stub constructors, allocation, parameter storage and graph registration; no constructed command runs.

| Slots | Constructed commands |
|---|---|
| 1 | MC still-encoder open |
| 2/3, 4, 5, 6, 7, 8/9 | Primary allocation, resize/rotate, DMM open, EncodeHeif, DMM close, frees |
| 10/11, 12, 13, 14, 15, 16/17 | Corresponding mode-4 group |
| 18/19, 20, 21, 22, 23, 24/25 | Corresponding mode-5 group |
| 26/27 | MC still-encoder close, resize/encode completion |

Encode slots **6/14/22** receive Yc-descriptor keys **`0x1058/0x105a/0x105c`**, modes **3/4/5**, and shared error key `0x1043`. The primary resolved-input flag is copied from input unit word `+0x34` bit 15; the other two receive 1.

The enabled-path flags come from three words at `+0x54/+0x60/+0x6c` of a block obtained with key `0x105e`. Every sequence begins with slot 1. If primary is enabled, bit 15 selects slots `2..9`; when clear it selects only `5,6,7`. Enabled mode-4 and mode-5 paths add `10..17` and `18..25`. Slots 26 and 27 follow, then `0xffff` terminal. Sixteen cases exhaust all combinations of these three flags and bit 15, checking both command registration and transitions. This proves the builder's conditional graph, not which graph a real still capture chooses or whether all synthetic configurations are valid.

## Wire identity and the missing joins

The emulator deliberately replaces CID/MID conversion with `0x3344/0x5566` sentinels and retains their logical inputs. Those bytes are **not usable command IDs**. The [separate command-bridge investigation](AVBBC_ENCODER_BRIDGE.md) executes the local converter: logical `0x534` is missing from its table and returns `0xffff`. Its bounded dynamic-symbol inventory finds these converter/table definitions only in this library; runtime binding or table changes remain unobserved. This report therefore distinguishes logical command arguments, the hardcoded send destination and expected response selector instead of inventing a complete working wire protocol. Locally defined PLT methods are deliberately bound to this source ELF for the isolated host tests; runtime interposition is not modeled.

Actual REA independently corroborates parameter construction (`ev_3f3ac57575ded305e4e9a0239be2d9a206fc792446a649300660ba258e1408e8`), asynchronous ExecCmd (`ev_86d623bef36238f0b7e7e0223a74ec6c34a3b0cd71fd39760ece4fe4fa30f1d4`), response handling (`ev_970c5d08731e021eb489c763f615b36fc39636ec96659e7e2304029355eed271`) and constructor fields (`ev_d45ba0946d75ff175426f3f2d2caa9a1ed963693d68ab79c1d78ce31261e3121`). Those records are separate from the ordinary emulation evidence and use the REA session's mapped addresses; original ELF addresses above remain source-bound.

This is a concrete resize/encode service boundary. It has not been proven to be the normal full-resolution capture route. No instruction edge here establishes selected MLUT output → this Yc resource, this MC completion → AP still-record result, or resulting stream → saved HEIF area/grid descriptors. The MC receiver, hardware scheduling, cancellation/timeout behavior and output ownership must be recovered before such a join is safe to claim. Existing [saved-file sink](STILL_SINK.md) code remains a separate container/file endpoint, and the prior [CP JPEG boundary](STILL_ENCODER_ROUTE.md) remains a separate codec path.
