# HEIF conversion gap: initialization, activation and a separate native receiver

The local AP conversion gap remains after this deeper offline pass. Original library initialization does not repair the table under the recorded host stubs; the default local message hook does not translate commands; and fully built HEIF requests still contain message ID `0xffff` under explicit local symbol binding. A separate native controller in **`av-cam.bin`** does implement still-encoder selectors `0x10c/0x10d/0x10e` and completion selectors `0x810c/0x810d/0x810e`. Those new selectors are absent from the AP library's forward **and inverse** conversion tables.

This does not establish that camera HEIF capture is broken. It establishes a compatibility gap in this particular AP resize/encode service path under the tested binding. Actual runtime routing, external interposition and the normal capture route remain unproved. No camera, USB, network, installation, firmware process or physical resource was accessed.

## Reproduce and evidence boundary

Run `.venv-research/bin/python research/trace_heif_binding.py`. The [JSON evidence](evidence/heif-binding.json) contains eight original instruction ranges, **672 decoded AArch64 instructions**, table protection/relocation metadata, **403 bounded host cases**, initializer observations, a synthetic GOT-dependency experiment, factory gates and native receiver/completion tests. These are ordinary source-bound/static/Unicorn results, **not REA Evidence records**. The original [HEIF encoder mapper](HEIF_ENCODER.md) remains unchanged and is imported as a hash-pinned support harness.

Source identities:

| Source | SHA-256 |
|---|---|
| `libInfraAvbbc.so` | `676197567dbbefa54e08f528968efc6ca2aaffe8bee3ee3eae8164382ed5a7e5` |
| `av-cam.bin` | `90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096` |

Both files come from the official A7C II 2.01 extraction. JSON records their exact repository paths. AP-library addresses are original ELF VAs. Native-controller addresses below are **file offsets**. Its `ORIL` version-5 header contains absolute pointers consistent with candidate link base `0xffffffc001000000`; that is an offline address model, not a running-camera observation. Native tests map the file at a file-offset alias and execute only selected PC-relative bodies with synthetic objects. They do not consume absolute linked-data pointers or attempt a whole-image relocation.

## What initialization can and cannot explain

`midConvTableToMc` is a 176-byte, global/default-visible symbol at ELF VA `0x16bd80`. It resides in `.rodata`, whose section flags are allocation-only, inside a PT_LOAD segment with flags 5: readable/executable, without the writable flag. The local converter at `0x141b98` loads its pointer through GLOB_DAT slot **`0x1b3668`**. A narrow `.text` scan finds the exact ADRP/LDR access pattern only at `0x141b98/0x141ba8`; this is not an exhaustive indirect-pointer or cross-module store analysis.

The table has 43 nonterminal pairs, ending with logical `0x531→0x109`, followed by the `0xffff` sentinel. Sixty query fixtures compare original lookup instructions with independently decoded table pairs, including missing `0x532/0x533/0x534`, positive controls and a value exercising 16-bit narrowing.

The generator executes **DT_INIT and all 15 init-array roots**. Reached local constructors run original bodies; five external MWF lock constructions and destructor registration are explicit stubs. Those paths perform **159 native stores into the mapped source-image region**, so the observation is not merely a skipped initializer list. No store touches the MC table or its GOT slot. Their bytes remain unchanged, and the original converter still returns `0xffff` for `0x532`, `0x533` and `0x534` afterward. `0x531` remains `0x109`.

This bounds the startup hypothesis: these local initializer bodies, with the listed external services stubbed, do not fill the missing mappings. It does not prove that an external service, another module, kernel component or debugger can never change memory. This ELF imports none of `mprotect`, `dlopen`, `dlsym`, `dlvsym` or `dlmopen`; that is a scoped import inventory, not a system-wide absence claim.

The symbol remains preemptible under ordinary ELF default visibility. An entirely synthetic host test redirects its GOT slot to a two-row table mapping `0x534` to marker `0x7a11`; the unchanged converter returns that marker. Restoring the slot restores `0xffff`. This proves the converter follows the GOT pointer. It is **not** a discovered runtime override, a proposed camera patch, or evidence that such an override is installed. No modified firmware or upload payload is generated.

## The local hook does not provide a translation

`ObjAvbbcCmn::CheckAndHookMsg` at `0x1423b8` is exactly `mov w0,#1; ret`. Three host fixtures vary component/command-like arguments and buffers; all return 1 without changing the buffer. The previously traced sender/receiver call this named hook through PLT entries, so an external override remains a separate loader-scope question. Its local definition supplies no command rewrite.

Five complete original request-wrapper tests use actual local CID/MID conversion rather than the earlier mapper's marker values. Three encode modes build header halfwords `(0x2003,0xffff,96,1)`; open/close build `(0x2003,0xffff,16,1)` and `(0x2003,0xffff,24,1)`. The final sender is intercepted and records bytes only. Successful wrapper return in this experiment does not mean the request was accepted or encoded.

## A concrete factory gate exists

