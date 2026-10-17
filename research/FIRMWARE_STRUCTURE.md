# A7C II 2.01: container, operating system, boot and application structure

This is an offline map of the exact official firmware already downloaded for this project. No camera connection or target execution was used. The generated JSON measurements come from Python, the pinned fwtool parsers, ELF metadata and focused Capstone disassembly; **they are ordinary offline evidence, not REA Evidence records**. The explicitly labeled REA corroboration below uses retained Ghidra decompilation Evidence records from the same investigation.

Run `.venv-research/bin/python research/map_firmware_structure.py` from the repository root. It writes [firmware-structure.json](evidence/firmware-structure.json): hashes for all 1,369 extracted regular files, the TAR member offsets, original ext2 metadata, ELF dependencies, module metadata, boot scripts, selected device-tree properties and the recovered initialization registry. Assertions check the DAT CRC, decoded FDAT header, 30-partition count, kernel magic, WBI2 identity and registry bounds/sentinel.

## What we can establish

The update is a collection of Linux filesystem images, a warm-boot snapshot, CP firmware, peripheral firmware, settings templates and updater programs. The main application is a large dynamically loaded ARM64 library. Linux and the CP software communicate through Sony-specific drivers and messaging libraries. A custom Linux process can use conventional Linux APIs, but an image-processing integration must also understand the Sony messaging and buffer interfaces.

The map establishes the shipped layout and executable dependencies. It does not establish which paths are active on the connected body, its installed version, usable RAM, normal process privileges or the complete secure-boot chain.

## 1. Update container and extraction boundaries

Input: `research/firmware/ILCE-7CM2-2.01/BODYDATA.DAT`, 1,139,929,560 bytes, SHA-256 `eb4943e6099754d64deaef5047942e9bfe258f232d0b7bc39fa23b3cd10e1e31`.

The outer header is `89 55 46 55 0d 0a 1a 0a`, followed by big-endian length/type chunks. The pinned `fwtool/sony/dat.py` validates the terminating DEND CRC32. The map independently recomputes that CRC over all preceding bytes. The extraction configuration identifies `CXD90057_k8`; `fwtool/sony/fdat.py` implements this as its AES-CBC generation with a separately handled first block and trailer IV. Its encryption method is explicitly unsupported. Successful extraction is not evidence that we can create a signed, installable update.

The decrypted `firmware.fdat` is 1,135,475,712 bytes. Its `UDTRFIRM` header is 512 bytes and describes model `0x20030014`, region 0, version 2.01:

| Decoded FDAT range | Offset | Length | Meaning |
|---|---:|---:|---|
| User filesystem | 512 | 1,253,376 | `updater.img`, SquashFS 4.0 |
| Production filesystem | 512 | 0 | Empty entry |
| Firmware payload | 1,253,888 | 1,134,221,824 | `firmware.tar`, 142 TAR entries |

The TAR has numbered stages such as `0100_config`, `0110_backup`, `0300_partconf`, `0700_part_image`, `0750_nor_loader`, `0800_appli` and `1050_i2c`, with corresponding checksum files. TAR member offsets and byte lengths are retained in the JSON rather than inferred from directory names alone.

**Extraction caveat:** fwtool's `writeFileTree` writes directories and regular files, but omits symbolic links and special files. Host modes are also not faithful original modes. Re-reading the original ext2 images recovers **70 rootfs symlinks, 85 /usr symlinks and 70 initrd symlinks**. In particular `/bin/sh -> busybox` and `/lib/ld-linux-aarch64.so.1 -> ld-2.25.so` exist in the image even though they are absent from the host extraction. Do not diagnose missing dependencies from the extracted directory alone.

## 2. Persistent partition map versus update-image sizes

`0300_partconf/partinf.conf` defines 30 partitions. Multiplying its sector units by 512 agrees with the byte capacities in `0100_config/mount.conf`. Several reserved entries have zero length. The package contains five partition images; their byte lengths are often smaller than the destination partition capacities, so this is **not a whole-device flash dump**.

