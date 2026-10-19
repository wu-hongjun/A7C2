# Sony menu/button and per-file completion findings

October 18, 2026, original A7C II 2.01, offline only. The actual FSYS-to-MPR
callback now has a source-bound layout and error path. The AP registration is a
**single-owner delegate** and cannot safely be replaced with another subscriber.
The [native integration core](../camera_worker/SONY_INTEGRATION.md) implements
the verified decoding/selection portions and leaves the unresolved capture,
identity and installation boundaries explicit. No camera was accessed.

## Evidence and coordinate system

[trace_sony_integration.py](trace_sony_integration.py) pins the full SHA-256 of
`appFw.so`, `libmpr.so` and `libInfraFsys.so`. Its [receipt](evidence/sony-integration.json)
contains 18 disassembly ranges, eight exact enum/value/length/pointer records
corroborated by ELF relocations, and **34 isolated original-instruction tests**.
External calls in these tests are recorded synthetic stubs. No Sony I/O, whole
process, service, codec or UI was run.

All addresses below are **original ELF virtual addresses in the named library**.
REA/Ghidra's shared-library analysis addresses add `0x100000`. Neither coordinate
is a camera ASLR address. The exact spans and their hashes are retained; a
bounded neighborhood is not presented as a complete recovered function.

The retained [MPR completion](evidence/rea/sony-mpr-completion/),
[MPR sequencer](evidence/rea/sony-mpr-events/) and
[FSYS wrapper](evidence/rea/sony-fsys-completion/) REA results supply Ghidra pseudocode
for MPR's completion queue/workers and FSYS's exported wrapper/IPC callbacks.
An additional `appFw.so` decompilation timed out during provider analysis; FSYS
worker `0x2f7e4` was not recognized as a function by default Ghidra analysis.
Those attempts are **not successful decompilation evidence**. Their important
edges below instead use independently decoded original instructions and
bounded emulation. No function was invented to conceal those limitations.

The derived readable C views trim trailing whitespace with
`research/summarize_rea.py --normalize-readable-prefix sony-`; original REA JSON
and exported evidence bundles are retained unchanged.

## Actual per-file callback

| Library / ELF VA | Observed role |
| --- | --- |
| `libmpr 0xf183c` | Previously traced `_write_async_and_sidb_update`; submits `Fsys_WriteDirectFileAsyncV2` at `0xf20ec` |
| `libInfraFsys 0x32eac` | Enqueues 0x508-byte request, worker `0x2f7e4`, selected by drive character |
| `libInfraFsys 0x2f7e4` | Copies original callback/context; registers IO completion `0x2d1d8`, ends IO queue |
| `libInfraFsys 0x2d1d8` | Selects prior operation error over IO queue status; forwards `(context, status, response)` |
| `libmpr 0xe81ac` | `fsys_callbackfunc(void*, int, FSYSWRITEDIRECTFILERES const*)`; queues a 32-byte owned work record |
| `libmpr 0xf1494` | `_write_finished`; error/countdown/database work, finished-count update, then `free(context)` |

In the V2 request, original callback and context sit at `+0x4f8/+0x500`.
The async worker stores them in its completion record at `+0xd0/+0xd8`.
The operation error is `+0xe0`; the result starts at `+0xe4`. At `0x2d234` the
original function pointer receives `x0=context`, `w1=selected status`, and
`x2=record+0xe4`. The bounded response copies cover 0x4d8 bytes. Its path begins
at offset zero, with a 0x40f-byte storage bound. The path belongs to Sony's
filesystem namespace; it is not automatically a Linux path.

Four original-dispatch tests verify success, queue failure, earlier operation
failure and both failures. A nonzero earlier error wins over the queue result.
The result and its enclosing record are callback-lifetime data. MPR does not
retain the path: its queued record stores the opaque context, status, an
allocation metric from response `+0x440/+0x444/+0x42c`, and function pointer
`_write_finished`. Context later reaches `free`. A post-call observer must not
dereference it or retain the response pointer.

This gives a concrete observation point **before forwarding** the original
callback. It does not prove that its context is already a shutter/frame token,
that it denotes only a full original, or that it is valid across processes.
The core only copies the cookie value; it never dereferences a Sony context.

## Close, flush, error and database distinctions

The translated V2 operation calls write core `libInfraFsys 0x2ec58` through
`0x2f5ac`. The core logs its `p_req->filePath`, open flags, file size, completion
procedure, completion parameter and write-vector parameters. It writes,
truncates, then calls helper `0x2e964` at `0x2f15c`. The post-call selection at
`0x2f178` keeps a prior nonzero write/truncate error; otherwise it uses the
helper's return status. Four original-slice tests verify that selection.

Crucially, `0x2e964` is **conditional**. When file-info `+0x470` is zero, it
calls actual close helper `0x2c9e0`; otherwise it calls `PrFILE_fsync` or
`fsysDepTNtfs_fsync` according to the drive's filesystem selector. Close helper
`0x2c9e0` calls `PrFILE_fclose` or `fsysDepTNtfs_fclose` for an active file.
Eight emulated cases verify the close-versus-fsync selection and return status.
They do not execute a filesystem or establish which branch ordinary JPEG/HIF
requests select at runtime.

