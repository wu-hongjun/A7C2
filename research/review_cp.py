"""Isolated original/candidate CP instruction review. Never accesses USB.

This models a flat relocated image and the fixed global parameter units.
Diagnostic logging and the optional chroma-adjustment helper are stubbed;
results cover dispatch/table selection, not the imaging hardware or startup.
"""
import json
from pathlib import Path
import struct
import sys

from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import core

BASE = 0x108000
ARENA = 0x5000000
STACK = 0x6000000
EXIT = 0x7000000
REGS = [UC_ARM_REG_R0, UC_ARM_REG_R1, UC_ARM_REG_R2, UC_ARM_REG_R3,
        UC_ARM_REG_R4, UC_ARM_REG_R5, UC_ARM_REG_R6, UC_ARM_REG_R7,
        UC_ARM_REG_R8, UC_ARM_REG_R9, UC_ARM_REG_R10, UC_ARM_REG_R11,
        UC_ARM_REG_R12]


def require(value, message):
    if not value:
        raise ValueError(message)


class Image:
    def __init__(self, binary, units):
        self.binary = binary
        self.units = units
        self.cpu = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
        self.cpu.mem_map(BASE, (len(binary) + 4095) & ~4095)
        self.cpu.mem_write(BASE, binary)
        for address, size in [(0x1790000, 0x10000), (0x2060000, 0x20000),
                              (ARENA, 0x10000), (STACK, 0x10000), (EXIT, 4096)]:
            self.cpu.mem_map(address, size)
        self.cpu.hook_add(UC_HOOK_CODE, self.hook)
        self.initial = self.cpu.context_save()
        self.reset()

    def reset(self):
        self.cpu.context_restore(self.initial)
        self.cpu.mem_write(ARENA, b"\0" * 0x10000)
        self.cpu.mem_write(STACK, b"\0" * 0x10000)
        self.cpu.mem_write(0x1790000, b"\0" * 0x10000)
        self.cpu.mem_write(0x1793bb0, self.units[0])
        self.cpu.mem_write(0x17960b0, self.units[1])
        self.cpu.reg_write(UC_ARM_REG_SP, STACK + 0x8000)
        self.cpu.reg_write(UC_ARM_REG_LR, EXIT | 1)
        self.logs = []

    def hook(self, cpu, address, size, user):
        offset = address - BASE
        if offset in [0x64d01e, 0xa3c57a, 0x6493c2]:
            self.logs.append(hex(offset))
            # Leave the zero-initialized chroma correction buffer unchanged.
            cpu.reg_write(UC_ARM_REG_R0, 0)
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))

    def run(self, offset, args=(), until=None):
        for register, value in zip(REGS, args):
            self.cpu.reg_write(register, value)
        stop = EXIT if until is None else BASE + until
        self.cpu.emu_start((BASE + offset) | 1, stop, count=3000)
        require(self.cpu.reg_read(UC_ARM_REG_PC) == stop,
                f"Instruction budget exhausted at {self.cpu.reg_read(UC_ARM_REG_PC):#x}")

    def dispatch(self, mode, still, variant=0, context_flag=1):
        self.reset()
        self.cpu.mem_write(ARENA, struct.pack("<II", ARENA + 0x100, ARENA + 0x200))
        self.cpu.mem_write(ARENA + 0x100 + 0x2e, bytes([mode]))
        self.cpu.mem_write(ARENA + 0x100 + 0x34, bytes([variant]))
        self.cpu.mem_write(ARENA + 0x200 + 3, bytes([context_flag]))
        output = ARENA + 0x400
        self.cpu.mem_write(output, b"\xa5" * 16)
        self.run(0x13865c if still else 0x138464, [ARENA, output])
        return bytes(self.cpu.mem_read(output, 16))

    def gamma(self, mode, column, overrides=0):
        self.reset()
        self.cpu.mem_write(ARENA + 0xc, struct.pack("<I", mode))
        self.cpu.mem_write(ARENA + 6, b"\x01")
        if overrides & 1:
            self.cpu.mem_write(ARENA + 5, b"\x01")
        if overrides & 2:
            self.cpu.mem_write(ARENA + 0x20, struct.pack("<I", 2))
        if overrides & 4:
            self.cpu.mem_write(ARENA + 0x34, b"\x01")
        self.cpu.mem_write(STACK + 0x8000, struct.pack("<I", ARENA + 0x504))
        self.run(0x323128, [ARENA, column, 1 if overrides & 4 else 0, ARENA + 0x500])
        return struct.unpack("<II", self.cpu.mem_read(ARENA + 0x500, 8))

    def name(self, mode, sr2=False):
        self.reset()
        self.run(0x149e14 if sr2 else 0x148e5e, [0, 0, mode],
                 until=0x149e84 if sr2 else 0x148eec)
        pointer = self.cpu.reg_read(UC_ARM_REG_R1)
        return bytes(self.cpu.mem_read(pointer, 16)).split(b"\0")[0].decode("ascii")

    def colormode(self, mode):
        self.reset()
        self.cpu.mem_write(ARENA, struct.pack("<I", ARENA + 0x100))
        self.cpu.mem_write(ARENA + 0x100 + 0x3f, bytes([mode]))
        self.cpu.reg_write(UC_ARM_REG_R5, ARENA)
        self.run(0x148ef2, until=0x148f12)
        return self.cpu.reg_read(UC_ARM_REG_R2)

    def chroma(self, parameter, secondary=False, illuminant=0):
        self.reset()
        self.cpu.mem_write(ARENA + 0x100, struct.pack("<I", ARENA + 0x200))
        # Neutral context disables optional runtime overrides.
        self.run(0x64963c if secondary else 0x6492e4,
                 [ARENA + 0x400, parameter,
                  illuminant if secondary else ARENA + 0x100, ARENA + 0x100])
        return bytes(self.cpu.mem_read(ARENA + 0x400, 48)), self.logs

    def matrix(self, parameter, selector, weights):
        self.reset()
        self.cpu.mem_write(ARENA + 4, struct.pack("<I", ARENA + 0x200))
        self.cpu.mem_write(ARENA + 0x200 + 0x1a0, struct.pack("<3H", *weights))
        self.run(0x649092, [ARENA + 0x400, selector, ARENA, parameter])
        return bytes(self.cpu.mem_read(ARENA + 0x400, 18))


