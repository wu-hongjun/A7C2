# A7C II 2.01 CP architecture and LUT processing

Follow-up: [native packing](NATIVE_LUT_FORMAT.md), [stock MLUT layout](MLUT_LAYOUT.md), and [sequence/lifecycle tracing](STILL_MLUT_ROUTE.md) extend this earlier evidence set. The ordinary payload is now recovered; one callback context is structurally joined, with a selector gate preventing a live-frame claim for that variant. Full-resolution saved-still routing remains unproven.

This offline investigation recovers the CP image's startup layout, identifies its RTOS build clues, and follows two LUT interfaces into real instructions. The most useful result is a connection between the AP's `0x4155` user-LUT message and a CP payload handler, plus an independent MLUT configuration path that reaches memory-mapped register writes. **The full-resolution still route into MLUT remains unknown.** No camera was accessed, no code was executed on the camera, and no deployment asset or official firmware file was changed. A bounded host-only Unicorn pass emulates isolated CP metadata builders with external operations stubbed; it does not emulate the camera's OS, DMA, image processor or timing.

Reproduce with `.venv-research/bin/python research/map_cp_architecture.py`. The source-bound [evidence JSON](evidence/cp-architecture.json) includes 46 disassembled regions, instruction bytes and addresses, 64 MLUT diagnostic command records, 48 host-emulated wrapper cases, four host-emulated SRAM descriptor-builder cases, the startup descriptor, four sets of processor-mode stacks, and selected call/address references. The input is the official `cpapp-b.bin`, SHA-256 `f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428`, 18,090,880 bytes (`0x1140b80`). This extends the isolated execution work in [CP review](CP_REVIEW.md); it does not replace its limited scope with a claim of whole-camera validation.

REA is being used for native AP libraries in the companion investigation. The raw CP file has mixed ARM and Thumb code and no ELF header, so this analysis uses explicit Capstone ARM/Thumb decoding and a recovered address map. Automatically importing it as an ordinary native library would hide important assumptions.

## The CP is a separate ARM execution environment

The file begins with ARM instructions rather than a normal ELF or container header. At file offset `0`, `ldr r1,[pc,#4]; add r1,r1,#4; mov pc,r1` loads the literal `0x10800c` at file offset `0xc` and transfers to `0x108010`. This independently anchors the existing emulator's base address:

**For the examined file-backed CP regions, runtime address = file offset + `0x108000`.** These are link/load-model addresses recovered offline, not addresses observed in a running camera. No AP ELF-address assumptions are used here.

Startup accesses CP15 system registers, changes CPSR modes, manages caches and the MMU, uses `DSB`/`ISB`, and reads the low processor-ID bits through `MRC p15,0,...,c0,c0,5`. That establishes privileged ARM code with an ARMv7-style system-register interface. It does not identify an exact Cortex core or SoC model. Most examined imaging routines use Thumb-2; startup and several low-level routines use ARM state. Function-pointer bit 0 distinguishes these states in the recovered diagnostic table.

The application contains explicit build source paths under:

`BuildWorkSpace/TOPPERS-FMP/rtos/toppers_fmp/fmp_1.4.0/`

These include `kernel/task.c`, `mutex.c`, `interrupt.c`, `eventflag.c`, `messagebuf.c`, and `solid_extension/rwlock/rwlock.c`. It also contains `SOLID_MEM_MapWithAttribute`, `SOLID_SNAPSHOT_RegisterCallBack`, and other SOLID-prefixed diagnostics, plus task names `CPAPP_RT0` through `CPAPP_RT3` and `CPAPP_BE`. This is strong embedded evidence for a TOPPERS-FMP/SOLID-based CP environment. The embedded path is a build clue, not a patch-level attestation of every RTOS component. Linux root access on the AP does not by itself become a Linux shell on this CP.

