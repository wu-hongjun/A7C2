"""Reproducible, offline scalar libjpeg-turbo + standalone worker build.

No package manager, download, installation or camera transport. An explicit
locally cached crate supplies the codec sources; only its libjpeg subtree is used.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tarfile

ROOT = Path(__file__).resolve().parents[1]
CODEC_SHA = '49b5a974bc59382d35927e3a07eef2ba9a24fe7227cefe52ec44b508e1b90f86'
S16 = 'jcapistd jccolor jcdiffct jclossls jcmainct jcprepct jcsample jdapistd jdcolor jddiffct jdlossls jdmainct jdpostct jdsample jutils'.split()
S12 = S16 + 'jccoefct jcdctmgr jdcoefct jddctmgr jdmerge jfdctfst jfdctint jidctflt jidctfst jidctint jidctred jquant1 jquant2'.split()
S8 = S12 + 'jcapimin jchuff jcicc jcinit jclhuff jcmarker jcmaster jcomapi jcparam jcphuff jctrans jdapimin jdatadst jdatasrc jdhuff jdicc jdinput jdlhuff jdmarker jdmaster jdphuff jdtrans jerror jfdctflt jmemmgr jmemnobs jpeg_nbits'.split()


def digest(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def build(archive, output, host=False, sanitize=False, zig='zig'):
    archive, output = Path(archive).resolve(), Path(output).resolve()
    if digest(archive) != CODEC_SHA:
        raise ValueError('Codec archive differs from pinned turbojpeg-sys 1.2.0 snapshot')
    output.mkdir(parents=True, exist_ok=False)
    source = output/'codec'; source.mkdir()
    prefix = 'turbojpeg-sys-1.2.0/libjpeg-turbo/'
    with tarfile.open(archive) as tar:
        for member in tar:
            if member.name.startswith(prefix) and member.isfile():
                relative = Path(member.name.removeprefix(prefix))
                if relative.is_absolute() or '..' in relative.parts:
                    raise ValueError('Unsafe codec archive path')
                path = source/relative; path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(tar.extractfile(member).read())
    config = output/'include'; config.mkdir()
    (config/'jconfig.h').write_text('''#define JPEG_LIB_VERSION 62
#define LIBJPEG_TURBO_VERSION 3.1.0
#define LIBJPEG_TURBO_VERSION_NUMBER 3001000
#define MEM_SRCDST_SUPPORTED 1
#ifndef BITS_IN_JSAMPLE
#define BITS_IN_JSAMPLE 8
#endif
''')
    (config/'jconfigint.h').write_text('''#define BUILD "20261018"
#define HIDDEN __attribute__((visibility("hidden")))
#define INLINE __inline__
#define THREAD_LOCAL __thread
#define PACKAGE_NAME "libjpeg-turbo"
#define VERSION "3.1.0"
#define SIZEOF_SIZE_T 8
#define HAVE_BUILTIN_CTZL 1
#define FALLTHROUGH __attribute__((fallthrough));
#ifndef BITS_IN_JSAMPLE
#define BITS_IN_JSAMPLE 8
#endif
''')
    (config/'jversion.h').write_text((source/'src/jversion.h.in').read_text().replace('@COPYRIGHT_YEAR@','1991-2024'))
    compiler = ['clang'] if host else [zig, 'cc', '-target', 'aarch64-linux-musl', '-mcpu=generic']
    flags = ['-O2', '-std=c11', '-D_GNU_SOURCE', '-ffp-contract=off', '-fno-fast-math',
             '-fno-strict-aliasing', '-DNDEBUG', '-ffile-prefix-map='+str(source)+'=CODEC',
             '-ffile-prefix-map='+str(ROOT/'camera_worker')+'=WORKER',
             '-I'+str(config), '-I'+str(source/'src')]
    if sanitize:
        if not host: raise ValueError('Sanitizers run only on the host variant')
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer']
    objects = output/'objects'; objects.mkdir()
    jobs = [(bits, name) for bits, names in ((8,S8),(12,S12),(16,S16)) for name in names]
    def compile_one(pair):
        bits, name = pair; obj = objects/f'{name}-{bits}.o'
        subprocess.run(compiler+flags+[f'-DBITS_IN_JSAMPLE={bits}', '-c', str(source/'src'/f'{name}.c'), '-o', str(obj)],
                       check=True, capture_output=True)
        return obj
    with ThreadPoolExecutor(max_workers=4) as pool:
        compiled = list(pool.map(compile_one, jobs))
    library = output/'libjpeg.a'
    archiver = ['ar'] if host else [zig, 'ar']
    subprocess.run(archiver+['rcs', str(library)]+[str(p) for p in compiled], check=True, capture_output=True)
    executable = output/('jpeg-worker-host' if host else 'jpeg-worker-aarch64')
    link = [] if host else ['-static', '-s', '-Wl,-z,noexecstack', '-Wl,--build-id=sha1']
    subprocess.run(compiler+flags+link+[str(ROOT/'camera_worker/worker.c'), str(library), '-lm', '-o', str(executable)],
                   check=True, capture_output=True)
    sources = [p for p in source.rglob('*') if p.is_file()]
    own = [ROOT/'camera_worker/worker.c', ROOT/'camera_worker/build.py', ROOT/'camera_worker/srgb_profile.h', ROOT/'native/sha256.h']
    record = {'schema':1, 'host_variant':host, 'sanitizers':sanitize, 'camera_accessed':False,
        'network_accessed':False, 'codec_archive_basename':archive.name, 'codec_archive_sha256':CODEC_SHA,
        'codec':'libjpeg-turbo 3.1.0 from turbojpeg-sys 1.2.0 crate; scalar, arithmetic disabled',
        'target':'native host' if host else 'aarch64-linux-musl static, generic ARMv8-A',
        'compiler':subprocess.check_output(compiler+['--version'], text=True).splitlines()[0],
        'zig_version':None if host else subprocess.check_output([zig,'version'],text=True).strip(),
        'compiler_executable_sha256':digest(shutil.which(compiler[0])),
        'compiler_target_arguments':compiler[1:],
        'compile_flags':[arg.replace(str(output),'<build>').replace(str(ROOT),'<repo>') for arg in flags],
        'link_flags':link, 'objects':len(compiled),
        'codec_source_tree_sha256':hashlib.sha256(json.dumps([(str(p.relative_to(source)),digest(p)) for p in sorted(sources)]).encode()).hexdigest(),
        'generated_headers':[{'file':p.name,'sha256':digest(p)} for p in sorted(config.iterdir())],
        'worker_sources':[{'path':str(p.relative_to(ROOT)),'sha256':digest(p)} for p in own],
        'executable':{'file':executable.name,'bytes':executable.stat().st_size,'sha256':digest(executable)}}
    (output/'build.json').write_text(json.dumps(record,indent=2)+'\n')
    return record


if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--codec-archive',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--host',action='store_true');p.add_argument('--sanitize',action='store_true');p.add_argument('--zig',default='zig')
    a=p.parse_args()
    try: print(json.dumps(build(a.codec_archive,a.output,a.host,a.sanitize,a.zig),indent=2))
    except subprocess.CalledProcessError as e:
        print((e.stderr or b'').decode(errors='replace'));raise
