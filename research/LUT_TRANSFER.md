# Native LUT staging and interchip transfer

The native file is copied into one AP staging resource, transferred to a second resource, and that **destination** resource ID is returned by LUT selection. This narrows the gap between native file bytes and CP consumption; it does not yet prove that a particular live CP lookup resolves the exact same allocation.

[trace_lut_transfer.py](trace_lut_transfer.py) and [lut-transfer.json](evidence/lut-transfer.json) retain nine instruction regions from the official A7C II 2.01 AP, interchip library and kernel module, plus bounded host-only checks. All input identities, original instruction bytes, file locations, kernel relocation annotations and imported call names are retained. Kernel addresses below are **`.text` section offsets**, not runtime addresses. AP/library addresses are ELF-relative.

No camera, USB endpoint, `/dev/mem`, system call, cache controller or DMA engine was accessed. Emulated map, transfer, cache and address-translation operations are explicit stand-ins. Native files and deployment assets are unchanged.

## Two resources, not one shared pointer

The original `appFw.so` helper `0x30210c8` uses two relocated bookkeeping objects:

| Field | Staging object, GOT `0x5b70df8` | Destination object, GOT `0x5bbdeb8` |
|---|---|---|
| `+4` | Source resource ID | Destination resource ID |
| `+8` | Address used in the AP map request | Not read by this helper |
| `+0x10` | Capacity checked against native length | Not read by this helper |

It checks the descriptor length, maps the source address, and copies exactly that length from the descriptor's first byte, with **no header skip**. It then unmaps and calls `0x34cb5e8` → `0x3514f54` with five 32-bit words:

```text
source_resource_ID, 0, destination_resource_ID, 0, actual_file_length
```

The two zero words are source and destination logical offsets in the constructed transfer request. Two further original-instruction tests run the selection wrapper and builder with per-selection flags 0/1, confirming that the destination ID reaches both registration inputs and that the flagged primary input carries offsets/lengths `(0,48000,48000,124)`. These tests intercept the message wrappers; the separately retained wire constructor establishes their field packing. `send_lut`, at `0x3428614–0x3428628`, subsequently returns **destination object `+4`** and the native descriptor length. Thus equating AP's mapped staging address with CP's resource base without following the transfer is unjustified.

Five original-instruction helper tests use distinct synthetic resource IDs and addresses. Lengths 48,000, 48,128 and 49,152 copy every byte, preserve the sentinel after the requested length and issue exactly the request above. Length zero and length 49,153 with capacity 49,152 fail before mapping or transfer. The map and transfer wrappers are intercepted, so these tests establish the AP helper's behavior rather than a completed cross-chip transfer.

## Submission and callback boundary

`0x3514f54` builds this request for the actual imported `interchip_transfer_async`:

| Offset | Value |
|---:|---|
| `0x00` | Pointer to source ID/offset pair |
| `0x08` | Pointer to destination ID/offset pair |
| `0x10` | Transfer byte count |
| `0x14` | Constant 1 |
| `0x18` | Callback `0x3514c88` |
| `0x20` | Callback user-data pointer |

The wrapper prepares a future/promise association before submission and takes a future retrieval path after successful submission. This differs from the separate LUT-registration command's null-response-buffer behavior. The final resource lookup, all callback paths, cancellation and memory lifetime have not been emulated here; the transfer's function name alone must not be used as a completion guarantee.

The selected provider `libInfraInterchipDatatrans.so` exports `interchip_transfer_async` at `0x990`. Its original body allocates a 48-byte message, copies 40 bytes of the caller's request, calls `osal_snd_sync_msg` for event **`0x02820746`**, reads status at message `+0x28`, then frees the message. That synchronous message exchange does not establish that its underlying DMA has finished; the caller's request still carries a callback.

## Kernel descriptors and a bounded counterexample

The official `icdt.ko` export `interchip_transfer_async`, `.text+0xf18`, unpacks the request and calls `.text+0xa50`. This setup distinguishes resources using ID bit 15, obtains segment records through `.text+0x2c8`, installs the supplied offsets, and calls the scatter/gather builder `.text+0x5d0` before eventual `pcidma_transfer_async` submission. `.text+0x2c8` asks the selected AP/CP resource service for a `0x802a` response and copies the returned addresses and segment lengths into its local descriptors. **No DMA submission is executed by our harness.**

The local descriptor consists of a segment count at `+0`, logical offset at `+4`, then 16-byte entries containing a 64-bit address and a 32-bit extent. The builder emits 24-byte DMA records with control, extent, source address and destination address, followed by a link/termination record. Tests use addresses above 4 GiB to check both address halves.

Eight original-instruction builder characterizations cover both direction values and both LUT file lengths:

- Four single-segment, zero-offset cases emit exactly 48,000 or 48,128 bytes. Every source and destination byte address agrees with an independently expanded logical allocation.
- Four deliberately fragmented cases use source extents `[20000,20000,20000]` at offset 127 and destination extents `[15000,20000,20000]` at offset 61. They preserve the entire requested prefix but emit **54,939 bytes**: an excess of **6,939** for a 48,000-byte request or **6,811** for 48,128.

The instruction-level explanation is that `w19` retains the total requested size, while the comparisons at `.text+0x700–0x714` clamp segment extents to that total rather than the remaining byte count. Cumulative size controls loop termination after a descriptor has been emitted. The retained JSON records every resulting DMA entry, not just a pass/fail summary. “All checks passed” means these expected characterizations, including the counterexamples, reproduced; it does **not** mean every transfer emitted the requested length.

This is a reproducible software-layer counterexample for synthetic segment layouts. It is **not a demonstrated overrun on the camera**, nor evidence that normal LUT resources have these allocations. Both tested allocation totals cover all emitted bytes. Actual resource segmentation, the PCI DMA consumer contract and any upstream allocation constraints remain unknown. A future custom caller should not assume arbitrary fragmented resources are equivalent to one contiguous LUT buffer.

## What remains unresolved

The AP producer, staging copy, two-resource request, kernel segment translation and CP LUT registration are now separately bounded. The missing end-to-end edge is the exact selected destination resource's registration/lifetime and CP address resolution. No guessed four-byte adjustment is warranted. This transfer research also does not connect MLUT's pixel input/output to a saved JPEG or HEIF frame.

Reproduce with `.venv-research/bin/python research/trace_lut_transfer.py`. The 15 characterizations compare 824,024 source/destination addresses in the eight kernel cases, with all external operations intercepted.

The fragmented descriptor schema, data/link record distinction and emitted byte totals were independently reviewed and reproduced in a separate analysis scope.
