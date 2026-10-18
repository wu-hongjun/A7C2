"""Read-only A7C II still media-object to asynchronous file-write evidence.

Run with .venv-research/bin/python. Unicorn tests only execute small isolated
instruction slices in synthetic host memory; no camera or firmware is changed.
"""
import hashlib
import json
import struct
import sys
from pathlib import Path

from unicorn import Uc, UC_ARCH_ARM64, UC_MODE_ARM
from unicorn.arm64_const import *

from map_ap_interfaces import Image, LIB

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = '4fb6d886c1ac620399d09c9c2ffc074315968cf0ada764062913c4e0783fc7f9'
APP_EXPECTED = '2ea02f7ecbc5bb917316fe8ef4ce5ffd87eb66efaae78ba2ce11420552df539b'
APP_REGIONS = [
    ('RecordStill AP adapter',0x357c9f0,0x357cde0,'bounded contiguous range; next prologue'),
    ('RecordStill instrumentation wrapper',0x34d40c8,0x34d4108,'complete tail dispatch; next prologue'),
    ('queued-result descriptor classifier and handoff',0x34acd48,0x34acef0,'bounded prefix and handoff; not whole procedure'),
    ('queued-result classifier selected branches',0x34acf70,0x34acfb8,'selected branch blocks, not whole procedure'),
    ('local queue dispatch input and switch',0x34ae1ac,0x34ae280,'selected dispatch slice, not whole procedure'),
    ('local queue case9',0x34ae304,0x34ae314,'selected dispatch case, not whole procedure'),
]
INPUT_MAP = [(0x00,0x08,'J0'),(0x08,0x18,'R0'),(0x10,0x28,'H0'),
             (0x18,0x10,'J1'),(0x20,0x20,'R1'),(0x28,0x30,'H1'),(0x30,0x38,'unnamed pointer')]
REGIONS = [
    ('factory', 0xc4de0, 0xc4e6c, 'dynamic symbol'),
    ('JPEG constructor', 0x26d090, 0x26d120, 'return; next entry'),
    ('ARW constructor', 0x267478, 0x2674d0, 'return; next entry'),
    ('HEIF constructor', 0x269fb0, 0x26a034, 'return; next entry'),
    ('addition adapter', 0x270b2c, 0x270dc4, 'existing REA procedure and next entry'),
    ('conversion adapter', 0x2715c0, 0x271630, 'existing REA procedure and next entry'),
    ('finalization adapter', 0x270f9c, 0x2710ec, 'all branch targets; next prologue'),
    ('JPEG addition slot +68', 0x26cd7c, 0x26cd8c, 'entire four-instruction leaf'),
    ('ARW addition slot +68', 0x267178, 0x267188, 'entire four-instruction leaf'),
    ('HEIF addition slot +68', 0x26a168, 0x26a250, 'all branch targets; next entry'),
    ('HEIF flag mapper', 0x26a150, 0x26a168, 'entire leaf'),
    ('ARW conversion slot +40', 0x267148, 0x267150, 'entire two-instruction leaf'),
    ('JPEG conversion slot +40', 0x26f24c, 0x26fda0, 'bounded contiguous range; next prologue'),
    ('HEIF conversion slot +40', 0x26ac20, 0x26b4f0, 'bounded contiguous range; next exported function'),
    ('JPEG finalization slot +18', 0x26e898, 0x26f24c, 'bounded contiguous range; next virtual entry'),
    ('ARW finalization slot +18', 0x268f5c, 0x2698ec, 'bounded contiguous range; next virtual entry'),
    ('HEIF finalization slot +18', 0x26a424, 0x26ac20, 'bounded contiguous range; next virtual entry'),
    ('metadata-object dispatch', 0x2796b0, 0x27970c, 'tail virtual dispatch; next prologue'),
    ('HEIF finalization', 0x25caf4, 0x25ccc4, 'dynamic symbol'),
    ('HEIF core finalization', 0x2659ac, 0x265fe8, 'dynamic symbol'),
    ('HEIF core add descriptors', 0x2653f4, 0x2659ac, 'bounded contiguous range; next exported function'),
    ('HEIF auxiliary diagnostic message helper', 0x26522c, 0x265354, 'all branch targets; next prologue'),
    ('JPEG get', 0x26d540, 0x26d704, 'dynamic symbol'),
    ('HEIF get', 0x26b4f0, 0x26b83c, 'dynamic symbol'),
    ('make header worker', 0xee528, 0xee994, 'dynamic symbol'),
    ('finalize header worker', 0xf09cc, 0xf0cc8, 'dynamic symbol'),
    ('async write and database update', 0xf183c, 0xf22a0, 'dynamic symbol'),
    ('write/transfer sequencer', 0xf22a0, 0xf234c, 'dynamic symbol'),
    ('filesystem write callback', 0xe81ac, 0xe8284, 'dynamic symbol'),
    ('MprIf EncodeStill overloads and WriteStill stubs', 0x2c8cd8, 0x2c8cf8, 'four complete dynamic symbols'),
    ('record implementation', 0xe85a0, 0xe9ab8, 'dynamic symbol'),
]
REA_QUERIES = {
    'virtual': [0x26f24c,0x26ac20,0x2796b0,0x25c3ac,0x25c118,0x25bd78,0x25caf4,
                0xee528,0xf183c,0xe81ac,0x26d540,0x26b4f0],
    'finalizers': [0x26e898,0x268f5c,0x26a424,0x270f9c,0xf09cc,0x2659ac,0x2653f4],
}
CLASSES = [
    ('JPEG', 1, 0x1558, 0x26d090, 0x2fd280, 0x2fb7f0, 0x26f24c, 0x26cd7c),
    ('ARW', 2, 0x14d8, 0x267478, 0x2fd678, 0x2fb6d0, 0x267148, 0x267178),
    ('HEIF', 4, 0x1560, 0x269fb0, 0x2fd8e0, 0x2fb760, 0x26ac20, 0x26a168),
]


