# Reproducing REA native analysis

The [toolchain record](rea.json) pins REA 6.5.0, Ghidra 12.1.4, the checksum-verified ARM64 macOS JDK archive and the locally built native decompiler. Firmware inputs remain in the ignored `research/extracted` tree. REA source was inspected at commit `4f7b5fb6023ce2ec449561782d09e3c7e5ac7c7b`; the published npm package is pinned independently. No source-to-package equivalence claim is made.

Run from the repository root. Create the local REA runtime with the retained lockfile:

```sh
mkdir -p research/build/tooling/rea-runtime
cp research/toolchains/rea-package.json research/build/tooling/rea-runtime/package.json
cp research/toolchains/rea-package-lock.json research/build/tooling/rea-runtime/package-lock.json
npm ci --prefix research/build/tooling/rea-runtime --ignore-scripts --no-audit --no-fund
```

Obtain the Ghidra and JDK archives from the exact official URLs in `rea.json`, verify their SHA-256 digests, and extract under `research/build/tooling`. Restore archive executable permissions. The official Ghidra archive lacks the required macOS ARM64 decompiler binary. Build its bundled source with the existing Xcode command-line tools:

```sh
cd research/build/tooling/ghidra_12.1.4_PUBLIC/Ghidra/Features/Decompiler/src/decompile/cpp
make -j4 ghidra_opt ARCH_TYPE="-arch arm64" OSDIR=mac_arm_64
```

Copy `ghidra_opt` into the otherwise absent `Ghidra/Features/Decompiler/os/mac_arm_64/decompile` path and make that copied file executable. These operations affect the project-local tool installation. They do not modify Sony firmware or register an MCP server in the agent's settings.

Set `JAVA_HOME` to the extracted JDK's `Contents/Home` and `GHIDRA_INSTALL_DIR` to the extracted Ghidra root for the following commands. The original investigation used Node 26.5.0 and Temurin 21.0.12.1. The system Homebrew JDK 26 stalled in macOS AWT native-library loading; a project-local JDK 21 completed analysis without changing OS security settings.

```sh
research/build/tooling/rea-runtime/node_modules/.bin/rea doctor --provider ghidra --json
node research/run_rea.mjs research/rea-jobs/bootin.json
```

Each [job](../rea-jobs/) specifies the exact input, static queries and a new output directory under `research/build/rea-rerun`. Choose a new empty directory for another run; the driver refuses overwriting retained output. For `appfw.json`, the investigation used `GHIDRA_HEADLESS_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1800000`. Large-file auto-analysis can take substantial time. These are host analysis resources, not measurements of camera memory or performance.

The separate `appfw-focused.json` recipe explicitly sets `disabled_analyzers` to `Decompiler Switch Analysis`. The driver passes those exact names through REA's supported `REA_GHIDRA_DISABLED_ANALYZERS` option; REA records the resulting profile. This profile omits automatic switch-table recovery and is not interchangeable with default-analysis coverage. That redundant run was cancelled after default analysis completed; no focused-profile results are claimed. The selected instruction maps remain independent cross-checks. A [partial diagnostic snapshot and run outcomes](../evidence/rea-analysis-diagnostics.json) retain exception-table and data-flow analyzer errors from the default large AP analysis. The current appFw recipes request the overview and main selection helper; the original failed procam request remains in the retained raw results.

The driver uses REA's public `scripts/rea.mjs mcp` launcher and the matching MCP client package. It retrieves actual tool schemas, permits only selected static queries, optionally exports the session's Evidence bundle, then explicitly closes the binary and client. `research/summarize_rea.py` indexes the retained original investigation under `research/evidence/rea` and renders labeled pseudocode. Rerun outputs have their own paths until deliberately reviewed and indexed.

Decompiled shared libraries in this investigation use a Ghidra image base of `0x100000`. Keep that analysis bias separate from ELF virtual addresses and real camera ASLR. File-offset selectors are supported for pseudocode queries; caller/callee queries require the canonical Ghidra address. Failed requests document selector distinctions and function-discovery limitations, and remain excluded from successful Evidence counts.
