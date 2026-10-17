# AP native LUT resource path: bounded static follow-up

Later [producer tracing](NATIVE_LUT_FORMAT.md) recovers the ordinary 48,000-byte format, parser and paired writer, and narrows the alternate converter's output boundary. The read/mapping observations below retain their original scope; they do not establish full-resolution still routing.

This investigation follows the immediate application-side helpers called by the previously identified LUT selection path. It uses the exact official A7C II 2.01 `appFw.so` with SHA-256 `2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b`.

Run `.venv-research/bin/python research/map_ap_lut_resources.py`. The [resulting evidence](evidence/ap-lut-resources.json) preserves each exact instruction's bytes, ELF-relative address, file offset, imported-call names, selected global relocations, string contents and source hashes. This is **ordinary offline Capstone/ELF evidence, not REA Evidence**. No camera access or target execution occurred.

## Numbered native files on the dedicated LUT partition

At `0x3020430`, instructions load format string **`%s/LUT%03d.LUT`** from file/VA `0x4bfa3b2`, directory **`/usr/data/lut`** from `0x4bfa3d3`, add one to the supplied index and tail-call `snprintf`. Index 0 therefore produces `/usr/data/lut/LUT001.LUT`.

This connects the application resource reader to the partition-30 mount destination in `0100_config/mount.conf`. These are native **`.LUT` resources**, not proof that this code reads text `.cube` files. No regular `.LUT` or `.cube` sample was found in the bounded official 2.01 extraction search recorded by the mapper; the partition-30 image itself is not shipped among its five partition images.

## Read descriptor and its two capacity branches

`0x3021228` takes an internal index and an output descriptor. It accepts unsigned indices **0 through 49**; larger values take an error path, clear output length and pointer, and return false. This is an internal function bound, **not 50 user-visible slots**.

The examined outer helper at `0x3428510` uses a narrower range: with its activation argument nonzero, selections **3 through 18** call the resource reader with `selection - 3`, giving indices **0 through 15**. Values at or below 2 bypass this read path; values above 18 take an error path. That establishes the 16 slots reachable through this particular branch, without claiming all other internal indices are exposed anywhere.

The reader calls filename helper `0x3020430`, then chooses one of two buffers:

| Branch | Maximum fread length | Buffer pointer relocation |
|---|---:|---|
| Normal path; also internal indices above 15 | `0xc000` = **49,152 bytes** | GOT `0x5b8f438` → static addend `0x88f0b08` |
| Alternate path selected by a per-index flag for indices 0–15 | `0xbc00` = **48,128 bytes** | GOT `0x5b7e8f8` → static addend `0x90fcb08` |

The flag is read from the object returned by `0x2887b48` under the nearby lock/unlock calls. Its complete meaning is not established here.

Read helper `0x301fadc` calls `fopen(path, "r")`, `fread(buffer, 1, capacity, stream)`, stores the **actual returned read length** through its fourth argument and calls `fclose`. It returns 1 if opening fails and 0 after the read/close sequence. The examined helper itself does not validate a particular native LUT file length or inspect payload fields. Those statements describe only this helper; downstream validation may exist.

On success, `0x3021228` writes the actual length as a 32-bit value at descriptor `+0`, and the chosen data pointer at descriptor `+8`. Its diagnostic string is `model_user_lut_files read3DLUTFile!! ... read_size[...]` at `0x4bfa25a`. The read capacities are upper bounds, not evidence that valid files always occupy exactly those lengths.

## Opaque copy into mapped memory

The next helper, **`0x30210c8`**, requires two global resource objects to be ready, a non-null descriptor and data pointer, a nonzero length and a length no greater than the destination object's capacity (`+0x10`). Its globals are reached through GOT entries `0x5b70df8` and `0x5bbdeb8`, whose relative relocation addends are `0x9108720` and `0x9108708`. These are static addresses of bookkeeping objects, not measured runtime physical-buffer addresses.

It constructs an open/map request naming **`/dev/mem`** (string `0x371004a`), passes the destination object's address and capacity to `0x34d37e8`, then calls `memset(mapped, 0, actual_length)` and `memcpy(mapped, descriptor.data, actual_length)`. The underlying helper `0x3552f54` imports `sysconf`, `open` and `mmap`; cleanup `0x3553038` imports `munmap` and `close`. The descriptor bytes are copied from their first byte without a visible `+4` header skip in this sequence.

Finally it sends the two resource IDs and actual length through wrapper `0x34cb5e8` → `0x3514f54`, checks the returned status and reports success/failure. The outer selection helper then returns a resource ID and the read length to its caller. Full ownership, resource-ID interpretation, mapping address and CP-side handling remain outside this bounded trace.

## The 48,000 and 124 constants are explicit fields

The input builder at **`0x342888c`** reads a per-selection flag indexed by `selection - 3`. Its primary stack descriptor begins at `frame + 0x60`. Relevant stores are:

| Descriptor offset | Observed store |
|---|---|
| `+0x04` | Constant `0x62a` |
| `+0x0c` | Zero from the initial descriptor clearing, retained in the flagged branch |
| `+0x10` | In the flagged branch, `0xbb80` = **48,000** |
| `+0x14` | In the flagged branch, `0xbb80` = **48,000** |
| `+0x18` | In the flagged branch, `0x7c` = **124** |
| `+0x1c` through `+0x5b` | Copies 64 bytes supplied through input argument `x4` |
| `+0x5c` | In the flagged branch, a byte flag set to 1 |

This descriptor is sent through `0x34cfac8`. The function then reconstructs another descriptor in the same stack area and, for its flagged branch, again stores the two 48,000 values at offsets `+0x10`/`+0x14` and 124 at `+0x18`, before calling `0x34c6418`. This provides concrete application-side counterparts to the fixed-size payload areas found separately in CP analysis.

Joining these stores to the [AP wire-message packing](AP_INTERFACES.md#ap-selection-to-cp-command-0x4155) and [CP handler](CP_ARCHITECTURE.md#user-lut-message-reaches-a-concrete-cp-handler) gives a narrower, explicit inference: the flagged request supplies SRAM offset **0**, length **48,000**, and register-data offset **48,000**, length **124**, relative to the resource base resolved by CP. This describes request-relative regions. The relationship between that resolved base and the mapped copy's first file byte has not been proved, so these are not yet established native-file byte offsets.

**No four-byte header or grid packing was recovered.** The alternate read capacity is 48,128; 48,000 + 124 is 48,124, leaving a numerical difference of four. That arithmetic is only a lead. The immediate functions examined here contain no parsing of a four-byte header and no evidence identifying channel order, grid dimensions, node precision, padding or register semantics. Treating the difference as an established header would go beyond the evidence.

The next useful offline step is to find a shipped/generated native `.LUT` sample or trace its producer and the receiving resource-offset fields. Text CUBE ingestion, native LUT preparation, resource transfer and CP loading must remain separate layers until the byte-level connection is proved.
