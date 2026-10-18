# av-cam MLUT boundary and allocation-identity limits

The additional AArch64 `av-cam.bin` contains an MLUT driver and a 64-entry diagnostic registration table. This pass binds the strings to actual handlers, recovers its geometry wrapper and records its embedded register-target lookup. **None of those facts identifies an MLUT pixel allocation with the capture encoder input.** The stronger allocation and encoder-input evidence is in [CP allocation provenance](MLUT_ALLOCATION.md) and [av-cam pixel resources](AV_CAM_PIXELS.md).

Run `.venv-research/bin/python research/trace_av_cam_mlut.py`. The [evidence](evidence/av-cam-mlut.json) pins the official A7C II 2.01 image, four instruction regions / 54 instructions, 67 bounded host cases and all 64 registration records. These are ordinary Capstone/Unicorn results; no new raw-image REA analysis is claimed. Existing REA Evidence stays separately indexed.

All code addresses below are original **file offsets**, distinct from ELF and CP Thumb addresses. The `ORIL` linked model uses `0xffffffc001000000 + file_offset`; the host emulator instead uses a file-offset alias. Selected code uses PC-relative references. No runtime mapping or physical processor assignment is observed.

## An MLUT-shaped interface is not a capture-buffer producer

The registration array at file `0xbe2348` contains 64 triples: a numeric command-name string, help text and an absolute handler pointer under the linked model. Its zero triple at `0xbe2948` terminates the array. For example, the geometry help text at `0x451544` points to handler `0x2b4630`, whose original code parses its command arguments and calls `0x2b230c`.

The retained registry also includes SRAM, register and enable commands. This establishes diagnostic registration structure. It does not show that capture dispatch invokes those commands, and the tests do not call the command handlers or execute their hardware operations.

Geometry dispatcher **`0x2b230c`** accepts IP argument zero and tail-dispatches to **`0x2beb94`**. That helper requires both supplied 32-bit dimensions to be below `32768`; otherwise it returns `-13`. Valid dimensions reach the register-writer boundary `0x2bfc24`. The emulator intercepts that boundary and records the IP and dimensions without reading or writing any register.

Forty-nine dimension cases exercise the Cartesian product of `0`, `1`, `4672`, `7008`, `32767`, `32768` and `0xffffffff`. This verifies the software check and transport, including supplied `7008×4672`. It establishes neither a supported camera resolution nor imaging throughput.

Wrapper **`0x2e3e34`** rejects a nonzero second argument. Otherwise it reads two u32 dimensions from its parameter `+0x20/+0x24`, selects IP zero and calls that dispatcher, converting a nonzero result to return value 1. Twelve fixtures verify the gate and dimension transport. This wrapper does not accept a pixel handle or address.

## Keep the register address models separate

Pure lookup **`0x2b05c8`** returns scalar `0xfffffffff3019000` for IP zero and zero for the other tested IP values. Six cases execute only its arithmetic/selection instructions. The scalar is never dereferenced.

The earlier CP MLUT evidence contains a seven-entry target table beginning `0xf4009000`, with `0x100000` steps. These embedded values have different address models. They are **not proof of distinct physical blocks**, because an alias or translation could exist; equally, they are not proof that an av-cam IP number equals a CP ownership slot. No physical-alias or shared-allocation identity is recovered here.

## What the bounded reference scan leaves open

The generator scans aligned words at file `0..0x320000` for direct `B`/`BL` encodings targeting the lookup, dispatcher, validator and wrapper. It retains every matching source instruction. The dispatcher and validator have concrete incoming branches; no direct branch into wrapper `0x2e3e34` appears in this scan. Indirect calls, constructed pointers, overlays and runtime dispatch remain outside that search. This is not proof that the wrapper is unused.

The important missing edge is still allocation provenance: a capture graph must supply an MLUT-processed resource to the same handle/offset later resolved by the encoder, with compatible direction and lifetime. Numeric diagnostic entries, dimension agreement, an MMIO-like scalar, or the word `VFX` on a resource class cannot establish that identity. This pass provides a bounded native-driver boundary to compare against the actual resource traces, without inventing a live MLUT-to-encoder route.
