# CP JPEG boundary and the unresolved saved-still join

The A7C II CP image contains a native JPEG request implementation with recoverable input geometry and buffer descriptors. **This pass still does not prove that the selected MLUT pipeline processes the full-resolution frame saved as JPEG or HEIF.** The missing evidence is buffer ownership and dataflow between those two boundaries, rather than whether the CP has JPEG code or whether MLUT dimension registers can represent a large image.

All work is offline. Reproduce with `.venv-research/bin/python research/trace_still_encoder.py`. The [JSON evidence](evidence/still-encoder-route.json) pins the official CP image SHA-256, retains instruction bytes in **32 decoded regions**, separates RTTI/vtable/switch data from executable code, and verifies **1,311 isolated host cases**. Unicorn runs CP instructions against host memory with documented external service stubs; it does not run code on the camera, encode an image or model live frame flow.

## A concrete native JPEG request boundary

Three source-bound RTTI names identify `ApiEncodeJpeg`, `ApiEncodeJpegBase` and `ApiEncodeJpegWithWork`. Their vtables begin at CP runtime addresses `0xe70d48`, `0xe70de0` and `0xe70e78`, respectively. Each uses execution method `0x45f4c4` at virtual slot `+0x40` and request builder `0xaf2524` at `+0x48`; the WithWork class has initializer `0x4603a0` at `+0x74`. These names and pointers are read directly from typed data, rather than inferred from nearby text.

The pure message-name routine at `0x45d6bc` returns `MSG_ID_SDF_ENCODE_JPEG`, `MSG_ID_SDF_ENCODE_JPEG_REC` and `MSG_ID_SDF_ENCODE_JPEG_WITH_WORK` for numeric IDs **4, 6 and 8**. The mapper executes it for all inputs `0..0x502` without reaching external-call stubs and preserves every recognized name. This identifies diagnostic message IDs; it does not by itself identify a transport command that can be issued from the AP or a valid live request sequence.

The actual base initializer at runtime `0xaf1bfe` copies `0xc4` bytes from its incoming message into a local structure. It transports the following fields, with branch-specific behavior noted in the table:

| Incoming message offset | Object offset | Instruction-established behavior |
|---|---|---|
| `+0x34`, `+0x38`, `+0x3c` | `+0x78`, `+0x7c`, `+0x80` | `inputYcSdsUnitType`, `Handle`, `DataOffset`; the object `+0x38 == 0` branch loads these |
| `+0x40`, `+0x44`, `+0x48` | `+0xe0`, `+0xe4`, `+0xe8` | `streamUnitType`, `Handle`, `DataOffset` |
| Descriptor resolved by `0xaecc5c` | `+0xf0`, `+0xf4` | Stores returned `r0/r1` as a 64-bit value |
| `+0x5c`, `+0x5e` | `+0x104`, `+0x106` | `jpegImageSizeX`, `jpegImageSizeY` |
| `+0x60`, `+0x62` | `+0x108`, `+0x10a` | `primaryImageSizeX`, `primaryImageSizeY` |
| Bytes `+0x64..+0x69` | Words `+0x10c..+0x120`, stride four | `sameAreaProcess`, `encodeQuality`, `outputImageAspect`, `addSoi`, `outputYRange`, `outputYuvColorSpace`; enum meanings unresolved |
| Byte `+0x6a` | Byte `+0x124` | WithWork extension field |
| Word `+0x6c` | Word `+0x128` | WithWork diagnostic calls this `qParam` |

These names are tied to actual scalar loads by diagnostic printer `0x46020c`, called with the original message `+0x34` through virtual slot `+0x7c` and bridge `0xaf2646`. A host test supplies distinct field values and verifies all **20** load-to-format-string pairs. Additional fields are message `+0x4c` `address`, `+0x50` `streamAreaLength`, `+0x54` `streamOffset`, and `+0x58` `streamValidLength`. Their presence does not establish that a later response overwrites these input-message fields.

The WithWork initializer uses `qParam != 0xffffffff` to request an additional extent check for descriptor `{0, qParam, 0}`. It computes required bytes as **`(2 * geometry0 * geometry1) mod 2^32`**. Thus supplied values `7008 × 4672` yield **65,482,752 bytes** at this check. This arithmetic establishes a work-buffer requirement in that code path; it does not establish a pixel format, LUT dimensions or saved-image dimensions. The two-byte product is insufficient evidence to assign a YUV/RGB layout.

Eight layout tests execute the derived and base initializer bodies using synthetic messages. They verify geometry transport, extension fields, the `0xffffffff` omission case, and the optional extent argument. The common initializer, address resolver, extent checker and virtual tail are stubbed in these layout tests, so successful return is **not** proof that the camera would accept these requests. An intentionally extreme `65535 × 65535` fixture documents 32-bit arithmetic, not a supported size.

## Work-buffer extent validation

