# Offline build artifact

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
