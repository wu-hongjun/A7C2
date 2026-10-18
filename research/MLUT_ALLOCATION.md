# CP pixel allocation modes and a derived handle consumer

The selected fallback has **two allocation paths**, so its ring token should not be treated as the JPEG pixel handle. One path creates a direct-address ring record with handle `0xffffffff`; the other asks DMM for a real handle and stores it beside the returned extents. A conditional branch then consumes that actual record, requests a shared handle and caches the returned handle. This is new handle provenance, but the cache is still not connected to the AP encoder or SSYC pixel-entry writer.

Reproduce with `.venv-research/bin/python research/trace_mlut_allocation.py`. The [source-bound evidence](evidence/mlut-allocation.json) pins the official CP image, retains **20 instruction regions and 38 bounded host cases**, and lists every external stub. `py_compile` and the full generator pass. All execution is on the host: no camera, USB, network, native DMM allocation, queue, target process, DMA, codec or pixel operation runs.

## The ring is a conditional allocation mode

The selected fallback loop at `0x4c8ad2` checks **`0xb2cbde(1)`** before filling each enabled unit's record. For argument mask `m`, the instructions calculate:

```text
((byte[0x1ac5694] & m)
 & (byte[0x1b1e1e5] | (byte[0x1a53487] & 0x0f))) == m
```

The examined call uses mask 1. Eight cases execute all combinations of the three relevant fixture bits. Live global values and their firmware setting names remain unknown.

When the predicate is true, the fallback calls the direct ring selector **`0x4d3728`**. When false, it calls **`0xb449a2`**, which calls **`0x510b04`**, identified by its source-bound `AllocNewRdv` diagnostics and its DMM request construction. The destination record for the primary unit is **context `C+0x1d0`** in both branches.

| Record field | Direct ring branch | DMM allocation branch |
|---|---|---|
| u32 `+0` | `0xffffffff` | Handle returned by DMM |
| u16 `+4` | One extent when address/stride are nonzero | Returned extent count, limited to four |
| u8 `+6` | Zero | One |
| u32 `+8` | Pool initialization token | Allocation selector/token |
| u64 `+0x10` | Pool cursor address | First returned extent base |
| u32 `+0x18` | Ring stride | First returned extent length |

Thus `C+0x1d0` can contain an actual handle, `C+0x1d8` a distinct selector/token, and `C+0x1e0` an address. This distinction matters because the prior pixel-builder analysis transports `C+0x1e0` to port `+0x160`, while the native JPEG path resolves a handle and logical offset. Identifying a token or an address alone does not identify that handle.

Seven cases execute the original fallback allocation branch for `0x43f`, `0x2d9`, `0x4c4` in both modes and `0x63d` in the DMM mode. They execute the real predicate, branch, record allocator and DMM request/reply parsing, substituting the remote service. Distinct fixture handles and addresses make accidental token/handle conflation detectable.

## Pool queries and handle allocation share an endpoint, not a key

Startup `0x4d3680` attempts pool initialization with token `0x43f`, then `0x2d9` on failure, and can initialize the alternate pool with `0x4c4`. Initializer **`0xb2cc16`** calls **`0x511cb4`** and **`0x511edc`**. This pass executes those original wrappers and their original message constructor/buffer-allocation wrapper, rather than replacing the whole query functions.

| Wrapper / request | Request key fields | Response fields used here |
|---|---|---|
| `0x511cb4`, `GetExtentAreaInfo`, command `0x1a` | Token at `+8` | Base u64 `+0x10`, capacity u32 `+0xc` |
| `0x511edc`, `GetUnitSize`, command `0x18` | Token at `+4` | Stride u32 `+0x10` |
| `0x510b04`, `AllocNewRdv`, command `0x26` | Selector at `+4`, other options/arguments retained | Handle u32 `+0xc`, count u16 `+8`, extents beginning `+0x18` |
| `0x510d7c`, `AllocShare`, command `0x21` | Selector/token `+4`, **source handle `+8`** | New handle `+0xc`, extent table |