A separate seven-case test executes the actual checker `0xaefaec`, substituting only its capacity-query service and setup/diagnostics. The instructions require:

1. `queried_capacity >= descriptor_word_2`;
2. `queried_capacity - descriptor_word_2 >= requested_extent`.

Failures return `0x10`; exact fits return zero. A synthetic capacity of 65,482,752 passes that extent, while one byte less fails. Capacity lookup, address resolution, physical allocation, cache maintenance, asynchronous lifetime and JPEG completion remain outside these tests. They are not safe assumptions for a custom caller.

## Processing precedes generic message delivery

The actual virtual dispatcher **`0xaf4c0a`** invokes initializer slot `+0x74`, then processing slot `+0x78`, then message-delivery slot `+0x40`, on its success path. This order matters: **`0x68c67a` is not a demonstrated dispatch of JPEG encoding work**. It is a generic delivery helper reached after the processing method and may send a result notification.

Processing method `0xaf20e4` builds a `0x1c0`-byte parameter structure and calls `0x467f44`. A constructed object has source-bound RTTI name **`ModuleEncodeJpeg`**, vtable `0xe7189c`; the processing chain reaches a second named class **`WrapJpegEncode`**, vtable `0xe71d40`, through `0xaf8a7c` → `0xaf8a96` → `0xafdebc` → `0xafdfd4`. These class identities and call edges narrow the codec boundary beyond a generic message or isolated string.

Wrapper `0xafdfd4` can call start boundary `0x52e73c`, then waits through `0x5ea52c` with a channel selector, mask `3`, mode `1` and stack word **20,000**. The timeout unit is unresolved. It subsequently reads per-channel software results:

| Getter | Channel range | Read source | Wrapper output |
|---|---|---|---|
| `0x46a09c` | `0x25..0x2a` | u32 at `0x1a2fe8c + 4*(channel-0x25)` | result `+0` |
| `0x46a0f0` | `0x25..0x2a` | u64 at `0x171eb90 + 8*(channel-0x25)` | result `+8` |

These are software global reads, not demonstrated hardware register meanings. The wrapper tests execute actual wait-result branches and table-based getters for all six selectors using synthetic globals. Successful event flag `1` returns zero; zero flags produce `0x80000000`; a synthetic wait return of `-50` produces `0x400`. The tests stub the channel lookup so codec start is skipped, and substitute event clear/wait services. They establish result transport and branch behavior, with no hardware encode, interrupt or real completion.

The scalar travels through the Module caller output to API object `+0x134`. Processing mirrors it to `+0x138` on success. Builder `0xaf2524` includes object `+0x138` in its 16-byte message header at `+0xc`. The scalar is an **encoded-stream length candidate**: Module code can add two under the copied `addSoi == 1` field, consistent with a JPEG SOI marker, and the message has a separately named `streamValidLength`. This interpretation remains an inference; a demonstrated equality to AP saved-file extent or completed stream length is absent.

Delivery method `0x45f4c4` constructs a message, invokes builder slot `+0x48`, obtains a diagnostic name from object halfword `+0x2c`, and calls **`0x68c67a(object+4, object_word_20, message)`**. That helper allocates a message through `0x565204`, copies a length selected from its message-kind fields, writes the request tag into allocation metadata at payload `-0xc`, then calls `0x564b38`. The latter's retained instructions insert a node into a software queue/list; the destination handle and receiver identity remain unresolved. Four host cases characterize kinds 0/1/2/3 with allocation/send intercepted: kinds 1/2 copy the exact synthetic 16-byte payload and tag; invalid kinds return their documented numeric errors. No receiver or completion runs.

Together these instructions identify a JPEG processing/wait/result path and its outgoing message machinery. They still do not connect its source Yc descriptor to the selected MLUT graph, or connect its stream descriptor/result to an AP saved-file area descriptor.

## The selected MLUT fallback remains unclassified

The [previous lifecycle and geometry analysis](STILL_MLUT_ROUTE.md) established the selector's fallback pointer `0x4c79c1`, its entry `0x4c79c0`, LUT parameter building, and callback registration. This new evidence retains the actual fallback entry, geometry/builder call and registration neighborhood beside the native JPEG boundary so their current separation is inspectable. The fallback's geometry is populated from runtime descriptors; no source-bound actual dimensions, still/movie/display enum identity, JPEG input ownership or encoder result ownership has been recovered.

A CP diagnostic saying `Change StillRecMode to JPEG` and separate HEIF-related names are leads. Neither supplies the missing buffer/dataflow connection. No HEIF encoder ABI or common JPEG/HEIF pre-encode boundary is established by this pass; this is a bounded negative finding, not evidence that such a path is absent.

The remaining proof requires following the selected fallback's image buffer and completion state into an actual encoder input, then following the resulting encoded buffer into the AP JPEG/HEIF area descriptors and saved-file queue. Until both joins exist, native LUT still photography is a research possibility rather than a demonstrated feature. No camera testing or installation was performed.
