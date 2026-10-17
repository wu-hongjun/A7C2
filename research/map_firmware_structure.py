#!/usr/bin/env python3
"""Read-only structural map of the pinned A7C II 2.01 extraction.

Ordinary Python/ELF/archive analysis, not a REA Evidence producer. Never executes
firmware and never opens USB. Reconstructs symlinks from source ext2 metadata
because fwtool's writeFileTree deliberately emits only regular files/directories.
"""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import stat
import struct
import sys
import tarfile
import zlib

REPO = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO / "research/upstream/fwtool.py"))
from fwtool.archive.ext2 import readExt2
from fwtool.sony.fdat import FdatHeader, FdatFileSystemHeader, readFdat
from elftools.elf.elffile import ELFFile
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN


def sha(path):
    h = hashlib.sha256()
    with path.open("rb") as f:
        for b in iter(lambda: f.read(1024 * 1024), b""):
            h.update(b)
    return h.hexdigest()


def identity(path):
    return {"path": str(path.relative_to(REPO)), "bytes": path.stat().st_size,
            "sha256": sha(path)}


def image_info(path):
    info = identity(path)
    with path.open("rb") as f:
        d = f.read(4096)
    if d[1080:1082] == b"\x53\xef":
        blocks, = struct.unpack_from("<I", d, 1028)
        exponent, = struct.unpack_from("<I", d, 1048)
        info.update(format="ext2", filesystem_bytes=blocks * (1024 << exponent),
                    block_bytes=1024 << exponent, superblock_offset=1024)
    elif d[:4] == b"hsqs":
        info.update(format="squashfs", version=list(struct.unpack_from("<2H", d, 28)),
                    inode_count=struct.unpack_from("<I", d, 4)[0],
                    block_bytes=struct.unpack_from("<I", d, 12)[0],
                    filesystem_bytes=struct.unpack_from("<Q", d, 40)[0])
    elif d[54:62] in [b"FAT12   ", b"FAT16   "]:
        sector_size, = struct.unpack_from("<H", d, 11)
        sectors, = struct.unpack_from("<H", d, 19)
        if not sectors:
            sectors, = struct.unpack_from("<I", d, 32)
        info.update(format=d[54:62].decode().strip(), sector_bytes=sector_size,
                    sectors=sectors, filesystem_bytes=sector_size * sectors,
                    sectors_per_cluster=d[13])
    elif d[:4] == b"WBI2":
        info["format"] = "Sony WBI2 (undecoded)"
    else:
        info["format"] = "unrecognized"
    return info


def strings(data, regex):
    return [{"file_offset": m.start(), "text": m.group().decode("ascii")}
            for m in re.finditer(rb"[ -~]{6,}", data)
            if re.search(regex, m.group().decode("ascii"))]


def ext_inventory(path):
    entries = []
    with path.open("rb") as f:
        for item in readExt2(f):
            kind = ("directory" if stat.S_ISDIR(item.mode) else "regular" if
                    stat.S_ISREG(item.mode) else "symlink" if stat.S_ISLNK(item.mode)
                    else "special")
            row = {"path": item.path, "kind": kind, "mode": oct(item.mode),
                   "uid": item.uid, "gid": item.gid, "bytes": item.size}
            if kind == "symlink":
                row["target"] = item.contents.read().decode("ascii")
            entries.append(row)
    return {"source": identity(path), "counts": dict(collections.Counter(
            e["kind"] for e in entries)), "entries": entries}


