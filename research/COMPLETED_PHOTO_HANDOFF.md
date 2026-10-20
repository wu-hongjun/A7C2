# Owned JPEG/HIF bytes and frozen frame fan-out

October 20, 2026. All execution tests use generated local files or synthetic
memory. No camera, mounted media, USB enumeration, Sony process or G HUB access.

This pass adds a bounded encoded-byte collector and native frame-to-file job
fan-out. It does **not** install a capture hook or demonstrate automatic camera
processing.

## The encoded-byte boundary

[Original-instruction tests](trace_encoded_write.py) pin the stock 2.01
`libInfraFsys.so` SHA-256 and record [19 cases across eight ranges](evidence/encoded-write.json).
Addresses are original ELF virtual addresses, not runtime ASLR addresses.
External filesystem calls are recorded stubs; the tests execute no Sony I/O.

| Observation | Source and implication |
| --- | --- |
| Scratch allocation | `0x2eef0..0x2ef34` checks logical length against available data, rounds transfer length and calls the drive's scratch acquisition function. Incoming DMM/vector addresses are not treated as C pointers. |
| Contiguous transfer | At `0x2f088..0x2f0a0`, `x0` is the same full FSYS handle, `x1` is scratch base plus loop offset `x6`, `w2` is chunk length `w27`, and `x5` is the drive. This is a candidate copy boundary inside the synchronous worker. |
| Native acceptance | `0x2f0bc..0x2f0c4` rejects a negative result or a returned count different from the requested length. The loop advances only after exact success. Its native maximum chunk is 1 GiB; our smaller memory limit must decline unsupported transfers. |
| Backend byte ABI | PrFILE dispatch leads to `0x3cf6c..0x3cf80`, calling relocation-resolved `pf2_fwrite(data, 1, count, backing)`. The backing value is opaque, not a Linux descriptor. |
| Logical end | `0x2f11c..0x2f128` calls relocation-resolved `PrFILE_ftruncate(drive, slot, request[+0x42c])`. Alignment bytes beyond that logical length must be excluded. |
| Lifetime | `0x2ef80..0x2ef8c` releases scratch before the later async completion. Retaining that pointer until completion would be unsafe. |