C++ RTTI names such as `N5CpApp9AppConfigE`, `CpApp::AppManager` assertion text, and GNU-runtime diagnostic text indicate C++ components and linked runtime code. They do not establish an exact compiler or compiler version. A version-specific compiler fingerprint has not been recovered.

## Startup descriptor and memory map

At runtime `0x1083a4` onward, the boot code constructs source address `0x128000`, destination `0x108da0`, and length `0x17c`, then calls a copy routine. Thus the descriptor is at file offset `0x20000`; its first word is entry address `0x619ea0`. The entry pointer is later loaded from the copied descriptor and called with `BLX r0` at `0x10875c`. Its even address selects ARM state.

Four descriptor triples are consumed by the MMU mapping calls at runtime `0x1084bc`–`0x108520`. Each has a relative backing address, size and runtime virtual address. The relative address plus descriptor field `+0x16c` (`0x128000`) equals its runtime start. These roles are inferred from how the boot code uses the ranges and from their contents; there are no recovered ELF section names.

| Inferred role | File range, end exclusive | Runtime range, end exclusive | Bytes |
|---|---|---|---:|
| Main code, with embedded literals and diagnostic strings | `0x20000`–`0xb27490` | `0x128000`–`0xc2f490` | 11,564,176 |
| Read-only data candidate | `0xb28000`–`0xfbeb84` | `0xc30000`–`0x10c6b84` | 4,811,652 |
| Initialized data candidate | `0xfbf000`–`0x1100b80` | `0x10c7000`–`0x1208b80` | 1,317,760 |
| Zero-initialized working data candidate | Not fully file backed | `0x120a000`–`0x1b85f5c` | 9,944,924 |

The first `0x20000` file bytes include the ARM bootstrap and zero padding. There are small alignment gaps between the main ranges. The file has an additional **`0x40000` bytes of all-zero tail** after the initialized data; it does not contain the entire zero-initialized working range.

Descriptor fields also identify translation-table regions at `0x1b88000` (16 KiB) and `0x1b8c000` (32 KiB). The bootstrap generates page/section mappings with 4 KiB and 1 MiB granularity. This is a genuine mapping setup, not evidence that every numerical address in the image is executable code.

The entry routine at runtime `0x619ea0` selects one of four descriptor groups using the low two processor-ID bits. Each group contains six 8 KiB stack ranges for SVC, IRQ, FIQ, UND, ABT and SYS modes. CPU-index 0's ranges begin at `0x1b95000`, `0x1b98000`, `0x1b9b000`, `0x1b9e000`, `0x1ba1000` and `0x1ba4000`; their initial SP values are each start plus `0x2000`. All four groups are recorded in the evidence. The entry also enables the floating-point coprocessor and branches to distinct primary/secondary startup paths. This establishes a four-index startup design; static inspection alone does not measure which cores are online during a capture.

## User-LUT message reaches a concrete CP handler

The AP-side trace in [AP interfaces](AP_INTERFACES.md) constructs a 96-byte message with command ID `0x4155` at offset `+2`, including a shared resource field, offsets, lengths and a 63-byte name. This analysis finds the matching ID next to a Thumb handler pointer in CP initialized data:

| Item | File offset | Runtime address/value |
|---|---|---|
| Candidate dispatch row for command `0x4155` | `0xff4428` | `[0x4155, 0x35c54d, 1, 0, 0x8db6a1]` |
| Thumb handler, pointer bit removed | `0x25454c` | `0x35c54c` |
| `SET_USER_LUT_BINARY_DATA` diagnostic | `0xe73789` | `0xf7b789` |
| Address materialization for that diagnostic | `0x254668`, `0x254670` | `MOVW r0,#0xb789; MOVT r0,#0xf7` |

The table has adjacent 20-byte rows for commands `0x4152`, `0x4153`, `0x4154`, `0x4156`, and `0x4157`, with their own handler/callback pointers. Its exact dispatch semantics and the two unknown row fields remain to be named. The independent AP message producer makes `0x4155` substantially stronger evidence than a text-string association alone.

