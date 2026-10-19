# Sony menu and still-completion integration candidate

The native [observer core](sony_bridge.h) now provides LUT/lens menu state,
Sony 2.01 callback decoding, original-callback forwarding, a bounded completion
mailbox and per-capture matching. It builds for ARM64 Linux and is tested offline
with the existing JPEG/10-bit HIF launcher. **There is no installed or complete
Sony hook, Sony screen renderer, physical button binding, or automatic capture
handoff.** Nothing in this work accesses the camera or its mounted media.

The [firmware findings](../research/SONY_MENU_COMPLETION.md) identify the actual
per-file callback and explain why registering a new high-level callback would
break Sony's existing owner. The recovered addresses are research coordinates,
not runtime addresses to call or patch.

## Selector behavior

`PresetMenu` is a native view/controller model with two rows: look and manual
lens. Its indices refer to the launcher's existing immutable catalog, including
`none`. A future Sony screen adapter supplies labels from that catalog and draws
the selected row and draft values. No font/widget ABI is guessed here.

An explicitly verified custom-key binding opens the selector in still live view.
Up/down select the row; left/right cycle presets; Enter requests a commit; Menu
or the opening key cancels. Selection is a draft until a controller, **outside
the key callback**, successfully persists it through `photo-launcher --select`
and acknowledges the exact ticket. The controller must initialize the model
from the durable selection and generation, not reset it to defaults at startup.
The menu does not arm captures while a selection commit is pending.

Release/repeat handling retains ownership of consumed presses, passes unrelated
releases to Sony and returns the same disposition for duplicate event delivery
without repeating the action. Mode/modal/capture/shutdown notifications cancel
the overlay immediately through `update_context`; Sony's shutter continues
through its normal handler. The opening key defaults to **no installed binding**.
The constructor requires a verified UIBIZ `Custom` or `Custom2` code; these names
do not establish which physical A7C II button emits them.

The context permits AUTO still shooting. This is a custom postprocessing
selector: it does not unlock Sony's Creative Look menu in AUTO. Aperture remains
`unknown`; selecting a lens never invents a measured shooting aperture. A later
aperture control can use the launcher's existing validated aperture interface.

## Completion boundary and controller sequence

`decode_app_still201` translates the observed 0x58-byte asynchronous AP message
into bounded telemetry. Its success/error, media and count fields contain **no
filename or frame identity**, so this event never releases a photo job.

`CompletionMailbox::observe_write201` reads the actual FSYS response layout:
the first 0x40f bytes hold a NUL-terminated path; the supplied readable response
span must match the recovered 0x4d8-byte layout. It copies the opaque request
cookie, status and path without retaining any Sony pointer. Error responses do
not require reading the result. The adapter cannot validate arbitrary pointer
readability: a future trampoline must prove the correct buffer/lifetime first.

`forward_fsys201` observes **before** forwarding to the required original
callback, with the original arguments, even if observation fails. It neither
replaces a Sony global nor registers itself as the singleton delegate. The core
does no hashing, file I/O, allocation, process launch or wait inside a callback.
The fixed MPSC mailbox reserves one unique atomic ticket per observation and
publishes each owned slot separately. It uses no software mutex or application
retry loop and never recycles slots. Atomic operations are required to be
lock-free by compile-time checks; this is not a wait-free latency guarantee.
Concurrent producers cannot overwrite each other. Overflow or an invalid
successful response poisons the session; retain its work for review. If an
earlier producer has not published yet, the consumer returns and drains later.

The serialized controller must:

1. Persist/freeze the selection with the launcher's `--arm` **before exposure**,
   then call `CaptureTracker::arm` with the same unique event/token and generation.
   Skip worker arming during a pending selection transaction; never block Sony's
   shutter on a worker transaction.
2. Bind the exact full-original write request to that armed frame, opaque cookie,
   Sony path and explicitly mapped relative path. Bind before callbacks can run.
   JPEG/HIF use distinct tokens/jobs. RAW, thumbnails and alternate copies must
   not be asserted as full originals. Neither capture ordering nor filename
   similarity establishes this association.
3. Observe the actual successful **close**, with the independently established
   same-request cookie, and the successful FSYS async callback. A flush/fsync
   event must not be submitted as `observe_close`. Their arrival order may vary.
4. Drain on a controller thread. Matching status/path and both events permit one
   `take_candidate`. Errors, cancellation, conflicting duplicates and malformed
   events decline release. A cookie is never reused within this bounded session.
5. Establish a **same-file identity seal/snapshot** and digest through a trusted
   producer before `--enqueue-completed-srgb`. A digest computed from a possibly
   recycled pathname after the callback is insufficient. No such camera producer
   is established by this core. Apply JPEG sRGB provenance and HIF NCLX checks;
   HDR/HLG/PQ conversion remains outside the supported sRGB worker contract.
6. Run the ready job only when a validated capture-idle/power/resource policy
   permits it. Outputs stay separate from Sony's originals and media database.

There is deliberately no `take_candidate` → automatic enqueue connection. The
offline end-to-end test knows the synthetic source bytes/digests before replay;
that explicit fixture identity does not prove a camera filesystem identity API.

## Lifetime and bounds

The mailbox admits 128 owned observations total; the tracker admits 32 per-file tokens
for its entire lifetime and never recycles request-pointer values. This is a
bounded diagnostic session, not a continuous burst daemon. There is no timeout
that turns a missing callback into success. After a fault, do not silently reset
the queue or rearm entries. Restart requires a separately proven producer
quiescence/detachment and new session identity. Keep all callback objects alive
until no callback can still reference them; no hook removal ABI is supplied.

`PresetMenu` and `CaptureTracker` are controller-thread only. Only mailbox
observation methods are MPSC. The C entry points in `sony_bridge.cc` have **our**
ABI and must never be cast to Sony's C++ delegate/vtable types. The static archive
is a linkable core, not a `libJit.so` module, application-process injection or
boot service. It must not be loaded through the firmware installer.

## Offline build and acceptance

```sh
.venv-research/bin/python -m camera_worker.build_sony_bridge \
  --codecs research/build/hif/linux --output research/build/sony-bridge-linux
.venv-research/bin/python -m camera_worker.build_sony_bridge \
  --host --output research/build/sony-bridge-host
.venv-research/bin/python -m camera_worker.build_sony_bridge \
  --host --sanitize --output research/build/sony-bridge-asan
.venv-research/bin/python research/trace_sony_integration.py
```

These use existing local tools and sources only. `build_sony_bridge` checks the
archive member and links its exported entries into the self-test; Apple's `ar`
can otherwise silently omit a Linux ELF member. The repository retains a
rebuild receipt, linkable ARM64 archive and standalone self-test under `dist/`.

[Acceptance evidence](../research/evidence/sony-bridge-checks.json) includes
native release/ASan/UBSan checks, a four-producer callback-forwarding stress test,
execution of the actual ARM64 self-test under restricted Unicorn syscall models,
and native launcher/worker replay with distinct LUT/lens choices, reversed JPEG/
HIF completions, 10-bit HIF decode and stale-path rejection. The separate
[firmware evidence](../research/evidence/sony-integration.json) executes 34
original instruction cases and preserves eight relocation-backed menu records.
These tests establish the offline model's behavior, not camera compatibility of
the missing hooks, GUI, storage or scheduler.
