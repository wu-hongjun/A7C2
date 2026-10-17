# A7C II CP: LUT callback lifecycle and still-image routing

Research date: October 17, 2026. Target: official A7C II 2.01 `cpapp-b.bin`, SHA-256 `f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428`.

**The previously unresolved context-instance edge can now be explained for one structurally present sequence path:** the parameter builder's context pointer is stored as callback userdata, and the actual list dispatcher, adapter and wrapper forward that same pointer to the MLUT consumer. However, the recovered upstream selector does not choose this particular sequence variant for any of the 65,536 tested 16-bit modes. **Full-resolution saved-still routing through MLUT remains unproven.** A separate result establishes that the MLUT dimension interface accepts 7008 × 4672; register representability is therefore not the barrier visible in this layer.

Everything here is offline. No camera was accessed, no code ran on the camera, and no official firmware or deployment asset changed. Host-only Unicorn tests execute selected original CP instructions with explicit external-call stubs and ordinary host memory standing in for registers. They do not emulate the camera's operating system, image processor, DMA timing or actual frames.

Reproduce with `.venv-research/bin/python research/trace_still_mlut.py`. The [source-bound JSON](evidence/still-mlut-route.json) retains 32 decoded regions, instruction bytes, a 362-instruction syntactic control-flow path, one callback-identity test, 32 resource-bitmap cases, 48 dimension cases, all 65,536 16-bit mode-lookup cases and four additional 32-bit boundary cases. The full preservation region is retained, rather than only four-byte calls. This complements [CP architecture](CP_ARCHITECTURE.md), whose earlier lifecycle qualification remains an accurate description of that earlier evidence set.

## Upstream selection limits this sequence's applicability

A bounded search of the main CP code range found three Thumb `MOVW/MOVT` pairs constructing pointer **`0x4c5a7d`**. Two, at files `0xa224be`/`0xa224c2` and `0xa2263c`/`0xa22648`, compare an existing callback pointer while classifying a sequence descriptor. The other pair, at **`0x3b8972`/`0x3b897e`**, actually constructs the callback to submit. No literal four-byte occurrence of the pointer exists in the image. This candidate scan does not recover every possible computed alias or ARM-state reference.

The submission path is gated by this concrete sequence:

```text
mode argument -> 0x774a60(mode)
  -> 0x273a3c(mode, ..., output)
  -> when recognized, return output word +0x4; otherwise return 0
selector chooses 0x4c5a7d only when that returned property == 1
```

The `0x273a3c` implementation supplies default image-mode parameters; its diagnostic context includes `CamCore: ImgModeDefaultParam() No Table[%d] Flow`. The decoded leaves set output `+0x4` to zero. Exhaustive host execution across all **65,536 unsigned 16-bit inputs** recognizes 30 mode values, but that property is **zero for every tested value**. Four additional inputs (`0x10000`, `0x7fffffff`, `0x80000000`, `0xffffffff`) are unrecognized and also produce zero. The report retains the recognized mode rows, leaf instructions, jump-table destinations, getter and selecting branch.

Thus the recovered selector cannot choose the `0x4c5a7c` variant for the tested mode domain in the unchanged official image. It may be an inactive retained variant; that is an inference, not a proven explanation of why Sony included it. The callback-lifecycle test below demonstrates its structure, not a claim that the camera executes it in normal capture.

This finding **does not make every MLUT path unreachable**. The selecting code's alternative at file `0x3b899a` constructs pointer **`0x4c79c1`**. That alternative routine begins at file `0x3bf9c0`, also obtains its context through the incoming sequence object, calls the same LUT parameter builder at file `0x3bfd84`, and registers a separate callback `0xb2b57d` at file `0x3c0e7a`. That callback has a conditional call to consumer `0x4e876c`. These are retained neighboring-path leads; their mode identity, actual invocation and saved-still buffer routing are not established by this pass.

The complete recognized-mode output table is below, grouped only where all returned values are identical. The JSON retains each of the 30 mode rows individually. Field `+0` is a byte; `+4`, `+8` and `+0xc` are 32-bit words. Bytes `+1`–`+3` are untouched by this routine and are zero in the test because the caller/test initializes the output buffer.

| Numeric mode IDs | Byte `+0` | Word `+4` | Word `+8` | Word `+0xc` |
|---|---:|---:|---:|---:|
| `0821`, `0880`, `08c0`, `08c1`, `0b21`, `0bc1` | 1 | 0 | 3 | 14 |
| `0ae0`, `0ae1`, `0b20`, `0bc0` | 0 | 0 | 3 | 3 |
| `5000`, `5001`, `5002`, `5004`, `5200`, `5201`, `5400`, `5401`, `8000`, `c100`, `c110` | 0 | 0 | 1 | 0 |
| `5020`, `5021` | 0 | 0 | 2 | 0 |
| `a000`, `a030`, `a100`, `a130`, `e000` | 1 | 0 | 0 | 15 |
| `c000`, `c010` | 0 | 0 | 1 | 2 |