Original `InfraStillEncoder::ExecuteCommand` at `0x11785c` selects the named HEIF sequence when:

```text
inner_base = full_message + 8
u32(inner_base + 0x24) == 0x1500
u16(inner_base + 2)    == 2
u8(inner_base + 0x1c)  == 2
```

Thus the full-message offsets are family `+0x2c`, selector `+0x0a` and format byte `+0x24`. The corresponding constructor call is at `0x1179c0`. Other format-byte values in this family/selector branch construct ordinary `InfraStillEncoderSeqResizeEnc` instead.

A 283-case test exhausts all 256 format bytes for the target family/selector, plus selector/family controls. Allocation, sequence constructors, selection registration and sequence start are intercepted; the original factory instructions determine the selected class. This proves a reachable selection branch for synthetic inputs. It does not establish which real application call supplies those values, that this product exercises the branch, or that subsequent conversion succeeds. The selected sequence is still the resize/encode service documented previously, not a proven normal capture route.

## The separate native still-encoder controller

The independent native source contains strings naming `still_enc/sfmc_still_enc.cpp` and `STILL_ENC_ENCODE_FRAME`. Those names are joined to real instructions, rather than treated as functionality by themselves.

At **file `0x1a7e2c`**, the examined host-message dispatcher reads selector u16 at message `+2`:

| Selector | Original action boundary | Passed argument |
|---|---:|---|
| `0x10c` | file `0x1aac8c` | message `+8` |
| `0x10d` | file `0x1aaed8` | message `+8` |
| `0x10e` | file `0x1ab124` | message `+8` |

The corresponding original diagnostics identify open, close and encode-frame. Six host cases check those three dispatches and three unmatched selectors. Notably, `0xffff` does not reach the encode action in this function. The lower action functions are intercepted; their codec/resource/state-machine implementations do not run. This particular body checks the selector; category validation and admission by the outer receiver are not established here.

At **file `0x1a8470`**, a completion builder emits category **`0x2003`** and selectors:

| Completion | Selector | Constructed length |
|---|---:|---:|
| Open | `0x810c` | 24 bytes |
| Close | `0x810d` | 24 bytes |
| Encode frame | `0x810e` | 32 bytes |

The original encode reply stores status u32 at `+0x10` and the supplied u64 at `+0x18`; its diagnostic explicitly names the latter **`es_size`**. Eighteen tests vary status and sizes including `0x100000001`, confirming 64-bit transport. The send helper at file `0x1a94dc` is intercepted and records payload/count/destination only. A different synthetic argument supplies reply `+8` on the encode branch than on open/close, so those fields must not be conflated solely from similar logs. No elementary-stream bytes, actual encoder completion or resource ownership are proved.

The AP inverse converter at ELF `0x141ed8` recognizes older selectors `0x8107/0x8108/0x8109`, mapping them to `0x855f/0x8560/0x8561` in the tested controls. It does **not** write the caller's sentinel output for `0x810c/0x810d/0x810e`. This missing-result behavior is not a returned `0xffff` value: the inverse function leaves output pointers unchanged on a miss. The independently traced AP HEIF handlers expect logical response selectors `0x8563/0x8564/0x8565`, so the local inverse mapping also fails to supply the missing bridge.

## Result and remaining work

We now have both sides' local vocabulary and a specific missing bridge: AP logical requests `0x532/0x533/0x534` versus native controller requests `0x10c/0x10d/0x10e`, and native completions `0x810c/0x810d/0x810e` versus AP handler selectors `0x8563/0x8564/0x8565`. The table/GOT, initializer and default-hook experiments did not establish a runtime fixup. Matching category and field shapes alone are not permission to join the two routes into a working API.

The decisive remaining evidence is an actual alternate binding/translation or a different active service route, followed by resource ownership and a connection to saved HEIF image areas. The separate native capture-stage/StillInfo producer investigation must remain distinct until a real call or dataflow edge connects them. No conclusion here requires treating a successful camera HEIF feature as proof that this particular generic resize-service path works.

Actual REA corroboration remains separately identifiable: [factory selection](evidence/rea/readable/heif-dispatch-binding-00-procedure_pseudo_code.c), Evidence `ev_1e1ee4ab09f0fdab1f531b16e9729780223a5bdfa4ca0d0c056201651c07dc85`; [local hook](evidence/rea/readable/heif-dispatch-binding-02-procedure_pseudo_code.c), Evidence `ev_66797ed9dae2c4eef6efc582a09a67a099dfcec8ce36343da602a8c29cc3786d`; and [AP message receiver](evidence/rea/readable/heif-dispatch-binding-01-procedure_pseudo_code.c), Evidence `ev_32b5a491f34dfb99bb2237166ce13a336fc6b9e5ba976da2176e336323671c4a`. The receiver decompilation sends non-`0x8201` messages in category `0x2003` through general response conversion; it does not expose a special bypass for the three new still-encoder completion selectors. Its full asynchronous routing and caller-initialized miss behavior were not emulated in this pass.