def identity(path):
    raw = path.read_bytes()
    return {'path': str(path.relative_to(ROOT)), 'bytes': len(raw),
            'sha256': hashlib.sha256(raw).hexdigest()}


def original_offset(im, address):
    for s in im.elf.iter_segments():
        if s['p_type'] == 'PT_LOAD' and s['p_vaddr'] <= address < s['p_vaddr']+s['p_filesz']:
            return address-s['p_vaddr']+s['p_offset']
    raise AssertionError(hex(address))


def u32(data):
    return struct.unpack('<I', data)[0]


def host_machine(im):
    uc = Uc(UC_ARCH_ARM64, UC_MODE_ARM)
    uc.mem_map(0, 0x400000)
    for s in im.elf.iter_segments():
        if s['p_type'] == 'PT_LOAD':
            uc.mem_write(s['p_vaddr'], im.blob[s['p_offset']:s['p_offset']+s['p_filesz']])
    uc.mem_map(0x10000000, 0x20000)
    uc.reg_write(UC_ARM64_REG_SP, 0x1001f000)
    uc.reg_write(UC_ARM64_REG_LR, 0x1001e000)
    return uc


def leaf_tests(im):
    """Test real +68 bodies on invented descriptors, not a camera ABI."""
    uc = host_machine(im)
    src, out, desc = 0x10000000, 0x10001000, 0x10002000
    stop = 0x1001e000
    selectors = []
    table = im.bytes_at(0x2dd454, 4)
    targets = {0x26a1a0: 0x158, 0x26a204: 0x1b8,
               0x26a210: 0x218, 0x26a21c: 0x188}
    for selector, b in enumerate(table):
        target = 0x26a1a0 + struct.unpack('b', bytes([b]))[0]*4
        selectors.append({'selector': selector, 'branch_ELF_VA': hex(target),
                          'input_descriptor_pointer_offset': hex(targets[target])})
    count = 0
    for entry in (0x26cd7c, 0x267178):
        for selector in range(4):
            uc.mem_write(out, bytes([0xa5])*16)
            for r, v in [(UC_ARM64_REG_X0, 0), (UC_ARM64_REG_X1, 0),
                         (UC_ARM64_REG_X2, selector), (UC_ARM64_REG_X3, out)]: uc.reg_write(r, v)
            uc.emu_start(entry, stop, count=32)
            assert bytes(uc.mem_read(out, 16)) == bytes([0xa5])*8 + struct.pack('<II', 255, 0)
            assert uc.reg_read(UC_ARM64_REG_W0) == 0
            count += 1
    for selector in range(4):
        for flag in (0, 0x20, 0x40, 0xe0):
            for code in range(4):
                uc.mem_write(src, bytes(0x300))
                uc.mem_write(src+targets[int(selectors[selector]['branch_ELF_VA'], 16)], struct.pack('<Q', desc))
                uc.mem_write(desc+4, struct.pack('<II', 6000+selector, 4000+selector))
                uc.mem_write(desc+0x19f, bytes([code<<6, flag]))
                uc.mem_write(out, bytes([0xa5])*16)
                for r,v in [(UC_ARM64_REG_X0,0), (UC_ARM64_REG_X1,src),
                            (UC_ARM64_REG_X2,selector), (UC_ARM64_REG_X3,out)]: uc.reg_write(r,v)
                uc.emu_start(0x26a168, stop, count=128)
                expected_code = (1 if code == 1 else 0 if code == 2 else 2) if selector < 3 else 2
                assert bytes(uc.mem_read(out,16)) == struct.pack('<IIII',6000+selector,4000+selector,int(flag!=0),expected_code)
                assert uc.reg_read(UC_ARM64_REG_W0) == 0
                count += 1
    for selector in range(4):
        uc.mem_write(src, bytes(0x300))
        uc.reg_write(UC_ARM64_REG_X1, src)
        uc.reg_write(UC_ARM64_REG_X2, selector)
        uc.emu_start(0x26a168, stop, count=128)
        assert uc.reg_read(UC_ARM64_REG_W0) == ((-4,-3,-2,-5)[selector] & 0xffffffff)
        count += 1
    for pointer,selector,expected in [(0,0,-1), (src,4,-6), (src,0xffffffff,-6)]:
        uc.reg_write(UC_ARM64_REG_X1,pointer)
        uc.reg_write(UC_ARM64_REG_X2,selector)
        uc.emu_start(0x26a168,stop,count=128)
        assert uc.reg_read(UC_ARM64_REG_W0) == expected & 0xffffffff
        count += 1
    uc.emu_start(0x267148,stop,count=8)
    assert uc.reg_read(UC_ARM64_REG_W0) == 1
    return {'cases': count+1, 'selectors': selectors,
            'scope': 'Isolated original +68 leaves, helper and RAW +40; synthetic descriptors, no media or camera execution.'}