All mode IDs in this table are hexadecimal. The semantic enum names have not been recovered. The returned values do not establish image width or height, and their numeric grouping cannot safely label them as still, movie or preview modes.

### Nearest geometry source in the alternative sequence

The alternative sequence calls **`0xb2c1b8`** at file `0x3bfd7e` with its context in `r2` and selector `r1 = 1`, immediately before the LUT parameter builder. In that branch the initializer reads two records through the selected runtime descriptor:

- Record A at **`descriptor[0xc] + 0xa34`**: two halfwords at `+0x3c`/`+0x3e` are copied to context `+0x50c`/`+0x50e`; two further 20-byte blocks are copied into context `+0x510` and `+0x524`.
- Record B at **`descriptor[0xc] + 0xa84`**: four units with source stride `0x18` populate context units with destination stride `0x70`. The first unit copies halfword pairs `+8/+0xa`, `+0xc/+0xe` and `+0x10/+0x12` into context `+0x1b4/+0x1b6`, `+0x1b8/+0x1ba` and `+0x1bc/+0x1be`, with accompanying format-like words and flags.

Later initializer instructions pass the first halfword pair of subsequent units to helper **`0x4d3554`**. Its first arithmetic instruction multiplies the first two arguments; following branches apply format-dependent size scaling. This corroborates geometry/area-related use of the pair, beyond a string-name guess.

These values are supplied by **runtime descriptor contents**. This nearest-source trace recovers neither actual 7008/4672 values nor a named still-mode enum, and it does not join these units to a saved-frame encoder. It therefore improves the structure map without classifying the alternative as full-resolution still or VD preview. Reading a camera register or executing a sequence has not been used to fill that gap.

## Same context: parameter builder to registered callback

The sequence routine beginning at CP runtime address `0x4c5a7c` (file `0x3bda7c`) saves its incoming object in `sb` and loads `fp = [sb]` at file `0x3bda8c`. This becomes the context pointer relevant to both LUT operations:

| Evidence | File offset | What the instructions establish |
|---|---|---|
| Parameter builder receives this context | `0x3bde30`–`0x3bde36` | `mov r0,fp; bl 0xb2c406` |
| Userdata argument is set | `0x3beed0` | `mov r2,fp` |
| Adapter pointer is constructed | `0x3bee86`, `0x3bee92` | `r5 = 0xb2b4ad` |
| Final callback pointer is constructed | `0x3bef6c`, `0x3bef80` | `r0 = 0xb2b4c3` |
| Callback record is registered | `0x3bef78`–`0x3bef8e` | Stores the adapter and callback in a five-word record, passes the record and context userdata to `0xb28c16` |

The script decodes the complete region from file `0x3bde30` through `0x3bef8e` and verifies there is **no direct definition of `fp`** in that interval. It separately verifies there is no definition of `r2` between `mov r2,fp` and the registration call. A syntactic control-flow path exists from after the builder call to registration. This assumes normal callee-saved-register behavior and calls returning; it is not proof of branch feasibility for a particular live input or that every invocation reaches registration.

The builder at runtime `0xb2c406` is the same routine previously shown to copy user-LUT state from parameter snapshot `+0x68` into this context's `+0x13d0` when selector `+0x13c0 == 0x0a`. Thus the identity of the context supplied to the builder and later registered as userdata is no longer inferred merely from matching offsets.

## Registration, list dispatch and consumer forwarding

The ordinary linked-list machinery explains how that pointer survives the deferred callback:

1. Registrar `0xb28c16` constructs a 32-byte record containing the supplied five-word callback object, userdata and ownership information. It selects list head runtime **`0x1a69b34`** and calls `0xb294e2`.
2. `0xb294e2` allocates a node, copies the 32 bytes after the node's eight-byte links, and inserts it into the selected list. The context userdata is stored at node **`+0x1c`**. The adapter and final callback are at node `+0x8` and `+0x10`.
3. Dispatcher file **`0x3b750e`–`0x3b755a`** traverses that same list. It loads the callback object, loads userdata from node `+0x1c`, and invokes the adapter using the object's member-function representation.
4. Adapter **`0xb2b4ac`** loads the stored final callback from its object `+0x8`, moves userdata into `r0`, and branches to the callback.
5. Callback **`0xb2b4c2`** saves that context pointer in `r4`, constructs **`r1 = r4 + 0x13b8`**, loads the resource argument from context `+0x13b4`, and calls consumer **`0x4e876c`**.

```text
sequence context C
  -> builder(C) writes the LUT record
  -> register(adapter, callback, userdata=C)
  -> linked-list node stores C
  -> dispatcher passes node userdata to adapter
  -> adapter calls callback(C)
  -> callback calls MLUT consumer(resource, C + 0x13b8)
```

