# AP still-recording boundary and the MLUT question

The additional REA/Ghidra pass follows `mpr::RecordStill`, its implementation, parameter storage and media-object adapters in the original A7C II 2.01 `libmpr.so`. It identifies recording queues and deferred object operations, but **does not connect a full-resolution capture buffer to MLUT**. The name `RecordStill` is not itself a pre-encode pixel interface.

No camera connection was opened and no camera was accessed. No firmware code ran on the camera. REA performed native static analysis; this pass did not emulate these media operations or invoke any virtual method.

[trace_still_recording.py](trace_still_recording.py) reproduces eight complete dynamic-symbol instruction ranges, totaling **4,627 AArch64 instructions**, and verifies ten linked REA records. [still-recording-boundary.json](evidence/still-recording-boundary.json) retains source hashes, instructions, direct branch targets and the REA identities. Input SHA-256 is `4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9`. Addresses below are ELF-relative addresses; Ghidra adds `0x100000` for this library.

## Recording state, objects and deferred work

| Function | ELF address | Observed behavior |
|---|---|---|
| `SetStillRecParam` | `0xf3a8c` | Locks the integration mutex and delegates to the internal parameter setter |
| `SetStillRecParamInternal` | `0xf389c` | Copies `0x2a8` bytes into recording state under a write lock, handles referenced strings and slot/queue state |
| `RecordStill` | `0xeb260` | Uses a deque entry, checks errors, selects a trimming/header branch or calls `_record_still`, then updates outputs and removes a queue entry |
| `stillrecord::_record_still` | `0xe85a0` | Copies supplied `JpgInfo`, `RawInfo` and `HifInfo` records; constructs media interfaces, manages record metadata and schedules header work |
| `stillrecord::_make_add_if_t` | `0xe5264` | Assembles the addition interface from supplied records, resource descriptions, metadata and recording parameters |
| `stillrecord::_make_cnv_if_t` | `0xe6864` | Separate conversion-interface builder; its complete instruction range is retained |
| `stillutil::creater` | `0xc4de0` | Selects three different constructors for type values 1, 2 and 4, with an assertion path otherwise |
| `IntegRecordStillInit` | `0xf23b4` | Initializes recording bookkeeping and creates header, write/transfer-sequencer and write-finished worker threads |

The inspected `_record_still` copies record extents of `0xc0`, `0x150` and `0xe0` bytes from the supplied JPEG/RAW/HEIF information pointers. Those are **descriptor sizes**, not full image-buffer sizes. Their names and copies do not provide a verified RGB pixel layout, safe C++ ABI, ownership contract or interception point before image encoding.

The retained [RecordStill decompilation](evidence/rea/libmpr-still/00-procedure_pseudo_code.json), Evidence `ev_9c37341350a99b5014848b5702bbf55f56660ee6538bc37f9cca2c7edf94c7c7`, corroborates the deque, mutex, error branch, delegated implementation and deferred header work. The [implementation record](evidence/rea/libmpr-still/01-procedure_pseudo_code.json), Evidence `ev_a1016b61e20627fc080b541b24b186fe205c8e6960745c94b964966ab3adada9`, explicitly warns that type propagation did not settle. Its large generated stack structures are therefore not treated as recovered source types.

## Virtual interfaces remain a separate boundary

The [addition adapter](evidence/rea/libmpr-still-sink/02-procedure_pseudo_code.json), Evidence `ev_3bb6d43c041745510f2a2e0c4f5502fa9c12757da49e8c30623270f8a6267487`, copies `0x870` bytes of interface state, preserves selected referenced records and strings, and invokes a vtable slot at `+0x68` with four selector values. The [conversion adapter](evidence/rea/libmpr-still-sink/03-procedure_pseudo_code.json), Evidence `ev_171a892e87007414a550a10c7571df6682d828d74fce31f795626631ee495012`, checks object state and invokes slot `+0x40` before changing that state.

These are REA pseudocode observations of local wrappers. This pass does not resolve the selected vtables and every transitive implementation. It would be incorrect to turn the absence of a direct MLUT call in these inspected bodies into proof that all still processing bypasses MLUT. Conversely, a virtual conversion call does not prove that it invokes MLUT or receives a sensor-development buffer.

The implementation also contains a product comparison against `0x1fa401` (2,073,601), selecting different auxiliary-record handling around the 2,073,600 boundary. The source instruction range is retained. That conditional is not an MLUT maximum-resolution check, and it must not be presented as a two-megapixel still-capture limit.

## Consequence for native LUT integration

The [native LUT producer](NATIVE_LUT_FORMAT.md) establishes a concrete ordinary payload format. The [CP lifecycle trace](STILL_MLUT_ROUTE.md) establishes separate parameter/callback and dimension-interface relationships. These findings do not yet join to this AP recording boundary through a proven full-resolution capture buffer.

The remaining decisive edge is a specific capture/development buffer with known dimensions, pixel encoding and ownership, routed through the selected MLUT resource and onward to the saved JPEG/HEIF encoder. The necessary evidence includes the selected sequence variant and its mode gate, the actual buffer and resource assignments, and completion/cancellation behavior. This AP pass supplies concrete endpoints and rules out treating a named recording API as a complete integration contract.

Reproduce the ordinary instruction evidence with:

```sh
.venv-research/bin/python research/trace_still_recording.py
```

The two REA job recipes, [libmpr-still](rea-jobs/libmpr-still.json) and [libmpr-still-sink](rea-jobs/libmpr-still-sink.json), reproduce their selected native queries using the [project-local toolchain](toolchains/README.md). All completed sessions have successful close receipts.