def scatter_tests(im):
    """Actual f1f80..f1fd4 loop: pointer preservation and 4KiB alignment."""
    uc = host_machine(im)
    records, frame, a0, a1, a2 = 0x10000000,0x10008000,0x10009000,0x1000a000,0x1000b000
    results = []
    for lengths in [(0,), (1,), (4095,4096,4097), (0,17,0,8193,1), (0xffffffff,)]:
        uc.mem_write(a0, bytes(0x1000)); uc.mem_write(a1, bytes(0x1000)); uc.mem_write(a2, bytes(0x1000))
        expected = []
        for i,length in enumerate(lengths):
            uc.mem_write(records+i*0x30, struct.pack('<IIQQ', length,0,0x777000+i*100,0x888000+i*100)+bytes(0x18))
            if length: expected.append((0x888000+i*100,0x777000+i*100,(length+4095)&0xfffff000))
        uc.mem_write(frame+0x78,struct.pack('<Q',a0))
        uc.mem_write(frame+0x70,struct.pack('<Q',a1))
        uc.mem_write(frame+0x68,struct.pack('<Q',a2))
        for r,v in [(UC_ARM64_REG_X29,frame),(UC_ARM64_REG_X0,records),
                    (UC_ARM64_REG_X1,records+len(lengths)*0x30),(UC_ARM64_REG_X6,24),
                    (UC_ARM64_REG_W3,0),(UC_ARM64_REG_W27,0)]: uc.reg_write(r,v)
        uc.emu_start(0xf1f80,0xf1fd4,count=1024)
        actual = [tuple(struct.unpack('<Q',uc.mem_read(base+i*24,8))[0] for base in (a0,a1,a2)) for i in range(len(expected))]
        assert actual == expected
        assert uc.reg_read(UC_ARM64_REG_W27) == sum(lengths)&0xffffffff
        assert uc.reg_read(UC_ARM64_REG_W3) == len(expected)
        results.append({'input_lengths':list(lengths),'nonzero_regions':len(expected),
                        'observed_records':actual,'summed_u32_length':uc.reg_read(UC_ARM64_REG_W27)})
    return {'cases':len(results),'results':results,
            'scope':'Only original file-vector construction loop, synthetic source descriptors; no filesystem call executed.'}


