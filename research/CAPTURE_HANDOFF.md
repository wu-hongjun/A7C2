# Capture input transfer and the remaining MLUT join

Offline follow-up, October 18, 2026; original A7C II 2.01 inputs. **A concrete capture-request → PCIe transfer → JPEG input edge is now recovered for one variant. Its source is still not identified as MLUT output.** This changes the question from “where does av-cam get a pixel handle?” to “who puts the particular processed CP resource into the incoming request?”

Reproduce with:

```sh
.venv-research/bin/python research/trace_capture_handoff.py
```

[capture-handoff.json](evidence/capture-handoff.json) retains exact source hashes, 12 instruction regions, two literal dispatch tables and six bounded host cases. Capstone and Unicorn operate only on local bytes and synthetic RAM. No camera, USB, Bluetooth, network, native DMA, codec or target process runs. These results are ordinary source-bound analysis, separate from retained REA Evidence.

All **av-cam addresses are file offsets**; linked VA is offset plus `0xffffffc001000000`. CP addresses are Thumb runtime coordinates; subtract `0x108000` for file offsets. Host rebasing of a linked dispatch-table pointer is explicit in the generator.

## A transferred allocation reaches the JPEG input slot

For capture context `C`, incoming request `R`, and **context mode `C+0x7dc == 1`**:

| Original instructions | Observed construction edge |
| --- | --- |
| av-cam `0x1f32cc..0x1f32f0` | `R+0x2c4` u32 → `C+0x324`; neighboring source handles go to `+0x328/+0x32c` |
| `0x1f33d8..0x1f33e4` | Twelve geometry bytes from `R+0x2b4` → `C+0x3bc` |
| `0x202ebc..0x202f34` | Checks source-handle sentinel, calculates transfer length from two geometry halfwords, calls `ChipTransfer` (`0x208168`) with source handle and destination unit `0x4c8` |
| `0x2081d8..0x2081e8` | Independently allocates the destination through `0x208dfc(unit=0x4c8,count=1,wait=0xffffffff)` |
| `0x208320..0x208354` | Constructs handle/offset source and destination descriptors and submits PCIe DMA through import thunk `0x2f44c` |
| `0x20839c..0x2083c0` | Waits on event group `0x23`, pattern 4 in this caller; callback `0x2084bc` signals the event through `0x2e980` |
| `0x208434..0x208444` | Releases the **source** using `(handle,2,0x9061003c)` |
| `0x202f38..0x202f54` | Copies geometry to `C+0x410`, invalidates `C+0x324`, writes **destination allocation's handle** to `C+0x41c` |
| `0x200f40..0x200f58`, table `0x3779b0` | JPEG context-mode-1 target 3 dispatches to slot input reader `0x200f74` |
| `0x200f74..0x200fc8` | Selector 1 reads the resulting `C+0x41c` into encoder input descriptor `+4` and copies geometry |

The DMA request structure has source-descriptor pointer at `+0`, destination-descriptor pointer at `+8`, byte count at `+0x10`, count 1 at `+0x14`, callback at `+0x18`, and userdata at `+0x20`. Each selected descriptor is two u32 values: **handle, logical offset**. This boundary does not carry a raw pixel pointer.

The instruction-derived transfer length is a 32-bit width×height product, signed integer-to-float conversion, float multiplication by 2.5 and unsigned conversion. For the two tested positive sizes it equals `width*height*5/2`: 3,932,160 bytes at 1536×1024 and 81,853,440 bytes at 7008×4672. This is a transfer-size calculation; it does **not** identify sample depth, packing, stride or color space. The fixtures supply a sufficiently large destination; they do not measure real pool capacity.

The source release's controller value `0x9061003c` differs from the local input-query/release value `0x61003c`. Together with the `ChipTransfer` and `PCIeDma` diagnostics and separate allocation, this is strong evidence of a cross-domain copy boundary. The implementation of the imported DMA service remains outside execution. We do not claim observed physical DMA, coherency or completion on a camera.

Two successful cases execute the real request-field import, geometry import, complete transfer caller, complete `ChipTransfer` body, then the target-3 JPEG input reader on the same context. Allocation and DMA replies remain fixtures with deliberately different source/destination handles. No host code copies the source handle into the encoder slot to manufacture a join.