All four send to **destination `0x61003c`**. The earlier SSYC backing resolver's request `0x24` uses that same destination. These wrappers use fixed sequence `0x55` and expect matching `0x801a`, `0x8018`, `0x8026` or `0x8021` replies; the SDF resolver uses generated sequences and checks `0x8024`. A common service endpoint establishes a protocol relationship, not an equality between the supplied token and a dynamically returned handle.

Three host pool-initialization cases verify the original `0x1a`/`0x18` requests and response-to-pool transport. Seventeen allocator cases cover three selector values, zero/one/two/four/five returned extents, wrong sequence and invalid returned handle. Zero extents fail the tested open-option path. A five-extent reply is limited to four in the destination record. These are synthetic service responses, not accepted camera allocations.

The record cleanup body `0xb44b96` further corroborates the distinction: it checks the handle sentinel and byte `+6` before calling the DMM free wrapper, then clears the record. The direct ring's sentinel does not carry a dynamically owned handle through this cleanup path.

## A continuous actual-handle writer → share consumer

The most useful new consumer is in a conditional branch of the **same selected fallback**:

1. At `0x4c8ab8`, when helper `0x4d111c` returns 1, the code writes special selector **`0x63d`** into primary context field `C+0x1c8` and records a local state flag.
2. The unit loop allocates its record through the mode-dependent paths above. In the normal DMM branch, command `0x26` supplies the handle written into **`C+0x1d0`**.
3. After the four-unit loop, `0x4c8b58` passes **that same `C+0x1d0` record** to **`0x73b086`**, when the local state flag is nonzero.
4. With its separate state equal to 2, `0x73b086` calls **`0xb44abc`**, which calls **`0x510d7c`**. The actual `0x21` request derives its source handle from record `+0` and its selector/token from record `+8`.
5. The returned shared handle is stored at global **`0x10f7e28`**, and the state changes to 3. The function also passes the shared handle, selector and extent length through bookkeeping helper `0xb231e0`.

Two host cases execute the **continuous original fragment `0x4c8a9c–0x4c8b60`**, including selector assignment, the four-unit loop, actual normal allocator, actual share wrapper and cache write. They do not manually copy a fabricated source handle between those calls. The DMM replies remain fixtures; helper `0x4d111c` is explicitly stubbed to return 1, and the mode globals select the normal allocation branch. State 0 is a control case that does not share; state 2 produces an outgoing `0x21` request whose source handle is exactly the previous `0x26` reply. A third, shorter case verifies rejection of an invalid source handle.

This establishes a real dataflow edge from the context's handle writer to a handle consumer. It does **not** establish that the remote DMM service gives the alias the same physical backing on the camera. That backing identity is fixture-provided here. Nor does it identify this conditional state machine as JPEG, HEIF or saved-photo processing.

The retained cache neighbors include `0x73af56`, which closes a valid cached handle and resets the sentinel, and `0x73afac`, which checks that sentinel before trying allocations with selectors `0x347` and then `0x63d`. Their existence narrows further tracing targets. It does not supply an AP export or encoder input assignment.

## What remains before the pixel identities can be equated

The prior [MLUT context analysis](MLUT_ENCODER_LINK.md) traces owner/context and port-address transport. [SSYC mapping](SDYC_MAPPING.md) traces the independently supplied JPEG metadata descriptor to its nested pixel handle/offset and segmented backing address. This pass supplies the missing distinction between a direct ring record and a normal DMM handle-bearing record, plus one concrete derived-handle consumer.

The still-unrecovered edge is an actual assignment or resource alias chain from **`C+0x1d0` or cached handle `0x10f7e28`** to the AP av-cam encoder's handle state or the nested SSYC pixel descriptor. No retained instruction writes those AP destinations. Live allocation mode, share-state conditions, extent ownership, port direction, synchronization and saved-frame routing remain unknown. A matching synthetic reply, shared service endpoint or matching dimensions cannot replace that source-bound edge. No LUT still-photo capability or on-camera success is claimed.
