"""Execute the standalone ARM64 ELF in Unicorn with a narrow host-file sandbox.

Only explicit generated research/build jobs are visible as /job. No arbitrary
syscall forwarding, camera/device paths, process launch or network operations.
This models selected Linux syscalls; it is not a camera kernel or filesystem.
"""
from collections import Counter
import errno
import fcntl
import io
import os
from pathlib import Path
import re
import stat
import struct
import time

from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_INTR
from unicorn.arm64_const import UC_ARM64_REG_X0, UC_ARM64_REG_X8, UC_ARM64_REG_SP, UC_ARM64_REG_CPACR_EL1

ROOT = Path(__file__).resolve().parents[1]
PAGE=4096
NAMES={32:'flock',38:'renameat',25:'fcntl',29:'ioctl',35:'unlinkat',56:'openat',57:'close',62:'lseek',63:'read',64:'write',
       65:'readv',66:'writev',79:'newfstatat',80:'fstat',82:'fsync',93:'exit',94:'exit_group',
       96:'set_tid_address',98:'futex',99:'set_robust_list',113:'clock_gettime',123:'sched_getaffinity',134:'rt_sigaction',
       135:'rt_sigprocmask',172:'getpid',174:'getuid',175:'geteuid',176:'getgid',177:'getegid',178:'gettid',
       214:'brk',215:'munmap',222:'mmap',226:'mprotect',233:'madvise',261:'prlimit64',276:'renameat2',278:'getrandom'}