def input_handoff_tests(im):
    uc=Uc(UC_ARCH_ARM64,UC_MODE_ARM)
    for start,length in [(0x357c000,0x2000),(0x34ac000,0x2000)]:
        uc.mem_map(start,length);uc.mem_write(start,im.bytes_at(start,length))
    uc.mem_map(0x10000000,0x20000)
    source,frame,out=0x10000000,0x10008000,0x10004000
    values=[0x1111000000000000+i*0x1234567 for i in range(7)]
    uc.mem_write(source,struct.pack('<7Q',*values))
    uc.reg_write(UC_ARM64_REG_X19,source);uc.reg_write(UC_ARM64_REG_X29,frame)
    uc.emu_start(0x357ca60,0x357ca98,count=64)
    for i,(_,destination,_) in enumerate(INPUT_MAP):
        assert struct.unpack('<Q',uc.mem_read(frame+0xb8+destination,8))[0]==values[i]
    cases=[]
    for a in range(3):
        for b in range(3):
            uc.mem_write(source,bytes(0x800));uc.mem_write(out,bytes(0xe0))
            uc.mem_write(source,struct.pack('<I',a));uc.mem_write(source+0x2fc,struct.pack('<I',b))
            for r,v in [(UC_ARM64_REG_X29,frame),(UC_ARM64_REG_X20,out),
                        (UC_ARM64_REG_X22,source),(UC_ARM64_REG_X24,frame+0x100)]:uc.reg_write(r,v)
            uc.emu_start(0x34acdbc,0x34ace6c,count=512)
            expected=[0]*6;counts=[0]*3
            for record,tag in enumerate((a,b)):
                pointer_offset,slot={0:(0xc,0),1:(0x21c,2),2:(0xcc,1)}[tag]
                expected[slot+counts[tag]*3]=source+record*0x2fc+pointer_offset
                counts[tag]+=1
            actual=list(struct.unpack('<6Q',uc.mem_read(out,48)))
            assert actual==expected,(a,b,actual,expected)
            cases.append({'record_type_tags':[a,b],'pointer_offsets_from_payload':[hex(x-source) if x else None for x in actual]})
    return {'cases':10,'pointer_copy_cases':1,'classifier_cases':cases,
            'scope':'Original pointer-copy and two-record classification slices only; no adapter call, queue operation or capture executed.'}