The handler's coherent instructions show:

1. It checks message `+0x8` and length `+0x14`. If either is zero it selects an invalid/cleared resource state.
2. It tracks message `+0xc` in a resource context at runtime `0x16596d0` and uses helpers at `0xb44b96` and `0xb44a2c` when that resource changes. Those helper contracts are not yet recovered; they should not be called by a custom worker on the strength of this trace.
3. From the context's resolved pair of words at `+0x10`, it adds the message's `+0x10` and `+0x18` offsets using `ADDS` plus `ADC`. This builds two **64-bit addresses** in global state at runtime `0x165a720`, fields `+0x68` and `+0x70`.
4. It stores lengths from message `+0x14` and `+0x1c` into global fields `+0x78` and `+0x7c`, copies at most 63 name bytes from message `+0x20` to global `+0x80`, and increments a counter at global `+0xc0`.
5. Under a debug flag it logs those addresses, lengths and counter through the format string `FW: SET_USER_LUT_BINARY_DATA addr:0x%016llx size:%d regAddr:0x%016llx regSize:%d cnt:%d`. It returns `1`.

This handler **registers payload state**. Its examined body does not immediately copy an entire CUBE grid into hardware. A subsequent deeper pass follows that state into a VFX-shaped record and independently recovers a consumer of the same record layout that submits MLUT data. A specific context instance passing from producer to consumer through the state machine is still an unresolved edge. The AP submission returning success is not proof that a still frame has passed through a LUT, and the two binary payloads' packing remains unresolved.

## Downstream state: snapshot, VFX parameters and MLUT loads

The state is part of a much larger `0xd64`-byte parameter structure. Looking only for direct loads from `0x165a720+0x68` misses its downstream consumers because they operate on a snapshot and derived context records.

| Step | Concrete instruction evidence | Consequence |
|---|---|---|
| Obtain current parameter address | Getter runtime `0x8dad4a`, file `0x7d2d4a` | Returns `0x165a720` |
| Take parameter snapshot | Runtime `0x8dad54`, file `0x7d2d54` | Copies `0xd64` bytes from `0x165a720` into its supplied destination, after checking a synchronization helper's result |
| Initialize processing snapshot | Runtime `0xb2e140`, file `0xa26140` | Copies those `0xd64` bytes to `0x172fe50` |
| Construct pointer context | Calls at file `0xa26668` and `0xa268e8`, constructor `0xb2ebce` | Stores the snapshot base into the first word of the context at `0x173efe8`; additional words point into the larger processing context |
| Build VFX MLUT parameters | Runtime `0xb2c406`, file `0xa24406` | When output field `+0x13c0` equals `0x0a`, copies **`0x60` bytes from parameter snapshot `+0x68` to output context `+0x13d0`** |
| Submit VFX look/LUT | Runtime `0x4e876c`, file `0x3e076c` | Independently consumes a record passed from its caller's context `+0x13b8`, with a layout matching the producer's record |

The snapshot initializer retains `r4 = 0x172fe50` through the context-bind call; the reproducible script checks that the only direct definitions of `r4` in that interval are its two constant-building instructions. This follows the normal callee-saved register ABI. It is static dataflow evidence rather than a live snapshot or a complete interprocedural correctness proof.

The `0x60`-byte copy is particularly useful because it transfers all the fields registered by command `0x4155`: both 64-bit addresses, both lengths, the name bytes and intervening state. Relative to a record beginning at context `+0x13b8`, the producer's copied fields line up with the independently observed consumer accesses as follows:

| VFX record field | Meaning established by the joined instructions |
|---|---|
| `+0x8` | Selector tested against `0x0a` for the supplied-payload route |
| `+0x18`–`+0x1f` | First 64-bit payload address |
| `+0x20`–`+0x27` | Second 64-bit register-payload address |
| `+0x28` | First payload length |
| `+0x2c` | Register-payload length |

