# Pinned codec sources

These unmodified upstream source archives accompany the separate HIF worker.
`camera_worker/build_heif.py` verifies every SHA-256 before extraction. Builds
and tests perform no downloads. CMake 4.2.0, a C/C++ compiler and (for Linux ARM64)
Zig 0.16.0 are build tools, not runtime dependencies. CMake was downloaded from
Kitware's official release and checked against its published SHA-256 list.

| Archive | Official source | SHA-256 |
| --- | --- | --- |
| libheif-1.23.6.tar.gz | https://github.com/strukturag/libheif/releases/download/v1.23.6/libheif-1.23.6.tar.gz | `4484346dc5995319dbc11e3a1c35d0a2ec46511ce370900869337fd2c7033125` |
| libde265-1.1.3.tar.gz | https://github.com/strukturag/libde265/releases/download/v1.1.3/libde265-1.1.3.tar.gz | `554228bd17788c99a7e63b37ab5634722190e6e2bf60c1dcb01cef328e133905` |
| x265_4.2.tar.gz | https://bitbucket.org/multicoreware/x265_git/downloads/x265_4.2.tar.gz | `40b1ea0453e0309f0eba934e0ddf533f8f6295966679e8894e8f1c1c8d5e1210` |

libheif and libde265 use LGPL-3.0-or-later for the linked library code; x265
uses GPL-2.0-or-later. The combined HIF executable is distributed under GPL-3.0-or-later,
with upstream copyright notices and license texts retained in these archives
and in `licenses/`. Other repository files retain their existing licenses;
Adobe notices applicable to the shared color code remain in `NOTICE.md`.
The static C++ runtime's LLVM notices are included too.

Complete corresponding source for the HIF artifact consists of these three
archives, `camera_worker/{worker.c,heif_core.h,heif_worker.cc,build_heif.py,srgb_profile.h}`,
`native/sha256.h`, and their accompanying notices. The public archives have no
Sony firmware, user photo, Fujifilm profile table or per-camera backup data.
The project scripts supply the build configuration; no binary-only codec is needed.