def main():
    im = Image(LIB/'libmpr.so')
    assert hashlib.sha256(im.blob).hexdigest() == EXPECTED
    app=Image(LIB/'appFw.so')
    assert hashlib.sha256(app.blob).hexdigest()==APP_EXPECTED
    relocs = {r['offset']:r for r in im.relocations}
    symbols = {s.name:s['st_value'] for s in im.symbols if isinstance(s['st_shndx'],int)}
    classes = []
    for name,type_id,size,ctor,got,base,convert,addition in CLASSES:
        assert relocs[got]['type'] == 1027 and relocs[got]['addend'] == base
        methods=[]
        for slot in range(0,0x80,8):
            address=base+0x10+slot; r=relocs[address]
            target=r['addend'] if r['type']==1027 else symbols[r['symbol']]+r['addend']
            methods.append({'slot_from_vptr':hex(slot),'entry_ELF_VA':hex(address),
                            'resolved_target_ELF_VA':hex(target),'relocation':r})
        assert int(methods[8]['resolved_target_ELF_VA'],16)==convert
        assert int(methods[13]['resolved_target_ELF_VA'],16)==addition
        assert name.lower() in methods[4]['relocation']['symbol']
        classes.append({'class':name,'factory_type':type_id,'allocation_bytes':size,
                        'constructor_ELF_VA':hex(ctor),'constructor_GOT_relocation':relocs[got],
                        'vtable_base_ELF_VA':hex(base),'object_vptr_ELF_VA':hex(base+0x10),
                        'methods':methods})
    regions=[]
    for source_image,definitions in [(im,REGIONS),(app,APP_REGIONS)]:
      for role,start,end,boundary in definitions:
        body=source_image.bytes_at(start,end-start); rows=source_image.disasm(start,end)
        assert sum(len(bytes.fromhex(r['bytes'])) for r in rows)==end-start
        for row in rows:row['file_offset']=hex(original_offset(source_image,int(row['address'],16)))
        regions.append({'role':role,'start_ELF_VA':hex(start),'end_exclusive_ELF_VA':hex(end),
                        'source_path':str(source_image.path.relative_to(ROOT)),
                        'file_offset':hex(original_offset(source_image,start)),'extent_basis':boundary,
                        'bytes':len(body),'sha256':hashlib.sha256(body).hexdigest(),'instructions':rows})
    # Bind the filesystem callback through the GOT relocation, not names alone.
    assert relocs[0x2fde90]['symbol']=='_ZN3mpr11stillrecord17fsys_callbackfuncEPviPK22FSYSWRITEDIRECTFILERES'
    retained=[]; sessions=[]
    for session,expected in [('still-sink-virtual-r2',12),('still-sink-finalizers',7)]:
        directory=ROOT/'research/evidence/rea'/session
        records=sorted(directory.glob('[0-9][0-9]-*.json'))
        assert len(records)==expected, (session,len(records))
        close=json.loads((directory/'close.json').read_text())
        assert not close.get('isError')
        receipt=json.loads((directory/'receipts.json').read_text())
        assert len(receipt['operations'])==expected+3
        assert all(not x.get('isError') for x in receipt['operations'])
        sessions.append({'session':session,'successful_queries':expected,
                         'close':identity(directory/'close.json'),'receipts':identity(directory/'receipts.json')})
        for p in records:
            record=json.loads(p.read_text());assert not record.get('isError')
            s=record['structuredContent']
            assert s['subject']['digest']['sha256']==EXPECTED
            retained.append({**identity(p),'evidence_id':s['evidence_id'],'operation':s['operation'],
                             'parameters':s['parameters']})
    selected_edges=im.direct_sites({0x270b2c,0x2715c0,0x270f9c,0x7e270,0x7faf0,0x82010,
                                    0x7d030,0x7f530,0x81610,0x7def0,0x27963c,0x2796b0,
                                    0x25c3ac,0x25c118})
    dispatch_table=list(struct.unpack('<41h',app.bytes_at(0x4c5f418,82)))
    assert 0x34ae264+dispatch_table[9]*4==0x34ae304
    assert app.read_string(0x4c90208)=='mpr_RecordStill'
    assert app.read_string(0x4c8e6bb)=='%s J0, H0, R0:(0x%x, 0x%x,0x%x) J1, H1, R1:(0x%x, 0x%x, 0x%x)'
    result={'camera_accessed':False,'filesystem_write_invoked':False,
            'method':'Ordinary ELF relocation and Capstone evidence; isolated host Unicorn tests; separately retained actual REA Evidence.',
            'source':identity(LIB/'libmpr.so'),'sources':[identity(LIB/x) for x in ['libmpr.so','appFw.so']],
            'generator':identity(Path(__file__)),
            'address_convention':'All code/table coordinates are ELF VA; Ghidra adds0x100000. File offsets explicitly mapped with PT_LOAD.',
            'classes':classes,'regions':regions,'host_tests':{'addition_leaves':leaf_tests(im),'writer_scatter_loop':scatter_tests(im),
                                                           'appfw_input_handoff':input_handoff_tests(app)},
            'appfw_input_handoff':{
                'evidence_class':'ordinary original-file ELF/Capstone and isolated host emulation; no additional REA session',
                'source_path':str(app.path.relative_to(ROOT)),
                'adapter_ELF_VA':'0x357c9f0','RecordStill_PLT_callsite_ELF_VA':'0x357cbec',
                'adapter_input_pointer_map':[{'source_offset':hex(src),'recordStillInParam_offset':hex(dst),'diagnostic_role':role}
                                             for src,dst,role in INPUT_MAP],
                'diagnostics':[{'ELF_VA':hex(a),'value':app.read_string(a)} for a in [0x4c8e6bb,0x4c8e6f9,0x4c90208]],
                'wrapper_ELF_VA':'0x34d40c8','wrapper_callsite_ELF_VA':'0x34d40f0',
                'wrapper_direct_callers':app.direct_sites({0x34d40c8}),
                'adapter_direct_callers':app.direct_sites({0x357c9f0}),
                'queued_result_handler_ELF_VA':'0x34acd48','queued_result_record_stride':'0x2fc',
                'queued_result_types':[
                    {'local_header_tag':tag,'role':role,'descriptor_offset':hex(off),'corroborating_libmpr_copy_bytes':size}
                    for tag,role,off,size in [(0,'JPEG',0xc,0xc0),(1,'HEIF',0x21c,0xe0),(2,'RAW',0xcc,0x150)]],
                'selection_rule':'Walk two 0x20-byte local entries; gather like-format pointers with independent counters into J0/R0/H0/J1/R1/H1. Suffix0/1 is per format, not an asserted frame index.',
                'local_queue_dispatch':{'queue_entry_tag_offset':0,'queue_entry_payload_pointer_offset':8,'local_tag':9,
                                        'signed_halfword_table_ELF_VA':'0x4c5f418','branch_base_ELF_VA':'0x34ae264',
                                        'handler_callsite_ELF_VA':'0x34ae30c','table_values':dispatch_table},
                'limits':['Local queue tag9 is not an identified CP command or named capture-completion event.',
                          'Input+0x30 is copied to record+0x38 but mode6 overwrites it with an AP global pointer at0x357cb4c.',
                          'Type tags0/1/2 here are distinct from libmpr factory types1/2/4.',
                          'The source of this queued payload, CP result correlation, buffer lifetime and pixel encoding remain unresolved.']},
            'file_record_contract':{'object_count_offset':'0x11f0','object_array_offset':'0x11f8','record_stride':'0x30',
                                    'logical_length_in_record':'0x8','second_vector_value_in_record':'0x10',
                                    'first_vector_pointer_in_record':'0x18','write_allocation_alignment':4096,
                                    'writer_instruction_range':'0xf1f64..0xf1fd4',
                                    'callback_relocation':relocs[0x2fde90]},
            'selected_direct_callsites':selected_edges,
            'resolved_virtual_edges':[
                {'adapter_ELF_VA':hex(adapter),'callsite_ELF_VA':hex(site),'vptr_slot':hex(slot),
                 'class':c['class'],'target_ELF_VA':c['methods'][slot//8]['resolved_target_ELF_VA'],
                 'basis':'constructor GOT RELA + vptr adjustment + vtable slot relocation + indirect-call instruction'}
                for adapter,site,slot in [(0x270b2c,0x270c58,0x68),(0x2715c0,0x2715fc,0x40),(0x270f9c,0x2710bc,0x18)]
                for c in classes],
            'REA_records':retained,'REA_sessions':sessions,
            'limits':['Resolved +40/+68 methods establish media-container and descriptor operations, not pre-encode RGB pixel ownership.',
                      'No direct or transitive connection from these methods to CP MLUT or the full-resolution JPEG/HEIF encoder is proved.',
                      'Same fixed object offsets join finalization/conversion and file-vector assembly, but runtime object identity and every alternate state/format remain unobserved.',
                      'Fsys_WriteDirectFileAsyncV2 is a submission boundary, not proof of a completed persisted file.',
                      'No claim that all still processing bypasses MLUT follows from this selected region set.',
                      'The 0xffffffff synthetic length tests integer wrapping in a slice, not a reachable camera condition.']}
    out=ROOT/'research/evidence/still-sink.json';out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'regions':len(regions),'instructions':sum(len(r['instructions']) for r in regions),
                      'classes':len(classes),'host_cases':sum(x['cases'] for x in result['host_tests'].values()),
                      'REA_records':len(retained),'output':str(out)},indent=2))


if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--rea-job':
        which=sys.argv[2];assert which in REA_QUERIES
        print(json.dumps({'path':str((LIB/'libmpr.so').relative_to(ROOT)),
                          'output_directory':'research/build/rea-rerun/still-sink-'+which,
                          'queries':[{'name':'procedure_pseudo_code','arguments':{'procedure':f'file-offset:{hex(a)}'}}
                                     for a in REA_QUERIES[which]],'export_bundle':True},indent=2))
    else:
        assert len(sys.argv)==1
        main()