Therefore **async callback status zero is not yet sufficient proof of final
fclose**. The completion tracker requires a separately proven same-request
actual-close event. A hook at the common helper entry would falsely label fsync
as close. The close helper receives a filesystem handle/drive context, not the
MPR cookie directly; recovering that handle-to-request association is still
required. Do not use a shared “last filename” variable to guess it.

MPR's `_write_finished` also updates shared error state, countdowns, database
work and finished counts under its sequencer. File completion, database update,
whole-frame completion and Linux-readable durable visibility are distinct
events. We have not proven an app-process subscription path across all of them.

## AP completion registration and payload

| `appFw.so` ELF VA | Observed role |
| --- | --- |
| `0x356bd70` | `register_cb_still_write_finished` |
| `0x356be4c` | `unregister_cb_still_write_finished` |
| `0x9117898` | Singleton delegate pointer |
| `0x5ef43b0` | Registration/dispatch mutex object |
| `0x3573c2c` | Message conversion, virtual delegate dispatch and synchronous-reply branch |
| `0x3554d94` | Raw error conversion; table `0x4c8e470`, values above 11 map to 2 |
| `0x34a2df8` | Still sequence's error/media completion processing; not a full-file identifier |

Registration asserts if the slot is occupied. Unregistration asserts unless
the supplied pointer is the current owner. Four original-instruction tests
verify those checks with Sony's fatal path intercepted. Dispatch calls virtual
slot zero **while holding the mutex**. A replacement subscriber would break
the existing lifecycle, and a slow callback would stall that protected path.
The tiny exported `MprIf::SetCallbackFunc` at `libmpr 0x2c8aa8` simply returns
zero; it is not a usable event subscription API.

For incoming message `+4 == 1`, the AP dispatcher allocates 20 bytes and copies:

| Incoming offset | Converted payload offset | Observed meaning |
| --- | --- | --- |
| `+8` u32 | `+0` | Media value |
| `+0xc` u32 | `+4` | Opaque value; no frame semantics assigned |
| `+0x10` u32 | `+8` | Count |
| `+0x30` byte, if count > 0 | `+0xc` | First flag |
| `+0x50` byte, if count > 1 | `+0xd` | Second flag |
| `+0x54` u32 | `+0x10` | Converted error |

Nine original-dispatch tests cover counts 0/1/2 and raw errors 0/11/12, including
the original error converter. The core retains raw error values as telemetry;
it does not silently reinterpret them as errno. Its decoder declines counts
above two and non-0x58-byte input spans as unsupported. This conservative subset
does not establish a version-independent Sony ABI. **There is no pathname or
unique per-photo token in these copied fields.**

## Menu and button evidence

Relocation-backed UIBIZ records identify Up=2, Left=4, Down=6, Custom=14,
Menu=19, Enter=20, Right=22 and Custom2=223. The records retain each exact name,
value, NUL-inclusive string length, pointer and relocation. They do not prove
that physical C1/C2, USB remote keys or `ux::wgtsys::KeyEvent` use these numbers.
The native menu accepts this explicitly named enum domain and separately
normalized press/release/repeat events; it never guesses a physical binding.

`appFw 0x1794860`, identified by the `key_status::send_key(int,int,int)`
diagnostic, builds a 32-byte message and sends it to `0x5503e0`. Inputs are copied
at offsets `0/+8/+0x18`, with inhibition state at `+0x10`. Before sending, helper
`0x1795208` can **remap the key code**. Thus tapping this producer does not yet
establish the final UI-domain input/focus contract. The receiving dispatcher,
physical-button translation, edge values and consumption protocol remain open.

The histogram still controller at `0x22aea04` has a three-state enter-key
handler. State 0 sets a flag and value 50; states 1 and 2 conditionally dispatch
different helpers and clear that flag. Five original-state tests preserve this
behavior. The numeric states and timer units are not labeled as physical edges
without a traced producer. Overriding Enter globally could break Sony's own
long/short action handling. A custom overlay must own input focus explicitly
and pass Sony's shutter and unrelated events through.

The custom-key menu validator diagnostic also has a reference at `0x1b6e304`.
That is validation logic, not proof of an extension/plugin API. No custom
function ID, native widget vtable or menu item was patched or added here.

## What remains before a camera trial

1. Recover a Sony UI input/focus and rendering contract; prove physical custom
   button mapping and the normalized edge domain. Preserve the existing handler
   and pair consumption of each owned press/release.
2. Recover pre-exposure frame identity and exact full-original request mapping
   for JPEG/HIF, including burst, cancellation, dual-format and alternate images.
3. Bind the actual fclose path to that request; establish Linux-visible same-file
   identity and a read-only snapshot/digest without blocking a Sony callback or
   accepting a recycled pathname. FSYS's namespace/root mapping is still open.
4. Establish module/process ownership, a verified invocation/IPC path, ASLR and
   instruction signatures, original-function forwarding, reentrancy and safe
   unload/quiescence. This core supplies no inline hook/trampoline or installer.
5. Validate private writable POSIX storage, worker idle/power/thermal/RAM policy
   and eventual output discovery. DCF/database/playback registration is separate.

The [offline replay receipt](evidence/sony-bridge-checks.json) exercises native
LUT/lens selection through real launcher/worker processes and keeps 10-bit HIF
output. Its source identity and capture/close bindings are synthetic and explicit.
It advances the implementation without claiming those remaining firmware edges
are solved or authorizing a hardware test.