| Partition | Configured capacity | Role / destination | Shipped image |
|---|---:|---|---:|
| 1 | 23.9375 MiB | Updater system | Not in `0700_part_image` |
| 2 | 20 MiB | `/setting`, FAT, settings | Not in `0700_part_image` |
| 3 | 75 MiB | `/system`, FAT, kernel + initrd + other processor images | 32,752,640 bytes |
| 4 | 12 MiB | MPR flash backup, raw | Not shipped as a partition image |
| 5 | 300 MiB | Warm Boot Image | 267,171,648 bytes, WBI2 |
| 6 | 25 MiB | Jiritsu | Not shipped as a partition image |
| 7 | 16 MiB | Root filesystem, ext2 | 5,222,400 bytes |
| 8 | 64 MiB | `/cp`, FAT | 25,102,336 bytes |
| 10 | 12 MiB | `/tmp`, also `/etc` and `/var`, FAT | Not shipped as a partition image |
| 11 | 300 MiB | `/log`, FAT | Not shipped as a partition image |
| 12 | 40 MiB | `/cert`, FAT | Not shipped as a partition image |
| 13 | 4 MiB | Updater work | Not shipped as a partition image |
| 15 | 770 MiB | `/usr`, ext2 | 770,030,592 bytes |
| 18 | 20 MiB | `/lens`, FAT | Not shipped as a partition image |
| 22 | 65 MiB | `/usr/data/ai_dict/usr`, FAT | Not shipped as a partition image |
| 23 | 64 MiB | `/PMBP`, prfile | Files supplied separately |
| 30 | 22 MiB | `/usr/data/lut`, FAT | Not shipped as a partition image |

The LUT and AI mount destinations above come from the updater's mount configuration. They do not appear in the early normal-boot fstab, so the later mounting mechanism remains to be traced. The presence of a LUT partition does not establish a still-photo LUT pipeline.

Image hashes are in the evidence. Two central identities:

- `nflasha7`: `f0d6a0f38807833e8fabf161834f4ede2294ae80f34810b97b4fecfa60561f87`
- `nflasha15`: `621d152c075712369464248b0bfb46451220f6fc8e26a058a5ed5dcb898bd915`

## 3. CPU environment, kernel and memory

`nflasha3_unpacked/vmlinux.bin` is an uncompressed ARM64 Linux Image, 6,610,952 bytes, SHA-256 `620a6a3c2b4f09a3696f2fbf06bab98b49be5eaad6bdb366a1f33319fcb6e9d2`. The Image magic is at file offset `0x38`; the declared text offset is `0x80000`. Its banner at **file offset `0x430090`** identifies Linux **4.9.337-scel+**, SMP PREEMPT, GCC 6.4.0 from Sony CE Linux 14.0.1.0, built October 11, 2025. Kernel-module `.modinfo` independently contains `vermagic=4.9.337-scel+ SMP preempt mod_unload aarch64`.

The root image contains glibc **2.25** and its AArch64 dynamic linker. All **650 ELF files** recovered from the extraction identify `EM_AARCH64`: 127 executable, 395 dynamic/shared and 128 relocatable kernel modules. There are 198 with `.symtab`; many other libraries retain `.dynsym` APIs. These counts include duplicates in the initrd/rootfs and nested updater, and must not be read as 650 distinct running components. The raw CP binaries are not ELF and are outside that architecture count.

`/root/chassis` says `CXD90057+CXD90058`; the supplied DTB identifies `cxd,cxd90057`. `kemco.txt` configures `mem=4G` and extensive reserved areas, including memory above the 32-bit physical-address range. The device tree's generic `/memory@400000000` reg describes a larger 8 GiB aperture. These two shipped configurations do **not** establish installed physical RAM or the memory available to an ordinary process. The cpuset scripts define CPUs 0–3 and reserve CPU3 from the migratable CPU set; this is a four-CPU scheduling configuration, not a benchmark or proof of every CPU's current availability.

No `IKCFG_ST` or `IKCFG_ED` markers were found in the exact uncompressed kernel Image. Thus a kernel build configuration was not recovered by this method. The running camera's `/proc/config.gz` remains unchecked.

## 4. Cold-boot startup: directly readable scripts

The boot arguments in `0800_appli/setting/kemco.txt` specify `init=/sbin/init`, `root=/dev/ram0`, `rootfstype=ext2`, and an initrd address. `nflasha3` contains `initrd.img`, itself an ext2 filesystem. Its `/sbin/init` and helper scripts match the cold-boot sequence in the root filesystem:

