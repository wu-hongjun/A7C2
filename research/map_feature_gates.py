"""Offline evidence for Creative Look controls and candidate EXIF API gaps.

Reads pinned local ELF files and emits instruction evidence only. No network,
USB, subprocess, target execution, camera calls, or external-project traversal.
Addresses are original ELF virtual addresses, without a runtime load bias.
"""

import hashlib
import json
import struct
from pathlib import Path

from map_ap_interfaces import Image, LIB

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "research/evidence/feature-gates.json"
EXPECTED = {
    "appFw.so": "2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b",
    "libmpr.so": "4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9",
}


def identity(image):
    digest = hashlib.sha256(image.blob).hexdigest()
    assert digest == EXPECTED[image.path.name], (image.path.name, digest)
    return {
        "path": str(image.path.relative_to(ROOT)),
        "bytes": len(image.blob),
        "sha256": digest,
    }


def region(image, label, start, end, extent_basis):
    raw = image.bytes_at(start, end - start)
    return {
        "label": label,
        "source": image.path.name,
        "address_space": "ELF virtual address without runtime load bias",
        "start": hex(start),
        "end_exclusive": hex(end),
        "extent_basis": extent_basis,
        "sha256": hashlib.sha256(raw).hexdigest(),
        "bytes_hex": raw.hex(),
        "instructions": image.disasm(start, end),
    }


def main():
    app = Image(LIB / "appFw.so")
    mpr = Image(LIB / "libmpr.so")
    sources = [identity(app), identity(mpr)]

    option_jumps = app.bytes_at(0x41624D0, 8)
    assert option_jumps.hex() == "000f13171b1f2327"
    option_targets = [0x21AE028 + n * 4 for n in struct.unpack("8b", option_jumps)]
    assert option_targets == [0x21AE028, 0x21AE064, 0x21AE074, 0x21AE084,
                              0x21AE094, 0x21AE0A4, 0x21AE0B4, 0x21AE0C4]
    selector_table = app.bytes_at(0x4162580, 17)

    stubs = []
    for symbol, address in [
        ("_ZN5MprIf11SetExifInfoEbPN3MPR10BufferAreaE", 0x2C8CC8),
        ("_ZN5MprIf14ModifyExifInfoEmN3MPR7EntryIdEPvjPNS0_7GpsInfoEPNS0_15ExifSrcInfoAreaE", 0x2C9258),
    ]:
        matching = [s for s in mpr.symbols if s.name == symbol]
        assert len(matching) == 1
        assert matching[0]["st_value"] == address
        assert matching[0]["st_shndx"] != "SHN_UNDEF"
        assert mpr.bytes_at(address, 8).hex() == "00008052c0035fd6"
        stubs.append({
            "symbol": symbol,
            "address": hex(address),
            "exported_size": matching[0]["st_size"],
            "observed_behavior": "Return integer zero immediately; no metadata operation in this body.",
            "interpretation_limit": "No claim about zero's API status semantics or alternate private implementations.",
        })

    report = {
        "format": 1,
        "purpose": "offline_feature_gates_and_named_exif_api_gap",
        "generator": {"path": str(Path(__file__).relative_to(ROOT)),
                      "sha256": hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},
        "sources": sources,
        "safety_scope": {
            "camera_operations": 0, "target_execution": False,
            "network_operations": 0, "external_project_traversal": False,
        },
        "creative_look_option_gate": {
            "entry": "0x21ade88",
            "naming_literal": {
                "address": "0x4162a88",
                "text": app.read_string(0x4162A88),
                "references": ["0x21adeec/0x21adef4", "0x21adf88/0x21adf90"],
            },
            "selected_look_index_chain": ["0x21ad98c", "0x1b6eb2c", "0x2d508c0"],
            "controller_subselection_helper": "0x21adba4",
            "subselection_table": {
                "address": "0x4162580", "bytes_hex": selector_table.hex(),
                "input_range": [15, 31], "outside_range_result": 1,
                "values": list(selector_table),
            },
            "option_jump_table": {
                "address": "0x41624d0", "bytes_hex": option_jumps.hex(),
                "subselection_range": [1, 8],
                "targets": [hex(x) for x in option_targets],
            },
            "table_getters": [hex(x) for x in [
                0x287EE9C, 0x287EEC4, 0x287EEEC, 0x287EF14,
                0x287EF3C, 0x287EF64, 0x287EF8C, 0x287EFB4]],
            "observed_record_access": "Returned table + selected_look_index * 0x18 + 0x14, read one byte.",
            "direct_call_sites": app.direct_sites({0x21ADE88}),
            "conclusion": "This is a per-option table gate; an AUTO shooting-mode restriction is not established.",
        },
        "auto_customize_creative_look_icon": {
            "entry": "0x23e3144",
            "naming_literal": {
                "address": "0x41b49a0", "text": app.read_string(0x41B49A0),
                "reference": "0x23e3284/0x23e328c",
            },
            "observed_model_field_offsets": ["0xb8", "0x90", "0x00", "0x68", "0x20", "0x50"],
            "observed_look_enum_upper_bound": 12,
            "conclusion": "An icon translator exists. Its field producers, capture setting acceptance, and AUTO-mode behavior are untraced.",
        },
        "named_exif_exports": stubs,
        "external_profile_input_status": {
            "requested_repository": "https://github.com/TingfengLuo/Camera-Profile-for-Fujifilm-Film-Simulation",
            "available_inputs_to_this_mapper": [],
            "status": "No requested profile assets supplied to this local mapper; repository contents unexamined.",
            "prior_scoped_inventory": "A bounded /Users/hongjunwu/Git inventory found no matching repository or usable .dcp/.cube input outside ignored build/extracted content.",
            "inventory_limit": "The generator does not repeat or broaden that external inventory; absence is not a whole-machine assertion.",
        },
        "unknowns": [
            "Whether a user-selected Creative Look is accepted and retained in Intelligent AUTO.",
            "Whether AutoCustomize icon state reflects independent Creative Look selection or scene-driven settings.",
            "The requested Fujifilm repository's actual formats, color contract, licensing, and camera calibration.",
            "A working pre-header manual-lens EXIF injection API, capture-time profile snapshot, and UI binding.",
            "Any untraced alternate implementation behind the two named MprIf EXIF stubs.",
        ],
        "regions": [
            region(app, "Creative Look option inhibit helper", 0x21ADE88, 0x21AE0D4,
                   "Entry prologue through eight getter tails; next procedure prologue."),
            region(app, "Controller option sub-selection mapping", 0x21ADBA4, 0x21ADBDC,
                   "Complete bounded helper with out-of-range return path; next procedure prologue."),
            region(app, "AutoCustomize Creative Look icon translator", 0x23E3144, 0x23E332C,
                   "Entry through final branch back to shared return; next procedure starts with ret."),
            region(mpr, "MprIf SetExifInfo no-operation body", 0x2C8CC8, 0x2C8CD0,
                   "Exact defined dynamic symbol body."),
            region(mpr, "MprIf ModifyExifInfo no-operation body", 0x2C9258, 0x2C9260,
                   "Exact defined dynamic symbol body."),
        ],
    }
    assert "is_creative_look_option_inhibited" in report["creative_look_option_gate"]["naming_literal"]["text"]
    assert "translator_icon_auto_customize_creative_look" in report["auto_customize_creative_look_icon"]["naming_literal"]["text"]
    OUT.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({"output": str(OUT.relative_to(ROOT)),
                      "regions": len(report["regions"]), "named_exif_stubs": len(stubs),
                      "camera_operations": 0, "network_operations": 0}))


if __name__ == "__main__":
    main()
