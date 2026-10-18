"""Offline Sony BLE-client boundary and local InstantLink port requirements.

Reads firmware and source bytes only. It never imports InstantLink, opens a device,
loads a target library, scans Bluetooth, or executes a firmware instruction.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

from map_ap_interfaces import Image, LIB, ROOT


EXPECTED = {
    "libInfraBluetoothCore.so": "f58ec9d2b38982dd04ad4cc33b7646d220c3f8caba2ee818a99dcbfac25c45fc",
    "libInfraBluetoothLowEnergy.so": "bdeb26bb34dbbe3cb03adac0992a17dc4ffb5539ee95223dca0a6f54637fc7a1",
}
SOURCE_FILES = [
    "AGENTS.md", "Cargo.toml", "crates/instantlink-core/Cargo.toml",
    "crates/instantlink-core/src/transport.rs", "crates/instantlink-core/src/protocol.rs",
    "crates/instantlink-core/src/models.rs", "crates/instantlink-core/src/device.rs",
    "crates/instantlink-core/src/image.rs", "crates/instantlink-core/src/commands.rs",
    "crates/instantlink-ffi/include/instantlink.h", "bridge/pyproject.toml",
    "bridge/src/instantlink_bridge/camera/ftp.py", "bridge/src/instantlink_bridge/app.py",
    "bridge/scripts/build-firmware-bundle.sh", "docs/reference/protocol.md",
]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def git(repo, *args):
    # Local object/index access only; no fetch, remote, credential or network calls.
    return subprocess.check_output(["git", "-C", str(repo), *args])


def symbol(image, name):
    return next(s for s in image.symbols
                if s.name == name and s["st_shndx"] != "SHN_UNDEF")


def region(image, name):
    s = symbol(image, name)
    start, length = s["st_value"], s["st_size"]
    data = image.bytes_at(start, length)
    offset = next(seg["p_offset"] + start - seg["p_vaddr"]
                  for seg in image.elf.iter_segments()
                  if seg["p_type"] == "PT_LOAD"
                  and seg["p_vaddr"] <= start < seg["p_vaddr"] + seg["p_filesz"])
    return {"source": image.path.name, "symbol": name,
            "start_va": hex(start), "file_offset": hex(offset),
            "bytes": length, "sha256": digest(data), "raw_hex": data.hex(),
            "instructions": image.disasm(start, start + length)}


def locate(text, token):
    at = text.index(token)
    return {"line": text[:at].count("\n") + 1, "token": token}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--instantlink", type=Path, default=ROOT.parent / "InstantLink")
    args = parser.parse_args()
    repo = args.instantlink.resolve()
    images = {name: Image(LIB / name) for name in EXPECTED}
    sources = []
    for name, image in images.items():
        assert digest(image.blob) == EXPECTED[name], name
        assert image.elf["e_machine"] == "EM_AARCH64"
        dynamic = image.elf.get_section_by_name(".dynamic")
        sources.append({"path": str(image.path.relative_to(ROOT)),
                        "bytes": len(image.blob), "sha256": EXPECTED[name],
                        "needed": [t.needed for t in dynamic.iter_tags()
                                   if t.entry.d_tag == "DT_NEEDED"]})
    core = images["libInfraBluetoothCore.so"]
    le = images["libInfraBluetoothLowEnergy.so"]
    request_specs = [("BSA_BleClConnect", 0x6aa, 8),
                     ("BSA_BleClWrite", 0x6ae, 0xf4),
                     ("BSA_BleClNotifRegister", 0x6b1, 0x40)]
    regions, requests = [], []
    for name, opcode, size in request_specs:
        r = region(core, name)
        regions.append(r)
        ins = r["instructions"]
        boundary = next(i for i in ins if i["mnemonic"] == "b"
                        and "bsa_send_message_receive_status@plt" in i.get("symbols", []))
        for operands in ["x1, x20", f"w2, #{hex(size) if size > 9 else size}",
                         f"w0, #{hex(opcode)}"]:
            assert any(i["mnemonic"] == "mov" and i["operands"] == operands for i in ins)
        requests.append({"producer": name, "producer_va": r["start_va"],
                         "message_opcode": hex(opcode), "request_bytes": size,
                         "request_pointer_assignment": "x1 = x20 (original x0)",
                         "boundary_branch": boundary,
                         "scope": "Static request producer; no server or controller executed."})
    le_names = [
        "_ZN29InfraBluetoothLowEnergyCommon9ClConnectEjPFvNS_17ST_MSG_ASYNC_COMPEE",
        "_ZN29InfraBluetoothLowEnergyCommon19pvtCoreBleClConnectEPc",
        "_ZN29InfraBluetoothLowEnergyCommon11ClRegNotifyEjNS_10ST_BT_UUIDE",
    ]
    # Exact names of the longer read/write signatures are taken from the pinned
    # symbol table, avoiding speculative reconstruction of C++ parameter layouts.
    le_names += [s.name for s in le.symbols if s["st_shndx"] != "SHN_UNDEF"
                 and (s.name.startswith("_ZN29InfraBluetoothLowEnergyCommon7ClWriteE")
                      or s.name.startswith("_ZN29InfraBluetoothLowEnergyCommon6ClReadE"))]
    assert len(le_names) == 5
    regions += [region(le, name) for name in le_names]
    exported = []
    for image in [core, le]:
        for s in image.symbols:
            if s["st_shndx"] == "SHN_UNDEF":
                continue
            if (s.name.startswith("BSA_BleCl")
                    or s.name.startswith("_Z19sony_ble_cl_connectP")
                    or s.name.startswith("_Z24sony_ble_cl_connect_initP")
                    or s.name in le_names):
                exported.append({"source": image.path.name, "symbol": s.name,
                                 "va": hex(s["st_value"]), "bytes": s["st_size"]})
    imported_calls = {(r["source"], i["target"])
                      for r in regions for i in r["instructions"]
                      if i.get("symbols") and any(s.endswith("@plt") for s in i["symbols"])}
    relocations = []
    for image in [core, le]:
        plt, rel = image.elf.get_section_by_name(".plt"), image.elf.get_section_by_name(".rela.plt")
        syms = image.elf.get_section(rel["sh_link"])
        for n, item in enumerate(rel.iter_relocations()):
            addr = plt["sh_addr"] + 32 + n * 16
            if (image.path.name, hex(addr)) in imported_calls:
                relocations.append({"source": image.path.name, "plt_va": hex(addr),
                                    "relocation_offset": hex(item["r_offset"]),
                                    "relocation_type": item["r_info_type"],
                                    "symbol": syms.get_symbol(item["r_info_sym"]).name})
    commit = git(repo, "rev-parse", "HEAD").decode().strip()
    texts, local_sources = {}, []
    for name in SOURCE_FILES:
        data = (repo / name).read_bytes()
        texts[name] = data.decode()
        committed = git(repo, "show", f"HEAD:{name}")
        local_sources.append({"path": name, "bytes": len(data), "sha256": digest(data),
                              "matches_head_blob": data == committed})
        assert data == committed, f"Uncommitted source needs separate provenance: {name}"
    protocol = texts["crates/instantlink-core/src/protocol.rs"]
    models = texts["crates/instantlink-core/src/models.rs"]
    square = re.search(r"PrinterModel::Square => ModelSpec \{(.*?)\n\s*\},", models, re.S).group(1)
    values = {key: int(re.search(rf"\b{key}: ([\d_]+)", square).group(1).replace("_", ""))
              for key in ["width", "height", "chunk_size", "max_image_size",
                          "packet_delay_ms", "pre_execute_delay_ms", "success_code"]}
    assert values == {"width": 800, "height": 800, "chunk_size": 1808,
                      "max_image_size": 105000, "packet_delay_ms": 150,
                      "pre_execute_delay_ms": 1000, "success_code": 12}
    assert "pub const MTU_SIZE: usize = 182;" in protocol
    assert "pub const HEADER: [u8; 2] = [0x41, 0x62];" in protocol
    assert "pub const RESPONSE_HEADER: [u8; 2] = [0x61, 0x42];" in protocol
    transport = texts["crates/instantlink-core/src/transport.rs"]
    uuids = {key: re.search(rf"pub const {key}: Uuid = Uuid::from_u128\(([^)]+)\)", transport).group(1)
             for key in ["SERVICE_UUID", "WRITE_CHAR_UUID", "NOTIFY_CHAR_UUID"]}
    anchors = {
        "crates/instantlink-core/src/device.rs": ["transport: Box<dyn Transport>", "async fn print_prepared_unlocked", "async fn send_image_data"],
        "crates/instantlink-core/src/transport.rs": ["pub trait Transport", "WriteType::WithoutResponse", "D-Bus connection"],
        "crates/instantlink-core/src/image.rs": ["pub fn resize_image", "pub fn encode_jpeg", "fn prepare_image_inner"],
        "bridge/src/instantlink_bridge/camera/ftp.py": ["def on_file_received"],
        "bridge/src/instantlink_bridge/app.py": ["Run one FTP-received image through the auto-print flow."],
    }
    out = {"format": 1, "purpose": "Offline camera-native Instax transport feasibility",
           "method": "Hash-pinned ELF symbols, PLT relocations and Capstone disassembly; local source text inspection. No emulation or runtime imports.",
           "camera_accessed": False, "printer_accessed": False, "network_accessed": False,
           "target_executed": False, "hardware_operations": 0,
           "address_model": "ELF virtual addresses for exact firmware sources; not observed runtime addresses.",
           "generator": {"path": str(Path(__file__).relative_to(ROOT)), "sha256": digest(Path(__file__).read_bytes()),
                         "support": {"path": "research/map_ap_interfaces.py",
                                     "sha256": digest((ROOT / "research/map_ap_interfaces.py").read_bytes())}},
           "firmware_sources": sources, "exported_candidates": exported,
           "request_producers": requests, "regions": regions, "call_relocations": relocations,
           "instantlink": {"repository_hint": "../InstantLink (or --instantlink PATH)", "commit": commit,
                           "sources": local_sources, "square_model": values,
                           "ble_uuid_source_literals": uuids, "current_ble_fragment_bytes": 182,
                           "request_header_hex": "4162", "response_header_hex": "6142",
                           "source_anchors": {name: [locate(texts[name], token) for token in tokens]
                                              for name, tokens in anchors.items()}},
           "counts": {"firmware_sources": len(sources), "instruction_regions": len(regions),
                      "instructions": sum(len(r["instructions"]) for r in regions),
                      "request_producers": len(requests), "local_source_files": len(local_sources)},
           "unknowns": ["Sony BLE client initialization, message-server lifecycle, request/UUID layouts, callback and concurrency contracts remain incomplete.",
                        "Exports and request producers do not prove arbitrary printer UUIDs are accepted or that the controller connects in the relevant camera mode.",
                        "The existing btleplug/BlueZ/D-Bus backend is not established as available on the camera; an ARM64 build alone is insufficient.",
                        "Sony ATT payload/MTU, write type, CCCD subscription and coexistence with phone/remote services require future explicit validation.",
                        "Worker ABI (Linux 4.9/glibc2.25), launch/stop/resume lifecycle, completed-file notification, memory and UI integration remain prototype work.",
                        "Current 182-byte fragmentation is an InstantLink implementation constant, not a measured camera/printer negotiation.",
                        "No JPEG decoding, image conversion, BLE operation, print, saved-still LUT effect, or camera-installed prototype is demonstrated."]}
    output = ROOT / "research/evidence/printer-transport.json"
    output.write_text(json.dumps(out, indent=2) + "\n")
    print(json.dumps(out["counts"], indent=2))


if __name__ == "__main__":
    main()
