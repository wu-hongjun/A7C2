"""Rebuild fixed installer data and ELF with pinned Zig 0.13.0. No camera I/O."""
import io
import json
from pathlib import Path
import subprocess
import sys

from elftools.elf.elffile import ELFFile

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT))
import core


def main():
    patches=core.read_json(ROOT/'assets/patches.json')
    binaries=[]
    for chip,name in enumerate(core.PROGRAMS):
        files=list((ROOT/'research/extracted/ILCE-7CM2-2.01').rglob(name))
        if len(files)!=1:raise ValueError('Expected one official '+name)
        original=files[0].read_bytes()
        candidate=core.patch_program(original,chip)
        if core.patch_program(candidate,chip,True)!=original:raise ValueError('Reverse patch mismatch')
        binaries.append(original)
    # Keep the existing payload-first CP ordering before all AP entries.
    rows=[]
    for chip in [1,0]:
        for row in sorted(patches[chip]['writes'],key=lambda r:(int(r['file_offset'],16)!=0x1100b80,int(r['file_offset'],16))):
            rows.append((chip,row))
    if len(rows)!=40:raise ValueError('Unexpected patch window count')
    lines=[];windows=[]
    for i,(chip,row) in enumerate(rows):
        before=bytes.fromhex(row['before_hex']);after=bytes.fromhex(row['after_hex'])
        for label,data in [('old',before),('new',after)]:
            lines.append(f'static const u8 {label}_{i}[]={{'+','.join(map(str,data))+'};')
        windows.append('{'+f'{int(row["file_offset"],16)}UL,{len(before)},{chip},old_{i},new_{i}'+'}')
    lines.append('static const struct Window windows[]={'+','.join(windows)+'};')
    lines.append(f'static const u32 window_count={len(rows)};')
    lines.append('static const u64 file_sizes[]={'+','.join(f'{len(b)}UL' for b in binaries)+'};')
    for label,hashes in [('original',core.ORIGINAL),('final',core.FINAL)]:
        arrays=['{'+','.join(map(str,bytes.fromhex(h)))+'}' for h in hashes]
        lines.append(f'static const u8 {label}_hashes[2][32]={{'+','.join(arrays)+'};')
    (ROOT/'native/patch_manifest.h').write_bytes(('\r\n'.join(lines)+'\r\n').encode())
    version=subprocess.check_output([sys.executable,'-m','ziglang','version'],text=True).strip()
    if version!='0.13.0':raise ValueError('Pinned Zig 0.13.0 required')
    output=ROOT/'research/build/zig-0.13.0/installer-fixed.so';output.parent.mkdir(parents=True,exist_ok=True)
    flags=['-target','aarch64-linux-gnu','-shared','-fPIC','-O2','-ffreestanding','-fno-builtin',
           '-fno-stack-protector','-fno-unwind-tables','-fno-asynchronous-unwind-tables',
           '-nostdlib','-Wl,--no-undefined','-Wl,-Bsymbolic','-Wl,-z,noexecstack']
    subprocess.run([sys.executable,'-m','ziglang','cc',*flags,'-o',str(output),'native/installer.c'],cwd=ROOT,check=True)
    built=output.read_bytes()
    previous=(ROOT/'assets/installer.so').read_bytes()
    old,new=ELFFile(io.BytesIO(previous)),ELFFile(io.BytesIO(built))
    unchanged_text=old.get_section_by_name('.text').data()==new.get_section_by_name('.text').data()
    if not unchanged_text:raise ValueError('Installer executable instructions unexpectedly changed')
    (ROOT/'assets/installer.so').write_bytes(built)
    manifest=core.read_json(ROOT/'assets/manifest.json')
    manifest['release']='0.1.1-preview'
    manifest['hardware_scope']='Revised CP movie fallback and installer guards: offline validated only; no hardware installation. Historical 0.1.0 preview was reported on one body.'
    for name in manifest['files']:manifest['files'][name]=core.sha((ROOT/'assets'/name).read_bytes())
    core.save_json(ROOT/'assets/manifest.json',manifest)
    report={'compiler':version,'compiler_flags':flags,'native_text_unchanged':unchanged_text,
            'source':'native/installer.c','manifest':'native/patch_manifest.h',
            'installer_sha256':core.sha(built),'program_candidates':dict(zip(core.PROGRAMS,core.FINAL)),
            'camera_accessed':False}
    core.save_json(ROOT/'research/evidence/fix-build.json',report)
    core.assets()
    print(json.dumps(report,indent=2))


if __name__=='__main__':main()
