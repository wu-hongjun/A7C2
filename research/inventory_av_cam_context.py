"""Bounded original av-cam header/string inventory; never executes the image."""
import hashlib
import json
from pathlib import Path
import re
import struct

ROOT=Path(__file__).resolve().parents[1]


def main():
    path=ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha3_unpacked/av-cam.bin'
    blob=path.read_bytes();digest=hashlib.sha256(blob).hexdigest()
    assert digest=='90343f5f2edc0393ad8809d907cfe648dfd3a82ce0ef39cf06167929d8955096'
    assert blob[8:12]==b'ORIL'
    strings=[]
    for m in re.finditer(rb'[\x20-\x7e]{5,}\x00',blob):
        value=m.group()[:-1].decode('ascii')
        if re.search(r'mlut|ssyc|sdyc',value,re.I):
            strings.append({'file_offset':hex(m.start()),'bytes_including_NUL':len(m.group()),'text':value})
    assert any(r['text']=='ReadSsycImageSizeInfo'for r in strings)
    assert any(r['text']=='WriteSsycImageSizeInfo'for r in strings)
    assert any(r['text'].startswith('ddl_mlut_set_l3d_sram ')for r in strings)
    result={'source':{'path':str(path.relative_to(ROOT)),'sha256':digest,'bytes':len(blob)},'generator':{'path':str(Path(__file__).relative_to(ROOT)),'sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()},'camera_accessed':False,'network_access':False,'target_executed':False,'method':'Full-file bounded NUL-terminated printable-ASCII inventory, matched case-insensitively for MLUT/SSYC/SDYC. Original file offsets retained.','header':{'file_offset':0,'first_88_bytes_hex':blob[:88].hex(),'marker_at_8':'ORIL','u32_at_c':struct.unpack_from('<I',blob,12)[0],'first_u64_pointer':hex(struct.unpack_from('<Q',blob)[0])},'matches':strings,'counts':{'matching_strings':len(strings),'MLUT_matches':sum(bool(re.search('mlut',r['text'],re.I))for r in strings),'SSYC_matches':sum(bool(re.search('ssyc',r['text'],re.I))for r in strings)},'limits':['String presence establishes inspected artifact contents only; it does not establish invocation, physical processor identity, active imaging mode, resource ownership or a saved-frame MLUT route.','No raw firmware has been registered as ELF, and no unsupported REA raw-image analysis is claimed.','The notification and encoder follow-ups independently retain code/data references and address-model evidence; these strings alone are not those execution edges.']}
    (ROOT/'research/evidence/av-cam-lut-context.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result['counts'],indent=2))


if __name__=='__main__':main()
