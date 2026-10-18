# A7C II feature priorities and a concrete prototype path

October 18, 2026. This pass used the local original A7C II 2.01 firmware and the existing local InstantLink checkout. No camera, printer or external service was accessed. Git publication is separate from the offline investigation.

**The manually invoked host completed-JPEG worker is now implemented:** [usage and limits](../photo_worker/README.md), [checks](evidence/jpeg-worker-checks.json). It snapshots custom sRGB CUBE and manual-lens selections, processes a separate JPEG copy and preserves allowlisted capture EXIF. A full 7008×4672 synthetic test and independent ExifTool readback pass. It remains a host prototype; printing and a camera-compatible build are separate work. These features do not require the still-unresolved MLUT-to-encoder link.

The repository already has a [working computer-only CUBE reference](lut_reference.py), with [independent numerical/image checks](evidence/lut-reference-checks.json). It accepts 17³/33³ 3D-only CUBE grids, uses red-fastest trilinear interpolation, handles domain limits and ICC/orientation, and writes a separate JPEG/PNG. It is a correctness reference, **not an installed camera program**: Pillow holds whole images, metadata is deliberately not preserved, and no camera-side memory/time or playback integration is established.

## What is available and what remains

| Requested feature | Current evidence | Smallest useful next result | Main gap |
| --- | --- | --- | --- |
| Creative Looks in AUTO | FL2/FL3 installation verified on one body; user reported captures with FL1, FL2, FL3 and Auto. An AUTO Creative Look icon translator exists. | Establish whether a selected look is accepted and retained specifically in Intelligent AUTO, then identify the governing gate if blocked. | Existing report does not prove simultaneous AUTO + chosen look; model producers/setting acceptance remain untraced. |
| Custom Fujifilm looks | Supplied pack inventoried: camera DCPs plus scene-referred XMP film profiles; host sRGB CUBE worker implemented. | Validate an sRGB JPEG bake or a calibrated RAW rendering of the actual profiles. | Embedded Adobe RAW tables cannot be treated as direct JPEG transforms. |
| Camera-native InstantLink printing | Local core has protocol/image code and a transport abstraction; firmware contains real BLE client request producers. | Reuse core protocol with an independently validated Sony transport adapter. | BLE initialization/ABI/callbacks, camera-compatible executable and launch/UI lifecycle. |
| Manual-lens EXIF presets | Host worker snapshots named presets and writes independently verified standard lens tags on JPEG copies. | Camera shutter-time association and native quick-switch integration. | A functioning pre-header native metadata hook and UI. Cached PTP LensModel is read-only; two named EXIF exports are no-op stubs. |

### 1. Creative Looks in AUTO

“AUTO” must distinguish **Intelligent AUTO** from P mode with Auto ISO/AWB; the latter is not evidence for the former. The prior report, “I took a photo with FL1, FL2, FL3, and auto mode,” is kept at that scope. There was no request to operate the camera in this pass, so no new menu or capture test occurred.

[feature-gates.json](evidence/feature-gates.json) retains the original AP gate and icon bytes. ELF VA `0x21ade88..0x21ae0d4`, named `is_creative_look_option_inhibited`, chooses an option table and reads byte `table + selected_look*0x18 + 0x14`. It is **not established as the AUTO shooting-mode gate**. Forcing its return to zero could expose unsupported adjustments without making capture accept a look.

The `AutoCustomizeCreativeLook` icon translator at ELF VA `0x23e3144` reads multiple model fields and handles look enum values through 12. Its presence is a concrete UI lead, not proof of user selection or saved-image processing. Trace the producers of those model fields, mode changes and the eventual look-setting consumer before proposing a patch.

Future acceptance needs an explicitly selected look while Intelligent AUTO is active, selection/readback persistence through half-press and scene changes, and saved full-resolution image evidence. Check FL2/FL3 separately; metadata IDs alone are not proof of the intended rendering. None of those camera tests is authorized or executed by this offline task.

### 2. Importing Fujifilm profiles

