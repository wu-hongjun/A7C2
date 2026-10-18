"""Bounded loader for the converter's self-contained CLF subset.

Builds OCIO transforms from numeric arrays. Never delegates XML/file resolution
to OCIO; Reference, paths, external resources, DTDs and entities are rejected.
This is deliberately not a general CLF implementation.
"""
from dataclasses import dataclass
import math
from xml.etree import ElementTree as ET

MAX_CLF_BYTES = 8*1024*1024
NS = '{http://www.smpte-ra.org/ns/2136-1/2024}'
LAYOUT = ('LUT1D', 'Matrix', 'Matrix', 'Range', 'LUT1D', 'LUT3D', 'Range',
          'LUT1D', 'Matrix', 'Range', 'Matrix', 'Range', 'LUT1D')


@dataclass(frozen=True)
class CLFLut:
    group: object


def parse_clf(raw):
    import PyOpenColorIO as o
    if not isinstance(raw, bytes) or len(raw) > MAX_CLF_BYTES:
        raise ValueError('CLF exceeds byte bound')
    text = raw.decode('utf-8-sig')
    if '\0' in text or '<!DOCTYPE' in text.upper() or '<!ENTITY' in text.upper():
        raise ValueError('CLF DTD/entities/alternate encodings are forbidden')
    try:
        root = ET.fromstring(text)
    except ET.ParseError as exc:
        raise ValueError('Malformed CLF XML') from exc
    if (root.tag != NS+'ProcessList' or set(root.attrib) != {'compCLFversion', 'id'} or
            root.attrib['compCLFversion'] != '3' or
            tuple(x.tag.removeprefix(NS) for x in root) != LAYOUT or len(list(root.iter())) > 50):
        raise ValueError('Unsupported CLF process list/layout')
    transforms = []
    base = {'inBitDepth': '32f', 'outBitDepth': '32f'}
    for node, kind in zip(root, LAYOUT):
        attrs = dict(base)
        if kind.startswith('LUT'):
            attrs['interpolation'] = 'linear' if kind == 'LUT1D' else 'tetrahedral'
        if node.tag != NS+kind or node.attrib != attrs:
            raise ValueError('Unsupported CLF operator attributes')
        if kind == 'Range':
            names = ('minInValue', 'maxInValue', 'minOutValue', 'maxOutValue')
            if tuple(x.tag for x in node) != tuple(NS+n for n in names):
                raise ValueError('Invalid CLF clipping operator')
            if any(x.attrib or len(x) or (x.text or '').strip() != str(i % 2) for i, x in enumerate(node)):
                raise ValueError('Only unit CLF clamps supported')
            transforms.append(o.RangeTransform(minInValue=0, maxInValue=1, minOutValue=0, maxOutValue=1))
            continue
        dim, count = {'Matrix': ('3 3', 9), 'LUT1D': ('4097 1', 4097),
                      'LUT3D': ('32 32 32 3', 32**3*3)}[kind]
        if len(node) != 1 or node[0].tag != NS+'Array' or node[0].attrib != {'dim': dim} or len(node[0]):
            raise ValueError('Unsupported CLF numeric array')
        tokens = (node[0].text or '').split()
        if len(tokens) != count:
            raise ValueError('CLF array length mismatch')
        try:
            values = [float(x) for x in tokens]
        except ValueError as exc:
            raise ValueError('Invalid CLF number') from exc
        lo, hi = (-16, 16) if kind == 'Matrix' else (0, 1)
        if any(not math.isfinite(x) or not lo <= x <= hi for x in values):
            raise ValueError('Nonfinite/out-of-bound CLF array')
        if kind == 'Matrix':
            m = [0.]*16
            for r in range(3):
                for c in range(3): m[4*r+c] = values[3*r+c]
            m[15] = 1.
            transform = o.MatrixTransform(matrix=m)
        elif kind == 'LUT1D':
            transform = o.Lut1DTransform(length=4097, interpolation=o.INTERP_LINEAR)
            for i, v in enumerate(values): transform.setValue(i, v, v, v)
        else:
            transform = o.Lut3DTransform(gridSize=32, interpolation=o.INTERP_TETRAHEDRAL)
            for i in range(32**3):
                transform.setValue(i//1024, (i//32) % 32, i % 32, *values[3*i:3*i+3])
        transforms.append(transform)
    return CLFLut(o.GroupTransform(transforms))


def processor(lut):
    import PyOpenColorIO as o
    return o.Config.CreateRaw().getProcessor(lut.group).getOptimizedCPUProcessor(o.OPTIMIZATION_LOSSLESS)
