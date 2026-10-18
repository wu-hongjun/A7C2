# av-cam encoder input pixels and resource handoff

The normal JPEG and HEIF prepare paths select their input from the same capture-context slots: geometry at `+0x410/+0x428` and handles at `+0x41c/+0x434`. A resize stage demonstrably writes its output handle back into those slots. The JPEG path then resolves that handle through DMM, copies the first extent address into its codec parameters, adjusts it for geometry, and converts its CPU address form into hardware address words.

**This does not yet link a particular MLUT-processed allocation to an encoder input.** The context can receive pixels through several paths. The CP MLUT pool token, returned DMM handle, CPU alias and hardware address are different values that must be joined through actual construction and conversion edges. No such complete cross-processor identity is asserted here.

## Evidence and coordinate model

Run `.venv-research/bin/python research/trace_av_cam_pixels.py`. The generator verifies the full source hash and retains 20 code regions, 1,464 AArch64 instructions, the literal address conversion table, and eight host cases in [av-cam-pixels.json](evidence/av-cam-pixels.json). These are ordinary raw-binary Capstone/Unicorn results, not REA Evidence.

Source: `av-cam.bin`, 13,797,088 bytes, SHA-256 `90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096`. It is the ORIL version-5 raw AArch64 image described in [STILL_INFO_PRODUCER.md](STILL_INFO_PRODUCER.md). **Every code address below is an av-cam file offset**. Add `0xffffffc001000000` for linked VA; this is neither a CP address nor an ELF VA. The runtime mapping remains unobserved. Host emulation uses normalized base `0x01000000` on original bytes and follows no absolute high-address dispatch tables.

## Shared input selection, separate outputs

JPEG PrepareEncode begins at `0x200e68`. Its normal context-`+0x7dc==0`, target-0/3 path calls slot helper `0x1f4148`, which maps selector 1 to index 0 and selector `0x10` to index 1. Instructions `0x200f74..0x200fc8` read:

| Selected field | Capture context source | Prepare output |
| --- | --- | --- |
| Input handle | `+0x41c + index*0x18` | Input descriptor `+4` |
| Six geometry halfwords | `+0x410 + index*0x18` | Twelve-byte geometry descriptor |

HEIF normal mode 0 does the same at `0x1ffc34..0x1ffc80`, with the same slot helper. Its other modes select different context offsets and are not emulated here. JPEG's context-`+0x7dc==1` path also has different target-dependent inputs. The shared normal input selection does not establish shared JPEG/HEIF codec execution: JPEG uses resource `0x1f9d8c` and codec API `0x2a58f0`; the separately investigated HEIF encode route uses a different resource interface.

Materializer `0x2097fc` takes **both** an existing input handle and a requested output unit type:

- Input: `0x208f3c` calls `0x20cb7c` with the existing handle, operation argument 5 and controller `0x61003c`. The latter references `dmm GetUnitAddr`, builds category `0x2a` / ID `0x55`, invokes an imported transport interface, validates the reply and copies `0xd8` bytes into the input-info structure.
- Output: `0x207388` calls allocator wrapper `0x208dfc` with the requested output unit type, count/reference argument 1, and wait value `0xffffffff`. The returned output handle is inserted into the local tracking object, with tracked mode 2.

These are separate operations. Treating the output allocation as the input pixel origin would reverse the dataflow.

After success, JPEG instructions `0x201144..0x201150` copy the input-info first extent's address and length from info `+0x18/+0x20` into input descriptor `+8/+0x10`. HEIF instructions `0x1ffda8..0x1ffdb4` independently perform the equivalent copy. Output extent values are copied into a separate output descriptor.

The input address query does not add the existing input handle to this local tracking object. This is a bounded local ownership observation; DMM's external reference-count or allocation semantics are not executed by the tests.

## JPEG address propagation into the codec

`ExecEncode` at `0x201340` constructs its codec parameter block. At `0x20136c/0x201380`, input descriptor `+8` becomes the codec block's first u64. Its geometry mapping is retained instruction by instruction:

| Codec block field | Source |
| --- | --- |
| `+0` u64 | Input descriptor `+8`: first resolved input extent address |
| `+8` u32 | Geometry halfword `+0` |
| `+0xc` u32 | Geometry halfword `+2` |
| `+0x10` u32 | Geometry halfword `+8` |
| `+0x14` u32 | Geometry halfword `+0xa` |
| `+0x20/+0x24` u32 | Zero on this construction path |
| `+0x28/+0x2c` u32 | Geometry halfwords `+4/+6` |