## What this proves for JPEG and leaves for HEIF

The previous [encoder pixel analysis](AV_CAM_PIXELS.md) already follows the selected slot through DMM address resolution, JPEG codec parameters and address normalization. The new evidence supplies an earlier producer of that slot and the actual transfer request consuming its source handle.

This is **one variant**, not a universal still-capture graph. The literal mode-1 JPEG input table maps target 0 to a diagnostic, targets 1/2 to `C+0x344/+0x348`, and target 3 to the slot reader. Target names and live selection remain unclassified. Do not assume “target 3” means the user's full-size original JPEG. Context-mode-0 paths, decoded-JPEG inputs and resized inputs remain separate producers.

HEIF's selected slot reader at `0x1ffc50..0x1ffc80` is retained, and its normal input path also uses `C+0x41c/+0x434`. This pass does not execute a continuous transfer-to-HEIF selection path. [HEIF ownership](HEIF_OWNERSHIP.md) and [saved-image finalization](STILL_SINK.md) remain useful downstream evidence, but sharing a slot layout does not prove the same live upstream variant is used.

## A failure-mode constraint for a prototype

Two fault fixtures make **all five DMA submissions return nonzero**, while diagnostic functions are intercepted as returning. The examined instruction path skips the completion wait, proceeds to source release and returns; the caller still publishes the destination handle and clears the source field. Thus **seeing a destination handle in the slot is insufficient evidence that a copy completed**.

This is a bounded property of the original bytes under the stated diagnostic behavior, not a demonstrated camera bug. Actual fatal/assert/log policy and higher-level error handling remain unknown. A future prototype must track a positive successful submission **and** completion for its own frame, retain source ownership until success, and stop on failed transfer. It must not infer valid pixels merely from handle presence or reuse this internal helper as an undocumented standalone API.

## The CP shared handle has an export record, with a different identity

The previous [allocation trace](MLUT_ALLOCATION.md) follows the selected fallback's real DMM handle through AllocShare into cache `0x10f7e28`, with bookkeeping helper `0xb231e0`. This pass follows that helper's actual writes:

| CP global | Value written by `0xb231e0` | Read by export handler |
| --- | --- | --- |
| `0x1a53278` | Shared handle | Export record u32 `+8` |
| `0x1a5327c` | Selector/token | Export record u32 `+0x10` |
| `0x1a53280` | Extent length | Export record u32 `+0x18` |

Handler `0xb23788` also sets record type **`0x108`**, byte `+4=1`, and word `+0x14=0`. The table pair at `0xe782b8` is selector **`0x72`**, handler pointer `0xb23789` (Thumb bit included). Two cases run the actual publication helper and then actual export handler, verifying transport through these globals. Publication notifications (`0xb24e6c`) are intercepted.

This is an additional export surface to investigate. **It is not still-info message `0x1006`, not proof of an AP consumer, and not a recovered writer of `R+0x2c4`.** The nearby `CS/CS_ExportAPI.cpp` diagnostic name cannot supply that missing consumer. The cache allocator's primary selector `0x347` and fallback `0x63d` likewise do not establish a saved-photo role.

## The next bounded reverse-engineering targets

1. Recover the upstream writer of the av-cam incoming request's **`+0x2c4`**, identify its unit/handle domain, and classify which shooting mode and JPEG target use this variant. Other mode-dependent request fields must be traced separately.
2. Follow selector-`0x72`/type-`0x108` consumers, or another actual CP export, from the already derived shared handle. Require a real assignment into a capture request or a proven resource-alias lineage. A request fabricated from that handle does not establish one.
3. Establish MLUT port direction, live allocation branch and frame completion. Map pixel layout, stride, crop, range/gamut, shaper/3D stage ordering and coherency through the transfer.
4. Join that same frame to the full-resolution JPEG **and separately HEIF** saved-image area. Existing downstream notification/save ownership is reusable; identify whether it is the original, screen image or thumbnail.

Until those steps are source-bound, a native MLUT capture patch is not a working still-LUT prototype. A completed-JPEG software worker can be developed independently; its concrete plan and the four requested features are in [STILL_PROTOTYPE_PLAN.md](STILL_PROTOTYPE_PLAN.md).