The [collector](../camera_worker/sony_owned_image.h) copies readable chunks into
a dedicated caller-owned lease **before** their synchronous writes return. It
requires contiguous offsets, matching cookie and generated handle, exact write
counts, successful logical truncation and the
[identity ledger's](CAPTURE_FILE_IDENTITY.md) RecordStill/submission/real-close/
async-completion gates. Short writes, errors, missing gates, overlapping source
and destination memory, out-of-order chunks, repeats and capacity exhaustion
cannot release an image. Final logical length excludes padding.

An explicit `start_at` gate requires a successful **same-open zero-position**
observation before any chunk. Loop offset zero alone does not prove the backend
file position is zero. Recovering that observation is still a deployment gate;
fixtures supply it synthetically. Open flags and `fpstat` are not relabelled as
position proof.

The collector allocates nothing and performs no I/O. Its caller supplies at most
64 MiB of dedicated storage and retains that lease until consumption finishes.
The returned const byte view does not own or extend the storage lifetime. Use
one instance per tracked write and serialize access through consumer delivery.
Copying belongs in the verified FSYS worker scope, not the late completion
callback. Its memory/bandwidth cost still needs measurement.

## One frame selection, separate completed files

The native launcher now supports:

```text
photo-launcher --arm-frame WORKSPACE frame-73
photo-launcher --derive-file WORKSPACE frame-73 0 0
photo-launcher --derive-file WORKSPACE frame-73 1 3
```

These are interface examples, not installation commands. Prepare the frame arm
durably during idle time before binding its token to a verified capture request.
Do not run this CLI from a shutter callback.

`--arm-frame` freezes the LUT, lens, actual-aperture value and generation in a
parent with state `frame-armed`. `--derive-file` copies its frozen selection
into an exclusively created child such as `frame-73-g0-m0`. Group/media must
come from checked native identity, never arrival order. Two format groups and
four destinations are accepted; a repeated group/media cannot create a second
job. Parent IDs are bounded to 57 characters for the existing 63-character
derived-ID limit.

A global preset switch after frame arming has no effect on its children. Each
destination gets a distinct job while retaining the shutter selection. The
parent cannot accept a completed photo or run an encoder. Children use existing
durable ingress, digest/selection binding, worker lease and exclusive result
publication. `frame.txt` records lineage for inspection; the ready binding
still governs execution. An interrupted partial child is retained, never
automatically recycled.

## Offline prototype

[check_capture_handoff.py](check_capture_handoff.py) runs the native ledger and
collector with two writes from **one** selected recording object/frozen frame:
JPEG group 0/media 0 and HIF group 1/media 3. Each has its own cookie, handle,
slot and storage. The replay destroys scratch before completion, drops padding
and exports owned snapshots after all success gates. The launcher frame is
armed before replay; its actual generation passes through the ledger and is
checked on both identities.

Generated source pathnames are then removed. Surviving owned copies feed real
native JPEG and 10-bit HIF workers, with ExifTool metadata readback and FFmpeg
HIF decoding. Eight reverse-order child jobs cover all fan-out combinations
after global selection changes: each retains Classic Chrome, Helios 58 mm and
unknown shooting aperture. JPEG orientation 6 survives. No camera pathname is
reopened or hashed as identity proof.

These are synthetic source/zero-position contracts, not a Sony hook delivering
real bytes. HIF remains 10-bit HIF; existing SDR sRGB restrictions apply. HLG/PQ,
camera playback/database registration and full Adobe RAW/DCP rendering remain
outside this pass. See the [host](evidence/completed-handoff-checks.json),
[sanitizer](evidence/completed-handoff-sanitizers.json),
[ARM64](evidence/completed-handoff-arm64.json) and
[launcher regression](evidence/completed-handoff-launcher-regression.json) receipts.

The core passes **885 assertions** in each release-host, ASan/UBSan and ARM64
instruction-emulation build. Each host variant passes 12 new integration groups.
The [build receipt](evidence/completed-handoff-builds.json) records all six
launcher/core builds; [ARM64 launcher checks](evidence/completed-handoff-launcher-arm64.json)
cover startup/list/select only. Native child HEVC workers are release builds;
sanitizers cover the collector, launcher and its linked parser/codec libraries.

To reproduce using existing local codec prefixes and validated CLFs:

```sh
.venv-research/bin/python research/trace_encoded_write.py
.venv-research/bin/python -m camera_worker.build_sony_bridge --host \
  --output research/build/handoff-new/bridge
.venv-research/bin/python -m camera_worker.build_launcher --host \
  --codecs research/build/hif/host --output research/build/handoff-new/launcher
.venv-research/bin/python -m research.check_capture_handoff \
  --bridge research/build/handoff-new/bridge/sony-bridge-test-host \
  --launcher research/build/handoff-new/launcher/photo-launcher-host \
  --jpeg research/build/launcher-jpeg-host-publish/jpeg-worker-host \
  --hif research/build/hif/host/hif-worker-host \
  --fixture research/build/hif/host/heif-fixture \
  --luts research/build/fujifilm-srgb --work research/build/handoff-new/checks \
  --report research/build/handoff-new/checks.json
```

Use fresh output directories. These commands do not fetch dependencies, open a
device or produce an installer. Add `--host --sanitize` to both builders with
the matching fully instrumented codec prefix for the sanitizer variant; use
the existing Linux codec prefix for target builds as documented in each builder.

## Reference project: native film-style looks

The user-linked [QAQLeonard source](https://github.com/QAQLeonard/a7c2-fl23-research/tree/c046013c53883cd6f0fe660a9f5fec7a48d94722)
was read at `c046013c53883cd6f0fe660a9f5fec7a48d94722`; reviewed files are
hash-pinned in [the reference receipt](evidence/qaqleonard-custom-looks-review.json).
Only source retrieval and Git publication use the network. No upstream device
CLI, import preflight, installer or recovery command was run.

It supplies another route: fit DCP look/tone or a declared ST-input CUBE into
Sony's native 5×9×9×3 signed correction grid, 1,215 bytes per table. Six custom
ST-based slots use markers 249–254, checksums, an AP selector/name adaptation
and a CP native-table helper. A preview retry requests re-upload of cached
tables. This does not use our completed-photo worker or prove the MLUT
allocation-to-encoder link.

The author's [validation record](https://github.com/QAQLeonard/a7c2-fl23-research/blob/c046013c53883cd6f0fe660a9f5fec7a48d94722/docs/en/technical/validation.md)
reports photo/preview operation and an existing-six-look update/reload on one
Windows-connected body. RAW/HEIF/burst/video regressions remain future work.
The default bridge is a one-scene approximation, not complete DCP fidelity or
measured Fujifilm equivalence. That evidence does not establish HIF, AUTO look
selection, a Mac installation on our body or manual-lens EXIF support.

For native preview and film-style JPEG capture, evaluating that table route is
now a concrete alternative to finishing an on-camera HEVC worker. Our exact
RGBTable-to-sRGB conversion and its larger LUT are not drop-in native tables.
A later offline port needs an explicit Sony ST input model, bounded fitting
error, exact AP/CP compatibility review against our FL23 patches and preview
regressions. No upstream firmware code or installer was merged in this pass.

## Remaining hardware-prototype gates

1. A process-local nested/reentrant adapter at request/write/real-close sites,
   with owned observations, lock order, overflow/cancellation handling and
   quiescent teardown. No loader or trampoline is produced here.
2. Actual readable scratch bounds, same-open zero position and exclusion of
   concurrent seek/overwrite. Paths bypassing this loop and TNtfs remain
   excluded; a filename suffix cannot make them supported.
3. Pre-exposure identity across supported drive modes. If a prepared token,
   byte lease or identity is unavailable, decline processing while leaving Sony
   capture running.
4. Private writable storage, execution permissions, SDR provenance, idle/power
   policy and memory/bandwidth/thermal measurements. Offline ELF execution does
   not demonstrate capture coexistence.

The worker can now be exercised end to end with owned bytes and frozen frame
presets on the host. Automatic camera handoff remains gated on those native
observations and adapter work.
