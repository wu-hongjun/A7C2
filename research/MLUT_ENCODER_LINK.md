# MLUT context, pixel resources and the missing JPEG descriptor join

This pass connects the selected fallback's MLUT configuration to an actual pixel-parameter builder through **the same context and resource owner**, and follows a buffer address from a resource-backed ring into its port records. It still does **not** connect that address to the JPEG input's `inputYcSdsUnitType/Handle/DataOffset`, or establish that a full-resolution saved still passes through MLUT.

Reproduce with `.venv-research/bin/python research/trace_mlut_encoder_link.py`. The [source-bound JSON](evidence/mlut-encoder-link.json) pins the official CP image and retains **23 instruction regions and 39 bounded host cases**. Nothing accesses the camera, USB, network, DMA, codec or a live image. Host Unicorn executes original CP instructions with the service/driver stubs listed in the evidence. Synthetic dimensions and addresses are transport fixtures, rather than captured values.

## Same context and owner reach MLUT and pixel configuration

The selected fallback at runtime `0x4c79c0` obtains context `C` from the incoming sequence object's first word. Its setup calls the existing LUT parameter builder and registers callback `0xb2b57d` with `C` as user data. The [earlier lifecycle analysis](STILL_MLUT_ROUTE.md) retains the selector and registration boundary.

Actual callback `0xb2b57c`, when predicate `0xb2bf66(C)` returns 1, performs this sequence:

1. Calls **`0x4e876c(*(C+0x13b4), C+0x13b8)`**, the previously traced MLUT parameter consumer.
2. With context byte `+0x1514 == 0`, calls **`0x4c96b0(C)`**, after a further conditional helper when halfword `+0x578 >= 2`.
3. `0x4c96b0` builds pixel parameters and loads **the same owner `*(C+0x13b4)`** for `0x4e7edc`.
4. `0x4e7edc` calls driver boundary **`0x54d9c8`**, passing that owner by pointer. The driver's retained entry resolves it through **`0x54d788`**, the same owner-bitmap-to-IP mapper used by the MLUT path.

Two host cases execute the actual callback. They verify that predicate zero invokes neither boundary and predicate one passes the identical synthetic owner/context to both. MLUT consumption and the subsequent pixel-context routine are intercepted for this identity test. It does not prove branch feasibility for a particular live camera mode or that pixels were processed.

The fallback sets up `C+0x13b4` as an output-pointer argument to resource query **`0x46f9c4` with selector `0x10`**, through helper `0xb30e92`. The mapper executes that caller/helper transport and captures selectors `0x10` and `0x11`, including their distinct output pointers `C+0x13b4` and `C+0x1528`. The final query service is stubbed; its returned owner is a fixture. An MLUT owner/IP allocation is not automatically an SDS image-unit handle.

## An address producer, rather than an unexplained context field

A retained branch in the fallback loads selector `C+0x1c8` and passes **`C+0x1d0`** as a destination record to **`0x4d3728`**. That function selects one of two resource-backed rings and tail-calls **`0xb2cc72`**:

| Selector passed to `0x4d3728` | Ring global |
|---|---|
| `0x295`, `0x2d9`, `0x43f`, `0x4fa` | `0x172fad0` |
| `0x4c3`, `0x4c4` | `0x172faf0` |

These numeric selectors have not been assigned firmware enum names. Startup routine `0x4d3680` attempts to initialize the first pool with token **`0x43f`**, then `0x2d9` on failure; it can initialize the second with **`0x4c4`**. A selection token and the token that initialized the pool can differ.

Initializer `0xb2cc16` calls `0x511cb4(token, response, 5)` and `0x511edc(token, response, 5)`. Actual loads from those responses populate:

| Pool field | Meaning established by subsequent arithmetic |
|---|---|
| `+0` u32 | Initialization token |
| `+8` u64 | Wrap/base address |
| `+0x10` u64 | Current address |
| `+0x18` u32 | Capacity used for end calculation |
| `+0x1c` u32 | Allocation stride |

Allocator `0xb2cc72` writes record `+0 = 0xffffffff`, copies the pool token into record `+8`, and, when address and stride are nonzero, writes halfword `+4 = 1`, the current u64 address into record `+0x10`, and stride into record `+0x18`. It advances the pool current address by one stride and wraps when another stride would exceed base plus capacity.

For destination `C+0x1d0`, these are **validity field `C+0x1d4`, pool token `C+0x1d8`, address `C+0x1e0` and stride `C+0x1e8`**. The address is therefore a concrete allocation/cursor result, rather than an arbitrary word near a LUT setting. Its physical backing, image direction and relationship to a logical SDS handle remain unresolved.

