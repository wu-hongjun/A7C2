#!/usr/bin/env python3
"""Computer-only sRGB-to-sRGB 17/33-point CUBE reference evaluator.

No camera transport imports, target execution, firmware mutation or automatic
file discovery. The standard-library core uses red-fastest trilinear sampling.
Pillow is imported only for the explicit image command.
"""
from __future__ import annotations
import argparse
from dataclasses import dataclass
import io
import json
import math
from pathlib import Path
import shlex

MAX_CUBE_BYTES = 8 * 1024 * 1024
SUPPORTED_SIZES = (17, 33)
MAX_IMAGE_PIXELS = 40_000_000


def triple(tokens, context):
    if len(tokens) != 3:
        raise ValueError(f"{context}: expected exactly three numbers")
    try:
        values = tuple(float(x) for x in tokens)
    except (ValueError, TypeError, OverflowError) as exc:
        raise ValueError(f"{context}: invalid number") from exc
    if not all(math.isfinite(v) for v in values):
        raise ValueError(f"{context}: numbers must be finite")
    return values


@dataclass(frozen=True)
class Cube:
    size: int
    values: tuple[tuple[float, float, float], ...]
    domain_min: tuple[float, float, float] = (0.0, 0.0, 0.0)
    domain_max: tuple[float, float, float] = (1.0, 1.0, 1.0)
    title: str = ""

    def __post_init__(self):
        if self.size not in SUPPORTED_SIZES or len(self.values) != self.size ** 3:
            raise ValueError("CUBE requires exactly 17^3 or 33^3 entries")
        lo = triple(self.domain_min, "DOMAIN_MIN")
        hi = triple(self.domain_max, "DOMAIN_MAX")
        if any(not math.isfinite(b - a) or b <= a for a, b in zip(lo, hi)):
            raise ValueError("Every domain maximum must exceed its minimum with a finite span")
        object.__setattr__(self, "domain_min", lo)
        object.__setattr__(self, "domain_max", hi)
        object.__setattr__(self, "values", tuple(triple(row, "CUBE entry") for row in self.values))

    def apply(self, rgb):
        """Evaluate encoded sRGB channels; clamp input to domain, retain float output.

        The LUT operates on encoded sRGB values, with no implicit linearization.
        Out-of-range LUT outputs are retained here and clipped only for RGB8 export.
        """
        rgb = triple(rgb, "RGB input")
        coords = []
        for v, lo, hi in zip(rgb, self.domain_min, self.domain_max):
            t = 0.0 if v <= lo else 1.0 if v >= hi else (v - lo) / (hi - lo)
            p = t * (self.size - 1)
            left = min(math.floor(p), self.size - 2)
            coords.append((left, p - left))
        (r, fr), (g, fg), (b, fb) = coords
        n = self.size
        def at(dr, dg, db):
            return self.values[(b + db) * n * n + (g + dg) * n + r + dr]
        def lerp(a, z, t):
            return tuple(x * (1.0 - t) + y * t for x, y in zip(a, z))
        low = lerp(lerp(at(0, 0, 0), at(1, 0, 0), fr),
                   lerp(at(0, 1, 0), at(1, 1, 0), fr), fg)
        high = lerp(lerp(at(0, 0, 1), at(1, 0, 1), fr),
                    lerp(at(0, 1, 1), at(1, 1, 1), fr), fg)
        return triple(lerp(low, high, fb), "Interpolated output")

    def apply_rgb8(self, rgb):
        if len(rgb) != 3 or any(type(v) is not int or not 0 <= v <= 255 for v in rgb):
            raise ValueError("RGB8 input must contain three integer channels in [0,255]")
        return tuple(math.floor(min(1.0, max(0.0, v)) * 255.0 + 0.5)
                     for v in self.apply(tuple(v / 255.0 for v in rgb)))


def parse_cube(text):
    if len(text.encode("utf-8")) > MAX_CUBE_BYTES:
        raise ValueError("CUBE exceeds the 8 MiB reference limit")
    fields, values = {}, []
    for lineno, line in enumerate(text.splitlines(), 1):
        try:
            tokens = shlex.split(line, comments=True, posix=True)
        except ValueError as exc:
            raise ValueError(f"Line {lineno}: {exc}") from exc
        if not tokens:
            continue
        tag = tokens[0]
        if tag in {"TITLE", "LUT_3D_SIZE", "DOMAIN_MIN", "DOMAIN_MAX"}:
            if values:
                raise ValueError(f"Line {lineno}: headers must precede all data")
            if tag in fields:
                raise ValueError(f"Line {lineno}: duplicate {tag}")
            args = tokens[1:]
            if tag == "TITLE":
                if len(args) != 1:
                    raise ValueError(f"Line {lineno}: TITLE needs one quoted string")
                fields[tag] = args[0]
            elif tag == "LUT_3D_SIZE":
                if len(args) != 1 or args[0] not in {"17", "33"}:
                    raise ValueError(f"Line {lineno}: only LUT_3D_SIZE 17 or 33 is supported")
                fields[tag] = int(args[0])
            else:
                fields[tag] = triple(args, f"Line {lineno} {tag}")
        else:
            if "LUT_3D_SIZE" not in fields:
                raise ValueError(f"Line {lineno}: expected supported header before data")
            if len(values) >= fields["LUT_3D_SIZE"] ** 3:
                raise ValueError(f"Line {lineno}: too many CUBE entries")
            values.append(triple(tokens, f"Line {lineno} CUBE entry"))
    if "LUT_3D_SIZE" not in fields:
        raise ValueError("Missing LUT_3D_SIZE")
    return Cube(fields["LUT_3D_SIZE"], tuple(values),
                fields.get("DOMAIN_MIN", (0., 0., 0.)),
                fields.get("DOMAIN_MAX", (1., 1., 1.)), fields.get("TITLE", ""))


