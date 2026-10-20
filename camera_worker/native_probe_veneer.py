"""Generate ONE offline AArch64 instruction probe, never patch/install it.

The caller must supply an empty, executable, nearby allocation in a future
authorized loader. No address below is a usable runtime camera address.
"""
import struct
import io
import subprocess
import tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile

SITE=0x7af08
DISPLACED=bytes.fromhex('201f40f9') # checked against original ldr x0,[x25,#0x38]

def build(site,veneer,callback,original):
    if original!=DISPLACED:raise ValueError('Pinned displaced instruction differs')
    if any(type(a) is not int or a<4096 or a%4 or a>2**64-0x10000 for a in [site,veneer,callback]):raise ValueError('Invalid aligned probe address')
    def branch(a,b):
        delta=b-a
        if delta%4 or not -(1<<27)<=delta<(1<<27):raise ValueError('Probe branch out of range; no long-branch fallback')
        return struct.pack('<I',0x14000000|((delta//4)&0x3ffffff))
    lines=['sub sp,sp,#800']
    for i in range(0,30,2):lines.append(f'stp x{i},x{i+1},[sp,#{i*8}]')
    lines+=['str x30,[sp,#240]','add x16,sp,#800','str x16,[sp,#248]']
    for i in range(0,32,2):lines.append(f'stp q{i},q{i+1},[sp,#{256+i*16}]')
    for register,offset in [('nzcv',768),('fpcr',776),('fpsr',784)]:lines+=[f'mrs x16,{register}',f'str x16,[sp,#{offset}]']
    def constant(reg,value):return [f'movz {reg},#{value&65535}']+[f'movk {reg},#{(value>>shift)&65535},lsl #{shift}' for shift in (16,32,48)]
    lines+=constant('x16',site)+['str x16,[sp,#792]','mov x0,sp']+constant('x16',callback)+['blr x16']
    for register,offset in [('nzcv',768),('fpcr',776),('fpsr',784)]:lines+=[f'ldr x16,[sp,#{offset}]',f'msr {register},x16']
    for i in range(0,32,2):lines.append(f'ldp q{i},q{i+1},[sp,#{256+i*16}]')
    for i in range(0,30,2):lines.append(f'ldp x{i},x{i+1},[sp,#{i*8}]')
    lines+=['ldr x30,[sp,#240]','add sp,sp,#800']
    with tempfile.TemporaryDirectory(prefix='a7-offline-veneer-') as directory:
        source=Path(directory)/'veneer.S';obj=Path(directory)/'veneer.o'
        source.write_text('.text\n'+ '\n'.join(lines)+'\n')
        subprocess.run(['clang','-target','aarch64-linux-gnu','-march=armv8-a','-c',str(source),'-o',str(obj)],check=True,capture_output=True)
        elf=ELFFile(io.BytesIO(obj.read_bytes()))
        if elf['e_machine']!='EM_AARCH64' or elf.get_section_by_name('.rela.text'):raise ValueError('Unresolved veneer object')
        code=elf.get_section_by_name('.text').data()+original
    code+=branch(veneer+len(code),site+4)
    return {'patch':branch(site,veneer),'veneer':code,'stack_bytes':800,
            'scope':'Offline byte plan only; no process access, allocation, patch write, loader or installation'}