This consumer has a diagnostic string `FW: call ddl_VfxSetMlut lookname=%d,%d, l3d=%x,%u, reg=%x,%u`, which corroborates the two payload roles. Its branch for selector `0x0a` calls one of two driver routines:

- `0x54e15c` receives the first payload when the register length is zero.
- `0x54e398` receives both payloads when the register length is nonzero.

Those routines translate 64-bit resource addresses through a four-entry mapping table into CP-accessible pointers, then pass pointer/size pairs to the **same lower-level MLUT entry points independently identified through the diagnostic command table**: `0xb799de` for 3D SRAM data, `0xb78c60` for look selection, and, in the dual-payload route, `0xb79ab4` for register data. The first two appear in both routes. These names describe corroborated roles, not recovered exported function symbols. The lower-level queue and transfer contract is recovered below; payload packing remains unresolved.

The retained caller neighborhoods at files `0xa234c2`–`0xa234e8`, `0xa2357c`–`0xa2359a` and `0xa239e8`–`0xa23a12` establish the consumer's input contract: each saves its incoming context pointer in `r4`, constructs `r1 = r4 + 0x13b8`, reads the accompanying argument from context `+0x13b4`, then calls `0x4e876c`. A separate retained wrapper at file `0xa2311e`–`0xa23138` passes its incoming context to the record builder. **These matching layout contracts do not yet prove the wrappers receive the same context instance during a particular processing transition.** Resolving that state-machine/lifecycle edge is required before describing the sequence as a continuous capture path.

```text
AP command 0x4155
  -> CP parameter global +0x68
  -> 0xd64-byte snapshot
  -> producer writes VFX-shaped record, selected-payload branch 0x0a
  ... matching layout; actual instance/lifecycle edge unresolved ...
consumer receives record at its context +0x13b8
  -> translate address and length pairs
  -> MLUT SRAM / look selection / optional register-data submission
```

This establishes considerably more than the presence of LUT strings: user-LUT registration feeds a parameter-record producer, while a layout-compatible VFX consumer feeds MLUT load routines. It **does not establish a continuous per-frame route or identify it as full-resolution still capture**. The hardware IP is obtained through resource/handle lookup helpers, including runtime `0x54d788` and `0xbb42da`; it is not a fixed index in these call sites. The meanings of those handles, their allocation during still versus movie/display work, and their relationship to the seven MLUT register bases remain open. No observed frame, camera mode or silicon port was inferred solely from a function name.

## MLUT: diagnostic table to hardware register access

A separate table at file `0x1031cac` contains 64 entries, each a command-key string, help string and function pointer. The first 43 are numbered consecutively; later entries include diagnostic keys 100–108, 999 and 200–210. The report retains each actual pointer and ISA. These are firmware-side diagnostic handlers discovered offline; their presence does not establish a currently accessible USB command or permission to invoke them.

For key `5`, help text is `ddl_mlut_set_mode_flow [IPTYPE][MLUT_MODE_FLOW][MLUT_MODE_FLOW_3DLUT]`. The row is at file `0x1031cdc`; its Thumb pointer is `0xb7a137`. The table pointer to the string is data, rather than an instruction reference, but following its handler provides a real call chain:

```text
diagnostic handler 0xb7a136
  parses three numeric arguments after checking argc == 4
  calls 0xb78b88
    checks IP index <= 6, flow <= 5, 3D mode < 2
    calls 0xb829d4
      calls 0xb781e6: update bits [2:0] of register base + 0x10
      tail-branches 0xb78212: update bit [16] of register base + 0x10
```

The two low-level setters load their base address from a seven-entry pointer table at runtime `0xe9515c` (file `0xd8d15c`), read the existing register, update only the selected field with `BFI`, write it, read back the selected field, and issue `DSB`. The base values are:

`0xf4009000`, `0xf4109000`, `0xf4209000`, `0xf4309000`, `0xf4409000`, `0xf4509000`, `0xf4609000`.