Twelve host cases verify two initializations with synthetic resource-service responses, all six selectors, and four consecutive allocations across a three-stride wrap. Initialization and record-constructor services are intercepted. No real resource is allocated, reserved or released.

## Address and geometry reach four port records

The actual caller setup at `0x4c9946–0x4c99dc` supplies:

- owner from `C+0x13b4`;
- geometry pointer **`C+0x1b8`**, established by the earlier setup;
- address pointer **`C+0x1e0`** if halfword `C+0x1d4` is nonzero, otherwise null;
- four per-unit flags and a global metadata-pointer family.

The complete builder **`0x4e7edc`** constructs four `0x318`-byte parameter blocks. The host fixtures show this direct transport for every block:

| Context or metadata source | Port-block destination |
|---|---|
| u64 `C+0x1e0`, conditionally enabled | u64 `+0x160` |
| u16 `C+0x1b8` | u32 `+0x168` |
| u16 `C+0x1ba` | u32 `+0x16c` |
| Per-port metadata record word `+8` | word `+0x18` |
| Per-port metadata record word `+0x14` | word `+0x170` |

The pure router `0xb2dcec → 0xb2db88` constructs the metadata pointers from global **`0x1a6a144`**. This call uses records at **global `+0x80 + 0x150*port`**, for ports 0–3. The evidence preserves all ten pointer families produced by the router, distinguishing their structural relationship from live metadata contents.

Twenty cases execute the ring address producer and the actual caller/builder bodies for synthetic `7008 × 4672` and `1920 × 1080`, five context format words and enabled/disabled address conditions. The enabled cases carry the ring-produced address all the way to each port record. Disabled cases explicitly override the allocator's validity flag to exercise the caller's null-pointer branch. The final driver is intercepted, and port predicates/ancillary services are stubbed. All full parameter words are retained so field transport can be independently reviewed.

This is stronger than checking whether a dimension register accepts 7008 and 4672. It establishes that the same-context pixel-parameter path can transport those supplied dimensions and an allocation address. **It does not show that the live camera supplies those dimensions, that the format words select a still mode, which direction the address represents, or that a port writes a saved still.**

## Native JPEG command alignment: a narrower protocol match

The actual `ApiEncodeJpeg` vtable `0xe70d48` supplies handler **`0xaf1bdc`** at slot `+8`. This handler reconstructs a 16-bit command from incoming raw-message bytes **`+0x24/+0x25`**. Command **7** selects virtual slot `+0x4c`, whose actual class entry is **`0xaf4c06`** and leads to the initialization/processing/delivery lifecycle traced in the encoder report. Command 1 and other values take an error/delivery path. Three host cases use the actual class vtable and instruction gate, intercepting the chosen lifecycle/delivery entry before processing occurs.

Two independent CP header references corroborate a raw-message prefix of `0x1c`: helper **`0xaf4df4`** copies the first `0x34` bytes and passes local-copy `+0x1c` to header printer `0xaefcdc`; diagnostic bridge **`0x464108`** passes original raw-message `+0x1c` to header printer `0x45f954`.

The AP layout independently reported by the parent analysis has inner command 7 at wire `+8` and the input YC triple at wire `+0x18/+0x1c/+0x20`. These line up with CP raw `+0x24` and `+0x34/+0x38/+0x3c` under a uniform **`+0x1c`** displacement. The CP byte checks in this generator support that alignment and inner-command match. They do not trace the AP category `0x2000`/message 4 receive demultiplexer, construct the prefix or independently validate the AP source. This is a narrowed protocol match, not an end-to-end receive-path proof.

## The remaining join is specific

The [JPEG boundary analysis](STILL_ENCODER_ROUTE.md) identifies a different structure: the original JPEG message's `+0x34/+0x38/+0x3c` are named `inputYcSdsUnitType`, `inputYcSdsUnitHandle` and `inputYcSdsUnitDataOffset`. The JPEG software module and result path are independently traced. Neither this evidence nor the earlier encoder evidence proves equality between that logical unit/offset and the ring-produced address above.

The missing chain can now be stated precisely:

```text
ring token/base/current -> C+1e0 -> pixel port+160   [instruction transport traced]
                -> logical SDYC unit / backing mapping -> JPEG input triple
                                                        [not joined]
JPEG result / stream descriptor -> AP saved JPEG/HEIF area descriptors
                                                        [not joined]
```

A useful next comparison is the SDS/SDYC handle-to-backing-address conversion and any copy, resize, ownership transfer or cache operation between the port buffer and JPEG input. Equating the MLUT owner with the JPEG handle, or recognizing a common dimension, would skip that essential evidence. This pass establishes no customized LUT still-photo feature and performs no installation.