The resource `Encode` wrapper at `0x1f9d8c` forwards this parameter pointer, callback and userdata to `0x2a58f0`. The codec API wrapper invokes request setup `0x2acb14`. At `0x2acbe8..0x2acc18`, that setup passes the original parameter pointer, its `+0x28` dimensions, and request destination `+0x90` to input builder `0x2aa3e8`.

In the direct-address branch, after its alignment and size checks, `0x2aa68c..0x2aa6a8` computes:

```
pixel_address = base + 2 * ((field24 + field14) * stride + field10 + field20)
```

Here `base=codec+0`, `stride=codec+8`, and the numbered fields are offsets in that codec parameter structure. With the two zero fields above, this becomes `base + 2*(geometry[+0xa]*geometry[+0] + geometry[+8])`. The factor two is actual instruction arithmetic; it is insufficient by itself to name the pixel encoding or channel layout. When mode flag `w7` bit 0 is set, this builder skips direct-address construction; that path remains outside the tests.

## CPU address conversion must precede comparison with CP

Helper `0x2aeed4` searches four literal entries at file `0x3320d8`, each with stride `0x20`, and converts a matching address by `address-source_base+destination_base`:

| Source range base | Destination base | Length |
| --- | --- | --- |
| `0x400000000` | `0` | `0x100000000` |
| `0x500000000` | `0x100000000` | `0x100000000` |
| `0xfe000000` | `0x3fe000000` | `0x6c0000` |
| `0` | `0` | `0` |

The resulting request words are **`[(destination >> 24) & 0xffffff00, destination & 0xffffffff]`**, as written at `0x2aef3c..0x2aef44`. These two words are not a normal little-endian u64. A raw pointer comparison against the CP's physical-address field would therefore be misleading, even if the underlying allocation were the same.

The literal conversion and its use by the codec input builder establish address-form normalization. They do not supply the missing allocation identity. In particular, CP direct-ring pool token `0x43f/0x2d9` is not automatically a DMM handle, and a returned handle must not be equated with its unit type or physical backing.

## Concrete resize-output handoff

The actual `PostResize` path references `StillMP_Stage_ResizeYc.cpp` and `PostResize`. At `0x2065ec..0x2066cc` it:

1. Reads the old input handle and calls `0x208c5c(old_handle, 2, 0x61003c)` at `0x206614`.
2. Reads the new output handle from output descriptor `+4` at `0x2065f0`.
3. Uses the same slot helper and writes that new handle to capture context `+0x41c+index*0x18` at `0x206694`, in its normal context path.
4. Calls local tracker detach `0x2075f8` at `0x2066cc`. The matched tracker entry is changed to handle `0xffffffff` and mode 0, so the local tracker destructor will skip that detached entry.

This is a static construction edge from resize output into the exact context field later read by JPEG and HEIF prepare code. It is more specific than fixture equality or shared names. It still does not identify resize input as MLUT output, prove the runtime stage order for every mode, or prove successful DMA completion. Other decode and transfer paths can also populate these context slots.

## Validation and unresolved join

Four host cases execute the original slot selector, context reads, actual DMM request builder/reply parser, materializer and output tracker insertion, JPEG parameter construction, pixel address arithmetic and literal-table conversion. They cover both slots and three nonempty mapping ranges. External DMM replies and output allocations are synthetic; logs, memory-copy helpers, allocation and transport imports are intercepted. No actual pixel processing occurs.

Four further cases execute the original selected PostResize path for both slots and targets 0/3. They verify release arguments, the new context handle and tracker detachment. These checks validate local instructions and ownership bookkeeping. Synthetic matching values never establish that a real CP MLUT allocation reached the encoder.

The remaining critical join is **a source-bound export of a specific CP MLUT output allocation/handle into the av-cam capture-context input slot, including the relevant mode and lifetime**. Different DMM handles may share backing through AllocShare; the missing evidence can therefore be an actual share/export lineage, not necessarily numerical handle equality. Native av-cam MLUT driver availability, a VFX or resize name, and shared DMM services do not close that gap. No camera, USB, installation, network, live transport, encoding, saved-image behavior or LUT effect was tested.
