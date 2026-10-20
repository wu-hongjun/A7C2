# Capture ID, recording object and real file-close linkage

**October 20 follow-up:** [owned encoded bytes and frozen fan-out](COMPLETED_PHOTO_HANDOFF.md)
adds a traced scratch-write copy boundary, explicit zero-position gate, native
collector and per-frame JPEG/HIF child jobs. Remaining-work descriptions below
are historical; real Sony adapter/storage observations remain unproved, while
the offline fan-out/reconstruction model is now implemented.

October 18, 2026. Offline ILCE-7CM2 2.01 analysis and native model tests.
No camera, USB, mounted card, live process or installation was accessed.

The returned **capture ID → selected MPR recording object → write cookie →
FSYS open handle → actual close → original async callback** now has concrete
observation points. The [native identity ledger](../camera_worker/sony_identity.h)
checks that chain on copied data. This resolves the metadata matching design;
it does **not** make an installed capture callback or authorize worker enqueue.
Two remaining proofs are the earliest arming point across capture modes and
ownership of the encoded bytes from that same open file.

## Evidence and limits

[trace_capture_identity.py](trace_capture_identity.py) pins all four original
images. Its [receipt](evidence/capture-identity.json) retains **31 bounded
instruction ranges and 45 original-instruction cases**, with synthetic external
calls. It supplements the earlier 34 callback/registration/menu instruction
cases, rather than replacing them. No filesystem, device, full Sony process or
real codec is executed by these instruction tests.

Addresses for `appFw.so`, `libmpr.so` and `libInfraFsys.so` below are original ELF
virtual addresses. REA/Ghidra adds `0x100000` for these libraries. AV addresses
are **raw file offsets**, with firmware base `0xffffffc001000000`; they are
never runtime ASLR addresses. Successful new REA sessions retain
[MPR allocation](evidence/rea/sony-capture-allocation/) and
[RecordStill](evidence/rea/sony-capture-record/) pseudocode, original responses
and exported bundles. Decompiler pseudocode supplements original instructions;
a struct name or inferred C type alone is not layout proof.

## Freeze before the image arrives

The normal data-entry handler `appFw 0x34a60d8` calls the MPR Capture wrapper at
`0x34a6228`. This allocates recording state after capture data arrives.
**It is too late to freeze the photographer's LUT/lens selection there.**
`mpr::Capture` is not an exposure-start notification despite its name.

The locked AP allocator `0x352d794` increments a 32-bit ID. At `0xfffffffe` it
wraps to `0x80000001`; it is neither a durable monotonic counter nor a timestamp.
For one observed sequence, `0x34a0378` obtains that ID, stores it in owner
`+0x14`, and places it in the request at `0x34a0388`, before wrapper
`0x34cdc08`. Adapter `0x3539910` puts the same word at `+4` of its 12-byte
`0x4040` release-key spinal-setting command. This is an **earlier arming
candidate**, not proof that this command alone represents every exposure.
Self-timer, focus-bracket and burst paths also allocate/use IDs and need their
own acceptance/cancellation audit. Never arm at the physical key edge and
assume the next file belongs to it.

There is also an explicit `set_capture_start` adapter at `0x3539f44`, reached
through wrapper `0x34d1588`. It emits a 16-byte `0x4038` packet with the ID at
`+4`. The native `decode_capture_start201`/`arm_start` checks that source-bound
packet shape instead of guessing a physical shutter event; original-prefix
emulation tests verify the serializer. Capture-request acceptance, actual
exposure timing and one-versus-many frames for all drive modes remain separate
proofs. An ID-bearing request is not by itself proof that an exposure occurred.

On the returned side:

| Stage | Exact identity observation |
| --- | --- |
| AV `0x1fb2ec` | Resource acquisition copies request `+8` into stage `+0x1c8` |
| AV `0x1f3438` | Stage `+0x1c8` copied to recording context `+0x7bc` |
| AV `0x2035bc` | Still-info producer copies context `+0x7bc` to message `+0xc` |
| AP `0x35332f0` | Copies wire `+0xc` into converted payload `+0x688` |
| AP `0x34acd48` | Retains that payload in `x22` across MPR RecordStill wrapper call `0x34aceb0` |
| AP `0x34acffc` | Reads the same retained payload's `+0x688` after that call |
| AP `0x34aca5c` | Sony's own pending deque explicitly matches state `1`, ID at entry `+4`, and flag at `+8` |

The ID appears in the AP still-info data consumed for JPEG/RAW/HIF recording,
not in the later 0x58-byte public MPR finished notification. The latter still
must not be used as a frame/path token. **The complete AP command → AV resource
acquisition ingress remains unjoined.** These words must not be declared the
same across that missing edge merely because their names look plausible.

