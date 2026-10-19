"""Run original ARM64 launcher menu/selection instructions with restricted syscalls.

No capture ingress/fork/exec is forwarded or emulated. The only writable guest
paths are selection.bin and selection.bin.next in a generated build workspace.
"""
import argparse
import hashlib
import json
from pathlib import Path
from research.check_camera_worker import elf_check
from research.emulate_camera_worker import execute

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--binary',type=Path,required=True)
    p.add_argument('--workspace',type=Path,required=True);p.add_argument('--report',type=Path,required=True);a=p.parse_args()
    rows=[]
    def case(name,mode='--version',selection=None,ok=True):
        r=execute(a.binary,a.workspace if selection is not None else None,mode=mode,launcher_selection=selection)
        assert (r['exit_code']==0)==ok,r;rows.append({'case':name,**r});return r
    case('startup')
    case('catalog-menu','--list',[])
    case('native-selection-CRC-and-generation','--select',['classic-chrome-srgb','example-helios-58','4'])
    r=case('readback','--list',[]);assert 'look=classic-chrome-srgb lens=example-helios-58 aperture=4' in r['stdout']
    before=(a.workspace/'selection.bin').read_bytes()
    case('invalid-selection-preserves-current','--select',['none','none','nan'],False)
    assert (a.workspace/'selection.bin').read_bytes()==before
    doc={'schema':1,'scope':'Exact cross-built ELF startup/list/select under Unicorn and restricted syscall models; not a Linux/camera process test',
        'ELF':elf_check(a.binary),'cases':rows,'camera_accessed':False,'network_accessed':False,
        'model_limits':'No camera/file tree outside explicit build workspace, no device/network/process syscalls; lock/rename/fsync models do not establish target filesystem semantics',
        'source_sha256':{name:hashlib.sha256(Path(name).read_bytes()).hexdigest() for name in ['research/check_launcher_emulation.py','research/emulate_camera_worker.py']}}
    a.report.write_text(json.dumps(doc,indent=2)+'\n');print('5 ARM64 launcher scenarios passed')

if __name__=='__main__':main()
