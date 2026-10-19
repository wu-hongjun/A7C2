# 來源及授權

原生服務通訊使用 ma1co 的 Sony-PMCA-RE，固定提交 `a82f5baaa8e9c3d9f28f94699e860fb2e48cc8e0`。附帶Python來源位於 `vendor/pmca`，MIT授權全文見 `licenses/Sony-PMCA-RE-MIT.txt`。

macOS整合使用 [MDHan/a7c2-fl23-macos](https://github.com/MDHan/a7c2-fl23-macos/tree/d9eac5a3f805a65a894722d9a4b8800ac6b8df15) 的固定USB與工作流程適配層，另以 `b625c78f43eca9208e6c00f564cebbf353031891` 的WBI收尾實作作為本次受限恢復的來源。主適配層與收尾來源分別固定，未修改下載的原始上游套件。原作者MIT授權保留於 [licenses/a7c2-fl23-macos-MIT.txt](licenses/a7c2-fl23-macos-MIT.txt)。實際分工、驗證範圍與私人資料排除方式見 [Mac安裝經驗](research/MACOS_INSTALLATION_LESSONS.md)。

研究方法與對照資料：

- [lzh2016zzz/a7cr-fl23-research](https://github.com/lzh2016zzz/a7cr-fl23-research/tree/43ba74441c8f10459ade41e84a379ad8005e6ef4)：A7CR FL2／FL3 分層移植與參數、標籤、WBI 的研究參考。本工具為 A7C II 另行定位及實作，沒有把A7CR偏移直接刷入。
- [YuuLuo/a7c2-service-preload](https://github.com/YuuLuo/a7c2-service-preload/tree/1619f938663a36d41e72302a8cfee89a0f6acd1a)：A7C II 2.01 服務模式研究參考，文件為 CC BY-SA 4.0。本工具採原生 `libJit.so` 載入入口，沒有使用預載配置或會改時鐘的 DEBUG 命令。
- Sony官方 A7C II 2.01 與 Alpha 1 II 4.00 是程式／色彩資料分析來源；Sony商標與韌體資料權利仍屬原權利人。本包沒有完整Sony韌體、官方更新容器、任一機身的完整備份或校準單元。新程式碼的MIT授權不宣稱涵蓋Sony的韌體資料。

這是非Sony官方的研究預覽版；發布內容與驗證狀態以對應倉庫說明及 `VALIDATION.json` 為準。

## Adobe DNG SDK reference and derived color algorithms

The RGBTable predictor/format interpretation, color matrices, transfer functions
and interpolation harness in `photo_worker/adobe_table.py`,
`photo_worker/fujifilm.py`, `camera_worker/worker.c` and
`research/adobe_rgb_table_oracle.cpp` use the
user-supplied Adobe DNG SDK 1.7.1 source as their reference. Applicable Adobe
portions retain the [DNG SDK license](licenses/Adobe-DNG-SDK.txt); this repository's
MIT license does not replace it.

Copyright 2006-2023 Adobe Systems Incorporated (dng_reference.cpp).
Copyright 2015-2023 Adobe Systems Incorporated (dng_big_table.cpp/.h).
Copyright 2006-2019 Adobe Systems Incorporated (dng_color_space.cpp).
All Rights Reserved.

NOTICE: Adobe permits you to use, modify, and distribute these files in
accordance with the terms of the Adobe license agreement accompanying them.

The offline checker extracts the original reference function into an ignored
build folder, preserving its source notice. The SDK archive, original profiles,
demo photographs and generated profile LUT arrays remain local; committed
evidence records their fingerprints and validation results. Fujifilm and Adobe
names identify source material and do not imply an official rendering match.

## Standalone JPEG worker dependencies

This software is based in part on the work of the Independent JPEG Group.
The standalone worker links unmodified scalar libjpeg-turbo 3.1.0 sources from
the locally cached `turbojpeg-sys` 1.2.0 crate. The archive and generated build
configuration are pinned in the offline build record. The original
[license roll-up](licenses/libjpeg-turbo-LICENSE.md) and complete
[IJG README and notices](licenses/libjpeg-turbo-README.ijg) are retained.

The ARM64 executable statically links Zig's bundled musl runtime. Its
[complete copyright/license file](licenses/musl-COPYRIGHT.txt) and
[Zig contributor license](licenses/Zig-MIT.txt) are included. The camera worker
does not link or redistribute any Sony executable or library. Its output ICC
marker is a fixed-date generated sRGB profile; no LittleCMS library is linked
into the native executable.

## Separate 10-bit HIF worker

The standalone `hif-worker-aarch64` links libheif 1.23.6 (LGPL-3.0-or-later),
libde265 1.1.3 (LGPL-3.0-or-later) and x265 4.2 (GPL-2.0-or-later).
The combined HIF executable is provided under GPL-3.0-or-later; this does not
replace the licenses of separate repository components or their attribution
requirements. Complete pinned codec sources and build instructions accompany
it in [camera_worker/heif_sources](camera_worker/heif_sources/README.md).
Upstream notices are retained verbatim inside those archives. Full
[LGPL/GPL version 3 texts](licenses/libheif-LGPL3-GPL3.txt),
[libde265 notices](licenses/libde265-COPYING.txt) and
[x265 GPL version 2 text](licenses/x265-GPL2.txt) are included.

The static target also uses Zig's LLVM C++ runtime components; their
[libc++](licenses/LLVM-libcxx.txt), [libc++abi](licenses/LLVM-libcxxabi.txt) and
[libunwind](licenses/LLVM-libunwind.txt) notices are retained, in addition to
the musl/Zig notices above. The HIF executable contains no Sony program or
Fujifilm LUT table; the selected profile table is a private job asset.

The separate `photo-launcher-aarch64` links the same static HEIF/de265/x265 and
LLVM/musl components under the same combined GPL-3.0-or-later distribution
terms. Its complete project sources include `camera_worker/launcher.cc`,
`ingest_exif.h`, `heif_io.h`, `heif_core.h`, `worker.c`, `native/sha256.h` and
`camera_worker/build_launcher.py`, alongside the pinned codec archives above.
