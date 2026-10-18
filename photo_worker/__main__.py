"""Host-only JPEG LUT/lens worker. Never connects to a camera."""
import argparse
import json
from pathlib import Path

from .worker import SRGB_CONTRACT, load_lenses, prepare, run


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    p = commands.add_parser('prepare', help='Snapshot an explicitly completed JPEG, LUT and lens selection')
    p.add_argument('input', type=Path)
    p.add_argument('job', type=Path)
    p.add_argument('--cube', '--lut', dest='cube', type=Path, help='3D CUBE or self-contained converter-subset CLF')
    p.add_argument('--lut-contract', choices=[SRGB_CONTRACT])
    p.add_argument('--lens-presets', type=Path)
    p.add_argument('--lens', dest='lens_id')
    p.add_argument('--aperture', type=float, help='User-entered actual shooting f-number; never inferred')
    p.add_argument('--assume-srgb', action='store_true')
    p.add_argument('--completed', action='store_true', help='Assert selected JPEG is closed and complete')
    p.add_argument('--quality', type=int, default=95)
    p = commands.add_parser('run', help='Process a frozen job into a new JPEG')
    p.add_argument('job', type=Path)
    p.add_argument('output', type=Path)
    p = commands.add_parser('list-lenses')
    p.add_argument('presets', type=Path)
    p = commands.add_parser('inspect-profiles', help='Audit local Fujifilm XMP/DCP pack without applying it')
    p.add_argument('directory', type=Path)
    p = commands.add_parser('convert-fujifilm', help='Convert embedded RGBTable stages to explicit sRGB JPEG CLF LUTs')
    p.add_argument('directory', type=Path, help='Folder containing the actual look XMPs')
    p.add_argument('output', type=Path, help='New output directory; never overwritten')
    p = commands.add_parser('inspect-rgb-tables', help='Decode and fingerprint local XMP RGBTable blobs; not an sRGB bake')
    p.add_argument('directory', type=Path)
    args = parser.parse_args()
    try:
        if args.command == 'prepare':
            values = vars(args).copy()
            values.pop('command'); source = values.pop('input'); job = values.pop('job')
            result = prepare(source, job, **values)
        elif args.command == 'run':
            result = run(args.job, args.output)
        elif args.command == 'list-lenses':
            result = {'lenses': load_lenses(args.presets)}
        elif args.command == 'inspect-profiles':
            from .profiles import inspect_pack
            result = inspect_pack(args.directory)
        elif args.command == 'inspect-rgb-tables':
            from .adobe_table import inspect_decoded_profiles
            result = inspect_decoded_profiles(args.directory)
        else:
            from .fujifilm import convert_directory
            result = convert_directory(args.directory, args.output)
        print(json.dumps(result, indent=2, allow_nan=False))
    except (ValueError, OSError, KeyError, TypeError, UnicodeError) as exc:
        parser.exit(2, f'Error: {exc}\n')


if __name__ == '__main__':
    main()