def main():
    original = next((ROOT / "research/extracted/ILCE-7CM2-2.01").rglob("cpapp-b.bin")).read_bytes()
    candidate = core.patch_program(original, 1)
    factory = next((ROOT / "research/extracted/ILCE-7CM2-2.01").rglob("CX89700_ALLLANG.bin"))
    records = core.backup_records(factory.read_bytes())
    units = tuple(records[1][k].data for k in core.CP_KEYS)
    old = Image(original, units)
    new = Image(candidate, core.merge_colors(*units))
    report = {"camera_accessed": False, "cases": {}, "observations": {}}
    differences = []
    for still in [False, True]:
        for mode in range(256):
            for variant in range(4):
                for flag in [0, 1]:
                    before = old.dispatch(mode, still, variant, flag)
                    after = new.dispatch(mode, still, variant, flag)
                    if before != after:
                        differences.append({"still": still, "mode": mode, "variant": variant,
                                            "context_flag": flag,
                                            "before": before.hex(), "after": after.hex()})
                        require(mode in [19, 20], f"Existing dispatch changed: {mode}")
    report["cases"]["dispatch"] = 4096
    report["observations"]["dispatch_changes"] = differences
    gamma_changes = []
    for mode in range(21):
        for column in [0, 1]:
            for overrides in range(8):
                before, after = old.gamma(mode, column, overrides), new.gamma(mode, column, overrides)
                if before != after:
                    gamma_changes.append({"mode": mode, "column": column, "overrides": overrides,
                                          "before": before, "after": after})
                    require(mode in [19, 20], f"Existing gamma selection changed: {mode}")
    report["cases"]["gamma_selection"] = 336
    report["observations"]["gamma_changes"] = gamma_changes
    names = []
    for sr2 in [False, True]:
        for mode in range(256):
            before, after = old.name(mode, sr2), new.name(mode, sr2)
            if before != after:
                require(mode in [19, 20], f"Existing photo name changed: {mode}")
                require(after == ("FL2" if mode == 19 else "FL3"), "Wrong new photo name")
                names.append({"sr2": sr2, "mode": mode, "before": before, "after": after})
    report["cases"]["photo_names"] = 512
    report["observations"]["name_changes"] = names
    for mode in range(256):
        before, after = old.colormode(mode), new.colormode(mode)
        require(after == ({19: 22, 20: 23}.get(mode, before)), f"ColorMode mismatch: {mode}")
    report["cases"]["color_mode"] = 256
    chroma_changes = []
    for parameter in range(57):
        for secondary in [False, True]:
            for illuminant in ([0, 1] if secondary else [0]):
                before, _ = old.chroma(parameter, secondary, illuminant)
                after, logs = new.chroma(parameter, secondary, illuminant)
                if before != after:
                    require(parameter in [55, 56], f"Existing chroma changed: {parameter}")
                    chroma_changes.append({"parameter": parameter, "secondary": secondary,
                                           "illuminant": illuminant, "after": after.hex()})
                require(parameter == 53 or "0x64d01e" not in logs,
                        f"Valid parameter hits diagnostic: {parameter}")
    report["cases"]["chroma_selection"] = 171
    report["observations"]["chroma_changes"] = chroma_changes
    matrix_changes = []
    weights = [(1024, 0, 0), (0, 1024, 0), (0, 0, 1024), (256, 512, 256)]
    for parameter in range(57):
        for selector in range(16):
            for illuminants in weights:
                before = old.matrix(parameter, selector, illuminants)
                after = new.matrix(parameter, selector, illuminants)
                if before != after:
                    require(parameter in [55, 56], f"Existing matrix output changed: {parameter}")
                    matrix_changes.append({"parameter": parameter, "selector": selector,
                                           "weights": illuminants, "after": after.hex()})
    report["cases"]["matrix_selection_and_blending"] = 57 * 16 * len(weights)
    report["observations"]["matrix_changes"] = matrix_changes
    oracle_cases = 0
    for parameter, slot in [(55, 23), (56, 25)]:
        # Route the exact new record through an existing, untouched parameter
        # in the ORIGINAL code, then compare it with the new parameter path.
        a, b = [bytearray(unit) for unit in core.merge_colors(*units)]
        a[0x1200 + 52] = slot
        b[0x196 + 52] = slot
        reference = Image(original, (bytes(a), bytes(b)))
        for selector in range(16):
            for illuminants in weights:
                require(new.matrix(parameter, selector, illuminants) == reference.matrix(52, selector, illuminants),
                        f"New matrix differs from existing processing path: {parameter}")
                oracle_cases += 1
        for secondary, illuminant in [(False, 0), (True, 0), (True, 1)]:
            require(new.chroma(parameter, secondary, illuminant)[0] == reference.chroma(52, secondary, illuminant)[0],
                    f"New chroma differs from existing processing path: {parameter}")
            oracle_cases += 1
    report["cases"]["new_records_through_original_code_oracle"] = oracle_cases
    report["observations"]["movie_fallback"] = [
        {"mode": mode, "color_parameter": new.dispatch(mode, False)[1],
         "gamma_pair": new.gamma(mode, 0),
         "FL_color_parameter": new.dispatch(15, False)[1],
         "FL_gamma_pair": new.gamma(15, 0)} for mode in [19, 20]]
    fallback_cases=0
    for mode in [19,20]:
        for variant in range(4):
            for flag in [0,1]:
                require(new.dispatch(mode,False,variant,flag)==new.dispatch(15,False,variant,flag),
                        'Movie color fallback differs from existing FL')
                fallback_cases+=1
        for overrides in range(8):
            require(new.gamma(mode,0,overrides)==new.gamma(15,0,overrides),
                    'Movie gamma fallback differs from existing FL')
            fallback_cases+=1
        require(new.gamma(mode,1)==((48,48) if mode==19 else (49,49)),
                'Still-photo FL2/FL3 gamma changed')
    report['cases']['consistent_FL_movie_fallback']=fallback_cases
    # The old table has 47 rows, each with seven pointers. All valid rows are
    # retained exactly; row 47 is an admitted but empty hole in the new table.
    old_table = original[0x1000d0c:0x1000d0c + 47 * 28]
    new_table = candidate[0x1101480:0x1101480 + 50 * 28]
    require(new_table[:len(old_table)] == old_table, "Existing gamma pointer row changed")
    require(new_table[47 * 28:48 * 28] == b"\0" * 28, "Unexpected row 47")
    report["observations"]["gamma_table"] = {
        "original_47_rows_preserved": True, "row47_is_empty": True,
        "rows48_49_nonzero": all(struct.unpack_from("<I", new_table, row * 28 + col * 4)[0]
                                  for row in [48, 49] for col in range(7))}
    # The larger initialized region stays before the next zero-init region.
    initialized_size = struct.unpack_from("<I", candidate, 0x20140)[0]
    initialized_end = 0x10c7000 + initialized_size
    require(initialized_end <= 0x120a000, "Added CP data overlaps next load region")
    report["observations"]["load_extent"] = {"initialized_end": hex(initialized_end),
        "next_zero_init_start": "0x120a000", "remaining_gap_bytes": 0x120a000 - initialized_end}
    # Byte provenance for the adapted hardware selector: existing comparable
    # records also use 5 on this body where the donor uses 3 at offset 0x21c.
    donor = next((ROOT / "research/extracted/ILCE-1M2-4.00").rglob("cpapp-b.bin")).read_bytes()
    pairs = []
    for pointer in sorted(set(struct.unpack("<329I", old_table)) - {0}):
        offset = pointer - BASE
        record = original[offset:offset + 576]
        donor_offset = donor.find(record[:512])
        if donor_offset < 0 or donor.find(record[:512], donor_offset + 1) >= 0:
            continue
        source = donor[donor_offset:donor_offset + 580]
        adapted = source[:572] + source[576:]
        differences = [i for i, (a, b) in enumerate(zip(record, adapted)) if a != b]
        if differences == [0x21c] and record[0x21c] == 5 and source[0x21c] == 3:
            pairs.append({"target_offset": hex(offset), "donor_offset": hex(donor_offset)})
    require(len(pairs) >= 4, "No existing-record support for 3-to-5 selector adaptation")
    report["observations"]["existing_records_supporting_selector_3_to_5"] = pairs
    report["total_cases"] = sum(report["cases"].values())
    report["limits"] = ["Isolated instruction blocks; not full CP execution",
                        "Neutral synthetic state and factory color units",
                        "Diagnostic and optional chroma adjustment calls stubbed",
                        "No sensor, ISP, DMA, boot, photograph, or USB validation"]
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