A future trampoline must arm a durable preset snapshot before forwarding the
verified ID-bearing capture request, then match the returned ID explicitly. Prepare durable event tokens during
idle time and bind them to the observed ID with bounded memory operations;
never do disk I/O or wait on the shutter callback. If a suitable prepared token
is unavailable, pass the capture through without worker processing.
The offline ledger rejects ID reuse/wrap within its bounded session. If the
pre-request freeze cannot be proved for a mode, decline processing that frame;
Sony capture continues normally. Unknown shooting aperture remains unknown.

## Returned ID to the actual recording object

`mpr::Capture` (`libmpr 0xeb948`) allocates a zeroed `recordStillParam_t` of
`0xd08` bytes, reserves filenames and SIDB IDs, and pushes the pointer onto
MPR's FIFO. Error routes can push a **NULL sentinel**. Capturing this pointer
or counting Capture calls does not establish a shutter association.

Inside `mpr::RecordStill` (`0xeb260`), Sony locks its integration API mutex at
`0xeb2a8`. It obtains the actual front slot, loads its pointer at `0xeb2d4`,
and passes that exact pointer to `_check_error` at `0xeb2e0`. Normal processing
uses that selected object. This is the join point: observe it **inside the
same AP `0x34aceb0` dynamic call scope** that owns the returned ID. Do not run
an external parallel FIFO, infer correspondence by arrival order, or pop Sony's
queue yourself. Exclude NULL, rejected, dummy/cache and error routes.

The identity model's `select_record` requires the copied 0x68c-byte AP payload
and that exact selected pointer. Hook implementation must prove the dynamic
scope and mutex/ABI/lifetime handling; the model itself does not install one.
Do not save a Sony pointer for a later dereference: copy bounded fields while
their owners are alive. The cookie also has a shorter independent lifetime.

The full-original async submit is `libmpr 0xf20ec` in
`_write_async_and_sidb_update` (`0xf183c`). At this particular call site:

| Field | Observed value/layout |
| --- | --- |
| Record format groups | Two, stride `0x588`, base `+0x1f4` |
| Group `+0` | StillType `1=JPEG`, `2=RAW`, `4=HEIF` |
| Record `+0x20c + group*0x588` | Selected descriptor index; `0xffffffff` means absent |
| Record `+0x130 + media*4` | SIDB unique ID for one of four destinations |
| Record `+0x23c + group*0x588 + media*0x106` | Destination's original pathname |
| V2 request `+0x410` | `0x206` open flags on this submit path |
| V2 request `+0x4f8/+0x500` | Original callback / `malloc(0x1680)` cookie |
| Cookie `+0x15e0` | Media index |
| Cookie `+0x15f0` | Original pathname, 0x80-byte field, copied with limit 0x7f |
| Cookie `+0x1670/+0x1678` | Exact parent record pointer / format group |

**Do not read SIDB ID from cookie `+0x10` unconditionally.** Its first 0x15e0
bytes are copied only on the last-writer database branch and can otherwise be
uninitialized. The decoder obtains SIDB ID from the still-live parent record.
It bounds all three input spans and all group/media indices, matches the
record/cookie/request paths and pointers, requires JPEG or HEIF type plus the
correct suffix, and rejects absent descriptors and truncated paths. RAW cannot
be promoted by changing a filename. The ledger also observes both the native `RecordStill` return and the V2
submission return. `take` requires success from both, in addition to real close
and async completion. These returns may be observed after racing callbacks;
early callbacks cannot bypass the acceptance gates. Immediate submission
failure or an MPR rejection declines the write; do not wait for a callback that
will never arrive.

Multiple formats/destinations can share one frame token. Their jobs must use
separate event IDs derived from the already frozen frame selection, group and
media. Do not rerun the launcher's ordinary `--arm` against whatever selection
happens to be current when a late file completes. The controller's per-file job
fan-out from one frozen selection remains to be implemented.

## Request to the same open, including deferred close

The V2 FSYS worker (`0x2f7e4`) owns the original request. It copies callback and
cookie into its IO completion context before its synchronous translated write.
Keep a **nested per-thread request scope** from that worker through translator
`0x2f5ac`, core `0x2ec58`, and successful open `0x2eec0`. This is how an observer
can attribute the exact open to the original cookie.

A tempting shortcut is wrong: the translated core's `p_completeProc/Param`
(`+0x430/+0x438`) are zeroed, **not the original MPR cookie**. A test executes
the actual translation prefix to verify that. No global “last request,” last
filename or callback-order association is safe under concurrent writes.

The core saves its logical handle at frame `+0x80`, uses it for the write and
truncation, and passes that same saved value to close/flush helper `0x2e964` at
`0x2f15c`. `fsys_GetFileInfo` (`0x2e880`) validates the full handle against
active file-info `+0x20`, after selecting its drive from low bits. Open creates
it from drive letter and a wrapping **27-bit serial** at `0x2d17c`. It is an
opaque FSYS generation token, **not a Linux fd, inode, or persistent identity**.