The requested source is [TingfengLuo/Camera-Profile-for-Fujifilm-Film-Simulation](https://github.com/TingfengLuo/Camera-Profile-for-Fujifilm-Film-Simulation). It was unavailable in the initial bounded Git-directory inventory and was not fetched. The user then supplied a local Downloads copy. [The new inventory](evidence/fujifilm-profile-pack.json) hashes 1,500 files: 1,464 DCPs, 22 XMPs, 12 JPEGs and two READMEs. Eleven film XMP profiles carry embedded RGB tables and explicitly disable output-referred use; eleven wrappers select those profiles. The A7C II DCP has camera-specific calibration, an identity tone curve and HueSatMap dimensions 90×30×1. This is an Adobe RAW-profile workflow, not a directly usable sRGB JPEG LUT. The worker rejects XMP/DCP as LUT inputs; a validated JPEG bake remains necessary. No original profile bytes are republished.

Keep three different inputs distinct:

| Input | Meaning | Possible implementation |
| --- | --- | --- |
| Sony base look + eight adjustments | A native recipe, limited to Sony's existing controls | Save/switch native presets; approximate a simulation |
| RGB 3D CUBE | A transform in a specified rendered color encoding | Host reference, then software JPEG worker; native MLUT still route remains open |
| Camera/DNG profile, if supplied | May combine camera matrices, illuminants, hue/saturation maps, look tables and tone curve | Format-aware calibrated RAW-development pipeline or carefully baked approximation; not a rename to `.cube` |

The cached [llr DCP parser](upstream/llr/apps/worker/src/llr_worker/dcp.py) illustrates why a camera profile is not inherently a rendered sRGB LUT. The supplied A7C II profile was summarized with a bounded first-IFD reader and independently cross-checked with ExifTool; no RAW rendering ran. The cached [pysonycam recipes](upstream/pysonycam/docs/creative_recipes.json) and [recipe example](upstream/pysonycam/examples/creative_look_recipes.py) are a separate source of native-slider approximations, not those requested assets or measured film matches.

First import deliverable: inventory exact files/hash/license; identify format, camera calibration and expected input/output; generate test charts and a color-managed host rendering. For a CUBE, begin with an explicitly sRGB-to-sRGB contract. For a RAW profile, identify an appropriate RAW developer and camera transform before baking. A log LUT cannot simply grade an already rendered JPEG without the correct transform; clipped data cannot be recovered.

Native Creative Look fitting needs a measured objective and error report on independent charts/images. Native curves/matrices/chroma records are not an unrestricted 3D LUT; six custom slots in cached PTP controls select native bases/adjustments, not six arbitrary imported transforms. Do not overwrite body calibration to simulate an importer.

### 3. Running programs and printing without the Bridge

The completed hardware work demonstrates bounded native tasks through the service-loaded shared-library path on this one body. It does not create a persistent application launcher or establish an unattended background process. Root-level access does not supply a printer driver, UI integration, compatible libraries or safe resource scheduling.

Local InstantLink was inspected at commit `6877a83d5e4df59659cc649a70220ac2aa5cef72`; its files were unchanged. [printer-transport.json](evidence/printer-transport.json) pins 15 source files and two firmware libraries, retains eight instruction regions and resolves the actual message-producer calls. Reproduce with `.venv-research/bin/python research/map_printer_transport.py --instantlink /path/to/InstantLink`.

Relevant source boundaries in that checkout:

| Local InstantLink source | Reusable behavior |
| --- | --- |
| `crates/instantlink-core/src/transport.rs`, `device.rs` | `Transport` trait, serialized transactions, notifications, timeout/cancellation and print state machine |
| `crates/instantlink-core/src/models.rs`, `image.rs` | Square sizing, crop/contain/stretch, JPEG quality/size limit, chunk preparation |
| `crates/instantlink-core/src/protocol.rs`, `commands.rs` | Instax framing, checksums and commands |
| `bridge/src/instantlink_bridge/camera/ftp.py`, `app.py` | Completed FTP upload ingress; on-camera worker can instead receive an explicitly completed local JPEG |

The Square model requires 800×800 JPEG, maximum 105,000 bytes; core image chunks are 1,808 bytes with a 150 ms packet delay and 1,000 ms pre-execute delay. The current BLE layer fragments writes into 182 bytes and uses WriteWithoutResponse plus notifications. These are **InstantLink implementation values**, not measured Sony ATT capacity. Preserve negotiated transport limits; do not assume the same fragment size on camera.

The maintained Linux transport uses btleplug/BlueZ/D-Bus. Its existing ARM64 FFI binary is not automatically camera-compatible. The firmware uses AArch64 Linux 4.9/glibc 2.25 and a Sony Bluetooth stack; a bundled libdbus alone does not supply BlueZ or adapt it.

There is a better native lead than replacing the stack. `libInfraBluetoothCore.so` contains these actual ELF request producers:

| Export / ELF VA | Forwarded opcode | Request size |
| --- | --- | --- |
| `BSA_BleClConnect`, `0x322e0` | `0x6aa` | 8 bytes |
| `BSA_BleClWrite`, `0x327dc` | `0x6ae` | `0xf4` bytes |
| `BSA_BleClNotifRegister`, `0x32adc` | `0x6b1` | `0x40` bytes |

Each forwards to PLT `0x13a80`, resolved to `bsa_send_message_receive_status`, and checks null requests. `libInfraBluetoothLowEnergy.so` has higher-level `ClConnect`, `ClWrite`, `ClRead`, `ClRegNotify` and a private connect helper invoking `sony_ble_cl_connect_init` then `sony_ble_cl_connect`. These are request-producing code paths, not only strings. Server acceptance, arbitrary printer UUID support and controller success remain unobserved.

Recover registration/connection lifetime, discovery and structured UUID layouts, notification/CCCD setup, write type, payload limit, callback threading and ownership. Then adapt **InstantLink's existing transport abstraction**, preserving its authoritative protocol implementation. The Python appliance, GPIO/LCD services and FTP server need not move into the camera. No printer protocol code was copied into this repository.

The first future device experiment would be a temporary register/connect/discover/capability probe, separately authorized. Printing, automatic capture triggering and persistent startup follow only after the transport and lifecycle work. No such probe or printing ran here.

### 4. Manual-lens profiles and EXIF

A preset should contain a stable ID, display name, LensMake, LensModel, focal length and maximum aperture. If desired, expose a separate user-entered **actual shooting aperture**. A lens without contacts does not report that value, so do not invent `FNumber`. Zoom lenses need per-shot focal length rather than a fixed guess.

Snapshot the selected preset **at shutter time**, store it with a frame/job ID, and use that immutable snapshot when saving or postprocessing. Otherwise a quick preset switch can label a queued older photo with the new lens. Start with a small on-card preset file and a host selector; native quick-menu/custom-button integration requires its own control/event trace.

Standard EXIF destinations from the cached [ExifTool definitions](upstream/exiftool/lib/Image/ExifTool/Exif.pm): `FocalLength` `0x920a`, `MaxApertureValue` `0x9205`, `LensInfo` `0xa432`, `LensMake` `0xa433`, `LensModel` `0xa434`. MaxApertureValue is APEX `2*log2(f-number)`, not the f-number itself; LensInfo is four rationals (focal range and maximum apertures at its endpoints). IBIS focal length and lens corrections are separate controls; editing metadata does not configure them.

Two plausible named APIs are dead ends in the pinned build: `MprIf::SetExifInfo` at ELF VA `0x2c8cc8` and `MprIf::ModifyExifInfo` at `0x2c9258` are both exactly **`mov w0,#0; ret`**. [feature-gates.json](evidence/feature-gates.json) asserts their symbol identities and eight bytes. Returning zero alone is not evidence they write metadata. Cached Sony PTP [LensModel](upstream/pysonycam/docs/sdk/camera_control_ptp_3_reference/page_0251.md) is Get-only/read-only. The separate `ExecSetLensProfile` recording-data path has not been shown to set still LensModel.

For the software-copy prototype, write verified standard EXIF into a new JPEG and retain original metadata privately without copying unsafe offset-sensitive MakerNotes wholesale. Native original JPEG/HEIF integration requires the actual metadata builder **before** header/container finalization, plus thumbnail consistency. ARW writing is outside this first prototype. Lens EXIF and the LUT worker can share frame/job metadata without depending on MLUT.

## Build order and acceptance gates

| Stage | Concrete output | Must pass before advancing |
| --- | --- | --- |
| A: host reference (exists) | Separate color-managed JPEG/PNG from selected CUBE and image | Independent interpolation oracle, channel permutation, domain/malformed-input and orientation/ICC checks |
| B: host integration (JPEG/lens implemented) | [Worker](../photo_worker/README.md): selected completed JPEG + snapshotted look/lens job → separate graded JPEG; Square preparation remains separate | Originals unchanged, color contract enforced, metadata/orientation verified, no-overwrite/low-space/interruption tests and full-size proof pass; no printer integration claimed |
| C: camera-compatible build | Isolated ARM64 Linux executable with pinned codec/runtime; no shared-library constructor doing long image work | Match loader/libc/kernel or use a compatible static build; host tests/emulation first; bounded image/metadata memory; no deploy/install in this task |
| D: temporary manual camera worker | One explicitly selected completed JPEG → new output; clean start/stop/status | Separately authorized experiment, measured RAM/time/thermal impact and capture coexistence; originals and ARW untouched; playback/database registration assessed separately |
| E: native extensions | AUTO setting path, capture-time lens hook, BLE transport and eventual native MLUT capture | Each has its own source-bound ABI/ownership proof and independent acceptance tests |

For a 7008×4672 RGB8 image, one full frame is 98,224,128 bytes (~93.7 MiB). A CUBE is small by comparison. Use a true scanline/strip codec pipeline in the camera build; the current Pillow reference's strip loop still retains full images and does **not** demonstrate low peak memory. Printing can often decode at reduced resolution and needs only the final 800×800 image. No timing, available-RAM or battery estimate is claimed.

For native MLUT capture specifically, [CAPTURE_HANDOFF.md](CAPTURE_HANDOFF.md) now supplies a real request-to-transfer-to-JPEG edge and an additional CP export record. The remaining blocker is the **writer/alias chain from the MLUT allocation to that request's source**, plus live graph selection, pixel format/color contract, positive completion, and original/full-resolution saved-frame identity. Matching dimensions, a destination handle or an unchanged filename cannot satisfy that proof.

First meaningful native-LUT result should compare identity and a deliberately obvious channel permutation against the host reference on the saved full-resolution JPEG, with compression tolerance, followed independently by HEIF. LCD preview, histogram, thumbnail and embedded LUT metadata are insufficient. The present research narrows where to look but does not establish this capability.
