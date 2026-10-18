"""Offline native LUT transfer boundary: original instructions, synthetic RAM only.

No USB, device mappings, OS calls, DMA, or camera execution are performed.
"""
import hashlib
import json
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm64_const import UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5, UC_ARM64_REG_X30, UC_ARM64_REG_PC, UC_ARM64_REG_SP

ROOT = Path(__file__).resolve().parents[1]
BASE = ROOT/'research/extracted/ILCE-7CM2-2.01/firmware.tar_unpacked/0700_part_image/dev/nflasha15_unpacked'
AP_HASH = '2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
REGS = [UC_ARM64_REG_X0, UC_ARM64_REG_X1, UC_ARM64_REG_X2, UC_ARM64_REG_X3, UC_ARM64_REG_X4, UC_ARM64_REG_X5]


def ident(path):
    raw = path.read_bytes()
    return {'path': str(path.relative_to(ROOT)), 'bytes': len(raw), 'sha256': hashlib.sha256(raw).hexdigest()}


def region(raw, start, end, file_offset, role, relocations=None):
    body = raw[file_offset:file_offset+end-start]
    ins = list(Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN).disasm(body, start))
    assert len(ins)*4 == len(body)
    rows = []
    for i in ins:
        row = {'address': hex(i.address), 'file_offset': hex(file_offset+i.address-start),
               'bytes': i.bytes.hex(), 'mnemonic': i.mnemonic, 'operands': i.op_str}
        if relocations and i.address in relocations:
            row['relocation'] = relocations[i.address]
        rows.append(row)
    return {'role': role, 'start': hex(start), 'bytes': len(body),
            'file_offset': hex(file_offset), 'sha256': hashlib.sha256(body).hexdigest(), 'instructions': rows}


