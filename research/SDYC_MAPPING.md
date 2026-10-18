# CP SSYC metadata and segmented pixel backing

The native JPEG path uses **two descriptors**: an outer descriptor locates `SSYC` metadata, and an image entry inside that metadata supplies a second unit/handle/offset triple for the actual pixel buffer. The original CP parser successfully reads all four metadata fixtures emitted by the original AP builder. The backing-address resolver is also recovered: it translates a logical offset across segment lengths, rather than assuming one contiguous allocation.

This closes a metadata-format gap. **It still does not identify the live JPEG pixel handle with the MLUT ring allocation or prove that MLUT affects the saved still.** Cross-processor metadata compatibility, physical backing identity and live image ownership are separate questions.

Reproduce with `.venv-research/bin/python research/trace_sdyc_mapping.py`, after the parent's `research/trace_sdyc_metadata.py` has generated its fixtures. [sdyc-mapping.json](evidence/sdyc-mapping.json) retains **18 byte-bound CP regions and 87 bounded host cases**, the official CP image hash, verified AP binary hashes and hashes of every consumed AP metadata fixture. This is host-only emulation: no camera, USB, network, target process, allocation service, DMA, codec or pixel operation executes.

## Logical handle and offset become one backing address

Helper **`0xaecc5c`** receives a three-word descriptor. Its examined instructions use handle `+4` and logical byte offset `+8`. They do not consult the first word; that does not establish that higher layers ignore the unit type.

It calls `0xaecc7e`, which:

1. Copies the handle into a temporary `{handle, flags=0}` query input.
2. Calls **`0xaeccea`**, constructing request **`0x24`** for service **`0x9056015c`**. The request is `0x98` bytes; handle is at `+8`, sentinel `0xffffffff` at `+0xc`, generated sequence at `+0x14`, encoded flags at `+0x16`.
3. Extracts response **`0x8024`**, checks its command, sequence and translated status, and copies its segment description.
4. Issues release request **`0x25`** through `0xaed0ee`, expecting response **`0x8025`**.
5. On success, calls actual arithmetic helper **`0x45c214`** with the descriptor and copied table.

The transport call is `0x68c968(..., 0x61003c, envelope)`. The harness intercepts it with a synthetic response; it executes the original request construction, sequence generation, response extraction, validation, table copying and release caller.

| Structure | Instruction-established fields |
|---|---|
| Query response | u16 command `+0`; s16 status `+2`; u16 sequence `+6`; u16 segment count `+8` |
| Response segment `i` | u64 base `+0x18 + 16*i`; u32 length `+0x20 + 16*i` |
| Copied table | u16 count `+0`; u64 base `+8 + 16*i`; u32 length `+0x10 + 16*i` |

`0x45c214` scans **eight length slots**, subtracting each complete segment length until the remaining offset is strictly smaller than a segment length. It returns that segment's 64-bit base plus the remaining offset. It scans the fixed eight slots rather than using the copied count. The query routine zeros unused lengths before populating them. An offset equal to the total length is outside the allocation and returns zero after diagnostics.

Fifty-seven cases cover contiguous and fragmented layouts, zero-length slots, the eighth segment, exact boundaries and three values of the first descriptor word. For example, with synthetic segments `(A,100)`, `(B,50)`, offset 99 returns `A+99`, while offset 100 returns **`B`**, even when `B` is physically distant. Five fault cases cover transport failure, wrong response command, wrong sequence, negative query status and negative release status; the combined resolver returns zero in each fixture.

Sixteen direct-query cases also verify flag encoding. Input bits 0/1/2/3 map to request bits 0/2/1/4. The combined descriptor resolver supplies zero flags. Calling the query function alone does not issue the release; that is performed by its caller in the combined helper.

## A point is not a contiguous image extent

Capacity helper **`0xaeccc0`** uses the same query/release table and sums all eight lengths with 32-bit arithmetic. Two cases verify a normal sum and an intentionally extreme wraparound fixture. A summed logical capacity does **not** prove a physically contiguous extent starting at the returned address. `0x45c214` resolves one point and receives no requested pixel length. Separate callers may apply additional extent or format checks.

The release also needs care: `0xaecc5c` releases the queried handle before returning its physical scalar. That is observed control flow, not proof that an allocation is freed or that using the address later is safe. The service's ownership, pinning, cache and lifetime contract remain unresolved.

Response counts are trusted in the copy loops. The host fixtures test at most eight segments and do not establish the remote service's count guarantees. No malformed-service or on-camera failure is claimed here.

## The outer descriptor locates metadata

