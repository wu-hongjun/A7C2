# AP encoder commands, completion fields and transport limits

The official A7C II 2.01 `libInfraAvbbc.so` contains named JPEG and HEIF encoder command classes. This pass recovers the JPEG request and completion ABI, alongside the [HEIF command sequence](HEIF_ENCODER.md). It does not prove that either command consumes the selected MLUT buffer, or that these completion records produce the [still-info notification passed to recording](ENCODER_SAVE_LINK.md).

Reproduce with `.venv-research/bin/python research/trace_avbbc_encoder_bridge.py`. The [byte-bound evidence](evidence/avbbc-encoder-bridge.json) contains 11 instruction regions, 1,639 instructions and 274 bounded host cases. Original AArch64 code runs in Unicorn with synthetic descriptors and explicit service stubs. No camera, codec, physical memory or message transport runs. The harness imports the HEIF mapper's ELF/Unicorn support and pins that file's hash.

## JPEG request and result

`ExecCmd_SDF_EncodeJpegMain::makeParamEncodeJpegMain` at ELF VA `0x121ba0` obtains output information through ParameterControl key `0x1047` and resize/encode payload through `0x104a`. The resulting 80-byte command has these fields:

| Command offset | Source or observed value |
|---|---|
| `+8` u16 | Inner command `7` |
| `+c/+e`, `+10/+12` u16 | Converted category `0x2000`, selectors `0x8310/0x8311` |
| `+18/+1c/+20` u32 | Payload `+8/+c`, low 32 bits of payload u64 `+20` |
| `+24/+28/+2c` u32 | Stream unit `0`, output record handle `+54`, offset `0` |
| `+34/+38/+3c` u32 | Output record `+58 + +5c`, `+58`, `+5c` |
| `+40/+42`, `+44/+46` u16 | Payload dimensions `+68/+6a`, repeated |
| `+49` byte | Quality input 0→0, 1→1, 3→6, all other byte values→2 |
| `+4c/+4d` byte | Payload `+71/+72` |

Firmware diagnostics name the stream fields and image dimensions. The 256 quality fixtures compare original instruction output with these expected mappings. The aspect helper is explicitly stubbed to return 1; its semantics are not recovered by these tests.

`MakeInputYcStdData` at `0x1218a8` separately maps payload u64 `+18 + +20` using `AvbbcPhycMem`, length `0x400`, clears 32 bytes and populates SDYC image-size/state/timing entries. This is a static trace; metadata construction is intercepted in the host command-builder tests. The relationship between this mapping and the [CP ring-produced pixel address](MLUT_ENCODER_LINK.md) is unproved.

Wrapper `ExecCmd` at `0x1226c4` selects mode 0 main JPEG, 1 thumbnail or 2 screen nail, checks the DMM response status at `+2`, copies command bytes `+8..+4f`, and submits an 80-byte message toward OSAL destination `0x82830752`. Actual local converters produce category `0x2000` and message ID `4` from logical `(3, 0x304)`. Three wrapper fixtures cover success and two DMM failures; only success reaches the intercepted send.

Completion `ExecCmd_SDF_EncodeJpegMainComp::ResCmd` at `0x122b44` publishes response status `+8` to ParameterControl `0x104c`, obtains shared output through key `0x1048`, then stores response u32 `+c` into output `+5c`, `+68` or `+74` for modes 0, 1 or 2. Firmware diagnostics explicitly call the value **Encode Size**. Error status clears the selected length and publishes a converted error to key `0x1043`, returning `0x201`. Twelve fixtures cover all three modes, invalid mode, success and errors. This strengthens the interpretation of the independently traced [CP JPEG result scalar](STILL_ENCODER_ROUTE.md), but matching scalar locations alone does not prove reply delivery between the two handlers.

## The envelope explains part of the layout, not the receive route

Native REA decompilation of `InfraAvbbcMsgManager::OsalSndMsg` at `0x139158` distinguishes dataflow messages from global messages. Both JPEG destination `0x82830752` and HEIF destination `0x8057060a` take the dataflow construction branch, which calls `DataflowInfraMsg::SetDataflowMsg`, sets reply/source IDs, and eventually invokes `DataflowInfraSender::SendASync` subject to its hook predicate.

In `dataflowInfraFramework.so`, `MakeDataflowHeader` at `0x32538` creates a 24-byte header with marker `0xa0a0a0a0`. `SetDataflowMsg` at `0x32564` checks command length and appends the original command bytes unchanged after that header. The code also enforces its bounded message capacity. Retained pseudocode: [sender](evidence/rea/readable/avbbc-encoder-transport-04-procedure_pseudo_code.c), [command copy](evidence/rea/readable/dataflow-encoder-envelope-00-procedure_pseudo_code.c).

The CP command-7 gate and input fields are consistent with the AP command displaced by **`0x1c`**, as detailed in [MLUT/encoder evidence](MLUT_ENCODER_LINK.md). A 24-byte dataflow header alone does not establish the remaining four bytes or the receiver's actual dispatch. Do not concatenate these traces into a demonstrated AP→CP execution path.

## HEIF command conversion is a concrete compatibility gap

Local `ConvertCid(5)` returns `0x2003`. However, the dynamically named `midConvTableToMc` at `0x16bd80` terminates after logical `0x531` and contains no `0x534`, the logical frame-encode command requested by the HEIF builder. Executing the original locally bound converter returns **`0xffff`** for `0x534`; `0x531` returns `0x109` as a positive control. The HEIF mapper intentionally intercepts this conversion and records its logical arguments, so its request tests do not establish a usable wire ID.

The reproducible symbol inventory inspects all 650 extracted ELF files, 522 with `.dynsym`. The four sought converter/table definitions occur only in `libInfraAvbbc.so`. This is a bounded negative result: runtime symbol scope, table changes, message interception, alternate conversion mechanisms and activation conditions remain unobserved. It is not proof of a camera bug or that HEIF photography cannot work. It is a reason that these exported command names cannot yet be treated as a usable custom-program ABI.

## What remains to prove

The current evidence establishes command construction and response-field transport. It still lacks the receiver dispatch and resource-ownership chain that connects MLUT pixels to an encoder input, and the producer that turns encoded output into message `0x1006` image areas. Full-resolution custom LUT JPEG/HEIF capture remains unproven. No installation was performed.