class APMap:
    """Only the original copy helper; mapping/transfer wrappers are intercepted."""
    DESC, SRC, DST, OBJ1, OBJ2 = 0x10000000, 0x10010000, 0x10020000, 0x10030000, 0x10031000
    STACK, RETURN = 0x20000000, 0x30000000

    def __init__(self, raw):
        self.u = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
        self.maps, self.transfers, self.diagnostics = [], [], []
        self.commands = []
        self.native_length = 0
        for va, size in [(0x3021000, 0x1000), (0x5b70000, 0x1000), (0x5bbd000, 0x1000),
                         (0x34d3000, 0x1000), (0x34cb000, 0x1000), (0x165d000, 0x1000),
                         (0x3680000, 0x1000), (0x3428000, 0x1000), (0x2e06000, 0x1000),
                         (0x2887000, 0x1000), (0x34cf000, 0x1000), (0x34c6000, 0x1000),
                         (self.DESC, 0x40000), (self.STACK, 0x10000), (self.RETURN, 0x1000)]:
            self.u.mem_map(va, size)
        self.u.mem_write(0x3021000, raw[0x3021000:0x3022000])
        self.u.mem_write(0x3428000, raw[0x3428000:0x3429000])
        self.u.mem_write(0x5b70df8, struct.pack('<Q', self.OBJ1))
        self.u.mem_write(0x5bbdeb8, struct.pack('<Q', self.OBJ2))
        self.u.hook_add(UC_HOOK_CODE, self.hook)

    def back(self, value=0):
        self.u.reg_write(UC_ARM64_REG_X0, value)
        self.u.reg_write(UC_ARM64_REG_PC, self.u.reg_read(UC_ARM64_REG_X30))

    def hook(self, u, pc, size, _):
        a = [u.reg_read(r) for r in REGS]
        if pc == 0x3021228:
            assert a[0] == 0
            u.mem_write(a[1], struct.pack('<I4xQ', self.native_length, self.SRC))
            self.back(1)
        elif pc in (0x2e06cc0, 0x2e068cc):
            self.back()
        elif pc == 0x2887b48:
            self.back(self.DESC+0x400)
        elif pc in (0x34cfac8, 0x34c6418):
            self.commands.append({'wrapper':hex(pc),'input':bytes(u.mem_read(a[0],96)).hex()})
            u.mem_write(a[1],bytes(4));self.back()
        elif pc == 0x34d37e8:
            request = bytes(u.mem_read(a[0], 0x18))
            self.maps.append({'physical_address': struct.unpack_from('<Q', request, 8)[0],
                              'capacity': struct.unpack_from('<I', request, 16)[0]})
            u.mem_write(a[1], struct.pack('<Q', self.DST)+bytes(24)+struct.pack('<I', 0))
            self.back()
        elif pc == 0x34d3828:
            u.mem_write(a[1], bytes(4)); self.back()
        elif pc == 0x34cb5e8:
            words = list(struct.unpack('<5I', bytes(u.mem_read(a[0], 20))))
            self.transfers.append(words)
            u.mem_write(a[1], bytes(4)); self.back()
        elif pc == 0x165d270:
            u.mem_write(a[0], bytes([a[1] & 255])*a[2]); self.back(a[0])
        elif pc == 0x165d3e0:
            u.mem_write(a[0], bytes(u.mem_read(a[1], a[2]))); self.back(a[0])
        elif pc == 0x36804c8:
            self.diagnostics.append(a[1]); self.back()
        elif not (0x30210c8 <= pc < 0x3021228 or 0x3428510 <= pc < 0x34289f0 or pc == self.RETURN):
            raise AssertionError('Unexpected AP instruction '+hex(pc))

    def run(self, length, capacity=49152, selection_flag=None):
        payload = bytes((n*37+11) & 255 for n in range(length))
        self.u.mem_write(self.SRC, payload)
        self.u.mem_write(self.DST, b'\xa5'*49152)
        self.u.mem_write(self.DESC, struct.pack('<I4xQ', length, self.SRC))
        self.u.mem_write(self.OBJ1, struct.pack('<IIQI', 0, 0x1234, 0x123456780000, capacity))
        self.u.mem_write(self.OBJ2, struct.pack('<IIQI', 0, 0x9234, 0x234567890000, capacity))
        self.u.reg_write(UC_ARM64_REG_X0, self.DESC)
        if selection_flag is not None:
            self.native_length = length
            entry = self.DESC+0x100
            self.u.mem_write(entry+0x14,struct.pack('<II', 0x456,3))
            self.u.mem_write(entry+0x6c,struct.pack('<I',1))
            self.u.mem_write(entry+0x2c,b'OFFLINE SYNTHETIC LUT\0'+bytes(42))
            self.u.mem_write(self.DESC+0x400,bytes([selection_flag]))
            self.u.reg_write(UC_ARM64_REG_X0,entry)
        self.u.reg_write(UC_ARM64_REG_SP, self.STACK+0xf000)
        self.u.reg_write(UC_ARM64_REG_X30, self.RETURN)
        self.u.emu_start(0x30210c8 if selection_flag is None else 0x34289a8,
                         self.RETURN, timeout=5_000_000, count=10000)
        assert self.u.reg_read(UC_ARM64_REG_PC)==self.RETURN
        ret = self.u.reg_read(UC_ARM64_REG_X0)
        valid = 0 < length <= capacity
        assert ret == int(valid)
        if valid:
            assert self.transfers == [[0x1234, 0, 0x9234, 0, length]]
            assert self.maps == [{'physical_address': 0x123456780000, 'capacity': capacity}]
            assert bytes(self.u.mem_read(self.DST, length)) == payload
            assert bytes(self.u.mem_read(self.DST+length, 49152-length)) == b'\xa5'*(49152-length)
        else:
            assert not self.transfers and not self.maps
        if selection_flag is not None:
            assert len(self.commands)==2
            primary=bytes.fromhex(self.commands[0]['input'])
            secondary=bytes.fromhex(self.commands[1]['input'])
            assert struct.unpack_from('<3I',primary)==(0x456,0x62a,0x9234)
            assert struct.unpack_from('<2I',secondary)==(3,0x9234)
            assert struct.unpack_from('<4I',primary,12)==((0,48000,48000,124) if selection_flag else (0,length,0,0))
            assert primary[92]==selection_flag
        return {'length': length, 'capacity': capacity, 'return': ret,
                'source_resource': '0x1234', 'destination_resource': '0x9234',
                'transfer_request_words': self.transfers, 'selection_flag':selection_flag,
                'command_inputs':self.commands,'passed': True}


