# A7C II 2.01 AP and imaging interfaces

Later work recovers the [ordinary native LUT producer/format](NATIVE_LUT_FORMAT.md), [MLUT register layout](MLUT_LAYOUT.md) and [additional AP recording boundary](STILL_RECORDING_BOUNDARY.md). The [CP callback/selector trace](STILL_MLUT_ROUTE.md) structurally joins one context but does not establish a selected saved-still path. The sections below describe the earlier interface evidence.

This pass maps executable dependencies and follows two different LUT paths through actual AArch64 instructions. It uses the original downloaded A7C II 2.01 firmware only. No camera was contacted, no target code was executed, and no extracted binary or deployment asset was changed.

These results are **ordinary offline static evidence**, separate from the REA Evidence records produced by the main investigation. Reproduce them with `.venv-research/bin/python research/map_ap_interfaces.py`. The complete inventory, branch sites, relocations, instruction bytes and recovered fields are in [ap-interfaces.json](evidence/ap-interfaces.json).

## Main result

There are at least two distinct AP LUT interfaces:

1. An AP-to-CP request with command **0x4155**, whose 96-byte message construction can now be followed from LUT-selection code into a shared request submission routine. This is the stronger lead for tracing imaging behavior.
2. `mpr::SetLutData`, whose observed implementation parses a CUBE file, constructs a **SonyLookControl KLV / SONYBCUB payload**, and distributes it to recording objects. Treating this API alone as a way to program the imaging hardware would be premature. Its recovered consumers support an embedded recording-data role.

Similarly, `stillplay::GetLutNrtInfo` reads non-real-time metadata and extracts a LUT identifier. Its existence does not establish that full-resolution captured still pixels pass through a user LUT.

## Binary identity and library map

The pinned inputs are:

| Binary | Bytes | SHA-256 |
|---|---:|---|
| `appFw.so` | 99,960,744 | `2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b` |
| `libmpr.so` | 3,528,576 | `4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9` |
| `libosal_uipc.so` | 43,752 | `09ebf5980237fc473077dc844763dcb00cba423f431646e3be7c0ca49ea70ff3` |

All addresses below are ELF virtual addresses for these exact files, without a runtime load bias. They are research locations, not safe callable runtime addresses.

The scan covered every ELF beneath the extracted `0700_part_image/dev` directory, including nested updater files: **619 ELF files, 573 distinct hashes, all AArch64**. There are 553 files under `nflasha15_unpacked`, 33 under `nflasha3_unpacked`, and 33 under `nflasha7_unpacked`. These counts describe extracted files, not simultaneously loaded processes or libraries.

The graph contains **2,714 DT_NEEDED edges**. Of these, 107 have no filename/SONAME candidate within the extraction; every unresolved edge names `libxtables.so.12`. This is a bounded packaging observation, not evidence that the camera fails to resolve them at runtime. Filename and SONAME matches are candidate providers; the actual loader search paths, namespaces and selected providers require separate loader/configuration evidence.

`appFw.so` exposes only one ordinary defined dynamic symbol and imports 946 symbols. In contrast, `libmpr.so` retains 3,534 defined dynamic symbols and imports 354. Consequently, AP functions generally need address-level tracing while the media library supplies useful C++ names.

The AP's 19 DT_NEEDED entries include network/security/UI libraries, pthreads, C++, libc and related runtime libraries. **It does not list `libmpr.so`.** Nevertheless, its `mpr::SetLutData`, `mpr::RecordStill` and `mpr::ReadStillImage` imports exactly match definitions in `libmpr.so`. No scanned ELF lists `appFw.so`, `libmpr.so`, `libstill.so`, or `libInfraAvbbc.so` in DT_NEEDED. These facts point to a loader/plugin boundary beyond ordinary ELF dependency edges; this pass does not claim how those objects are loaded.

Useful symbol-level partitions include the AP's model/sequence adapters; media-library `stillrecord`, `stillplay`, `MoviePlayer`, `Codec`, `DmmMemInfo`, `KlvConverter`, and SIDB database code; plus separate `libInfraAvbbc.so`, `libDmacController.so`, and `libInfraInterchipDatatrans.so` libraries. Names help organize the graph, but only the specifically traced calls below establish behavior.

## AP selection to CP command 0x4155

