"""Offline structural audit of the starting toolkit; no USB access.

The AP test executes only the published 112-byte converter window. This is
not full firmware emulation, a test of the pre-window special-value path,
or a reproduction of the historical 2,550-case suite.
"""
import json
from pathlib import Path
import re
import struct
import sys

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM
from unicorn.arm64_const import UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_LR, UC_ARM64_REG_PC

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import core


def require(condition, message):
    if not condition:
        raise ValueError(message)


def elf_bytes(elf, address, length):
    for segment in elf.iter_segments():
        if segment["p_type"] != "PT_LOAD":
            continue
        start = segment["p_vaddr"]
        if start <= address and address + length <= start + segment["p_filesz"]:
            elf.stream.seek(segment["p_offset"] + address - start)
            return elf.stream.read(length)
    raise ValueError(f"Unmapped ELF address: {address:#x}")


def native_audit(patches):
    reports = {}
    expected = {}
    for chip, program in enumerate(patches):
        for window in program["writes"]:
            key = chip, int(window["file_offset"], 16)
            expected[key] = (bytes.fromhex(window["before_hex"]),
                             bytes.fromhex(window["after_hex"]))
    for name in ["installer", "probe", "runtime", "wbi"]:
        with (ROOT / "assets" / f"{name}.so").open("rb") as file:
            elf = ELFFile(file)
            require(elf.elfclass == 64 and elf["e_machine"] == "EM_AARCH64",
                    f"Unexpected architecture: {name}")
            tags = list(elf.get_section_by_name(".dynamic").iter_tags())
            require(not any(t.entry.d_tag == "DT_NEEDED" for t in tags),
                    f"Dynamic dependency: {name}")
            require(any(t.entry.d_tag == "DT_INIT" for t in tags),
                    f"Missing loader entry: {name}")
            relocations = [rel for section in elf.iter_sections()
                           if section["sh_type"] == "SHT_RELA"
                           for rel in section.iter_relocations()]
            require(all(rel["r_info_type"] == 1027 for rel in relocations),
                    f"Non-relative relocation: {name}")
            reports[name] = {"architecture": "AArch64", "needed_libraries": [],
                             "relative_relocations": len(relocations)}
            if name != "installer":
                continue
            symbols = {s.name: s for s in elf.get_section_by_name(".symtab").iter_symbols()}

            def symbol_bytes(symbol):
                s = symbols[symbol]
                return elf_bytes(elf, s["st_value"], s["st_size"])

            table = symbols["windows"]
            raw = symbol_bytes("windows")
            fixups = {r["r_offset"]: r["r_addend"] for r in relocations}
            observed = {}
            for at in range(0, len(raw), 32):
                offset, length, chip, old_ptr, new_ptr = struct.unpack_from("<QIIQQ", raw, at)
                old_ptr = fixups.get(table["st_value"] + at + 16, old_ptr)
                new_ptr = fixups.get(table["st_value"] + at + 24, new_ptr)
                key = chip, offset
                require(key not in observed, "Duplicate native window")
                observed[key] = (elf_bytes(elf, old_ptr, length),
                                 elf_bytes(elf, new_ptr, length))
            require(observed == expected, "Native binary patch table differs from JSON")
            require(symbol_bytes("original_hashes").hex() == "".join(core.ORIGINAL),
                    "Native original hashes differ")
            require(symbol_bytes("final_hashes").hex() == "".join(core.FINAL),
                    "Native final hashes differ")
            reports[name]["program_sizes"] = list(struct.unpack("<QQ", symbol_bytes("file_sizes")))
            reports[name]["patch_windows_match_json"] = len(observed)
    # Also independently compare the C source table and all byte arrays.
    source = (ROOT / "native/patch_manifest.h").read_text()
    arrays = {name: bytes(int(v) for v in values.split(",") if v.strip())
              for name, values in re.findall(r"static const u8 (old_\d+|new_\d+)\[\]=\{([^}]*)\}", source)}
    source_windows = {}
    for offset, length, chip, old, new in re.findall(
            r"\{(\d+)UL,(\d+),(\d+),(old_\d+),(new_\d+)\}", source):
        key = int(chip), int(offset)
        require(key not in source_windows, "Duplicate C source window")
        require(len(arrays[old]) == len(arrays[new]) == int(length), "C window length mismatch")
        source_windows[key] = arrays[old], arrays[new]
    require(source_windows == expected, "C source patch table differs from JSON")
    reports["c_source_patch_windows_match_json"] = len(source_windows)
    return reports