def elf_info(path):
    with path.open("rb") as f:
        if f.read(4) != b"\x7fELF":
            return None
        f.seek(0)
        elf = ELFFile(f)
        dynamic = elf.get_section_by_name(".dynamic")
        needed = []
        if dynamic:
            needed = [t.needed for t in dynamic.iter_tags()
                      if t.entry.d_tag == "DT_NEEDED"]
        sym = elf.get_section_by_name(".dynsym")
        imports, exports = [], []
        if sym:
            for s in sym.iter_symbols():
                if not s.name:
                    continue
                if s.entry.st_shndx == "SHN_UNDEF":
                    imports.append(s.name)
                else:
                    exports.append({"name": s.name, "va": s.entry.st_value,
                                    "bytes": s.entry.st_size})
        info = {"path": str(path.relative_to(REPO)), "bytes": path.stat().st_size,
                "machine": elf.header.e_machine, "class_bits": elf.elfclass,
                "type": elf.header.e_type, "entry_va": elf.header.e_entry,
                "has_symtab": elf.get_section_by_name(".symtab") is not None,
                "dynamic_import_count": len(imports),
                "dynamic_export_count": len(exports), "needed": needed}
        modinfo = elf.get_section_by_name(".modinfo")
        if modinfo:
            info["module_info"] = [x.decode("ascii", "replace")
                                   for x in modinfo.data().split(b"\0") if x]
        if path.name in {"im.elf", "bootin.elf", "libIMDB.so", "appFw.so"}:
            info["sha256"] = sha(path)
            info["imports"] = imports
            if path.name != "appFw.so":
                info["exports"] = exports
            else:
                info["exports"] = [s for s in exports
                                   if s["name"] == "Application_System_Init"]
            interp = next((s.get_interp_name() for s in elf.iter_segments()
                           if s.header.p_type == "PT_INTERP"), None)
            info["interpreter"] = interp
        return info


def imdb_registry(path):
    d = path.read_bytes()
    with path.open("rb") as f:
        elf = ELFFile(f)
        syms = elf.get_section_by_name(".dynsym")
        sym = next(s for s in syms.iter_symbols() if s.name == "imdb_raw")
        sec = elf.get_section(sym.entry.st_shndx)
        base = sym.entry.st_value
        offset = base - sec.header.sh_addr + sec.header.sh_offset
        ro = elf.get_section_by_name(".rodata")
        lo, hi = ro.header.sh_addr, ro.header.sh_addr + ro.header.sh_size
        def resolve(va):
            if not va:
                return None
            if lo <= va < hi:
                p = va - lo + ro.header.sh_offset
                return d[p:d.index(b"\0", p)].decode("ascii")
            return {"unresolved_va": va}
        # Instruction-derived: IMDB_get_entries adds 0x100 to imdb_raw;
        # IMDB_find_entry adds 0x48 per record, stopping at first word 0x28.
        entries = []
        for off in range(offset + 0x100, offset + sym.entry.st_size, 0x48):
            words = struct.unpack_from("<4I7Q", d, off)
            row = {"ordinal": len(entries), "file_offset": off,
                   "va": off - offset + base, "raw_u32_fields": list(words[:4]),
                   "raw_pointer_fields": list(words[4:]),
                   "resolved_strings": [resolve(p) for p in words[4:]]}
            entries.append(row)
            if words[0] == 0x28:
                break
        md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
        asm = [{"va": i.address, "mnemonic": i.mnemonic, "operands": i.op_str}
               for i in md.disasm(d[0xa1f0:0xa2e4], 0xa1f0)]
        return {"source": identity(path), "symbol_va": base, "file_offset": offset,
                "symbol_bytes": sym.entry.st_size, "record_start_delta": 0x100,
                "record_stride": 0x48, "entries": entries,
                "instruction_evidence": asm,
                "limits": "Record stride/start/sentinel established from instructions; "
                "unlabeled flags and pointer-field semantics remain partly unresolved. "
                "Registry presence is not proof every module loads on this camera."}