An adjacent helper at runtime `0xb77ff8` independently calculates `0xf4009000 + (IP index << 20)` for indices below 7. These are strong static signs of replicated memory-mapped MLUT blocks. **The physical port meanings, enum labels and connection to the full-resolution still path are not recovered.** No register was read or written on the camera.

The same diagnostic table identifies controls for YC-to-RGB/RGB-to-YC, three 1D LUT blocks, four matrix blocks, exposure-index gain, YR1D, ASC CDL, 3D LUT, SRAM selection, hardware dimensions, look names, cache flushing and DMA status. That establishes a configurable multi-stage color-processing subsystem. The list of block names does not establish their order in a selected pipeline.

## Below the MLUT payload loaders: exact lengths and banked transfers

A bounded follow-up below `0xb799de` and `0xb79ab4` recovers concrete native-payload constraints. These are **hardware-oriented binary payloads**, not evidence that the CP accepts CUBE text or a known RGB sample ordering.

| Wrapper | Required length | Word count stored in queued request | Operation selector |
|---|---:|---:|---:|
| SRAM data, runtime `0xb799de` | Exactly **48,000 bytes (`0xbb80`)** | 12,000 (`0x2ee0`) | 1 |
| Register data, runtime `0xb79ab4` | Exactly **124 bytes (`0x7c`)** | 31 (`0x1f`) | 3 |

Both wrappers reject IP indices above 6. Their examined argument validation does not check source-address alignment. The isolated host tests confirm that both aligned and one-byte-unaligned pointers pass this wrapper when the index and length are valid. This establishes only what this layer accepts; it does not establish that the DMA engine or silicon permits unaligned transfers. The wrappers return the 32-bit representation of `-13` for the tested invalid index/length cases.

Each wrapper constructs a `0x38`-byte request containing operation, IP index, callback pointer, payload pointer and word count, then calls runtime `0xb83282`. That routine allocates and copies **only the request metadata** before submitting it to queue-like ID `0x2ae085a`. It does not copy the 48,000-byte or 124-byte payload. A separate worker at runtime `0xb82c22` receives requests and calls dispatcher `0x53533c`; its six-entry jump table maps operation 1 to `0xb82d7c` and operation 3 to `0xb82e00`.

The SRAM worker calls descriptor builder `0xb8336c`. For the fixed 12,000-word request, it produces **48 descriptors arranged in 24 pairs**:

1. Write a one-word control value to the selected MLUT base plus **`0x408`**.
2. Transfer **500 words / 2,000 bytes** from the payload to the base plus **`0x3000`**.

The payload pointer advances 2,000 bytes per pair. The descriptors themselves have a `0x8c`-byte stride, are built in a global working area starting at runtime `0x262d0f0`, and refer to a control-word pointer table at runtime `0xe953fc`. The words select a bank/window configuration, but their precise channel/bank meanings are not recovered. The register worker separately builds a 31-word transfer to the selected MLUT base plus **`0x400`**.

**48,000 bytes and 24 × 2,000-byte transfers do not establish LUT dimensions, channel ordering, numerical precision or a conversion from CUBE.** They describe the native transfer contract and its subdivision. No interpretation based on a convenient factorization of that size is warranted.

The next routine, `0xb83062`, sets up a DMA-style descriptor path, stores the callback, submits the transfer and waits through a synchronization helper with immediate `0x3e8`. Its time unit is not established here. A completion callback at `0xb831f8` calls the saved callback and signals another synchronization helper. This provides concrete lifetime concerns: metadata submission, worker consumption, transfer completion and use by an image frame are distinct events. The source pointer must remain valid until the actual ownership/completion contract permits release. **The sender's return alone is not established as payload-consumption or frame-completion acknowledgement.** The exact OSAL blocking semantics, cache-coherency requirements, interruption behavior and payload lifetime remain unresolved.