def ap_audit(patches):
    window = patches[0]["writes"][0]
    start = int(window["file_offset"], 16)
    code = bytes.fromhex(window["after_hex"])
    stop = start + len(code)
    decoder = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    disassembly = [f"{i.address:#x}: {i.mnemonic} {i.op_str}" for i in decoder.disasm(code[:24], start)]
    wanted = [0, 3, 2, 1, 16, 15, 17, 18, 5, 13, 19, 20]
    require(list(code[24:36]) == wanted, "AP lookup table mismatch")
    values = list(range(256)) + [0x7fffffff, 0x80000000, 0xffffffff]
    for value in values:
        emulator = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
        emulator.mem_map(start & ~4095, 4096)
        emulator.mem_write(start, code)
        output = 0x200000
        emulator.mem_map(output, 4096)
        emulator.mem_write(output, b"\xa5")
        emulator.reg_write(UC_ARM64_REG_X1, value)
        emulator.reg_write(UC_ARM64_REG_X2, output)
        emulator.reg_write(UC_ARM64_REG_LR, stop)
        emulator.emu_start(start, stop, count=32)
        require(emulator.reg_read(UC_ARM64_REG_PC) == stop, "AP converter did not reach handoff")
        result = emulator.mem_read(output, 1)[0]
        require(result == (wanted[value] if value < 12 else 0xa5),
                f"AP converter failed for {value:#x}")
    return {"cases_passed": len(values), "valid_look_mapping": wanted,
            "invalid_inputs_leave_output_unchanged_at_handoff": True,
            "disassembly": disassembly,
            "scope": "112-byte AP window only; no original out-of-window code or hardware"}


def audit(allow_baseline_changes=False):
    core.assets()
    baseline = json.loads((ROOT / "research/baseline-inventory.json").read_text())
    changed=[]
    for row in baseline["files"]:
        digest=core.sha((ROOT / row["path"]).read_bytes())
        if digest!=row['sha256']:
            require(allow_baseline_changes,f"Starting file changed: {row['path']}")
            changed.append({'path':row['path'],'baseline_sha256':row['sha256'],'current_sha256':digest})
    vendor = []
    newline_only = []
    for row in baseline["files"]:
        if row["path"].startswith("vendor/pmca/"):
            rel = row["path"].removeprefix("vendor/")
            local = (ROOT / row["path"]).read_bytes()
            upstream = (ROOT / "research/upstream/Sony-PMCA-RE" / rel).read_bytes()
            require(local.replace(b"\r\n", b"\n") == upstream.replace(b"\r\n", b"\n"),
                    f"Vendored source content differs from upstream: {rel}")
            if local != upstream:
                newline_only.append(rel)
            vendor.append(rel)
    patches = core.read_json(ROOT / "assets/patches.json")
    stats = []
    for chip, program in enumerate(patches):
        require(program["source_sha256"] == core.ORIGINAL[chip], "JSON source hash mismatch")
        spans = []
        for window in program["writes"]:
            before, after = bytes.fromhex(window["before_hex"]), bytes.fromhex(window["after_hex"])
            require(len(before) == len(after) == window.get("bytes", len(before)), "Patch length mismatch")
            offset = int(window["file_offset"], 16)
            spans.append((offset, offset + len(before)))
        spans.sort()
        require(all(a[1] <= b[0] for a, b in zip(spans, spans[1:])), "Overlapping windows")
        stats.append({"program": core.PROGRAMS[chip], "windows": len(spans),
                      "covered_bytes": sum(end - start for start, end in spans),
                      "changed_bytes": sum(sum(a != b for a, b in zip(bytes.fromhex(w["before_hex"]), bytes.fromhex(w["after_hex"])))
                                           for w in program["writes"])})
    report = {"date": "2026-10-17", "baseline_commit": baseline["baseline_commit"],
              "baseline_files_unchanged": len(baseline["files"])-len(changed),
              "baseline_files_modified": changed,
              "vendored_files_match_pinned_upstream_after_newline_normalization": len(vendor),
              "vendored_files_with_newline_only_differences": newline_only,
              "deployment_assets_hash_verified": True, "patches": stats,
              "native": native_audit(patches), "ap_converter": ap_audit(patches),
              "camera_accessed": False, "complete_official_firmware_checked": False}
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    import argparse
    parser=argparse.ArgumentParser()
    parser.add_argument('--allow-baseline-changes',action='store_true',
                        help='Report intentional source revisions while auditing current assets')
    audit(parser.parse_args().allow_baseline_changes)