def scatter_case(text, relocations, src, dst, src_offset, dst_offset, size, direction):
    """Execute original kernel descriptor builder, never its DMA submission."""
    code, ram, stack, stop = 0x1000000, 0x2000000, 0x3000000, 0x4000000
    u = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
    for addr, length in [(code, 0x2000), (ram, 0x10000), (stack, 0x10000), (stop, 0x1000)]:
        u.mem_map(addr, length)
    u.mem_write(code, text)
    sptr, dptr, output = ram, ram+0x1000, ram+0x2000
    def descriptor(address, segments, offset):
        u.mem_write(address, struct.pack('<II', len(segments), offset))
        for n, (base, length) in enumerate(segments):
            u.mem_write(address+8+n*16, struct.pack('<QI4x', base, length))
    descriptor(sptr, src, src_offset); descriptor(dptr, dst, dst_offset)
    cache = []
    def hook(machine, pc, ins_size, unused):
        off = pc-code
        rel = relocations.get(off)
        if rel and rel['type'] == 283:
            name = rel['symbol']; a = [u.reg_read(r) for r in REGS]
            if name == 'memset':
                u.mem_write(a[0], bytes([a[1] & 255])*a[2])
            elif name == 'arch_va_to_phys':
                u.reg_write(UC_ARM64_REG_X0, a[0]+0x100000000)
            elif name == 'icdt_proc_measure_start':
                u.mem_write(a[0], bytes(4)); u.reg_write(UC_ARM64_REG_X0, 0)
            elif name == 'icdt_proc_measure_end':
                u.reg_write(UC_ARM64_REG_X0, 0)
            elif name == '__udif_cache_ctrl':
                cache.append(a[:3]); u.reg_write(UC_ARM64_REG_X0, 0)
            else:
                raise AssertionError('Unexpected kernel import '+name)
            u.reg_write(UC_ARM64_REG_PC, pc+4)
    u.hook_add(UC_HOOK_CODE, hook)
    for r,v in zip(REGS,(size,direction,sptr,dptr,output)):
        u.reg_write(r,v)
    u.reg_write(UC_ARM64_REG_SP,stack+0xf000); u.reg_write(UC_ARM64_REG_X30,stop)
    u.emu_start(code+0x5d0,stop,timeout=5_000_000,count=100000)
    assert u.reg_read(UC_ARM64_REG_PC)==stop
    assert u.reg_read(UC_ARM64_REG_X0)==0 and len(cache)==1
    records=[]
    for n in range(25):
        words=struct.unpack('<6I',bytes(u.mem_read(output+24*n,24)))
        if words[0]==6:break
        records.append({'flags':words[0],'bytes':words[1], 'source':words[2]+(words[3]<<32),
                        'destination':words[4]+(words[5]<<32)})
    else: raise AssertionError('No descriptor terminator')
    def logical(segments,offset,length):
        values=[]
        for base,count in segments:
            if offset>=count:offset-=count;continue
            take=min(count-offset,length-len(values))
            values.extend(range(base+offset,base+offset+take));offset=0
            if len(values)==length:return values
        raise AssertionError('Insufficient synthetic allocation')
    want_src=logical(src,src_offset,size);want_dst=logical(dst,dst_offset,size)
    got_src=[];got_dst=[]
    for row in records:
        got_src.extend(range(row['source'],row['source']+row['bytes']))
        got_dst.extend(range(row['destination'],row['destination']+row['bytes']))
    assert got_src[:size]==want_src and got_dst[:size]==want_dst
    emitted=sum(r['bytes'] for r in records)
    assert emitted>=size
    assert got_src==logical(src,src_offset,emitted) and got_dst==logical(dst,dst_offset,emitted)
    assert emitted==(size if len(src)==len(dst)==1 else 54939)
    return {'bytes':size,'direction':direction,'source_offset':src_offset,'destination_offset':dst_offset,
            'source_segments':src,'destination_segments':dst,'DMA_descriptors':records,
            'requested_prefix_addresses_match':True,'emitted_bytes':emitted,
            'excess_bytes':emitted-size,'exact_requested_length':emitted==size,'characterization_passed':True}


