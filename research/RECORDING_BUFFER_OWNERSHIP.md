# AP recording-side sharing and original-reference cleanup

The HEIF capture implementation moves its output into the capture context, detaches local cleanup, publishes its descriptor and conditionally invalidates sender fields. The AP recording implementation supplies a corresponding **share-before-free** policy in its non-cache caller branch. This is more than copying a descriptor, but remains software control-flow evidence: actual DMM reference counts, concurrent writes and final shared-reference reclamation are not observed.

This report complements [av-cam HEIF ownership](HEIF_OWNERSHIP.md) and the established [notification-to-recording descriptor path](ENCODER_SAVE_LINK.md). No camera, USB, physical memory, real IPC, codec or filesystem writer is used.

## Sources and reproduction

Run `.venv-research/bin/python research/trace_recording_ownership.py`. Its [source-bound evidence](evidence/recording-ownership.json) pins official A7C II 2.01 `libmpr.so` SHA-256 `4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9`, six instruction regions / 425 instructions and 21 bounded host cases. Original ELF bodies and local PLT methods execute in Unicorn; DMM service helpers are intercepted before transport, and host `memset` substitutes for libc. No native firmware process or real resource service executes.

The separate native REA session retains seven newly decompiled functions, including [share enumeration](evidence/rea/readable/still-buffer-ownership-00-procedure_pseudo_code.c), [cleanup enumeration](evidence/rea/readable/still-buffer-ownership-01-procedure_pseudo_code.c), [deduplicated area cleanup](evidence/rea/readable/still-buffer-ownership-02-procedure_pseudo_code.c), [free-finalization information](evidence/rea/readable/still-buffer-ownership-04-procedure_pseudo_code.c), and [write-finished handling](evidence/rea/readable/still-buffer-ownership-05-procedure_pseudo_code.c). These are actual REA Evidence, separately identified from the host results. Decompiled missing arguments and inferred types were checked against original instructions.

All code addresses below are **original libmpr ELF VAs**, distinct from av-cam raw-image offsets. The REA shared-library view adds its recorded analysis bias; the generator maps ELF `PT_LOAD` ranges and retains original source offsets.

## The caller establishes share-before-free order

In `mpr::stillrecord::_record_still`, original instructions at **`0xe93bc..0xe9418`** check record mode at `+0xc`. Mode 3 skips the following pair at this point. Other modes call:

1. **`_shrare` at `0xe274c`**, through PLT `0x81f40`.
2. **`_free` at `0xe3c14`**, through PLT `0x81e20`.

The function name `_shrare` is the actual exported spelling. Six host cases execute this caller fragment plus the original called bodies for modes 0, 2 and 3, with both values of the flag argument. Non-cache fixtures issue share requests before original-reference free requests. Mode-3 fixtures skip both in this fragment; they are not evidence that the cache path never acquires or releases resources elsewhere.

## The encoded HEIF area participates

`_shrare` selects image areas according to format, mode and flags. Its HIF cases enumerate combinations including descriptor **`+0x90/+0xa0/+0xb0/+0xc0`**, with additional base/common/header areas on some branches. For nonzero handles it calls a helper whose assertion diagnostic explicitly names:

```text
app_dmm::share_and_open(area[j]->unitType, area[j]->memHandle, ...)
```

Wrapper `0x2b2258` transports unit type and handle and selects **command `0x21`, mode argument 1** before entering service helper `0x2b1d78`. The emulator intercepts that helper with success and records the arguments; no actual shared handle, mapping or reference count is produced.

The [av-cam HEIF trace](HEIF_OWNERSHIP.md) identifies encoded-output areas at HIF descriptor `+0x90/+0xb0/+0xa0` for numeric modes 0/1/2. Their descriptor bytes survive notification construction, AP conversion, queueing and `RecordStill` pointer construction. Here, the original AP share and free routines both select those same area offsets. This is a source-bound descriptor identity, rather than an equality invented by a resource-service fixture.

Four standalone sharing cases cover record modes 0/2 and both flag values. Four cleanup cases contain two HIF descriptors, deliberately repeating handles across the descriptors. The caller-order cases separately verify that the `+0x90/+0xa0/+0xb0` handles reach both service boundaries in the expected order.

## Cleanup frees distinct nonzero handles once per local list

`_free` enumerates HIF descriptor areas at base, `+0xa0`, `+0xb0`, `+0x90`, `+0xc0` and `+0xd0`, alongside format- and mode-specific paths for other information. It passes area pointers to **`_free_areaInfo` at `0xe3bb4`**.

That helper reads the area's first u32 handle. Zero is skipped. A previously seen nonzero numeric handle is skipped; a new one is appended to a local array and passed to wrapper **`0x2b24f0`**, which selects **command `0x23`, mode argument 2**, then enters service helper `0x2b2270`. The service is intercepted. Seven standalone sequence tests verify zero suppression, repeated handles and ordering. The `0xffffffff` fixture characterizes these selected instructions only; it is not an assertion that such a handle is valid input to a real DMM service.

Deduplicating equal numeric handles does not establish how different shared handles alias the same backing allocation. That is part of the external DMM contract. This helper also does not prove how many references remain after a free request.

## What still needs tracing

The native REA write-finished handler at `0xf1494` updates write state, posts further work, calls `SetFsysWriteFinished` and frees its writer-work object. It does **not** by itself establish when every image-area reference is reclaimed. `FreeStillInfoMemory`, cache release and finalization use additional mode/state gates and data structures, so they cannot be collapsed into “the buffer is freed when the file write finishes.”

The demonstrated local chain is:

```text
av-cam output allocation -> local cleanup detached -> descriptor publication
    -> AP RecordStill descriptor -> share/open request -> original-reference free
```

The caller-order fixtures demonstrate the last pair under documented synthetic inputs. They do not execute the IPC/DMM implementation, prove all error/cancellation paths, or establish the timing between successful message submission, AP receipt and asynchronous storage completion. Final receiver-side shared-reference reclamation remains a specific open ownership question. None of this establishes that the input pixels came from MLUT or that a saved image contains a LUT effect.
