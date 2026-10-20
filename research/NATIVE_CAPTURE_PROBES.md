# Native request scopes, stream bounds and same-open position

October 20, 2026. Offline stock ILCE-7CM2 2.01 instructions, generated photos
and our own test processes only. No camera, USB/media enumeration, mounted card,
G HUB, live Sony process or installation was accessed. Network is used only for
the requested Git publication.

## Correction to the preceding copy-point interpretation

**FSYS `0x2f088` does not establish a readable CPU buffer.** The drive initializer
at `0x50b74` installs allocator `0x4dc34` and releaser `0x4d728`. In both successful
mode-1 and mode-2 allocation paths, the allocator returns **cursor `1`** and
stores a vector descriptor at drive `+0x66a0`. FSYS adds a loop offset to that
cursor. Even forwarding it to a function named `pf2_fwrite` does not make it
a pointer to encoded bytes.

The previous [encoded-write arithmetic tests](trace_encoded_write.py) used a
synthetic base and therefore did not prove CPU readability. Their conclusions
and [receipt](evidence/encoded-write.json) are corrected. The older publication
receipts remain historical; their hashes/counts do not describe this build.
No such copy hook was installed on a camera. Generic CPU-span ingress now
rejects address `1`; native ingress uses vector resolution and a safe reader.

## Original-source proof

[trace_native_probe.py](trace_native_probe.py) pins `libInfraFsys.so` and
`libInfraMediaCommon.so`. Its [receipt](evidence/native-probe-trace.json)
records **27 original-instruction cases**, mapping/position ranges and four
veneer cases. All addresses below are ELF virtual addresses before ASLR.
External filesystem, mapping, allocation and lock calls are synthetic stubs.

| Link | Source-bound observation |
| --- | --- |
| Request cookie | V2 worker `0x2f7e4` receives the original request in `x2`; `+0x500` holds its cookie. Translator completion fields remain unsuitable. |
| Stream lease | `0x4dc34` builds a 0x38-byte header and 16-byte vector entries; type at `+0`, count `+4`, original array `+8`, working array `+0x10`, cursor start/end `+0x18/+0x20`. The original array starts at descriptor `+0x38`. |
| Physical mode | Mode 2 converts the padding CPU address through `mcmn_buf_getphysaddr` before recording it. The new prototype admits only this traced physical-vector mode. Other modes are declined. |
| CPU windows | MediaCommon's mapper uses two bounded CPU→physical windows. Inverting a matched live window gives a candidate CPU alias, not permission to cast an arbitrary physical address. |
| Read permissions | MediaCommon `0x4c71c..0x4c734` requests `mmap64(0, length, 3, 1, fd, physical)`: readable/writable, shared. The mapping and driver themselves are not executed by the tests. |
| Same-open backing | `pf2_fwrite` passes the opaque backing as PFFILE argument `x3`; the writer retains it in `x25`. Its own `fgetpos` implementation reads the 64-bit field at backing `+0x38`. |
| Locked position point | Writer `0x7af08` reads that same field after its append adjustment. It is in the writer's volume critical section, entered at `0x7abe0`. This avoids calling another Sony getter or taking another Sony lock from the probe. |

For future registration from the pinned MediaCommon image, the first window's
CPU base, physical base and size are globals `0x79f28`, `0x79eb8`, `0x79ec0`;
the second uses `0x79f48`, `0x79f08`, `0x79f10`. These need that module's own
validated load bias. They are **not** runtime addresses. Its destructor clears
and unmaps the windows. Reading the globals is not an ownership reference or
proof that a capture's vectors belong to those windows. Unregistered capture/DMM
regions must remain unsupported.

## Implemented native probe core

[sony_native_probe.h](../camera_worker/sony_native_probe.h) and
[sony_native_probe.cc](../camera_worker/sony_native_probe.cc) add explicitly
invoked library entry points, not an auto-loaded camera module:

- `a7_probe_enter_request201` checks the live request cookie and `0x206` flags
  using the supplied bounded reader. A fixed, constant-initialized thread-local
  stack preserves nested scopes. Untracked nested requests enter a null scope
  so they cannot inherit an outer capture. Overflow or non-LIFO exit faults
  observation and invalidates affected contexts.
  An atomic claim excludes a second producer. Scope exit publishes completion
  with release ordering; controller acquisition is required before any snapshot
  access. Early close/completion notifications cannot bypass this quiescence gate.
- `capture_stream` copies a bounded mode-2 descriptor/vector list while its
  request owns the lease. It validates cursor start/end, contiguous descriptor
  layout, lengths and the project's 520-entry / 64-MiB limits. Metadata and
  vector identities are rechecked at every write probe.
- `a7_probe_locked_write201` consumes a saved register frame for **exactly**
  `load_bias + 0x7af08`. It matches the active FSYS slot's full generated handle,
  drive and backing; checks that `x25` is that backing; and reads native saved
  arguments at `x29 + 0xe0/e8/f0` (count, element size, cursor). Element size
  must be one. Position must equal the accumulated successful byte count on
  **every** write; the first must be zero.