1. `/sbin/init` mounts proc and loads the earliest storage modules: `devno`, `dma330`, `crypt-emmc`, `emmc`.
2. `setup_nflash` maps physical/encrypted `pnflasha*` and `cnflasha*` devices to the `nflasha*` names.
3. `setup_rootfs` mounts partition 7 read-only as ext2 at `/rootfs`, performs `pivot_root`, moves proc/dev, releases the initrd RAM disk and executes `/sbin/init2`. Its NFS condition permits a different development path, but that path was not observed running.
4. `/sbin/init2` mounts sysfs, debugfs, the main fstab and fstab.ext, plus a separate tmpfs at `/tmp_wbshd`.
5. Unless `/setting/mode/dmode` selects mode 0, it activates additional drivers, configures cpusets and loads the second module group. If the service marker `/setting/sen/smode` contains `UU`, its conditional path launches `/usr/bin/sen.elf`. It then launches `/usr/bin/bootin.elf` in the background.
6. The scripts enter console/ash loops. The supplied production boot arguments use `console=ttynull0`, so a script containing an ash loop does not establish an accessible interactive shell.

Normal fstab mounts `/usr` read-only ext2, while `/setting`, `/system`, `/cp`, `/log`, `/cert`, `/lens` and `/tmp` use writable FAT with Sony `posix_attr` and sometimes `batch_sync` options. `fstab.ext` mounts **the same partition 10** at `/etc` and `/var`. In particular, `/tmp` is a persistent **12 MiB flash filesystem**, not the conventional Linux RAM-backed temporary area. `/tmp_wbshd` is the explicitly RAM-backed mount.

The second driver group includes `pciedma`, `pci_ep_cp`, `pci_ep_zq`, `pcie-if`, `osal_utm`, `osal_uipc`, `backup`, `dmm`, service USB drivers, multiple media/storage drivers and `codec_drv`. Their loading is directly present in the script. Their names suggest processor, IPC, dynamic-memory and codec roles; complete behavior requires driver analysis and has not been established just by those names.

## 5. Application bootstrap and recovered registry

The small executable `/usr/bin/bootin.elf` is 23,960 bytes. Its `_start` at VA `0x4018f8` loads the first `__libc_start_main` argument from file literal `0x1928`, which gives **main VA `0x4017e0`**. `im.elf` similarly has main VA `0x402490`, recovered from the literal at file `0x2b60`. These entry sequences and the direct `dlopen`/`dlsym` call anchors are retained in the mapper's JSON. The launch path is additionally traced by REA below.

`im.elf` imports `IMDB_get_entries`, `IMDB_find_entry`, `dlopen`, `dlsym`, and Sony OSAL messaging APIs. It also contains the path `/usr/lib/libObj.so`. The structured initialization registry is in **`libIMDB.so`**, not a filesystem configuration file:

| Registry fact | Exact location / basis |
|---|---|
| Exported `imdb_raw` | VA `0x1da20`, file `0xda20`, 17,248 bytes |
| First entry | `imdb_raw + 0x100` |
| Entry stride | `0x48` bytes |
| Lookup code | `IMDB_find_entry`, VA `0xa1f0` |
| Entry getter | `IMDB_get_entries`, VA `0xa25c` |
| Registry end | Record 235 has sentinel first word `0x28` |

The first-entry delta, stride, ID/mask comparisons and sentinel are derived from actual AArch64 instructions retained in the JSON. Each record has four 32-bit words followed by seven 64-bit pointer slots; the slots are resolved through `.rodata` bounds. The report deliberately retains unlabeled raw flag words because all bit semantics have not been recovered.

Useful records include:

| Record | File offset | Resolved library and function strings |
|---:|---:|---|
| 38 | `0xe5d0` | `libJiritsu.so`, `jiritsu_init`, `jiritsu_sus`, `jiritsu_res` |
| 85 | `0xf308` | `libmpr.so` |
| 151 | `0x10598` | `appFw.so`, `Application_System_Init`, `AdjChkConfig.so`, `AppConfig.so` |

`appFw.so` is **99,960,744 bytes**, SHA-256 `2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b`. It exports `Application_System_Init` at VA `0x1796918`, size 264 bytes; most internal function names are stripped. Its large undefined-symbol set still documents interfaces it consumes: `mpr::RecordStill`, `mpr::SetStillRecParam`, graphics/widget systems, database access, file APIs, interchip transfer and imaging wrappers. There are only 19 direct DT_NEEDED libraries. Other interfaces can be supplied by globally loaded libraries; a simple DT_NEEDED-only graph is incomplete for this architecture.