def execute(executable, job=None, mode='--version', count=150_000_000, fault=None, launcher_selection=None):
    if fault not in (None,'output_ENOSPC','publication_unsupported','directory_sync'):
        raise ValueError('Unknown emulator fault')
    if launcher_selection is not None:
        if mode not in ('--list','--select') or (mode=='--list' and launcher_selection!=[]) or (mode=='--select' and (len(launcher_selection)!=3 or not all(re.fullmatch(r'[a-z0-9_.-]{1,64}',s) for s in launcher_selection))):
            raise ValueError('Only bounded launcher list/select emulation is enabled')
    job = Path(job).resolve() if job else None
    if job and (not job.is_relative_to(ROOT/'research/build') or not job.is_dir()):
        raise ValueError('Emulator accepts only explicitly generated offline build jobs')
    elf=ELFFile(io.BytesIO(Path(executable).read_bytes()))
    if elf['e_machine']!='EM_AARCH64' or elf['e_type']!='ET_EXEC':raise ValueError('Expected ARM64 static executable')
    u=Uc(UC_ARCH_ARM64,UC_MODE_ARM);mapped=set();peak=0
    def mapping(addr,size):
        nonlocal peak
        for p in range(addr&-PAGE,(addr+size+PAGE-1)&-PAGE,PAGE):
            if p not in mapped:
                u.mem_map(p,PAGE);mapped.add(p)
        peak=max(peak,len(mapped)*PAGE)
        if peak>64*1024*1024:raise RuntimeError('Emulator mapping budget')
    loads=[s for s in elf.iter_segments() if s['p_type']=='PT_LOAD']
    for s in loads:
        mapping(s['p_vaddr'],s['p_memsz']);u.mem_write(s['p_vaddr'],s.data())
    stack=0x70000000;mapping(stack,8*1024*1024)
    cursor=stack+8*1024*1024-4096
    def put(raw):
        nonlocal cursor
        cursor-=len(raw);u.mem_write(cursor,raw);return cursor
    args=['jpeg-worker',mode]+(['/job'] if job else [])
    if launcher_selection is not None:args+=launcher_selection
    pointers=[put(a.encode()+b'\0') for a in args]
    random=put(bytes(range(16)));base=min(s['p_vaddr']-s['p_offset'] for s in loads)
    aux=[(3,base+elf['e_phoff']),(4,elf['e_phentsize']),(5,elf['e_phnum']),(6,PAGE),
         (9,elf['e_entry']),(11,1000),(12,1000),(13,1000),(14,1000),(23,0),(25,random),(0,0)]
    words=[len(args),*pointers,0,0,*[x for pair in aux for x in pair]]
    sp=(cursor-len(words)*8)&-16;u.mem_write(sp,struct.pack('<'+'Q'*len(words),*words))
    u.reg_write(UC_ARM64_REG_SP,sp);u.reg_write(UC_ARM64_REG_CPACR_EL1,3<<20)
    fds={};nextfd=3;stdout=bytearray();stderr=bytearray();calls=Counter();exitcode=None;limits=[]
    heap=0x10000000;brk=0x50000000;brk_end=brk
    allowed=re.compile(r'(job\.bin|input\.jpg|metadata\.exif|look\.bin|vectors\.(in|out)|result\.jpg|\.jpeg-worker-[0-9]+-[0-9]+\.tmp)\Z')
    if launcher_selection is not None:allowed=re.compile(r'(catalog\.bin|selection\.bin(?:\.next)?|lock)\Z')
    def string(addr):
        raw=bytearray()
        for i in range(256):
            b=bytes(u.mem_read(addr+i,1))
            if b==b'\0':return raw.decode()
            raw.extend(b)
        raise RuntimeError('Unterminated guest path')
    def path(dirfd,addr):
        name=string(addr)
        if name=='/job' and job:return job
        if dirfd not in fds or fds[dirfd]!=job or not allowed.fullmatch(name):raise RuntimeError('Guest path outside job sandbox: '+name)
        return job/name
    def statbuf(st):
        out=bytearray(128)
        struct.pack_into('<QQIIIIQQqIIq',out,0,st.st_dev,st.st_ino,st.st_mode,st.st_nlink,1000,1000,0,0,
                         st.st_size,4096,0,getattr(st,'st_blocks',0))
        return bytes(out)
    def guest_flags(flags):
        result=flags&3
        for a,b in [(0o100,os.O_CREAT),(0o200,os.O_EXCL),(0o1000,os.O_TRUNC),(0o2000,os.O_APPEND),
                    (0o4000,os.O_NONBLOCK),(0o40000,os.O_DIRECTORY),(0o100000,os.O_NOFOLLOW),(0o2000000,os.O_CLOEXEC)]:
            if flags&a:result|=b
        return result
    def do_call(n,a):
        nonlocal nextfd,heap,brk_end,exitcode
        if n in (93,94):exitcode=a[0];u.emu_stop();return 0
        if n==56:
            p=path(a[0],a[1]);flags=guest_flags(a[2])
            if p==job:
                if a[2]&(0o100|0o1000):raise RuntimeError('Directory mutation')
                value=job
            else:
                if p.name in ('job.bin','input.jpg','metadata.exif','look.bin','vectors.in') and a[2]&(3|0o100|0o1000):
                    raise RuntimeError('Attempt to mutate input')
                if launcher_selection is not None and p.name not in ('selection.bin','selection.bin.next') and a[2]&(3|0o100|0o1000):raise RuntimeError('Mutation outside launcher selection')
                value=os.open(p,flags,a[3]&0o777)
            fd=nextfd;nextfd+=1;fds[fd]=value;return fd
        if n==32 and launcher_selection is not None:
            if a[1] not in (1,2,5,6,8):raise RuntimeError('Unexpected flock operation')
            fcntl.flock(fds[a[0]],a[1]);return 0
        if n==38 and launcher_selection is not None:
            source,target=path(a[0],a[1]),path(a[2],a[3])
            if source.name!='selection.bin.next' or target.name!='selection.bin':raise RuntimeError('Unexpected launcher rename')
            os.replace(source,target);return 0
        if n==57:
            value=fds.pop(a[0]);
            if isinstance(value,int):os.close(value)
            return 0
        if n in (79,80):
            if n==80:
                value=fds[a[0]];st=os.stat(job) if value==job else os.fstat(value);dest=a[1]
            else:
                p=path(a[0],a[1]);st=os.stat(p,follow_symlinks=not bool(a[3]&0x100));dest=a[2]
            u.mem_write(dest,statbuf(st));return 0
        if n==63:
            raw=os.read(fds[a[0]],a[2]);u.mem_write(a[1],raw);return len(raw)
        if n==64:
            raw=bytes(u.mem_read(a[1],a[2]))
            if a[0] in (1,2):
                (stdout if a[0]==1 else stderr).extend(raw);return len(raw)
            return os.write(fds[a[0]],raw)
        if n in (65,66):
            if n==66 and a[0]>=3 and fault=='output_ENOSPC':return -errno.ENOSPC
            total=0
            for i in range(a[2]):
                ptr,size=struct.unpack('<QQ',u.mem_read(a[1]+16*i,16));got=do_call(63 if n==65 else 64,[a[0],ptr,size]);total+=got
                if got<size:break
            return total
        if n==62:return os.lseek(fds[a[0]],a[1] if a[1]<2**63 else a[1]-2**64,a[2])
        if n==25:
            if a[1] in (1,2,4):return 0
            if a[1]==3:return 0
            return -errno.EINVAL
        if n==29:return -errno.ENOTTY
        if n==82:
            value=fds[a[0]]
            if value==job and fault=='directory_sync':return -errno.EIO
            if value!=job:os.fsync(value)
            return 0
        if n==35:
            p=path(a[0],a[1])
            if not p.name.startswith('.jpeg-worker-'):raise RuntimeError('Unexpected unlink target')
            p.unlink();return 0
        if n==276:
            if fault=='publication_unsupported':return -errno.EOPNOTSUPP
            source,target=path(a[0],a[1]),path(a[2],a[3])
            if a[4]!=1 or not source.name.startswith('.jpeg-worker-') or target.name!='result.jpg':raise RuntimeError('Unexpected rename')
            # Host APFS emulation of RENAME_NOREPLACE; actual camera FS support is unproved.
            os.link(source,target);source.unlink();return 0
        if n==222:
            if a[4]!=2**64-1 or not a[3]&0x20 or a[2]&4:raise RuntimeError('Only anonymous non-executable mmap supported')
            size=(a[1]+PAGE-1)&-PAGE
            address=a[0] if a[3]&0x10 else heap
            mapping(address,size)
            if not a[3]&0x10:heap=address+size+PAGE
            return address
        if n==215:
            for p in range(a[0],(a[0]+a[1]+PAGE-1)&-PAGE,PAGE):
                if p in mapped:u.mem_unmap(p,PAGE);mapped.remove(p)
            return 0
        if n==214:
            if a[0]:
                if not 0x50000000<=a[0]<0x50400000:raise RuntimeError('brk budget')
                if a[0]>brk_end:mapping(brk_end,a[0]-brk_end)
                brk_end=a[0]
            return brk_end
        if n in (226,233):return 0 # Protection/advice modeled, no hardware effects.
        if n==261:
            if a[2]:limits.append({'resource':a[1],'limits':list(struct.unpack('<QQ',u.mem_read(a[2],16)))})
            if a[3]:u.mem_write(a[3],struct.pack('<QQ',2**64-1,2**64-1))
            return 0
        if n in (96,172,178):return 1
        if n in (174,175,176,177):return 1000
        if n==134:return 0
        if n==135:
            if a[2]:u.mem_write(a[2],bytes(a[3]))
            return 0
        if n in (98,99):return -errno.ENOSYS
        if n==113:u.mem_write(a[1],struct.pack('<qq',1,0));return 0
        if n==123:
            size=min(a[1],8);u.mem_write(a[2],b'\1'+bytes(size-1));return size
        if n==278:u.mem_write(a[0],bytes((i%256 for i in range(a[1]))));return a[1]
        raise RuntimeError('Unmodeled syscall '+str(n))
    def trap(uc,number,user):
        n=uc.reg_read(UC_ARM64_REG_X8);calls[NAMES.get(n,str(n))]+=1
        a=[uc.reg_read(UC_ARM64_REG_X0+i) for i in range(6)]
        try:value=do_call(n,a)
        except OSError as e:value=-e.errno
        uc.reg_write(UC_ARM64_REG_X0,value%(2**64))
    u.hook_add(UC_HOOK_INTR,trap)
    started=time.monotonic()
    try:
        u.emu_start(elf['e_entry'],0,timeout=30_000_000,count=count)
        if exitcode is None:raise RuntimeError('Guest did not exit within emulator budget')
        return {'exit_code':exitcode,'stdout':stdout.decode(errors='replace'),'stderr':stderr.decode(errors='replace'),
            'syscalls':dict(calls),'requested_resource_limits':limits,'peak_mapped_guest_bytes':peak,
            'elapsed_host_seconds':time.monotonic()-started,
            'injected_fault':fault,
            'scope':'Original cross-built ELF in Unicorn with selected syscall models and generated-job-only host file I/O; not actual Linux/camera execution.'}
    finally:
        for value in fds.values():
            if isinstance(value,int):os.close(value)


if __name__=='__main__':
    import argparse,json
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path)
    p.add_argument('--job',type=Path);p.add_argument('--mode',default='--version')
    a=p.parse_args();print(json.dumps(execute(a.executable,a.job,a.mode),indent=2))