- Vector extents resolve only through registered, non-overlapping physical/CPU
  windows. Unknown or crossing-window spans, arithmetic overflow, aliasing the
  output storage, changed vectors and cursor/length errors decline processing.
  Copying uses the collector's new resolver ingress; cursor `1` is never read.
- `a7_read_self201` uses Linux `process_vm_readv(getpid(), ...)` or macOS
  `mach_vm_read_overwrite(mach_task_self(), ...)`, with exact-length checks.
  Unmapped, inaccessible and partial reads fail without a raw dereference.
  There is no external PID, signal-handler recovery or unsafe fallback.
- `a7_probe_write_return201` records native write acceptance and returns the
  original status unchanged. Public probe callbacks preserve `errno`.
  `NativeProbe201::take` requires an unfaulted scope, complete bytes and the
  existing RecordStill/submission/truncate/actual-close/async gates.

The context and owned storage must be prepared before capture and retained
through consumption and thread quiescence. The safe reader proves that a read
succeeded in this process at that instant. It does **not** prove mapping lifetime,
DMA/cache visibility or absence of concurrent modification. Those still require
the native owner's synchronization and allocation registration.

The probe must execute **inside Sony's writing process**. Launching the static
JPEG/HIF worker as a separate process cannot read Sony memory through this
self-only API. The archive is not a Sony-compatible shared module; in-process
linkage/loader compatibility still needs a separately validated build and entry.

## ABI-preserving instruction probe candidate

[native_probe_veneer.py](../camera_worker/native_probe_veneer.py) produces a
372-byte candidate veneer and a four-byte nearby branch plan for `0x7af08`.
It verifies the exact displaced instruction, saves all 31 GPRs, all 32 SIMD
registers, SP, NZCV, FPCR and FPSR in an aligned 800-byte frame, invokes our
callback, restores state, executes the displaced load and resumes at the next
Sony instruction. It refuses wrong bytes, misalignment and out-of-range branches.
It does not allocate executable memory, read another process or patch anything.

Four Unicorn cases exercise two load biases and 64-bit positions. The callback
stub deliberately clobbers GPR/SIMD/control state; the veneer restores it, and
Sony's following store observes the original position. This tests ABI mechanics
with a synthetic callback, separately from host tests of the compiled C++ probe.
No claim is made about target instruction-cache maintenance, stack headroom,
runtime ASLR validation or a complete hooked process.

## Offline results

The compiled core passes **1,010 assertions** on the release host, under
ASan/UBSan and in ARM64 emulation. Separate host/sanitizer reader tests cover
valid mapped memory, a protected page, a read crossing into that page and an
unmapped region. A two-thread sanitizer test verifies nested/null-scope isolation.

The updated [JPEG/HIF replay](check_capture_handoff.py) now traverses the compiled
request scope and register-frame callback. Its data argument is cursor `1`, not
a readable source pointer. Generated mode-2 vectors resolve through a declared
physical→CPU mapping and the real kernel self-reader. The backing's position
field advances for each simulated native write. Destroying source bytes before
completion still leaves the correct owned snapshot. Eight reverse-order worker
jobs preserve frozen LUT/lens/unknown-aperture settings; HIF stays 10-bit HIF.

See [host](evidence/native-probe-photo-checks.json),
[sanitizers](evidence/native-probe-photo-sanitizers.json),
[ARM64](evidence/native-probe-arm64.json) and
[build/publication checks](evidence/native-probe-publication.json).
The Linux self-reader syscall is compiled but not executed by the ARM64 emulator;
its bounded fixture reader exercises the core. ASan/UBSan are not race detectors.

Rebuild with `camera_worker.build_sony_bridge` as before; add
`--host --sanitize` for the instrumented host variant. Run its executable with
`--self-read-tests` on the host. To reproduce source/veneer evidence:

```sh
.venv-research/bin/python research/trace_native_probe.py
.venv-research/bin/python research/trace_encoded_write.py
```

`research.check_capture_handoff` uses the same explicit local artifact/LUT
arguments documented in [the prior handoff](COMPLETED_PHOTO_HANDOFF.md).
Use fresh work directories. No command fetches dependencies or discovers devices.

## What still blocks automatic camera handoff

The same-open **position field and observation point** are now source-bound;
the stream cursor is now resolved without assuming it is readable memory.
Actual entry/exit/acquisition/close hook attachment, runtime image/bias checks,
nearby executable allocation, cache synchronization and safe detach/quiescence
are still missing. Only one instruction-point veneer is generated here; the C
scope/return APIs are not a complete installed hook set.

Real payload mappings need allocation-lifetime and DMA/cache-visibility proof.
Memory-read permissions, total stack/copy cost while Sony holds its volume lock,
capture coexistence, pre-exposure identity across drive modes, storage and idle
scheduling remain hardware gates. The code has no installer and is not ready
for unattended capture on the camera. Native capture must continue unchanged
when observation fails; no automatic retries or fallback pathname reads exist.
