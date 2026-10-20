# Offline build artifact

The [native probe follow-up](../../research/NATIVE_CAPTURE_PROBES.md) supersedes
the observer archive/self-test hashes with an explicitly invoked scoped probe
and fault-contained self-reader. It supplies no loader or installer. The veneer
generator is source-only and never patches an executable automatically.

`jpeg-worker-aarch64` is the static ARM64 Linux prototype, with its exact build
record in [build.json](build.json). It is **hardware untested** and is not used by
the repository's camera installer. No installation or launch script is included.

See [worker scope and offline tests](../README.md) and
[third-party notices](../../NOTICE.md). This software is based in part on the
work of the Independent JPEG Group. Rebuild/source and target-test evidence
identify the binary; ELF compatibility alone does not prove camera operation.

`libsony_bridge.a` and `sony-bridge-test-aarch64`, recorded in
[sony-bridge-build.json](sony-bridge-build.json), contain the transport-neutral
Sony observer/menu core and its standalone self-test. They contain no Sony
firmware, UI renderer, hook installer or loadable camera module. See
[integration scope](../SONY_INTEGRATION.md) before using these research artifacts.

The October 20 [handoff build receipts](../../research/evidence/completed-handoff-builds.json)
supersede earlier launcher/observer artifact hashes. Launcher 0.2 adds frozen
frame-to-file fan-out; the observer self-test includes the header-defined owned
encoded-byte collector. Host/ASan/UBSan and ARM64 core tests are retained in the
[handoff findings](../../research/COMPLETED_PHOTO_HANDOFF.md). There is still no
Sony capture hook, installed runtime or hardware validation. ARM64 launcher
emulation covers startup/list/select, not capture ingress or encoder execution.
