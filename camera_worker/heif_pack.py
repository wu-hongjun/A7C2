"""Freeze a completed local 10-bit sRGB HIF and host-selected LUT/lens preset.

Only reads the supplied file; never connects to a camera. Uses the independently
built host HIF worker for bounded format inspection and TIFF extraction.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
from PIL import Image
from camera_worker.pack import lut_binary
from photo_worker.worker import load_lenses, number, output_exif, read_limited, sha, sync_directory


def pack(source, destination, *, inspector, completed=False, clf=None, lens_presets=None,
         lens_id=None, aperture=None, quality=95):
    if not completed:
        raise ValueError('Explicit --completed assertion required')
    if type(quality) is not int or not 1 <= quality <= 100:
        raise ValueError('HEVC quality must be 1-100')
    if bool(lens_presets) != bool(lens_id):
        raise ValueError('Provide lens presets and lens id together')
    lens = None
    if lens_id:
        lens = next((x for x in load_lenses(lens_presets) if x['id']==lens_id), None)
        if lens is None:
            raise ValueError('Unknown lens id')
    if aperture is not None:
        if not lens:
            raise ValueError('Actual aperture requires a lens preset')
        number(aperture, lens['max_aperture'], 128, 'Actual aperture')
    raw = read_limited(source, 64*1024*1024)
    look = lut_binary(read_limited(clf, 4*1024*1024)) if clf else b''
    destination = Path(destination)
    destination.mkdir(mode=0o700, parents=False, exist_ok=False)
    # Freeze first, inspect exactly these bytes; ready marker only written last.
    def write(name, data):
        with (destination/name).open('xb') as f:
            f.write(data); f.flush(); os.fsync(f.fileno())
    write('input.HIF', raw)
    command = [str(Path(inspector).resolve())]
    info = json.loads(subprocess.check_output(command+['--inspect', str(destination/'input.HIF')]))
    original = Image.Exif()
    tiff = subprocess.check_output(command+['--metadata', str(destination/'input.HIF')])
    if tiff:
        original.load(b'Exif\0\0'+tiff)
    metadata = output_exif(original, lens, aperture, (info['width'],info['height']))
    metadata[305] = 'A7C2 HIF Worker 0.1.0'
    metadata = metadata.tobytes()
    if not 14 <= len(metadata) <= 65533:
        raise ValueError('Metadata exceeds native bounds')
    head = struct.pack('<8s8IQ32s32s32s', b'A7HIFS01', 1, int(bool(look)), quality,
        info['width'], info['height'], len(metadata), len(look), 0, len(raw),
        hashlib.sha256(raw).digest(), hashlib.sha256(metadata).digest(), hashlib.sha256(look).digest())
    assets = {'input.HIF':raw, 'metadata.exif':metadata}
    write('metadata.exif',metadata)
    if look:
        write('look.bin',look);assets['look.bin']=look
    ready=head+hashlib.sha256(head).digest();assets['job.bin']=ready
    manifest = {'schema':1, 'format':'A7HIFS01', 'input':info, 'lens':lens, 'actual_aperture':aperture,
        'quality':quality, 'clf_sha256':sha(Path(clf).read_bytes()) if clf else None,
        'assets':{k:{'bytes':len(v),'sha256':sha(v)} for k,v in assets.items()},
        'output':'result.HIF; 10-bit HEVC 4:2:2, sRGB SDR, original geometry/orientation',
        'preset_selection_time':'host preparation, not shutter time',
        'camera_installation':False, 'network_during_tests':False,
        'metadata_policy':'allowlisted capture and manual-lens EXIF; no MakerNote/GPS/thumbnail'}
    write('manifest.json',(json.dumps(manifest,indent=2)+'\n').encode())
    write('job.bin',ready);sync_directory(destination);sync_directory(destination.parent)
    return manifest


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('input',type=Path);p.add_argument('job',type=Path)
    p.add_argument('--inspector',type=Path,required=True);p.add_argument('--completed',action='store_true')
    p.add_argument('--clf',type=Path);p.add_argument('--lens-presets',type=Path);p.add_argument('--lens',dest='lens_id')
    p.add_argument('--aperture',type=float);p.add_argument('--quality',type=int,default=95)
    a=p.parse_args();print(json.dumps(pack(a.input,a.job,inspector=a.inspector,completed=a.completed,
        clf=a.clf,lens_presets=a.lens_presets,lens_id=a.lens_id,aperture=a.aperture,quality=a.quality),indent=2))