def read_cube(path):
    with Path(path).open("rb") as f:
        raw = f.read(MAX_CUBE_BYTES + 1)
    if len(raw) > MAX_CUBE_BYTES:
        raise ValueError("CUBE exceeds the 8 MiB reference limit")
    try:
        return parse_cube(raw.decode("utf-8-sig"))
    except UnicodeDecodeError as exc:
        raise ValueError("CUBE must be UTF-8 text") from exc


def process_image(cube, source, destination, *, assume_srgb=False, strip_rows=16):
    """Grade a separate RGB8 JPEG/PNG copy. Does not preserve original metadata."""
    from PIL import Image, ImageCms, ImageOps
    source, destination = Path(source), Path(destination)
    if source.resolve() == destination.resolve():
        raise ValueError("Input and output must differ")
    if destination.exists():
        raise ValueError("Output already exists; refusing overwrite")
    if destination.suffix.lower() not in {".png", ".jpg", ".jpeg"}:
        raise ValueError("Output must be PNG or JPEG")
    if type(strip_rows) is not int or not 1 <= strip_rows <= 256:
        raise ValueError("strip_rows must be an integer in [1,256]")
    srgb = ImageCms.ImageCmsProfile(ImageCms.createProfile("sRGB"))
    with Image.open(source) as opened:
        if opened.format not in {"JPEG", "PNG"} or opened.mode not in {"RGB", "RGBA"}:
            raise ValueError("Reference accepts only RGB8/RGBA8 JPEG or PNG")
        if opened.width * opened.height > MAX_IMAGE_PIXELS:
            raise ValueError("Image exceeds 40 million pixel reference limit")
        icc = opened.info.get("icc_profile")
        if not icc and not assume_srgb:
            raise ValueError("Untagged image requires explicit --assume-srgb")
        oriented = ImageOps.exif_transpose(opened)
        alpha = oriented.getchannel("A") if oriented.mode == "RGBA" else None
        if alpha is not None and destination.suffix.lower() != ".png":
            raise ValueError("RGBA input requires PNG output to retain alpha")
        rgb = oriented.convert("RGB")
        if icc:
            try:
                rgb = ImageCms.profileToProfile(rgb, ImageCms.ImageCmsProfile(io.BytesIO(icc)),
                                               srgb, renderingIntent=0, outputMode="RGB")
            except (OSError, ValueError, ImageCms.PyCMSError) as exc:
                raise ValueError("Invalid or unsupported embedded ICC profile") from exc
        output = Image.new("RGB", rgb.size)
        for y in range(0, rgb.height, strip_rows):
            box = (0, y, rgb.width, min(y + strip_rows, rgb.height))
            strip = rgb.crop(box)
            strip.putdata([cube.apply_rgb8(pixel) for pixel in strip.get_flattened_data()])
            output.paste(strip, box)
        if alpha is not None:
            output.putalpha(alpha)
        # Fresh output has no original EXIF/XMP/thumbnail; orientation is baked in.
        # Exclusive creation protects originals and concurrent output names.
        with destination.open("xb") as f:
            try:
                opts = {"icc_profile": srgb.tobytes()}
                if destination.suffix.lower() == ".png":
                    output.save(f, format="PNG", **opts)
                else:
                    output.save(f, format="JPEG", quality=95, subsampling=0, **opts)
            except Exception:
                destination.unlink(missing_ok=True)
                raise
        return {"input": str(source.resolve()), "output": str(destination.resolve()),
                "width": output.width, "height": output.height, "grid_size": cube.size,
                "contract": "encoded sRGB in -> encoded sRGB out",
                "interpolation": "trilinear", "input_icc_converted": bool(icc),
                "input_untagged_assumed_srgb": not bool(icc),
                "metadata": "fresh sRGB ICC; original EXIF/XMP/thumbnail omitted"}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("cube", type=Path)
    p.add_argument("input", type=Path)
    p.add_argument("output", type=Path)
    p.add_argument("--assume-srgb", action="store_true",
                   help="Assert that an untagged RGB image already uses encoded sRGB")
    args = p.parse_args()
    try:
        print(json.dumps(process_image(read_cube(args.cube), args.input, args.output,
                                       assume_srgb=args.assume_srgb), indent=2))
    except (ValueError, OSError) as exc:
        p.exit(2, f"Error: {exc}\n")


if __name__ == "__main__":
    main()