The decoder uses a bounded 0x474-byte file-info prefix through the counter,
not a claim about the complete structure size. File-info fields copied before
close are flags `+0x10`, drive/owner `+0x18`,
full generated handle `+0x20`, opaque backing pointer `+0x40`, path at `+0x60`
and temporary lock counter `+0x470`. On the PrFILE path, the identity ledger binds the same-request
cookie to slot, handle, owner and backing pointer. PrFILE's pathname comes through
`fsys_CreatePathInfo` and must not be assumed byte-identical to the external
request path. Request/cookie/record/async-response paths are compared within
their common namespace; the successful open is joined by its actual nested
request scope and full generated handle, never by basename. It rejects
any slot/handle reuse in this bounded session, even after a previous job ends.
A continuous service needs a separately proven generation/reclamation scheme.

The counter at `+0x470` is **temporary lock activity**, not an ordinary open
reference count. At `0x30460` it increments before releasing the device lock
around `PrFILE_flock`; at `0x3049c` it decrements after reacquiring it.
Close helper `0x2e964` calls fsync while that counter is nonzero and sets flag
`0x200` to request closure. When the outstanding lock operation returns, the
path at `0x304b8..0x304c8` checks that flag and a zero counter and calls actual
close. Hence an async write completion can precede the deferred real close.

The source trace identifies entry/return of the **actual underlying** `PrFILE_fclose` or
`fsysDepTNtfs_fclose` calls made by helper `0x2c9e0`. Copy active slot/handle/
owner/backing before the call; match that exact invocation on return. Inactive
helper calls can return zero without any underlying close. The helper clears
and unlinks the slot even when underlying close fails, so inspecting its
fields after return is too late. The ledger implements `prfile_close_enter` for the traced PrFILE lease and
requires successful underlying close **and** successful original write callback, in either order. Fsync alone,
wrong generation, inactive close, errors, duplicates and cancellation never
release a candidate. A deferred close with no observation stays pending.

**TNtfs lease handling is excluded from the native ledger.** Its open wrapper
`0x53df8` stores the driver-returned handle through a global at `0x53e58`.
That does not establish a per-slot PrFILE-style backing pointer. `opened`
therefore requires the observed drive filesystem selector and declines `0x10`.
The original-instruction tests cover TNtfs close dispatch, not a working TNtfs
storage adapter. JPEG versus HIF is independent of filesystem backend.

## What is still needed for safe worker handoff

The model's `take` yields **metadata only**. It never calls the launcher or
opens the Sony pathname. The launcher currently imports from a POSIX source
using an earlier expected digest. Computing that digest from a late pathname
would defeat the identity check if Sony reused/replaced that path.

A working producer needs one of these separately verified mechanisms:

- An owned byte snapshot read through this exact FSYS open before its real
  close, after writes/truncation and with verified read permissions, locking,
  cache visibility and lifetime. Avoid blocking the callback; this requires
  an explicit scheduling/storage design. Open flags `0x206` are not proof that
  the existing handle can safely be read.
- A proven stable file object/version and Sony-to-POSIX mapping that can be
  reopened after close without a replacement race.
- A bounded copy of the complete encoded write stream with verified DMM/vector
  mappings, lengths, padding/truncation semantics and ownership. The existing
  write-vector addresses are not established userspace byte pointers.

None is proved here. PrFILE/pf2/TNtfs backing pointers must never be cast to
POSIX descriptors. Close success does not establish card flush durability,
Sony database registration, Sony playback compatibility, sRGB provenance or a
resource-safe time to run an HEVC encoder. Keep original callback forwarding
unchanged and run copying/worker dispatch away from callback threads.

## Offline implementation and tests

`IdentityLedger201` is controller-thread-only. It admits 32 frames and 32 files
over its lifetime, never reuses identities and faults on ambiguous matched
events. Untracked frames/requests do not create bindings. A future callback
adapter needs bounded owned observations, overflow poisoning, nesting support,
quiescence/detachment, and original-call forwarding; none is installed here.
The existing MPSC mailbox has not silently gained these larger Sony spans.

The rebuilt static ARM64 archive includes the checked write decoder entry
point. Host, ASan/UBSan and restricted ARM64 execution cover original bridge
behavior plus identity tests: all two format groups/four destination columns
for both JPEG and HIF, poisoned optional cookie prefix, reverse frame order,
preset generations, fsync-only pending state, late close, close/async errors,
malformed spans, wrong pointers/paths, inactive/reused handles, duplicate
requests/completions, cancellation and bounds. Native JPEG and 10-bit HIF
worker replay remains a **synthetic prebound-byte fixture**, not a live Sony
storage proof. See [current checks](evidence/sony-identity-checks.json).
