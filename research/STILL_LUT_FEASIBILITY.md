# Custom LUT processing for A7C II still photos

**Latest offline follow-up:** [HQ conversion](HQ_LUT_FORMAT.md) now runs through the original serializer. [CP JPEG processing](STILL_ENCODER_ROUTE.md) reaches a start/wait/result boundary, and [AP JPEG/HEIF finalization](STILL_SINK.md) reaches the asynchronous file writer. The two missing joins remain MLUT output → JPEG input and encoded result → AP saved-image area. No camera experiment or installation was performed.

Research date: October 17, 2026. Target software: the downloaded, pinned A7C II 2.01 firmware.

The follow-up [firmware atlas](A7C2_FIRMWARE_ATLAS.md) distinguishes recording metadata from imaging control. `mpr::SetLutData` packages ELUT/KLV/SONYBCUB recording data. A separate AP command `0x4155` registers user-LUT payloads in CP state. The [ordinary native format](NATIVE_LUT_FORMAT.md) and [stock MLUT bank/register layout](MLUT_LAYOUT.md) are now recovered. One [callback context](STILL_MLUT_ROUTE.md) is structurally joined, but its selector gate and unclassified selected alternative prevent a live still-route claim. These findings supersede the ambiguous string-only leads below.

A custom program could apply a general 3D LUT to a completed still image using ordinary image-processing code. Running that program on the camera is a plausible use of the reported ARM64 Linux root-execution path, subject to confirming access and resource limits on this body. Applying a LUT during full-resolution capture, with matching live preview and normal shooting speed, requires additional work on Sony's imaging pipeline. Root execution alone does not establish that integration.

This research used local firmware inspection and public documentation. No USB session, module upload, camera execution, installation, or camera write occurred during this investigation. No deployable still-LUT program has been built.

## Existing support and newly identified entry points