Assertion strings provide source-style function names linked to code by exact ADRP/ADD references, rather than proximity alone:

| Function | Address range, end exclusive | Naming evidence |
|---|---|---|
| `sequence_select_lut::send_lut` | `0x3428510–0x342863c` | Reference at `0x3428564/0x342856c` to function text at `0x4c4b318` |
| `sequence_select_lut::send_lut_procam` | `0x342863c–0x342888c` | References at `0x34286a4/0x34286ac` and two other assertion branches to `0x4c4b200` |

The first helper initializes its two output words to zero. If an existence flag is false, or the selected enum is at most two, it returns success without a new lookup. Selections 3 through 18 use index `selection - 3` with `0x3021228`, followed by `0x30210c8`, before filling the outputs. Invalid high values enter assertion handling.

The procam helper's retained signature includes `setting_shooting_mode_base_setting_t` and `setting_target_display_t`. Its instructions distinguish mode values **1–3** from other values, then target-display values **0/1**. Depending on that branch, selected enum ranges map through `selection - 3`, `SDR selection - 5`, or `HDR selection - 4`.

REA's completed default Ghidra analysis [decompiles the main selection helper](evidence/rea/appfw/01-procedure_pseudo_code.json), corroborating its index arithmetic and resource-helper calls. Its separate [procam query](evidence/rea/appfw/02-procedure_pseudo_code.json) failed because that database recognized no procedure at or containing the requested offset. This is a function-discovery limitation, not proof that the retained bytes are absent or the feature is disabled; the procam interpretation here remains instruction-level evidence.

Relocated 16-byte value/length/name records recover the enum meanings. The evidence retains each raw record, its pointer relocation, the pointed-to text, and a check that the stored length equals the text length including NUL:

| Record address | Value | Name |
|---|---:|---|
| `0x589afc0` | 0 | `SHOOTING_MODE_BASE_SETTING_CUSTOM` |
| `0x589afd0` | 1 | `SHOOTING_MODE_BASE_SETTING_FLEXIBLE_ISO` |
| `0x589afe0` | 2 | `SHOOTING_MODE_BASE_SETTING_CINE_EI_QUICK` |
| `0x589aff0` | 3 | `SHOOTING_MODE_BASE_SETTING_CINE_EI` |
| `0x58739d0` | 0 | `TARGET_DISPLAY_SDR_BT_709` |
| `0x58739e0` | 1 | `TARGET_DISPLAY_HDR_HLG` |
| `0x58739f0` | 2 | `TARGET_DISPLAY_INVALID` |

Consequently, the values 1–3 distinguish log-shooting base configurations; **this is not a still/movie capture selector**. Enum entries may exist for features shared with other products, so the Cine EI names alone do not establish that the A7C II UI enables those features. The helper's complete direct-call scan found no incoming B/BL site; indirect references or retained unused code remain possible.

Another table at `0x5873600` maps SDR selection values 0–5 to `S_CINETONE`, `STANDARD`, `STILL`, `ITU709`, `709TONE`, and `USER1`. In particular, **`SELECT_LUT_SDR_STILL` is preset enum value 2**, alongside other rendering options. Its name is not evidence that a user LUT can grade a captured still. This resolves an ambiguity left by the earlier string survey.

A concrete request chain is now visible:

```text
0x34289a8 wrapper
  0x34289d0 -> send_lut (0x3428510)
  0x34289e0 -> request input builder (0x342888c)
    0x3428938 -> category 10, operation 0x114 wrapper (0x34cfac8)
      0x34cfaf0 -> wire request constructor (0x3537eac)
        0x3537f10 -> payload helper (0x352c85c)
        0x3537f40 -> shared request routine (0x35358b8)
```

The constructor sets event ID `0x90030001`, submits 96 bytes, and writes zero to its result pointer after submission. The deeper submission trace below establishes that **this request does not take the CP-response future-wait branch**. Its zero result is not a CP processing result or a frame-completion acknowledgement.

The following is the exact observed packing, with `input` referring to the structure passed to `0x3537eac`:

| Message offset | Width | Observed source/value |
|---:|---:|---|
| `0x00` | 1 | `5` |
| `0x02` | 2 | `0x4155` |
| `0x04` | 4 | `input + 0x00` |
| `0x08` | 4 | `input + 0x04` |
| `0x0c` | 4 | `input + 0x08`, with zero converted to `0xffffffff` |
| `0x10` | 4 | `input + 0x0c` if enable byte at input `+0x5c` is nonzero; otherwise zero |
| `0x14` | 4 | `input + 0x10` |
| `0x18` | 4 | `input + 0x14` if enabled; otherwise zero |
| `0x1c` | 4 | `input + 0x18` if enabled; otherwise zero |
| `0x20` | 63 | Copied from input `+0x1c` regardless of enable; final message byte stays zero |

The same command appears in the bounded `0x253b114–0x253b1e0` registration/response neighborhood and a mapping at `0x24c50b4`. Matching `0x4155` against the independent CP handler table strengthens the cross-processor connection. The AP fields above still need semantic matching to CP parser fields; no arbitrary request has been sent.

The subsequent [CP investigation](CP_ARCHITECTURE.md#downstream-state-snapshot-vfx-parameters-and-mlut-loads) follows those fields further: the handler registers two payload addresses/lengths, and a `0xd64`-byte parameter snapshot supplies a VFX record builder. An independently traced consumer accepts the matching record layout; selector `0x0a` takes its supplied-payload branch into MLUT SRAM loading, look selection and optional register-payload loading. **The same context instance has not yet been followed from builder to consumer through the processing state machine.** The binary payload formats, resource ownership/completion, IP selection and full-resolution still routing also remain unknown. This is distinct from the media library's embedded recording-data container below.

### Request allocation, copy and response-wait boundary

The complete shared submitter at `0x35358b8–0x3535b5c` is named by its assertion text as `infra_wrapper_imaging::detail::execute_infra_cmd_i(OSAL_ID, const void*, size_t, api_id_t, void*, size_t)`. Following its immediate calls establishes the following sequence:

1. It prepares a C++ future/promise state. Only a **non-null output-buffer pointer** inserts an API-ID/promise association into a protected tree at `0x91166b0`.
2. At `0x3535a44`, helper `0x3599b68–0x3599bf8` calls `osal_valloc_msg_wait`, sets the message source to `0x80020002`, and **copies exactly the envelope byte count** with `memcpy`. For this LUT request that is 96 bytes; the original stack envelope is not retained by this helper.
3. At `0x3535a64`, it calls `osal_snd_msg(0x90030001, allocated_message)` under a mutex. A nonzero send result takes assertion handling rather than producing a CP status result.
4. Only a non-null output-buffer pointer takes the later `0x3535afc` response-future retrieval branch. That branch eventually uses a libstdc++ futex wait loop at `0x31018f0–0x310197c`, copies the returned response into the caller's output, and calls `osal_free_msg` on that response.
5. The `0x4155` caller explicitly passes **null output buffer and zero output length**, so it skips both response association and response retrieval. It returns through cleanup after the send path, and the adapter writes zero to its output.

`libosal_uipc.so` supplies concrete matching definitions: `osal_snd_msg` at `0x5e60`, `osal_valloc_msg_wait` at `0x67b0`, and `osal_set_msg_src` at `0x7e20`. The last stores the source ID twelve bytes before the payload pointer. For event `0x90030001`, bit 15 is clear, so the public send function branches to `u_osal_snd_msg` at `0x3a10`, which forwards the ID and message pointer through `ioctl`. Allocation uses the corresponding kernel-facing `u_osal_valloc_msg_wait` at `0x3e98`. This is a kernel-mediated OSAL boundary; the kernel queue implementation and final receiver/free path are outside this pass. Symbol matching establishes candidate providers, not an observed live loader binding.

This is **asynchronous with respect to the explicit CP reply path**, not a guarantee that the call never blocks: allocation, mutex acquisition and the ioctl can wait. The helper copies only the 96-byte envelope. Resource handles/offsets inside that envelope do not cause its pointed-to LUT buffers to be deep-copied. Their lifetime, cache/DMA synchronization and a reliable indication that CP has finished using them remain separate requirements. No request-message free occurs on the normal null-output branch of this submitter; it must not be freed by a custom caller based on a guessed ownership rule.

## `mpr::SetLutData`: recovered contract and recording-data role

The sole direct AP call to its PLT entry remains `0x3561018 -> 0x165ff70`. It is reached through `0x34d5548 -> 0x3560f18`; two direct callers of the category/operation wrapper occur at `0x33a5c68` and `0x344e898`.

The AP adapter allocates 96 bytes and accesses a minimum packed extent of 95 bytes:

| Offset | Bytes | Observed use |
|---:|---:|---|
| `0x00` | 4 | Caller writes `0x6e`; role not established |
| `0x04` | 1 | Zero requests reset; nonzero requests execute |
| `0x05` | 26 | Filename; `strncpy(26)`, last-byte NUL check, then passed to file loading |
| `0x1f` | 64 | Name/identifier; `strncpy(64)`, last-byte NUL check; empty first byte rejected by execute |

This is an access-derived layout, not a complete safe runtime ABI. Required initialization, thread restrictions, object lifetimes and valid value domains are not recovered.

`SetLutData` at `0xd6044–0xd60e8` locks a mutex and checks the enable byte. Zero calls `ResetLutData`; nonzero calls `ExecuteSetLutData` at `0xd5e60`. Execution rejects an empty name/identifier and lazily allocates **0x6a1b9 bytes**. It calls a file-loading/conversion routine at `0x17e880`, then distributes a buffer pointer, 64-byte identifier and byte count to four candidate receiver slots, each 0x1ac0 bytes apart.

The receiver setter at `0x119de4–0x119e14` writes:

- receiver `+0xc28`: buffer pointer;
- receiver `+0xc30`: 64 bytes of identifier data;
- receiver `+0xc70`: byte count.

The neighboring exported methods belong to `XavcsMovieWriter`. More substantially, a consumer at `0x1211c8–0x12120c` checks a writer flag and the stored identifier, allocates the stored byte count, copies the stored buffer, and places pointer/size/enable information into a larger output record. That is actual data-use evidence supporting recording-data inclusion; the class name by itself would be insufficient.

### Embedded LUT representation

The file loader reads the named file, parses CUBE headers/samples, calls `mpr::KlvConverter::GetSerialize_SonyLookControl`, then builds an inner object starting with **`SONYBCUB`**. Its size is:

```text
45 + 0xd00 + 12 × grid_size³
```

The serializer's body at `0x19e638–0x19e708` returns a prefix length of 45 for a nonzero payload. The first 12 bytes are reserved for the outer `ELUT` header; the SonyLookControl KLV follows, and inner `SONYBCUB` begins at outer offset 45. Its 16-byte UL key is `060e2b34021301010e060c0201020242`, read from the named `UDAMSetSonyLookControlValue` symbol at `0x2d3e38`. The maximum allocation matches `33³ × 12 + 0xd00 + 45 = 0x6a1b9`. The allocation relationship is an arithmetic consistency check, not proof that every malformed or larger input is rejected safely.

Relative to the inner `SONYBCUB` start, instructions establish these areas:

| Offset | Bytes | Contents observed |
|---:|---:|---|
| `0x000` | 8 | `SONYBCUB` magic |
| `0x040` | 64 | Zeroed |
| `0x080` | 64 | Configuration, including six parsed domain float values |
| `0x0c0` | 64 | Zeroed |
| `0x100` | 768 | Zeroed, then name/identifier copied |
| `0x400` | 256 | Zeroed |
| `0x500` | 2,048 | Zeroed |
| `0xd00` | `12 × N³` | RGB triplets as float32 bit patterns with bytes reversed from little-endian storage |

The parser recognizes `DOMAIN_MIN`, `DOMAIN_MAX`, `LUT_3D_INPUT_RANGE` and `LUT_3D_SIZE` through calls to `strncmp`/`sscanf` in `0x17dd3c–0x17dfc8`. The sample loop at `0x17e500–0x17e880` uses `%lf %lf %lf`, converts doubles to float32, byte-swaps each 32-bit value through `0x17daec`, and copies the result into the payload. File order is retained. Cube-axis order and semantic channel conventions have not been independently validated.

The utility at `0x17dc48` writes another marker: byte sequence `ELUT` at outer buffer `+4`, after the swapped byte count at `+0`, with zero at `+8`.

This representation must not be equated with the CP hardware LUT table format. A metadata serializer and a display/capture LUT programmer can consume the same look while using different precision, layout and ownership rules.

## Still-recording and playback boundaries

The AP has separate calls into `mpr::RecordStill` at `0x357cbec` and `mpr::ReadStillImage` at `0x357b24c`, plus cancellation at `0x355d3d0`.

`RecordStill` at `0xeb260–0xeb654` locks a mutex, works with a deque of `recordStillParam_t` records, checks errors, uses a make-header execution queue, and calls `stillrecord::_record_still` at call site `0xeb620`. This provides a meaningful still-recording branch to investigate; its named API does not yet provide a safe pre-encode pixel-buffer hook.

`ReadStillImage` at `0xc72c8–0xc7674` accesses the SIDB record set, resolves/selects a media record, constructs read-finished event information, and calls `stillplay::_play` at `0xc7630`. This is a playback/decode path, not evidence of a capture-buffer interception point.

`stillplay::GetLutNrtInfo` at `0xc6448–0xc6520` calls two metadata helpers (`0x250da8`, `0x250dcc`), checks presence flags and a value equal to `0x10`, then sets an output flag and assigns an identifier string. The more general `mpr::GetLutNrtInfo` at `0xab640–0xab830` constructs a path containing `/M4ROOT/GENERAL/LUT/`, optionally under `/PRIVATE`. These behaviors support LUT metadata/path recovery during playback. Neither function applies a numerical LUT to pixels in its inspected body.

Retained REA/Ghidra libmpr records corroborate the Set/Execute/Reset operations, file-loading chain, receiver fields and ELUT header. Those shared-object addresses are rebased by `0x100000` relative to the ELF addresses used here. The [GetLutNrtInfo pseudocode](evidence/rea/libmpr/08-procedure_pseudo_code.json) removes several blocks as unreachable; this must not be treated as evidence that those instruction paths are disabled. Its first metadata helper ultimately tail-jumps through a vtable slot at object `+0x218` (`0x250da8 -> 0x2374b0 -> 0x240080`), preserving an output pointer; the indirect callee's possible writes are not represented by the simplified pseudocode. Reachability requires resolving that boundary. Likewise, the file-loader pseudocode omits the output-length argument through the serializer helper and leaves the prefix-length local looking like zero. The direct instruction accesses and serializer's explicit 45-byte result resolve that ambiguity. Decompiler output is useful corroboration, not a substitute for those accesses.

## Finding ledger and remaining work

| ID | Status | Conclusion and evidence boundary |
|---|---|---|
| AP-01 | Answered for extraction | 619 AArch64 ELF files and 2,714 explicit dependency edges; loader bindings remain unobserved |
| AP-02 | Answered | AP constructs a 96-byte command 0x4155 and submits it through a concrete call chain |
| AP-03 | Partially answered | Selection branches, index arithmetic, and base-mode/display enum values recovered; full still/movie gating remains unknown |
| AP-04 | Answered for inspected code | `SetLutData` parses CUBE and creates/distributes SonyLookControl/SONYBCUB recording data; no inspected call establishes ISP programming |
| AP-05 | Partially answered | Packed input, payload fields and writer consumer recovered; complete safe API contract remains unknown |
| AP-06 | Answered for inspected bodies | The named still LUT getters read metadata/identifiers; full-resolution still LUT rendering remains unproven |
| AP-07 | Partially answered | Companion CP trace identifies snapshot/VFX record production and a layout-compatible MLUT consumer; their context-instance lifecycle, payload packing, ownership, completion and still resource routing remain unknown |
| AP-08 | Unresolved | A still pre-encode pixel hook with proven format, ownership, cache synchronization and completion |
| AP-09 | Partially answered | Submission allocates/copies the envelope and crosses OSAL ioctl; `0x4155` skips explicit response-future waiting; kernel/receiver ownership and payload completion remain unresolved |

The next decisive static work is connecting the VFX context through its lifecycle, recovering the two hardware payload formats, tracing receiver-side ownership/completion, and identifying which allocated MLUT resources carry full-resolution still pixels. The AP submission/copy boundary is established; its return is not CP or DMA completion. The shooting/display enum values are now known, and the companion CP investigation identifies supplied-payload loading interfaces; neither result proves a still capture traverses that stage. REA decompilation can make these paths easier to inspect, but decompiler types and deleted branches should be corroborated against the retained instruction bytes and known field accesses.

No deployable still-LUT modification follows from these results. They establish useful structure and concrete investigation targets while keeping the pixel-processing question open.