The host test executes the actual callback-setup fragment, registrar, list-copy/insertion routine, dispatcher, adapter and final wrapper. It supplies a synthetic context and a null owner/refcount fixture, stubs allocation and the final imaging-related calls, then checks that the final consumer receives the exact synthetic context plus `0x13b8`, with the resource value read from its `+0x13b4` field. No full sequence or image-processing routine is executed in this test.

This closes a specific static callback-lifecycle edge, including the producer's context identity. It does not establish buffer validity until callback execution, cancellation ordering, concurrent mutations, ownership across a real frame, or whether this particular sequence is selected for a saved still photograph.

## Resource lookup: slot ownership and seven MLUT indices

The lower-level resource state is more concrete than the earlier generic description of a handle lookup suggested.

Runtime function **`0xbb42da`** scans seven two-word records at runtime **`0xe9d588`**. Their values are exactly `(0,0)` through `(6,6)`. It therefore performs a checked identity lookup on the MLUT IP index, returning failure for values absent from the table. This function does not itself translate a camera mode into an IP index.

The meaningful selection happens in **`0x54d788`**. It compares an owner token and direction-derived type against four eight-byte records beginning at runtime **`0x1a7e578`**, builds a four-bit ownership bitmap, and returns these indices:

| Matching ownership slots | Bitmap | MLUT IP index |
|---|---:|---:|
| Slot 0 | `0x1` | 0 |
| Slot 1 | `0x2` | 1 |
| Slot 2 | `0x4` | 2 |
| Slot 3 | `0x8` | 3 |
| All four slots | `0xf` | 4 |
| Slots 0 and 1 | `0x3` | 5 |
| Slots 2 and 3 | `0xc` | 6 |

Other bitmaps return error code 5 without updating the output index. The host tests cover all 16 masks for both direction selectors 0 and 1, with synchronization helpers stubbed. Selectors 0/1 correspond to stored type values 1/2 in this routine; **the semantic meaning of those directions is not recovered**.

Allocation code around runtime `0x54d2d4` writes ownership records for single-slot and combined-slot selections and builds an owner token using the selected index in its upper halfword. These instructions corroborate that the bitmap is active resource bookkeeping. They do not name any slot as still, movie, display or playback, nor establish how pixels are divided among combined slots.

The seven numeric indices feed the seven MLUT register-base values already recovered in [CP architecture](CP_ARCHITECTURE.md). Multiple slot combinations alone do not prove multi-core image processing, a wide-frame mode or a particular maximum resolution.

## Dimension contract: 7008 × 4672 fits, processing remains unproven

The dimension wrapper **`0xb78c1c`** first requires IP index below 7. It tail-calls **`0xb7cd46`**, which rejects either unsigned dimension when shifting it right by 15 produces a nonzero result. The accepted software range is therefore **0–32767 for each dimension**. Zero is accepted by this layer; its meaning or usability in hardware is not established.

On acceptance, the call chain reaches `0xb829f2`, then the two setters at `0xb7823e` and `0xb7826a`. They use `BFI` to write the low **15 bits** into the selected MLUT block's registers at **base `+0x50` and `+0x54`**, preserve other bits, read back the fields, and issue `DSB`.

The host tests execute those original validation and setter instructions for each valid IP index plus invalid index 7. The supplied dimensions include:

- 7008 × 4672 and its transposition: accepted and written to host-memory register stand-ins.
- 32767 × 32767: accepted by this interface.
- 32768 × 1 and 1 × 32768: rejected with `-13`, leaving the stand-in registers unchanged.
- 0 × 0: accepted by this interface.

These results rule out a **15-bit field-width or this wrapper's validation limit** as the reason a 7008 × 4672 frame could not be configured. They do **not** prove supported hardware dimensions, clocking, line-buffer capacity, pixel format, throughput, DMA compatibility or that a full-size saved still reaches the stage. A register field accommodating a value is a much narrower result than successful image processing.

## What remains before a full-resolution still claim

The inspected sequence contains the diagnostic name `SeqVD_CAM_1FrameFlow`. This is a search lead, not a mode classification: neither “VD” nor “Frame” proves movie or saved-still routing. The existing JPEG/HEIF recording and display paths must remain distinct until their actual buffers, formats and consumers are joined.

The next decisive evidence would be a source-bound route from a full-resolution still development buffer, through an allocated slot set and a configured MLUT stage, to the encoder buffer used for the saved photograph. That route must also establish:

- The meaning and assignment of the owner token supplied from context `+0x13b4`, including its relationship to still capture mode.
- Width/height values sourced from a still capture rather than manually supplied to an offline validation test.
- Pixel encoding and color-processing stage at the MLUT input and output.
- LUT data ownership and synchronization until hardware consumption and frame completion.
- Effects on the full saved image, separate from preview, thumbnails or metadata.

Until that route is recovered, the supported conclusion is: **we now understand how one parameter context reaches an MLUT callback and how resource groups and dimensions are encoded, but full-resolution still-image processing through MLUT remains unproven.**
