"""Reproduce candidate program hashes and compare donor records offline.

Reads only the downloaded firmware tree. Does not import transport, send USB
requests, save settings, write candidate programs, or install anything.
"""
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import core


def require(condition, message):
    if not condition:
        raise ValueError(message)


def unique_file(directory, name):
    files = list(directory.rglob(name))
    require(len(files) == 1, f"Expected one {name} under {directory}, found {len(files)}")
    return files[0]


def record_source(path, data):
    return {"path": path.relative_to(ROOT).as_posix(), "bytes": len(data), "sha256": core.sha(data)}


def audit():
    target = ROOT / "research/extracted/ILCE-7CM2-2.01"
    donor = ROOT / "research/extracted/ILCE-1M2-4.00"
    report = {"date": "2026-10-17", "camera_accessed": False, "programs": [],
              "donor_programs": [], "gamma_records": [], "color_records": [],
              "target_factory_templates": [], "donor_factory_templates": []}
    for chip, name in enumerate(core.PROGRAMS):
        path = unique_file(target, name)
        original = path.read_bytes()
        candidate = core.patch_program(original, chip)
        require(core.patch_program(candidate, chip, reverse=True) == original,
                f"Reverse patch mismatch: {name}")
        report["programs"].append({**record_source(path, original),
                                   "candidate_sha256": core.sha(candidate),
                                   "expected_candidate_sha256": core.FINAL[chip],
                                   "reverse_restores_original_exactly": True})
        path = unique_file(donor, name)
        report["donor_programs"].append(record_source(path, path.read_bytes()))

    cp = unique_file(donor, "cpapp-b.bin").read_bytes()
    patches = core.read_json(ROOT / "assets/patches.json")
    region = next(w for w in patches[1]["writes"] if w["file_offset"] == "0x1100b80")
    blob = bytes.fromhex(region["after_hex"])
    for index in range(4):
        added = blob[index * 576:(index + 1) * 576]
        offset = cp.find(added[:512])
        require(offset >= 0 and cp.find(added[:512], offset + 1) == -1,
                f"Donor gamma prefix not unique: {index}")
        source = cp[offset:offset + 580]
        require(source[572:576] == b"\0" * 4, "Nonzero donor layout word")
        adapted = source[:572] + source[576:]
        differences = [{"offset": at, "donor": a, "target": b}
                       for at, (a, b) in enumerate(zip(adapted, added)) if a != b]
        require(len(differences) == 1 and differences[0]["offset"] in [539, 540],
                f"Unexpected donor gamma differences: {index}")
        report["gamma_records"].append({"payload_index": index,
            "donor_file_offset": hex(offset), "donor_record_bytes": 580,
            "target_record_bytes": 576, "curve_prefix_bytes_identical": 512,
            "removed_zero_word_at": "0x23c", "remaining_control_byte_difference": differences,
            "target_record_sha256": core.sha(added),
            "limit": "Control-byte meaning and full CP dispatch semantics not established by byte comparison"})

    colors = core.read_json(ROOT / "assets/colors.json")
    donor_templates = sorted((donor / "firmware.tar_unpacked/0110_backup").rglob("*.bin"))
    require(len(donor_templates) == 12, "Unexpected donor factory-template count")
    for path in donor_templates:
        raw = path.read_bytes()
        records = core.backup_records(raw)
        chroma, matrix = [records[1][key].data for key in core.CP_KEYS]
        for row in colors:
            parameter = row["parameter"] + 1
            slot = row["slot"]
            require(chroma[0x1200 + parameter] == matrix[0x196 + parameter] == slot,
                    f"Donor mapping mismatch: {path.name}")
            require(chroma[0x1250 + slot * 64:0x1250 + slot * 64 + 64] == bytes.fromhex(row["chroma"]),
                    f"Donor chroma mismatch: {path.name}")
            require(matrix[0x1e0 + slot * 768:0x1e0 + slot * 768 + 576] == bytes.fromhex(row["matrix"]),
                    f"Donor matrix mismatch: {path.name}")
        report["donor_factory_templates"].append({**record_source(path, raw),
                                                 "both_color_records_match_payload": True})
    for row in colors:
        report["color_records"].append({"target_parameter": row["parameter"],
            "donor_parameter": row["parameter"] + 1, "record_slot": row["slot"],
            "chroma_bytes": 64, "matrix_bytes": 576,
            "chroma_sha256": core.sha(bytes.fromhex(row["chroma"])),
            "matrix_sha256": core.sha(bytes.fromhex(row["matrix"]))})

    target_templates = sorted((target / "firmware.tar_unpacked/0110_backup").rglob("*.bin"))
    require(len(target_templates) == 12, "Unexpected target factory-template count")
    for path in target_templates:
        raw = path.read_bytes()
        records = core.backup_records(raw)
        units = [records[1][key].data for key in core.CP_KEYS]
        merged = core.merge_colors(*units)
        # Verify every byte outside the four new mappings and four records.
        for chip, (before, after) in enumerate(zip(units, merged)):
            allowed = set()
            for row in colors:
                parameter, slot = row["parameter"], row["slot"]
                mapping = 0x1200 + parameter if chip == 0 else 0x196 + parameter
                start = 0x1250 + slot * 64 if chip == 0 else 0x1e0 + slot * 768
                size = 64 if chip == 0 else 576
                allowed.add(mapping)
                allowed.update(range(start, start + size))
            require(len(before) == len(after) and
                    all(a == b or at in allowed for at, (a, b) in enumerate(zip(before, after))),
                    f"Unexpected merged byte: {path.name}")
        report["target_factory_templates"].append({**record_source(path, raw),
            "merge_passed": True, "all_bytes_outside_new_records_and_mappings_preserved": True})
    report["limits"] = ["Factory templates are not a physical body's saved calibration",
                         "No CP or imaging hardware execution",
                         "No fresh photo, color accuracy, reset-value or recovery validation"]
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    audit()
