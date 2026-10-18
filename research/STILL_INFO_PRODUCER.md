# Still-info producer in A7C II 2.01

The producer of the AP `0x1006` still-info notification is now identified in **`av-cam.bin`**. Its `Stage_NotifyStillInfo` function builds the same kind, destination, source argument, tag, union layout and metadata fields consumed by the previously traced AP receiver. This closes the notification producer to recording-descriptor handoff. It does not yet identify the physical pixel allocation shared with MLUT processing or prove that loading a LUT changes a saved still.

The new producer also contains a still-specific JPEG encode stage. Its callback supplies a length value that is adjusted by an output descriptor field and written into the JPEG area later included in the notification. This is a stronger connection than matching encoder names. The actual lower-level CP response to that callback and the input pixel buffer identity remain unresolved.

## Sources and coordinates

Run `.venv-research/bin/python research/trace_still_info_producer.py` from the repository root. The mapper verifies exact complete-file SHA-256 identities, retains 17 instruction regions with their own byte hashes and 1,315 decoded instructions, and runs 83 isolated host cases. Its output is [the evidence JSON](evidence/still-info-producer.json). These are ordinary Capstone/Unicorn results, **not REA Evidence**. No camera, transport, codec or native target process is used.

| Source | SHA-256 | Coordinate system |
| --- | --- | --- |
| `av-cam.bin`, 13,797,088 bytes | `90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096` | Raw AArch64 little endian; file offset plus linked base `0xffffffc001000000` |
| Original `appFw.so`, 99,960,744 bytes | `2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b` | ELF VA through original `PT_LOAD` |

**All av-cam addresses below are file offsets**, unless explicitly called linked VAs. They are neither CP Thumb addresses nor ELF VAs. For example, notify entry file `0x203510` corresponds to linked VA `0xffffffc001203510`. The actual runtime mapping was not observed.

The address model is supported by several independent byte relationships: the header contains `ORIL` and version 5 at file `0x8`; ten header pointers subtract to entry offsets `0x58..0x1a4` containing valid AArch64 entry code; the absolute table at `0x3777a8` points to code `0x1f8dc0` and RTTI `0x32a4d0`, whose name is `N6Camera2MP7Capture11CapRsrc_VFXE`; and the notify and encode instructions use PC-relative references to the matching stage/file diagnostic strings. This establishes a linked coordinate model, not a measured runtime address.

## Actual notification construction

Entry `0x203510` loads the capture context from `[x0]`. After local resource helpers it performs the following operations:

| Instruction reference | Operation |
| --- | --- |
| `0x203570..0x203584` | Request `0x384` bytes for destination `0x80020002`, with output pointer at stack `+8` and argument 3 equal to 1 |
| `0x20358c..0x203598` | Pass the allocated message and source argument `0x90030001` to the metadata import wrapper |
| `0x20359c..0x2035ac` | Store byte kind 4 at message `+0` and halfword tag `0x1006` at `+2` |
| `0x2035b0..0x2035cc` | Populate message words `+4/+8/+c/+10` from context `+7d4`, constant `0xf000`, context `+7bc`, and context `+9c` |
| `0x2035d0..0x20360c` | Copy five halfwords from context `+7c2/+7c4/+7c6/+7c8/+7d0` to message `+378/+37a/+37c/+37e/+380` |
| `0x203604..0x203618` | Copy `0x364` bytes from context `+0x440` to message `+0x14` |
| `0x203640..0x20364c` | Send the message to `0x80020002` |

Allocation, source metadata and send roles follow their arguments, control flow, diagnostic strings and the matching AP receiver. The wrappers at `0x2f028`, `0x2f0a0`, and `0x2ef9c` dispatch through imported function slots; this pass does not enter their OS implementations. In host tests those external operations are intercepted.

A failed allocation returns 2. A failed send calls the release wrapper and returns 2. A zero send result follows local invalidation and returns 0. **That return is not proof of AP receipt, storage completion or successful image encoding.** The retained diagnostic is `Stage_NotifyStillInfo` at `0x43100c`, with `STILL INFO SEND START!!!` and `STILL INFO SEND OK!!!` messages nearby.

A bounded direct-branch scan through file `0x0..0x420000` finds four branches into notify: `0x1fd71c`, `0x1fd848`, `0x1fd918`, and `0x1fd98c`. They are state/sequence dispatch branches, not proof that all capture modes run the same stages.

## Descriptor identity across av-cam and AP

The body copy explains the AP layout exactly:

| Capture context | Accumulator-relative | Wire message | Previously recovered AP use |
| --- | --- | --- | --- |
| `+0x440` | `+0` | `+0x14` | Start of copied still-info structure |
| `+0x450` | `+0x10` | `+0x24` | Auxiliary 0x90-byte block to queued payload `+0x5f8` |
| `+0x4e4` | `+0xa4` | `+0xb8` | First typed result union |
| `+0x644` | `+0x204` | `+0x218` | Second typed result union |
| `+0x7bc` | Outside copied body | `+0xc` | Word to queued payload `+0x688` |