The host-only Unicorn checks execute 48 wrapper combinations covering IP indices 0/6/7, zero/exact/adjacent lengths, and aligned/unaligned pointers. Memory clearing, diagnostic mode and queue submission are stubbed; the queued metadata is inspected instead of sent. Four additional cases execute the real SRAM descriptor builder for two IP indices and two pointer alignments, checking every descriptor's source, offset and word count. They do not exercise the worker queue, synchronization helpers, DMA or imaging hardware.

## CamCore's existing 3D table path is another distinct lead

`CC: Create3DLUT` at file `0xe5680d` is materialized by `MOVW/MOVT` at `0x135d7c`/`0x135d84`, within a Thumb routine beginning at file `0x135c2c` (runtime `0x23dc2c`). Three direct callers are at files `0x1354f6`, `0x13572a`, and `0x1357e8`. A companion selection routine begins at runtime `0x23d80c`.

The examined region reads pointers and flags from the CamCore context, chooses among buffers using a `0x654` stride and a selector bounded by 2, and logs the source/destination pair. Later instructions perform integer color adjustments, combine bounded fields into a packed 32-bit word, and store through a table-indexed destination. The observed loop tail uses 9-by-9 outer iteration counts and an inner counter stepping by 2 until 10: 405 packed words, or **1,620 bytes (`0x654`)**. This agrees with the buffer stride. Those counts describe the examined packed-table branch, not a general 17³/33³ CUBE layout or a complete characterization of every branch of the routine.

Another CamCore region at file `0x324db0`–`0x324e1e` either copies `0x654` bytes or passes a table pointer in a small parameter block to helper runtime `0x78aa20`. An `ADR` instruction at file `0x324df0` references the inline `CamCore 3D-Lut table Write Start` string; the subsequent completion log is loaded using `MOVW/MOVT` at file `0x324e12`/`0x324e16`. This gives a concrete table-submission lead without claiming that the helper's hardware contract has been recovered.

This code is relevant to the existing Creative Look port, but should not be conflated with MLUT's user-LUT SRAM format. The official CP contains both facilities. Demonstrating one does not establish a safe conversion into the other or prove where a still image encounters either stage.

## AP/CP messaging, buffers and imaging responsibilities

The CP image embeds UIPC sender/receiver allocation diagnostics, OSAL headers and synchronization vocabulary, DMM shared-memory wrapper diagnostics, dataflow pin names and task-based processing. Source-component paths additionally separate `AppFw-CP_191H`, `Avbb-InfraBB_191H`, TOPPERS-FMP and lower-level libraries. The concrete 96-byte AP/CP message and CP-side 64-bit address construction provide stronger evidence of the boundary than those names alone.

The firmware also contains distinct still/JPEG processing diagnostics (`ENCODE_JPEG_WITH_WORK`, `DECODE_JPEG_RSS`, still-processing task messages), CamCore color work, MLUT low-level controls, OSD/GUI composition, audio/video infrastructure, metadata handling and memory/DMA management. This breadth means CP is not merely a LUT coprocessor. Static strings establish component vocabulary; only the instruction traces above establish specific data/control relationships in this investigation.

For custom still-LUT integration, the remaining questions are now narrower:

- Resolve the instance/lifecycle edge between the `0x4155`-derived record producer and the VFX consumer, and recover the two binary payload formats, including precision, channel order and table-bank layout.
- Name the seven MLUT IP indices and recover each flow value's routing. Determine which routes serve display, movie, playback and full-resolution still encoding.
- Join a still capture's buffer ownership and color encoding to one of those routes. Do not infer routing from a menu flag or from display output.
- Recover resource-handle lifetime, DMA/cache synchronization, locking and cancellation. A pointer and length alone do not establish a callable interface.
- Use independent offline test vectors before designing any temporary camera experiment. No runtime invocation is warranted by this map alone.

The recovered structure makes native LUT integration more concrete, but it is still a research interface map. It does not yet support a camera installer, a live-preview claim, or a claim that arbitrary user LUTs can be baked into captured stills.