def dtb_inventory(path):
    d = path.read_bytes()
    hdr = struct.unpack_from(">10I", d)
    assert hdr[0] == 0xd00dfeed
    off_struct, off_strings = hdr[2], hdr[3]
    pos = off_struct
    nodes, stack = [], []
    while pos < off_struct + hdr[9]:
        tok, = struct.unpack_from(">I", d, pos)
        pos += 4
        if tok == 1:
            end = d.index(0, pos)
            stack.append(d[pos:end].decode("ascii"))
            pos = (end + 4) & ~3
        elif tok == 2:
            stack.pop()
        elif tok == 3:
            size, nameoff = struct.unpack_from(">2I", d, pos)
            pos += 8
            end = d.index(0, off_strings + nameoff)
            name = d[off_strings + nameoff:end].decode("ascii")
            value = d[pos:pos + size]
            if name in {"model", "compatible", "device_type", "reg", "status",
                        "bootargs", "clock-frequency", "#address-cells", "#size-cells"}:
                row = {"node": "/".join(stack), "property": name,
                       "value_file_offset": pos, "hex": value.hex()}
                if all(32 <= c <= 126 or c == 0 for c in value):
                    row["strings"] = value.rstrip(b"\0").decode("ascii").split("\0")
                nodes.append(row)
            pos = (pos + size + 3) & ~3
        elif tok == 9:
            break
        elif tok != 4:
            raise ValueError((tok, pos))
    return {"source": identity(path), "header_u32_be": list(hdr),
            "selected_properties": nodes}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--output", type=Path, default=REPO / "research/evidence/firmware-structure.json")
    args = ap.parse_args()
    root = REPO / "research/extracted/ILCE-7CM2-2.01"
    fw = root / "firmware.tar_unpacked"
    dev = fw / "0700_part_image/dev"
    body = REPO / "research/firmware/ILCE-7CM2-2.01/BODYDATA.DAT"
    result = {"schema": 1, "method": "Ordinary offline Python, pinned fwtool parsers, pyelftools, Capstone; not REA Evidence",
              "camera_access": False, "target_execution": False,
              "source": identity(body), "tool": identity(Path(__file__)),
              "fwtool_source_commit": "3955d7890fc38daf942bb52827599bfe35de4a14"}
    with body.open("rb") as f:
        assert f.read(8) == b"\x89UFU\r\n\x1a\n"
        chunks = []
        crc = zlib.crc32(b"\x89UFU\r\n\x1a\n")
        while True:
            off = f.tell()
            header = f.read(8)
            size, tag = struct.unpack(">I4s", header)
            row = {"type": tag.decode(), "header_offset": off,
                   "data_offset": off + 8, "bytes": size}
            if tag == b"DEND":
                expected, = struct.unpack(">I", f.read(size))
                result["dat_crc32"] = {"expected": expected,
                                       "computed": crc & 0xffffffff,
                                       "valid": expected == crc & 0xffffffff}
                chunks.append(row)
                break
            crc = zlib.crc32(header, crc)
            remaining = size
            buf = bytearray()
            while remaining:
                block = f.read(min(1048576, remaining))
                if not block:
                    raise ValueError("Truncated DAT")
                crc = zlib.crc32(block, crc)
                remaining -= len(block)
                if size < 1000:
                    buf.extend(block)
            if buf:
                row["hex"] = buf.hex()
            chunks.append(row)
        result["dat_chunks"] = chunks
    with (root / "firmware.fdat").open("rb") as f:
        header = FdatHeader.unpack(f)
        parsed = readFdat(f)  # Includes pinned-parser header CRC validation.
        result["fdat"] = {"source": identity(root / "firmware.fdat"),
                          "model": parsed.model, "region": parsed.region,
                          "version": parsed.version, "header_bytes": FdatHeader.size,
                          "firmware_offset": header.firmwareOffset,
                          "firmware_bytes": header.firmwareSize,
                          "filesystem_count": header.numFileSystems,
                          "filesystem_headers": []}
        for i in range(header.numFileSystems):
            h = FdatFileSystemHeader.unpack(header.fileSystemHeaders, i * FdatFileSystemHeader.size)
            result["fdat"]["filesystem_headers"].append({
                "mode": h.modeType.decode("ascii", "replace"),
                "offset": h.offset, "bytes": h.size})
    result["extraction_config"] = (root / "config.yaml").read_text()
    with tarfile.open(root / "firmware.tar") as t:
        result["firmware_tar"] = {"source": identity(root / "firmware.tar"), "entries": [
            {"name": m.name, "bytes": m.size, "header_offset": m.offset,
             "data_offset": m.offset_data, "type": m.type.decode("ascii")}
            for m in t.getmembers()]}
    result["images"] = [image_info(p) for p in sorted(dev.iterdir()) if p.is_file()]
    result["images"].append(image_info(root / "updater.img"))
    result["ext2_metadata"] = [ext_inventory(dev / p) for p in [
        "nflasha7", "nflasha15", "nflasha3_unpacked/initrd.img"]]
    result["extraction_limit"] = "fwtool.py writeFileTree skips symlinks and special files; ext2_metadata recovers their names/types/targets. Extracted host file modes do not represent original mode bits."
    textfiles = [fw / "0300_partconf/partinf.conf", fw / "0100_config/mount.conf",
                 fw / "0800_appli/setting/kemco.txt", fw / "0800_appli/setting/development.txt",
                 dev / "nflasha7_unpacked/sbin/init", dev / "nflasha7_unpacked/sbin/init2",
                 dev / "nflasha7_unpacked/root/chassis",
                 dev / "nflasha7_unpacked/version.txt",
                 dev / "nflasha8_unpacked/tomco.txt",
                 root / "updater.img_unpacked/config/config.xml",
                 root / "updater.img_unpacked/config/body_version",
                 root / "updater.img_unpacked/bin/startupdate.sh",
                 root / "updater.img_unpacked/bin/us_mkfs.sh"]
    textfiles += sorted((dev / "nflasha7_unpacked/sbin/initrc").iterdir())
    result["text_sources"] = [{**identity(p), "lines": p.read_text(errors="replace").splitlines()}
                              for p in textfiles if p.is_file()]
    part = (fw / "0300_partconf/partinf.conf").read_text(errors="replace")
    partitions = []
    for m in re.finditer(r"# (/dev/nflasha\d+)[^\n]*\n(0x[^\n]+)", part):
        vals = [int(v, 16) for v in m[2].split(",")]
        partitions.append({"device": m[1], "comment": m[0].splitlines()[0],
                           "start_sector": vals[0], "sectors": vals[1],
                           "start_bytes_512": vals[0] * 512,
                           "bytes_512": vals[1] * 512, "type": vals[2], "flag": vals[3]})
    result["partitions"] = partitions
    kernel = dev / "nflasha3_unpacked/vmlinux.bin"
    kd = kernel.read_bytes()
    result["kernel"] = {"source": identity(kernel), "image_header_first_64_bytes": kd[:64].hex(),
                        "image_text_offset_u64": struct.unpack_from("<Q", kd, 8)[0],
                        "image_size_u64": struct.unpack_from("<Q", kd, 16)[0],
                        "image_flags_u64": struct.unpack_from("<Q", kd, 24)[0],
                        "banner": strings(kd, r"^Linux version"),
                        "ikconfig_start_offset": kd.find(b"IKCFG_ST"),
                        "ikconfig_end_offset": kd.find(b"IKCFG_ED"),
                        "limits": "No IKCFG markers found in this exact uncompressed Image. This does not prove the running camera lacks /proc/config.gz."}
    wbi = dev / "nflasha5"
    with wbi.open("rb") as f:
        wh = f.read(256)
    result["wbi"] = {"source": identity(wbi), "magic": wh[:4].decode(),
                     "header_first_256_bytes": wh.hex(),
                     "pinned_fwtool_parser_supports": "WBI1 only, version 0x20060224",
                     "status": "WBI2 payload retained intact; compressed memory sections not decoded"}
    result["dtb"] = dtb_inventory(fw / "0800_appli/setting/cxd90057.dtb")
    inventory, elf = [], []
    for p in sorted(root.rglob("*")):
        if p.is_file():
            inventory.append(identity(p))
            e = elf_info(p)
            if e:
                elf.append(e)
    result["regular_file_inventory"] = inventory
    result["regular_file_count"] = len(inventory)
    result["elf_inventory"] = elf
    result["elf_summary"] = {"count": len(elf), "machines": dict(collections.Counter(
        e["machine"] for e in elf)), "types": dict(collections.Counter(e["type"] for e in elf)),
        "with_symtab": sum(e["has_symtab"] for e in elf),
        "kernel_module_count": sum("module_info" in e for e in elf)}
    result["imdb_registry"] = imdb_registry(dev / "nflasha15_unpacked/lib/libIMDB.so")
    result["boot_chain_string_leads"] = []
    for name in ["bootin.elf", "im.elf"]:
        p = dev / "nflasha15_unpacked/bin" / name
        result["boot_chain_string_leads"].append({"source": identity(p), "strings": strings(
            p.read_bytes(), r"/usr/bin/im\.elf|/usr/lib/libObj\.so|app_argv|/proc/udm/upm_bootmode")})
    bp = dev / "nflasha15_unpacked/bin/bootin.elf"
    bd = bp.read_bytes()
    result["bootin_main_entry"] = {"source": identity(bp), "entry_va": 0x4018f8,
        "main_va": struct.unpack_from("<Q", bd, 0x1928)[0],
        "literal_file_offset": 0x1928,
        "instructions": [{"va": i.address, "mnemonic": i.mnemonic, "operands": i.op_str}
            for i in Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN).disasm(bd[0x18f8:0x1928], 0x4018f8)],
        "basis": "_start loads first __libc_start_main argument from literal 0x401928"}
    ip = dev / "nflasha15_unpacked/bin/im.elf"
    idata = ip.read_bytes()
    md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    md.skipdata = True  # Literal pools inside .text must not terminate the scan.
    with ip.open("rb") as f:
        ie = ELFFile(f)
        dynsym = ie.get_section_by_name(".dynsym")
        plt = ie.get_section_by_name(".plt")
        rel = ie.get_section_by_name(".rela.plt")
        targets = {}
        for n, r in enumerate(rel.iter_relocations()):
            name = dynsym.get_symbol(r.entry.r_info_sym).name
            if name in {"dlopen", "dlsym", "IMDB_get_entries", "IMDB_find_entry", "__libc_start_main"}:
                targets[plt.header.sh_addr + 32 + 16 * n] = name
        text = ie.get_section_by_name(".text")
        ins = list(md.disasm(text.data(), text.header.sh_addr))
        anchors = []
        for n, i in enumerate(ins):
            if i.mnemonic in {"bl", "b"} and i.op_str.startswith("#"):
                target = int(i.op_str[1:], 16)
                if target in targets:
                    prologues = [v.address for v in ins[:n]
                                 if v.mnemonic == "stp" and v.op_str.startswith("x29, x30, [sp, #-")]
                    anchors.append({"call_va": i.address, "target": targets[target],
                        "target_plt_va": target,
                        "nearest_prior_prologue_candidate_va": prologues[-1] if prologues else None,
                        "context": [{"va": c.address, "mnemonic": c.mnemonic, "operands": c.op_str}
                                    for c in ins[max(0, n - 6):n + 4]]})
    result["im_dispatch_anchors"] = {"source": identity(ip), "entry_va": 0x402b30,
        "main_va": struct.unpack_from("<Q", idata, 0x2b60)[0],
        "main_literal_file_offset": 0x2b60, "plt_targets": targets,
        "call_anchors": anchors,
        "limits": "AArch64 ELF PLT mapping plus direct bl/b scan only. Nearest prior standard frame prologue is a candidate, not authoritative function boundary. Indirect call targets are not enumerated."}
    assert result["dat_crc32"]["valid"]
    assert len(partitions) == 30
    assert kd[56:60] == b"ARM\x64"
    assert result["wbi"]["magic"] == "WBI2"
    assert result["imdb_registry"]["entries"][151]["resolved_strings"][:2] == ["appFw.so", "Application_System_Init"]
    assert result["imdb_registry"]["entries"][-1]["raw_u32_fields"][0] == 0x28
    assert result["im_dispatch_anchors"]["main_va"] == 0x402490
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps({"output": str(args.output), "regular_files": len(inventory),
                      "elf_summary": result["elf_summary"], "partitions": len(partitions),
                      "imdb_records_including_sentinel": len(result["imdb_registry"]["entries"])}))


if __name__ == "__main__":
    main()