`libmpr.so` is 3,528,576 bytes and retains 3,534 dynamic exported symbols. Its exposed still-recording, codec, playback and file-management entry points make it a useful smaller target for tracing the boundary between the AP application and imaging system. Exact definitions remain binary interfaces, not a supported SDK. Registry membership also does not prove every listed library is loaded for this product or boot mode.

### REA corroboration and additional code-level findings

The following are **REA/Ghidra 12.1.4 decompiler observations**, not runtime observations or original source. Complete provider limitations remain in each linked Evidence file. In particular, generated pseudocode and inferred types can be imperfect.

- [bootin main](evidence/rea/bootin/01-procedure_pseudo_code.json), Evidence `ev_ba0eacd4573bde764a26ae3bd3551c2215a1f57a2a93b01b7c36ed17ff4283c4`: function `0x4017e0` calls setup `0x40297c`, then `0x402b90`; on success it receives messages on OSAL ID `0x94020a`, dispatches them and frees them. Thus bootin remains a messaging supervisor after startup.
- [bootin launch preparation](evidence/rea/bootin-launch/00-procedure_pseudo_code.json), Evidence `ev_2163673152ffede8b9b70d71f6f902a3ce1b7307ea4e296036d8795d8704189b`: function `0x402b90` constructs the `/usr/bin/im.elf` command, appends configuration-dependent arguments and `lazy-global`, normally appends `&`, then passes it to launch helper `0x401aa4`. It also contains gated debugging/preload paths. The helper itself and the flags' actual selected runtime values have not been established by this record.
- [IMDB lookup](evidence/rea/imdb/01-procedure_pseudo_code.json), Evidence `ev_d0f48e2edd18e50291eba7b4de2a5a150c9098bff78319bbe24b765adb548b4d`: corroborates the 72-byte stride, `0x100` header, ID test, mask filter and `0x28` sentinel. Ghidra uses a `0x100000` load-image base for this shared object, so its `0x11db20` entry address corresponds to ELF-relative VA `0x1db20` in the mapper. Do not confuse that rebase with a different binary layout.
- [im dynamic initializer](evidence/rea/im/01-procedure_pseudo_code.json), Evidence `ev_0cb75d1f8999a3c48a9aae44586146a220531911b120f269d2d5eb9e74d74762`: function `0x405e24` calls `dlopen` using the library pointer at record `+0x10`, then `dlsym` using initializer name at `+0x18`, and calls the resolved function with one of the record's last pointer slots. Some flagged entries instead use `dlopen(NULL)` to resolve through the current process.
- [im aggregate-library initializer](evidence/rea/im/02-procedure_pseudo_code.json), Evidence `ev_f9d2b24b358a3220d5f0981bedf702aa24c281ef716f8ee75172b57714588bfa`: function `0x405f5c` selects `/usr/lib/libObj.so` when the record halfword at `+0xe` is nonzero, otherwise the named library. It resolves and invokes the initializer. This explains an aggregate-module mechanism; a module's logical registry name need not be its physical loaded file in every case.
- [im registry walk](evidence/rea/im/03-procedure_pseudo_code.json), Evidence `ev_502bca800b6b9d3936e1c7768b86520beb9f16ce22e5c21e8bd0781070783ed2`: function `0x4064fc` traverses those records, filters two mask words, stops at a phase marker or sentinel, and invokes dispatch helper `0x406374`. [im main](evidence/rea/im/00-procedure_pseudo_code.json), Evidence `ev_46f6a4316e682cdbc1b9802a70b60f8a3ff13c51bda85c46dab1ca9d326c5ef5`, performs multiple staged dispatches, waits for worker threads and enters its own OSAL message loop. Exact meanings of all stage/mask values remain unresolved.

## 6. CP and the other firmware domains

Partition 8 contains `cpapp-b.bin`, `mini_ldr.bin`, `loader_spi.bin`, `pcie_phy_fw.bin`, `cpImg.dat`, `DGU.bin`, `MIS.bin`, `SysSound.bin` and CP configuration. Partition 3 additionally contains `adam_app.bin`, `wole_app.bin`, `av-cam.bin`, `bonobo.bin` and Bluetooth firmware beside Linux. The TAR has separate NOR-loader and I2C-update payloads. This is a multi-domain firmware package; Linux root access is not equivalent to direct control of each separate execution domain.