JPEG reader **`0x45b62c`** calls **`0xaec230`** to resolve the outer descriptor with the same service query and segment arithmetic. It then uses a file-backed physical-to-alias window table at runtime `0xcee7b0`. The first two entries include physical `0x500000000 → alias 0x80000000` over span `0x7e000000`, and physical `0x800000000 → alias 0` over span `0x80000000`. Two further windows cover `0xfe000000` and `0xfe800000`. These are embedded mappings; a subsequent helper `0x5feb94` is an explicit identity stand-in in the harness, so actual MMU/cache behavior is not emulated.

The reader checks the literal four-byte signature **`SSYC`**. Despite the broader SDYC naming in APIs and filenames, these are the actual serialized bytes. It then uses `0xaec380 → 0xaec314` to walk entries:

| Metadata/entry field | Use |
|---|---|
| Metadata `+0xc` | Entry count |
| Metadata `+0x10` | First entry offset, relative to metadata base |
| Entry `+0` | Literal `ETRY` signature |
| Entry `+4` | Entry type; the JPEG image reader seeks type 0 |
| Entry `+0xc` | Payload offset, relative to metadata base |
| Entry `+0x14` | Next entry offset, relative to metadata base |

After parsing, the outer handle is released through `0xaec284 → 0xaed0ee`. The outer metadata address is not automatically the pixel address.

## The image entry supplies the pixel descriptor

Decoder **`0xaec69e`** copies the selected image payload into the JPEG image record:

| Image-entry payload | Decoded record | Use in the following JPEG path |
|---|---|---|
| u32 `+0` | u32 `+4` | Nested unit-type word |
| u32 `+4` | u32 `+8` | Nested pixel handle |
| u32 `+0x18` | u32 `+0xc` | Nested pixel offset |
| u16 `+0x2e` | u16 `+0x18` | Image dimension 0 |
| u16 `+0x30` | u16 `+0x1a` | Image dimension 1 |

The full decoder's other byte/halfword transports are retained in the evidence. Wrapper `0xaefbbe` subsequently uses the decoded dimensions and format fields for a separate extent calculation. In the native JPEG initializer, the image record begins at object `+0x84`; therefore its nested triple is object **`+0x88/+0x8c/+0x90`**. Actual instructions at `0xaf1d9e–0xaf1db8` copy that triple to a temporary, call **`0xaecc5c`**, and store the returned 64-bit pixel backing address at object **`+0xd8`**.

Three manual fixtures execute the original outer lookup, signatures, entry walk and decoder, then resolve the decoded nested descriptor. The nested offset cases include a physical segment boundary, confirming that the outer and nested handles are independently queried and released.

## Original AP metadata cross-decodes with original CP instructions

The parent's [AP metadata evidence](evidence/sdyc-metadata.json) contains four `1024`-byte metadata blocks emitted by the original AP `MakeInputYcStdData` and linked dataflow library instructions. This mapper consumes those exact bytes without editing them, pins and asserts the final parent evidence/generator hashes, verifies the referenced AP firmware binary hashes and records each block's SHA-256. The four CP cross-decode cases are included in this mapper's 87 checks; the parent's four AP producer checks are separate executions.

All four pass the original CP `SSYC`/`ETRY` parser and image decoder. The nested unit type, handle, offset and dimensions match every AP-supplied marker, including `7008 × 4672`, `1920 × 1080` and `640 × 480`. The actual CP backing resolver then translates each decoded nested handle/offset using a synthetic segment response.

These are two different guarantees: serialized format compatibility is demonstrated across original AP and CP code; physical mapping is fixture-driven. The CP test remaps the unchanged AP metadata bytes into a synthetic outer allocation inside a recognized alias window. It does not assert that the AP fixture's supplied physical address is a real CP-accessible address, or that either fixture represents an allocation on the connected camera. The decoded dimensions remain supplied test values.

## Where the MLUT connection still stops

[MLUT_ENCODER_LINK.md](MLUT_ENCODER_LINK.md) traces a separate resource-backed ring: pool token/base/current → context `+0x1e0` → pixel-port record `+0x160`, with the same context owner used by the MLUT configuration and pixel path. The present pass now traces outer JPEG metadata descriptor → image-entry pixel descriptor → segmented pixel backing address → JPEG object `+0xd8`.

The remaining evidence must identify the **live nested pixel handle's segment table** with that ring's token/base/current allocation, including any offset, copy, resize, ownership transfer and synchronization. No instruction retained here assigns the ring's identity to the JPEG nested handle. A shared size, firmware format or successful metadata decode cannot supply that missing equality. Port direction, live still-mode selection and saved JPEG/HEIF ownership remain unresolved; no camera program was installed or tested.
