"""Offline HEIF/HEVC source build after explicit source download.

Separate executable: the original JPEG worker and installer assets are unchanged.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tarfile

ROOT=Path(__file__).resolve().parents[1]
ARCHIVES={
 'libheif-1.23.6.tar.gz':('4484346dc5995319dbc11e3a1c35d0a2ec46511ce370900869337fd2c7033125','https://github.com/strukturag/libheif/releases/download/v1.23.6/libheif-1.23.6.tar.gz'),
 'libde265-1.1.3.tar.gz':('554228bd17788c99a7e63b37ab5634722190e6e2bf60c1dcb01cef328e133905','https://github.com/strukturag/libde265/releases/download/v1.1.3/libde265-1.1.3.tar.gz'),
 'x265_4.2.tar.gz':('40b1ea0453e0309f0eba934e0ddf533f8f6295966679e8894e8f1c1c8d5e1210','https://bitbucket.org/multicoreware/x265_git/downloads/x265_4.2.tar.gz')}


def sha(path):
    with Path(path).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()


def build(downloads,output,cmake,host=False,codecs_only=False,worker_only=False):
    downloads,output,cmake=Path(downloads).resolve(),Path(output).resolve(),Path(cmake).resolve()
    output.mkdir(parents=True,exist_ok=True);sources=output/'sources';sources.mkdir(exist_ok=True)
    for name,(expected,url) in ARCHIVES.items():
        archive=downloads/name
        if sha(archive)!=expected:raise ValueError('Source archive mismatch: '+name)
        if not worker_only:
            with tarfile.open(archive) as t:t.extractall(sources,filter='data')
    prefix=output/'prefix';prefix.mkdir(exist_ok=True)
    toolchain=output/'toolchain.cmake'
    if host:
        toolchain.write_text('set(CMAKE_C_COMPILER clang)\nset(CMAKE_CXX_COMPILER clang++)\n')
    else:
        zig=Path(shutil.which('zig')).resolve()
        wrappers={ 'cc':f'"{zig}" cc -target aarch64-linux-musl -mcpu=generic',
                  'cxx':f'"{zig}" c++ -target aarch64-linux-musl -mcpu=generic',
                  'ar':f'"{zig}" ar','ranlib':f'"{zig}" ranlib'}
        for name,command in wrappers.items():
            p=output/name;p.write_text('#!/bin/sh\nexec '+command+' "$@"\n');p.chmod(0o755)
        toolchain.write_text(f'''set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER "{output/'cc'}")
set(CMAKE_CXX_COMPILER "{output/'cxx'}")
set(CMAKE_AR "{output/'ar'}")
set(CMAKE_RANLIB "{output/'ranlib'}")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_FIND_ROOT_PATH "{prefix}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
''')
    common=[str(cmake),'-DCMAKE_TOOLCHAIN_FILE='+str(toolchain),'-DCMAKE_BUILD_TYPE=Release',
        '-DCMAKE_INSTALL_PREFIX='+str(prefix),'-DCMAKE_INSTALL_LIBDIR=lib','-DBUILD_SHARED_LIBS=OFF',
        '-DCMAKE_POLICY_VERSION_MINIMUM=3.5','-DCMAKE_EXPORT_COMPILE_COMMANDS=ON',
        '-DCMAKE_C_FLAGS=-ffp-contract=off -fno-fast-math',
        '-DCMAKE_CXX_FLAGS=-ffp-contract=off -fno-fast-math']
    env=os.environ.copy();env['SOURCE_DATE_EPOCH']='1792281600';env['PKG_CONFIG_LIBDIR']=str(prefix/'lib/pkgconfig')
    def stage(name,source,flags,target='install'):
        folder=output/name
        with (output/(name+'.log')).open('w') as log:
            subprocess.run(common+['-S',str(source),'-B',str(folder)]+flags,check=True,stdout=log,stderr=subprocess.STDOUT,env=env)
            subprocess.run([str(cmake),'--build',str(folder),'--target',target,'-j','4'],check=True,stdout=log,stderr=subprocess.STDOUT,env=env)
        print('Built '+name,flush=True)
    if not worker_only:
        stage('de265',sources/'libde265-1.1.3',['-DENABLE_SIMD=OFF','-DENABLE_DECODER=OFF','-DENABLE_ENCODER=OFF','-DENABLE_SDL=OFF'])
        stage('x265',sources/'x265_4.2/source',['-DENABLE_SHARED=OFF','-DENABLE_CLI=OFF','-DENABLE_ASSEMBLY=OFF',
            '-DENABLE_NEON=OFF','-DENABLE_NEON_DOTPROD=OFF','-DENABLE_NEON_I8MM=OFF','-DENABLE_SVE=OFF','-DENABLE_SVE2=OFF',
            '-DENABLE_LIBNUMA=OFF','-DHIGH_BIT_DEPTH=ON','-DMAIN12=OFF','-DENABLE_HDR10_PLUS=OFF','-DENABLE_LIBVMAF=OFF'])
        flags=['-DENABLE_PLUGIN_LOADING=OFF','-DWITH_EXAMPLES=OFF','-DWITH_GDK_PIXBUF=OFF','-DBUILD_TESTING=OFF',
               '-DBUILD_DOCUMENTATION=OFF','-DWITH_LIBSHARPYUV=OFF','-DENABLE_MULTITHREADING_SUPPORT=OFF',
               '-DENABLE_PARALLEL_TILE_DECODING=OFF','-DWITH_HEADER_COMPRESSION=OFF',
               '-DX265_INCLUDE_DIR='+str(prefix/'include'),'-DX265_LIBRARY='+str(prefix/'lib/libx265.a'),
               '-DLIBDE265_INCLUDE_DIR='+str(prefix/'include'),'-DLIBDE265_LIBRARY='+str(prefix/'lib/libde265.a')]
        for name in ['KVAZAAR','UVG266','VVDEC','VVENC','X264','OpenH264_DECODER','DAV1D','AOM_DECODER','AOM_ENCODER',
                     'SvtEnc','RAV1E','JPEG_DECODER','JPEG_ENCODER','OpenJPEG_ENCODER','OpenJPEG_DECODER','FFMPEG_DECODER',
                     'OPENJPH_ENCODER','UNCOMPRESSED_CODEC','WEBCODECS']:flags.append('-DWITH_'+name+'=OFF')
        stage('heif',sources/'libheif-1.23.6',flags)
    if codecs_only:return
    c=['clang'] if host else [str(output/'cc')]
    cpp=['clang++'] if host else [str(output/'cxx')]
    obj=output/'heif_core.o'
    subprocess.run(c+['-O2','-std=c11','-DA7_HIF_CORE','-D_GNU_SOURCE','-ffp-contract=off','-fno-fast-math',
        '-fno-strict-aliasing','-c',str(ROOT/'camera_worker/worker.c'),'-o',str(obj)],check=True)
    binary=output/('hif-worker-host' if host else 'hif-worker-aarch64')
    subprocess.run(cpp+['-O2','-std=c++20','-D_GNU_SOURCE','-ffp-contract=off','-fno-fast-math',
        '-I'+str(prefix/'include'),str(ROOT/'camera_worker/heif_worker.cc'),str(obj),
        str(prefix/'lib/libheif.a'),str(prefix/'lib/libde265.a'),str(prefix/'lib/libx265.a'),
        '-lpthread','-lm']+([] if host else ['-static','-s','-Wl,-z,noexecstack'])+['-o',str(binary)],check=True)
    record={'schema':1,'host_variant':host,'network_during_build':False,'camera_accessed':False,
        'archives':[{'file':name,'sha256':value[0],'url':value[1]} for name,value in ARCHIVES.items()],
        'target':'native macOS' if host else 'aarch64-linux-musl static generic ARMv8-A',
        'binary':{'name':binary.name,'bytes':binary.stat().st_size,'sha256':sha(binary)},
        'source_files':[{'path':name,'sha256':sha(ROOT/name)} for name in ['camera_worker/build_heif.py',
            'camera_worker/heif_worker.cc','camera_worker/heif_core.h','camera_worker/worker.c','native/sha256.h']],
        'reused_codec_build':worker_only,
        'static_library_sha256':{p.name:sha(p) for p in (prefix/'lib').glob('*.a')},
        'CMake_version':subprocess.check_output([str(cmake),'--version'],text=True).splitlines()[0],
        'compiler_version':subprocess.check_output(cpp+['--version'],text=True).splitlines()[0],
        'codec_configurations':'Scalar de265, 10-bit-only x265, static HEIF/de265/x265, plugins and optional codecs disabled',
        'distribution_license':'GPL-3.0-or-later combined HIF executable; corresponding sources supplied separately'}
    (output/'build.json').write_text(json.dumps(record,indent=2)+'\n');print(json.dumps(record,indent=2))


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--downloads',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);p.add_argument('--cmake',type=Path,required=True)
    p.add_argument('--host',action='store_true');p.add_argument('--codecs-only',action='store_true');p.add_argument('--worker-only',action='store_true',help='Reuse a previously built local codec prefix; record its library hashes')
    a=p.parse_args();build(a.downloads,a.output,a.cmake,a.host,a.codecs_only,a.worker_only)