`cpapp-b.bin` is 18,090,880 bytes, SHA-256 `f5a477588698ef965748903b8959a39ce4f59ffd4aee4d4234ef1670f56a1428`. The existing port's independently emulated Thumb code establishes CP instruction behavior in its tested regions. This structural map does not infer the entire CP operating system or memory map from that local success. `tomco.txt` contains CP memory, interrupt, watchdog and snapshot settings, including a disabled console flag. The JSON preserves them as configuration observations, not measured hardware state.

## 7. Warm boot and the updater are separate mechanisms

Partition 5 starts **`WBI2`**, with SHA-256 `db3730c8dbb2de71cfb7a66c343ba8e752e0f374a3a4104196ceac1e9d1b08dc`. Its payload has not been decoded. The pinned `fwtool/sony/wbi.py` accepts only **WBI1** and a particular older version, so applying that parser to this payload would be wrong.

The supplied boot arguments contain `warm.mode=2`, `wbimem`, `pm.resume` and related areas. The updater's `us_mkfs.sh` explicitly calls `ud_wbi_drop.elf` after filesystem work. The project's native WBI worker separately targets Sony `/sys/power/image_*` controls. Together these are strong evidence that disk changes and resumed memory state must be considered separately. The exact WBI2 compression/table format, snapshot lifecycle and normal resume sequence remain open questions. No warm-boot control was changed for this investigation.

The separate `updater.img` is SquashFS 4.0 and includes updater shared libraries, helper executables, shell scripts and `config/config.xml`. The XML maps payload keys to ordered shell/library actions. `startupdate.sh` preserves the camera's backup data and model/region identifiers for a body update. `us_mkfs.sh` handles mode-dependent partition/format operations and invokes the warm-boot drop. These shipped scripts explain why an official update is a coordinated procedure, rather than merely replacing `appFw.so`.

The existence of checksum helpers and decryptable payloads is insufficient evidence to describe Sony's full authenticity verification or boot trust policy. Those paths require separate binary tracing.

## Implications for a custom still-LUT program

- A freestanding ARM64 Linux worker or glibc-2.25-compatible executable fits the observed AP environment. The custom workers already in the project use direct syscalls to avoid dynamic-loader dependencies.
- A high-memory JPEG pipeline must account for reserved physical ranges and actual runtime memory availability. `mem=4G` is not a usable-memory budget.
- Do not stage a full 33 MP RGB frame in the default `/tmp`: its declared flash capacity is only 12 MiB. A future prototype needs an explicitly chosen RAM allocation or storage path and a measured space budget.
- The registered application/OSAL architecture explains why arbitrary library calls require lifecycle, thread and messaging knowledge. Captured symbol names alone do not define C++ object layouts, ownership or synchronization.
- The dedicated `/usr/data/lut` partition and existing movie-LUT interfaces are good trace anchors, but neither proves arbitrary LUT support in still capture.
- A normal cold boot and a WBI resume may exercise different paths. Any future firmware or persistent-worker experiment needs a plan for both; this investigation has only read offline artifacts.

## Remaining questions and next analysis targets

| Question | Status / next evidence |
|---|---|
| Container and filesystem boundaries | Answered for this exact package; JSON gives offsets/hashes |
| Cold boot shell sequence and early mounts | Answered statically; boot ROM/NOR dispatch and actual runtime remain unchecked |
| Application initializer membership | Recovered; REA confirms staged registry walking and dlopen/dlsym dispatch, but all flag/stage meanings remain unresolved |
| AP-to-CP messaging and buffer ownership | Partially identified via APIs/drivers; trace specific still-record messages |
| LUT storage mounting and native still routing | Open; follow partition-30 mount, command 0x4155, CP VFX context lifecycle and MLUT resource handles; SetLutData's recovered branch packages recording metadata |
| Kernel build configuration | Not recovered by bounded IKCFG search |
| WBI2 content and resume state | Not decoded; avoid interpreting it with the WBI1 parser |
| Device privileges, free memory and installed firmware | Not queried in this investigation |

The map is deliberately reproducible and broad, not a claim that every subsystem has been reverse engineered. It provides exact targets and evidence boundaries for deeper function-level work.