The two unions have stride `0x160`; their descriptors start at union `+0x10`. Wire types 1/2/4 become the AP's JPEG/RAW/HEIF descriptor selections. This pass executes the actual original AP transformation at ELF VA `0x3533214` on output from the actual av-cam sender in ten cases: all nine format pairs and one repeated pair with the logging-suppression flag. Every descriptor and the auxiliary block match byte for byte. Source/destination import internals are intercepted, so this verifies the construction and local receive transform, not a live IPC exchange.

The earlier [encoder-to-save investigation](ENCODER_SAVE_LINK.md) continues that AP descriptor through its registered callback, local queue tag 9, classifier and `RecordStill` adapter. [STILL_SINK.md](STILL_SINK.md) continues into libmpr recording and file-area writing. Those reports retain the limits on dimensions, buffer ownership and actual full-resolution image behavior. The two result slots must not be relabeled as two successive frames.

## Where the JPEG area values come from

A typed JPEG slot contains an area at accumulator `+0x144` or `+0x2a4`, equivalently JPEG descriptor `+0x90`. Router `0x1f4320` requires selector 1 or `0x10` and corresponding union type 1. It delegates to `0x1f435c`, whose argument map is:

| Argument | Area byte offset | Behavior |
| --- | --- | --- |
| `w2` | `+0` | Write unless `0xffffffff` |
| `w3` | `+0xc` | Write unless `0xffffffff` |
| `w4` | `+4` | Write unless `0xffffffff` |
| `w5` | `+8` | Write unless `0xffffffff` |

The `Stage_JpgEncode_Main` entry at `0x200c04` references `StillMP_Stage_EncodeJpg.cpp` and `Stage_JpgEncode_Main`. It prepares input/output descriptors, calls `ExecEncode` at `0x201340`, then passes its output length into `PostEncode` at `0x20172c`. The concrete length path is:

1. `ExecEncode` at `0x2015b0..0x2015f4` passes callback `0x207a74` and userdata at its stack `+0xd0` into resource `Encode` at `0x1f9d8c`, which forwards them to the codec wrapper at `0x2a58f0`.
2. Callback `0x207a74` stores input `w0` at userdata `+4` and input `w1` at `+8`, then signals the event identifier held at userdata `+0` through import `0x2e980`. Calling code at `0x201634` compares the first stored word with 0 and 7. No correspondence to a particular CP response scalar is established here.
3. The caller waits through `0x2077d0`; that wrapper uses imported wait `0x2e9a8`, retries after a particular failure, then uses `0x2e994`. Event names and lower OS behavior are not recovered.
4. At `0x2016e8..0x201700`, the successful result path reads callback userdata `+8`, adds output descriptor `+0x10`, and writes the result through output argument `x6`. A separate output holds the retry count. This is actual arithmetic, not an assumption that encoded length equals allocation capacity.
5. `Stage_JpgEncode_Main` loads those outputs at `0x200e10` and passes the adjusted length as `w7` into `PostEncode` at `0x200e38`.
6. In PostEncode's mode-0/context-`+0x7dc==0` branch, `0x201918` loads the output descriptor's first two words and `0x201944` calls the JPEG area router. The resulting area words are `[output descriptor+4, 0, callback length + output descriptor+0x10, output descriptor+0]`.

The shared diagnostic at file `0x4308b6` explicitly prints `streamLength`, `targetLength` and `codecLength`. It is used by both capture JPEG and HEIF code. The JPEG file diagnostic and call chain, rather than the shared `PostEncode` label alone, establish which path this is.

Output resource preparation also has retained instructions: `0x2010dc..0x2010f4` calls materializer `0x2097fc`; `0x201154..0x20116c` copies its resource/address/offset-related output into the output descriptor. The deeper allocator, physical backing, and codec-to-CP correspondence are outside the proven boundary. A descriptor word must not be equated with a CP input handle solely because both are called handles elsewhere.

HEIF router `0x1f490c` independently updates the area at accumulator `+0x154` or `+0x2b4`, HEIF descriptor `+0xa0`, for each slot whose type is 4. Caller `0x20043c` supplies its output-descriptor words and length. This report retains that concrete population edge while leaving the complete HEIF callback path and any correspondence to the separate MC/STLENC host route open.

## Host validation and remaining work

The 83 cases execute original instruction bytes with invented memory:

- 12 sender cases cover nine format pairs, send failure, allocation failure, and diagnostic suppression. Ten success cases also run the original AP descriptor transform.
- 64 area-writer cases cover the two JPEG selectors and two HEIF slots across all 16 combinations of the four `0xffffffff` preserve sentinels.
- Three actual callback cases check both stored words and event arguments.
- Four actual result-store/PostEncode cases check both JPEG slots, with zero/nonzero prefix values and the complete resulting four-word area.

The raw image is mapped at normalized host base `0x01000000` solely for isolated emulation. Relative instruction deltas are preserved. Tested paths do not traverse its absolute high-address tables; no source binary bytes are changed. Logs, allocation, metadata, message sending, event imports and copy helpers are intercepted as stated in the mapper.

The notification producer and JPEG callback-length-to-area chain are now established locally. Still unresolved are the complete input pixel-buffer origin, the lower codec/CP callback correspondence, physical allocation identity with the MLUT path, scheduling and ownership across processors, HEIF capture/MC route correspondence, and actual saved-image behavior. Resolving those joins is necessary before treating this as a supported custom-still-LUT implementation.