Sony supports importing 17-point and 33-point CUBE files into User1–User16. Its Picture Profile documentation explicitly limits PPLUT1–PPLUT4 recording to movies, although ordinary Picture Profiles can also be used for stills. These are different capabilities. [Manage User LUTs](https://helpguide.sony.net/ilc/2360/v1/en/contents/211h_manage_lut.html), [Picture Profile](https://helpguide.sony.net/ilc/2360/v1/en/contents/0412D_picture_profile.html).

Static inspection found the following in the official 2.01 input. Reproduce with `.venv-research/bin/python research/investigate_still_lut.py`; the complete results, file hashes and disassembly are in [the evidence JSON](evidence/still-lut-static.json).

| Component | Evidence | What it establishes |
|---|---|---|
| AP `appFw.so` | `LUT_3D_SIZE`, `read3DLUTFileForCube`, LUT storage paths, `sequence_select_lut::send_lut` | Import and selection infrastructure is present |
| AP to media library | Imported `mpr::SetLutData`, with a direct call at ELF virtual address `0x3561018` to PLT entry `0x165ff70` | A recording-data path whose payload and writer consumers are now recovered |
| `libmpr.so` | Defined `mpr::SetLutData` at ELF virtual address `0xd6044`, `ExecuteSetLutData` at `0xd5e60` | CUBE parsing and ELUT/KLV/SONYBCUB packaging; not established as an ISP programming API |
| CP `cpapp-b.bin` | `SET_USER_LUT_BINARY_DATA`, `Create3DLUT`, CamCore table-write messages, `MLUT_MODE_FLOW_3DLUT` | CP contains LUT configuration and table-programming code/data vocabulary |
| Still-related names | AP `SELECT_LUT_SDR_STILL`, library `stillplay::GetLutNrtInfo` | The former is rendering-preset enum 2; the latter reads metadata/identifiers. Neither establishes still-capture grading |

All addresses above are offline ELF addresses for these exact binaries. They are not instructions to call arbitrary functions in a running camera. The input packing for `setLutDataInParam` and the separate CP message is now partly recovered in [AP interfaces](AP_INTERFACES.md); complete object lifetime, initialization, ownership and execution contracts remain unresolved.

The existing FL2/FL3 port extends a native Creative Look implementation with adapted curves, chroma and matrices. It contains no general CUBE evaluator. Fitting a chosen LUT to those controls could approximate some looks, but it would need measured error bounds for each LUT and would not provide general 3D-LUT support.

## Implementation choices

| Approach | User experience | Main work | Assessment |
|---|---|---|---|
| Process a completed JPEG on the camera | Capture normally, then create a graded copy | Codec, CUBE evaluator, independent worker, file completion and output handling | Best first implementation candidate; throughput and camera-side execution remain unmeasured |
| Reuse Sony's existing LUT engine for still capture | Potentially a graded original JPEG/HEIF and matching preview | Trace AP selection to CP, recover table/command formats and prove the still route supports the stage | Most promising route for native capture behavior; feasibility remains open |
| Add software processing before still encoding | Grade an uncompressed image before JPEG/HEIF | Find a writable image buffer, establish format/lifetime and integrate before encoding | Larger intervention; no suitable hook has been established |
| Fit LUTs into Creative Look curves and matrices | Select a custom native look | Fit supported parameters, preserve calibration and quantify errors | Restricted approximation, not a general LUT importer |

## First program design

Start with a manually selected, completed SDR JPEG and a CUBE look that explicitly expects its RGB encoding. Decode it, evaluate the LUT, and write a new JPEG under a separate name. Preserve the original JPEG and ARW. Automatic processing, camera playback registration, HEIF, RAW development and live preview can follow independently.

The program would need:

1. **A small ARM64 Linux executable and a controlled launch mechanism.** Use a compatible static runtime or match the firmware's loader and library symbol versions, and target supported CPU instructions. Keep substantial image processing outside the shared-library initialization callback running in the Sony application process. The current one-shot module structure does not establish a reliable background application lifecycle or automatic startup. Process isolation protects the application's address space; CPU, memory, card access and other resources are still shared.
2. **JPEG decoding and encoding.** A self-contained codec avoids guessing private Sony codec ABIs. libjpeg-turbo supports ARM acceleration and standard JPEG APIs; its scanline API is a candidate for processing strips rather than allocating multiple full frames. A color-changing LUT generally requires decoding and re-encoding, so this adds compression loss. Pin and cross-compile dependencies before any device experiment. [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo).
3. **A real 3D CUBE evaluator.** Parse declared grid size, domain bounds, comments and RGB sample ordering; require the exact entry count; bound allocation; reject malformed/non-finite data and unsupported dialects. Implement and test trilinear or tetrahedral interpolation with an explicit edge policy. A first version can support a documented subset such as 17/33-point 3D-only files and reject combined shaper/3D files. OpenColorIO's CUBE reader is a useful independent reference, including red-fastest ordering and domain handling. [Reader source](https://github.com/AcademySoftwareFoundation/OpenColorIO/blob/main/src/OpenColorIO/fileformats/FileFormatIridasCube.cpp).
4. **An explicit color contract.** Record the image encoding, expected LUT input, expected LUT output and interpolation. A CUBE grid alone does not reliably identify these. A LUT intended for S-Log3/S-Gamut3.Cine cannot simply be applied to an sRGB JPEG. Converting a rendered JPEG into a log encoding cannot recover highlights or color information already clipped by its original rendering. Begin with sRGB-to-sRGB looks; bake other supported transforms on the host using a known color configuration. [OpenColorIO transform and baking tools](https://opencolorio.readthedocs.io/en/latest/guides/using_ocio/using_ocio.html).
5. **File and metadata handling.** Require a completed capture rather than assuming a file-size pause proves completion. Use exclusive output names, temporary output plus verified finalization, low-space handling and interruption recovery. Handle orientation, ICC tags and regenerated thumbnails; do not copy offset-sensitive MakerNotes blindly or label altered pixels as the original Creative Look. Camera playback may require media-database/DCF integration, so an initially valid JPEG might only be visible on a computer.
6. **Measured resource limits.** Establish available RAM, stack/process limits, runtime, interference with capture, battery draw and thermal behavior on the actual body before supporting unattended processing or bursts. No camera-side measurements exist for this proposed program.

HEIF is a later extension: container handling, codec dependencies, 10-bit precision, chroma sampling and SDR/HDR color descriptions need separate validation. libheif supplies a possible container/codec framework; its existence does not establish a suitable A7C II build or acceptable performance. [libheif](https://github.com/strukturag/libheif).

## Memory requirements

Sony lists full-size still images as 7008 × 4672, or 32,741,376 pixels. The following are calculated payload sizes, not measurements of free camera RAM. [Sony specifications](https://www.sony.com/electronics/support/e-mount-body-ilce-7-series/ilce-7cm2/specifications).

| Allocation | Bytes | Approximate MiB |
|---|---:|---:|
| One RGB frame, 8 bits per channel | 98,224,128 | 93.7 |
| One RGB frame, 16 bits per channel | 196,448,256 | 187.3 |
| One RGB frame, float32 per channel | 392,896,512 | 374.7 |
| 17³ LUT, three uint16 outputs | 29,478 | 0.028 |
| 33³ LUT, three uint16 outputs | 215,622 | 0.206 |

Decoder/encoder workspace, extra frames, metadata and process overhead add to those amounts. The image buffers dominate the LUT payload. Strip processing could reduce peak memory for the post-capture path. No seconds-per-photo or sustainable burst rate can be inferred from these sizes.

## Work needed for a LUT during capture

Continue from the recovered AP command `0x4155`, CP snapshots and VFX/MLUT records, keeping the separate `mpr::SetLutData` recording-data branch distinct. Classify the selected sequence alternative and follow its runtime geometry, resource allocation and image buffers to a saved-frame encoder. One callback context's structural identity is established, but that variant is not chosen by the recovered selector for the tested mode domain. Removing a menu restriction alone would not establish a usable still path.

The ordinary table's packing, output quantization, 17 active nodes per axis and 33-point decimation are established. Complete upstream HQ resampling semantics and the color/ownership contract: shaper curves, range, gamut, input/output stage, allocation/alignment, DMA ownership and cache synchronization. Determine whether full-resolution still pixels pass through that LUT stage before encoding and whether the same resource is already occupied by another correction. Preview, recorded stills and movie/HDMI paths must be distinguished.

The essential proof is a known transform affecting the full-resolution saved image in the expected way. An identity LUT, channel permutation and diagnostic color grid should be compared with independent host references. JPEG comparisons need tolerances for compression. Changes confined to the LCD, histogram, thumbnail or embedded LUT metadata would not satisfy that proof.

Then validate single capture, bursts, RAW+JPEG, HEIF, exposure/WB changes, picture-profile transitions, power cycles and LUT switching. Establish when settings/tables are restored and how a failed operation is classified without repeating writes. Preserve the camera's own calibration and keep the initial LUT work separate from the FL2/FL3 installation.

The downloaded LLR project includes a detailed 3D-LUT model for Sony Imaging Edge's desktop converter. Its warped Y/chroma coordinates and table expansion are useful research context, but desktop `ZcTask3DLut` addresses and structures must not be treated as camera firmware APIs. [Pinned desktop model](upstream/llr/sony_repro/notes/static-3dlut.md).

## Next steps without camera installation

1. **Completed:** the [computer-only reference](LUT_REFERENCE.md) validates the 17/33-point CUBE subset, checks identity/channel order/domain boundaries against OpenColorIO, and renders separate JPEG/PNG copies with an explicit sRGB contract. Its numerical and image checks do not establish a native camera ABI or performance.
2. Cross-compile an isolated ARM64 worker and check its dependencies, bounds and interruption behavior offline. This would establish a program candidate, not device compatibility or speed.
3. Continue static tracing of the recovered CP control path into its resource handles, context lifecycle and still-mode decisions. Establish a complete parameter contract before proposing any runtime call.
4. After the separate firmware-version and backup work succeeds, prepare a narrowly scoped temporary execution experiment for review. The current instruction is research only, so no such experiment has been run.

The immediate practical target is a manually invoked graded JPEG copy. For LUTs baked directly into capture with live preview, the next decisive result is proving that Sony's existing LUT stage can receive full-resolution still-image data.
