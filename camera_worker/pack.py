"""Export a verified host photo job for the standalone native runner.

Preparation runs on the host. The native executable needs no Python, Pillow,
OCIO, XML, JSON or camera SDK. No transport or installation is implemented.
"""
import argparse
import hashlib
import io
import json
import os
from pathlib import Path
import struct
from xml.etree import ElementTree as ET

from PIL import Image, ImageCms, ImageOps
from photo_worker.clf import parse_clf
from photo_worker.worker import canonical, load_job, output_exif, sha, sync_directory


def lut_binary(raw):
    parse_clf(raw)  # Shared bounded, numeric-only schema guard.
    nodes = list(ET.fromstring(raw))
    def numbers(i): return tuple(float(x) for x in nodes[i][0].text.split())
    if numbers(0) != numbers(7) or numbers(4) != numbers(12):
        raise ValueError('Native layout requires shared decode/encode transfer tables')
    values = sum((numbers(i) for i in (1,2,8,10,0,4,5)), ())
    assert len(values) == 106534
    return struct.pack('<8sII', b'A7LUT001',1,0)+struct.pack('<106534f', *values)


def pack_job(job, destination, *, normalize=False):
    doc, assets = load_job(job); options = doc['payload']
    if 'look.cube' in assets:
        raise ValueError('Native prototype supports converter CLF or lens-only jobs; CUBE remains host-only')
    look = lut_binary(assets['look.clf']) if 'look.clf' in assets else b''
    image_raw = assets['input.jpg']
    with Image.open(io.BytesIO(image_raw)) as image:
        original = image.getexif(); icc = image.info.get('icc_profile')
        needs_normalize = bool(icc or original.get(274,1)!=1 or image.info.get('progressive') or image.info.get('progression'))
        if needs_normalize and not normalize:
            raise ValueError('Native runner requires baseline sRGB/orientation 1; opt into host --normalize-input')
        if needs_normalize:
            rgb = ImageOps.exif_transpose(image)
            if icc:
                srgb = ImageCms.ImageCmsProfile(ImageCms.createProfile('sRGB'))
                rgb = ImageCms.profileToProfile(rgb,ImageCms.ImageCmsProfile(io.BytesIO(icc)),srgb,
                                               renderingIntent=0,outputMode='RGB')
            output = io.BytesIO();rgb.save(output,format='JPEG',quality=100,subsampling=0)
            image_raw = output.getvalue(); width,height=rgb.size
        else: width,height=image.size
        exif = output_exif(original,options['lens'],options['actual_aperture'],(width,height))
        exif[305]='A7C2 Native JPEG Worker 0.1.0'
        metadata = exif.tobytes()
    if max(width,height)>8192 or width*height>40000000 or len(image_raw)>64*1024*1024 or not 14<=len(metadata)<=65533:
        raise ValueError('Native job bounds exceeded')
    head = struct.pack('<8s8IQ32s32s32s',b'A7JOBS01',1,int(bool(look)),options['quality'],width,height,
        len(metadata),len(look),0,len(image_raw),hashlib.sha256(image_raw).digest(),
        hashlib.sha256(metadata).digest(),hashlib.sha256(look).digest())
    assert len(head)==144
    packed = {'input.jpg':image_raw,'metadata.exif':metadata,'job.bin':head+hashlib.sha256(head).digest()}
    if look: packed['look.bin']=look
    destination=Path(destination);destination.mkdir(mode=0o700,parents=False,exist_ok=False)
    # The native ready marker is job.bin: write it only after every asset is durable.
    for name in [n for n in packed if n!='job.bin']+['job.bin']:
        raw=packed[name]
        with (destination/name).open('xb') as stream:
            stream.write(raw);stream.flush()
            os.fsync(stream.fileno())
    manifest={'schema':1,'host_job_sha256':doc['sha256'],'host_input_sha256':sha(assets['input.jpg']),
        'input_normalized_on_host':needs_normalize,'extra_JPEG_reencode':needs_normalize,
        'dimensions':[width,height],'lens':options['lens'],'actual_aperture':options['actual_aperture'],
        'preset_selection_time':'host job preparation, not shutter time',
        'assets':{k:{'bytes':len(v),'sha256':sha(v)} for k,v in packed.items()},
        'output':'result.jpg, exclusively published in the native job directory',
        'camera_accessed':False,'camera_installation':False,'network_accessed':False,
        'limitations':'Native runner accepts already-rendered baseline 8-bit sRGB JPEG/orientation 1. Host resolves presets/metadata and CLF operators; no autonomous camera job producer.'}
    with (destination/'manifest.json').open('x') as stream:
        json.dump(manifest,stream,indent=2);stream.write('\n');stream.flush();os.fsync(stream.fileno())
    sync_directory(destination);sync_directory(destination.parent)
    return manifest


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('job',type=Path);p.add_argument('output',type=Path)
    p.add_argument('--normalize-input',action='store_true')
    args=p.parse_args()
    print(json.dumps(pack_job(args.job,args.output,normalize=args.normalize_input),indent=2))