def main():
    ap=BASE/'lib/appFw.so'; kernel=BASE/'kmod/icdt.ko'; provider=BASE/'lib/libInfraInterchipDatatrans.so'
    assert ident(ap)['sha256']==AP_HASH
    raw=ap.read_bytes()
    with kernel.open('rb') as f:
        elf=ELFFile(f);sec=elf.get_section_by_name('.text');text=sec.data();off=sec['sh_offset']
        syms=elf.get_section_by_name('.symtab')
        rels={r['r_offset']:{'type':r['r_info_type'],'symbol':syms.get_symbol(r['r_info_sym']).name,
                              'addend':r['r_addend']} for r in elf.get_section_by_name('.rela.text').iter_relocations()}
    regions=[region(raw,0x30210c8,0x3021228,0x30210c8,'AP mapped copy and two-resource dispatch'),
             region(raw,0x3514f54,0x3515288,0x3514f54,'AP transfer request and completion future'),
             region(raw,0x34285ec,0x3428630,0x34285ec,'selection returns destination resource ID'),
             region(provider.read_bytes(),0x990,0xa1c,0x990,'interchip async provider OSAL envelope'),
             region(text,0x2c8,0x584,0x2c8,'kernel resource segment lookup',rels),
             region(text,0x5d0,0xa38,0x5d0,'kernel scatter/gather DMA descriptor builder',rels),
             region(text,0xa50,0xea8,0xa50,'kernel transfer setup and DMA submission boundary',rels),
             region(text,0xf18,0xf3c,0xf18,'kernel exported interchip_transfer_async',rels)]
    for subject, selected in ((ap, regions[:3]), (provider, regions[3:4])):
        with subject.open('rb') as f:
            elf=ELFFile(f); symbols=elf.get_section_by_name('.dynsym')
            plt=elf.get_section_by_name('.plt')['sh_addr']
            imports={plt+32+16*i:symbols.get_symbol(r['r_info_sym']).name
                     for i,r in enumerate(elf.get_section_by_name('.rela.plt').iter_relocations())}
        for item in selected:
            item['source_path']=str(subject.relative_to(ROOT))
            for ins in item['instructions']:
                if ins['mnemonic']=='bl' and ins['operands'].startswith('#'):
                    target=int(ins['operands'][1:],16)
                    if target in imports:ins['import']=imports[target]
    for row in regions[4:]:
        row['source_path']=str(kernel.relative_to(ROOT))
        row['address_space']='icdt.ko .text section relative; relocations NOT applied'
        row['file_offset']=hex(int(row['file_offset'],16)+off)
        for ins in row['instructions']:ins['file_offset']=hex(int(ins['file_offset'],16)+off)
    regions.extend([region(raw,0x342888c,0x34289f0,0x342888c,'destination ID flows through selection wrapper into LUT registration input')])
    regions[-1]['source_path']=str(ap.relative_to(ROOT))
    copies=[APMap(raw).run(n) for n in (0,48000,48128,49152,49153)]
    selection=[APMap(raw).run(48000,selection_flag=0),APMap(raw).run(48128,selection_flag=1)]
    scatter=[]
    for n in (48000,48128):
        for direction in (0,1):
            scatter.append(scatter_case(text,rels,[(0x100000000,65536)],[(0x200000000,65536)],0,0,n,direction))
            scatter.append(scatter_case(text,rels,[(0x100000000,20000),(0x110000000,20000),(0x120000000,20000)],
                                        [(0x200000000,15000),(0x210000000,20000),(0x220000000,20000)],127,61,n,direction))
    result={'camera_accessed':False,'target_executed_on_camera':False,'host_emulation_performed':True,
            'method':'Original AP and kernel instructions in bounded host Unicorn; external operations intercepted.',
            'sources':[ident(ap),ident(kernel),ident(provider)],'generator':ident(Path(__file__)),
            'regions':regions,'AP_copy_cases':copies,'AP_selection_cases':selection,
            'kernel_scatter_cases':scatter,'all_checks_passed':True,
            'findings':{'AP_source_and_CP_destination_are_distinct_resources':True,
                        'AP_request_source_and_destination_offsets': [0,0],
                        'AP_selected_resource_is_destination':True,
                        'AP_primary_registration_input_uses_destination_ID':True,
                        'kernel_requested_prefix_preserved_across_segment_boundaries':True,
                        'synthetic_scatter_cases_exceed_requested_length':4,
                        'interchip_service_OSAL_event':'0x02820746',
                        'CP_resolved_resource_equality_proven':False},
            'limits':['Synthetic resource descriptors do not establish live allocation, physical addresses or camera behavior.',
                      'No DMA or cache operation executes; cache and address translation are stand-ins.',
                      'Four synthetic fragmented-allocation cases emit more DMA bytes than requested; these are retained counterexamples, not successful exact-length transfers.',
                      'The selected-resource registration and CP lookup must still be joined to this exact transfer destination.',
                      'Successful transfer submission is not completion; AP wrapper uses a callback/future boundary.',
                      'Kernel module addresses are section-relative and require real load relocations on camera.']}
    out=ROOT/'research/evidence/lut-transfer.json';out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'AP_copy_cases':len(copies),'AP_selection_cases':len(selection),'scatter_cases':len(scatter),
                      'source_destination_bytes_checked':sum(r['emitted_bytes']*2 for r in scatter),
                      'all_checks_passed':True,'output':str(out)},indent=2))


if __name__=='__main__':main()
