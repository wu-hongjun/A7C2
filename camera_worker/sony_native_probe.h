// Explicitly invoked offline/native probe core; no loader, patch or constructor.
#pragma once
#include "sony_owned_image.h"
namespace a7 {
struct alignas(16) ProbeRegisters201 {
    uint64_t x[31],sp;
    uint8_t q[32][16];
    uint64_t nzcv,fpcr,fpsr,pc;
};
static_assert(sizeof(ProbeRegisters201)==800);
static_assert(offsetof(ProbeRegisters201,sp)==248 && offsetof(ProbeRegisters201,q)==256 &&
              offsetof(ProbeRegisters201,nzcv)==768 && offsetof(ProbeRegisters201,pc)==792);
using ReadExact201=bool(*)(uint64_t,void*,size_t) noexcept;
// A lifetime-bound, already established physical -> CPU mapping. This is not
// permission to mmap arbitrary firmware addresses or infer virtual==physical.
struct CpuWindow201 { uint64_t physical=0,cpu=0,length=0; };
struct StreamPiece201 { uint64_t address=0,length=0; };
constexpr size_t StreamPieces201=520; // bounded policy, not Sony's struct limit
struct NativeProbe201 {
    // 0 prepared, 1 producer owns, 2 producer quiescent, 3 faulted, 4 taken.
    // Metadata/collector access is single-producer; release/acquire on scope
    // exit publishes it to the serialized controller. Contexts are never reused.
    std::atomic<uint32_t> phase{0};
    std::atomic<bool> scope_fault{false};
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    OwnedWriteImage201* image=nullptr;
    ReadExact201 read=nullptr;
    uint64_t cookie=0,handle=0,slot=0,drive=0,backing=0,load_bias=0;
    uint64_t descriptor=0,cursor_end=0,written=0;
    CpuWindow201 windows[2]{};
    StreamPiece201 pieces[StreamPieces201]{};
    size_t count=0;
    bool failed=false,lease_ready=false,pending=false;
    bool fail() noexcept { failed=true;phase.store(3,std::memory_order_release);return false; }
    bool claim_scope() noexcept {
        uint32_t expected=0;
        if(phase.compare_exchange_strong(expected,1,std::memory_order_acquire))return true;
        // A second producer cannot mutate ordinary context fields. No retry
        // loop or software mutex; the controller checks this atomic fault too.
        scope_fault.store(true,std::memory_order_release);
        return false;
    }
    void release_scope() noexcept {
        uint32_t expected=1;phase.compare_exchange_strong(expected,2,std::memory_order_release);
    }
    static bool range(uint64_t base,uint64_t n) noexcept { return base>=4096 && n && n<=UINT64_MAX-base; }
    bool mappings_valid() const noexcept {
        bool any=false;
        for (auto& w:windows) if(w.length) {
            if(!range(w.physical,w.length)||!range(w.cpu,w.length))return false;
            any=true;
        }
        auto& a=windows[0];auto& b=windows[1];
        return any && (!a.length||!b.length||
            ((a.physical+a.length<=b.physical||b.physical+b.length<=a.physical)&&
             (a.cpu+a.length<=b.cpu||b.cpu+b.length<=a.cpu)));
    }
    // Called while this request owns drive+0x66a0. Its return value of 1 is a
    // cursor, NOT a CPU byte buffer. Only the traced physical-vector mode 2 is
    // admitted. Header/vector copies use fault-contained self-memory reads.
    bool capture_stream(uint64_t address) noexcept {
        if(failed||lease_ready||!read||!image||!range(address,0x38)||!mappings_valid())return fail();
        uint8_t h[0x38]{},again[0x38]{};
        if(!read(address,h,sizeof(h)))return fail();
        uint32_t n=le32(h+4);uint64_t vectors=le64(h+8),work=le64(h+0x10),end=le64(h+0x20);
        if(le32(h)!=2||!n||n>StreamPieces201||le64(h+0x18)!=1||end<=1||
           end-1>EncodedPhotoLimit201||vectors!=address+0x38||
           !range(vectors,uint64_t(n)*32)||work!=vectors+uint64_t(n)*16)return fail();
        uint64_t total=0;
        for(uint32_t i=0;i<n;++i) {
            uint8_t entry[16]{};
            if(!read(vectors+16*i,entry,16))return fail();
            auto& p=pieces[i];p={le64(entry),le64(entry+8)};
            if(!range(p.address,p.length)||p.length>EncodedPhotoLimit201-total)return fail();
            total+=p.length;
        }
        if(total!=end-1||!read(address,again,sizeof(again))||std::memcmp(h,again,sizeof(h)))return fail();
        descriptor=address;cursor_end=end;count=n;lease_ready=true;return true;
    }
    bool copy(size_t offset,uint8_t* target,size_t bytes) noexcept {
        if(!lease_ready||failed||offset>=cursor_end-1||bytes>cursor_end-1-offset)return false;
        uint64_t skip=offset;size_t copied=0;
        for(size_t i=0;i<count && copied<bytes;++i) {
            auto p=pieces[i];if(skip>=p.length){skip-=p.length;continue;}
            uint64_t address=p.address+skip,n=p.length-skip;skip=0;
            if(n>bytes-copied)n=bytes-copied;
            const CpuWindow201* match=nullptr;
            for(auto& w:windows)if(w.length && address>=w.physical && address-w.physical<w.length && n<=w.length-(address-w.physical)) {
                if(match)return false;match=&w;
            }
            if(!match)return false;
            uint64_t cpu=match->cpu+(address-match->physical),dst=reinterpret_cast<uintptr_t>(target+copied);
            if(n>UINT64_MAX-dst || (cpu<dst+n && dst<cpu+n) || !read(cpu,target+copied,size_t(n)))return false;
            copied+=size_t(n);
        }
        return copied==bytes;
    }
    struct Copy { NativeProbe201* probe;size_t offset; };
    static bool copy_into(void* opaque,uint8_t* target,size_t n) noexcept {
        auto c=static_cast<Copy*>(opaque);return c->probe->copy(c->offset,target,n);
    }
    bool verify_stream() const noexcept {
        uint8_t h[0x38]{};
        if(!read(descriptor,h,sizeof(h))||le32(h)!=2||le32(h+4)!=count||
           le64(h+8)!=descriptor+0x38||le64(h+0x10)!=descriptor+0x38+count*16||
           le64(h+0x18)!=1||le64(h+0x20)!=cursor_end)return false;
        for(size_t i=0;i<count;++i) {
            uint8_t entry[16]{};
            if(!read(descriptor+0x38+i*16,entry,16)||le64(entry)!=pieces[i].address||
               le64(entry+8)!=pieces[i].length)return false;
        }
        return true;
    }
    // Exact probe site: PFFILE_fwrite+0x3fc, AFTER append handling, while Sony
    // holds the volume critical section. x25 is the same pf2/PFFILE backing.
    // Live locks/attachment and DMA/cache visibility are still hardware gates.
    bool locked_write(const ProbeRegisters201& r) noexcept {
        if(phase.load(std::memory_order_acquire)!=1||scope_fault.load(std::memory_order_acquire)||failed||pending||!lease_ready||!read||!image||!mappings_valid()||load_bias>UINT64_MAX-0x7af08||
           r.pc!=load_bias+0x7af08||r.x[25]!=backing||!range(r.x[29],0x108))return fail();
        uint8_t file[FileInfoPrefixBytes201]{},pf[0x40]{},args[24]{},active[8]{};OpenFile201 open{};
        if(!read(slot,file,sizeof(file))||!decode_open_file201(file,sizeof(file),open)||
           open.handle!=handle||open.drive!=drive||open.backing!=backing||
           !range(drive,0x66a8)||!read(drive+0x66a0,active,8)||le64(active)!=descriptor||
           !verify_stream()||!read(backing,pf,sizeof(pf))||!(le32(pf)&1)||!le64(pf+0x18)||
           !read(r.x[29]+0xe0,args,sizeof(args)))return fail();
        uint64_t n=le64(args),size=le64(args+8),cursor=le64(args+16),position=le64(pf+0x38);
        if(size!=1||!n||n>EncodedPhotoLimit201||cursor!=written+1||
           position!=written||cursor>=cursor_end||n>cursor_end-cursor)return fail();
        if(!written && !image->start_at(cookie,handle,position,0))return fail();
        Copy c{this,size_t(written)};
        if(!image->chunk_enter_resolved(cookie,handle,size_t(written),size_t(n),copy_into,&c))return fail();
        pending=true;return true;
    }
    // At the enclosing PrFILE/FSYS write return, not a pf2 element-count return.
    bool write_return(int32_t status) noexcept {
        if(failed||!pending||!image||!image->chunk_return(cookie,handle,status))return fail();
        written+=uint32_t(status);pending=false;return true;
    }
    bool take(IdentityLedger201& ledger,OwnedPhoto201& out) noexcept {
        out={};
        if(phase.load(std::memory_order_acquire)!=2||scope_fault.load(std::memory_order_acquire)||failed||pending||!lease_ready||!image||!image->take(ledger,out))return false;
        uint32_t expected=2;
        if(phase.compare_exchange_strong(expected,4,std::memory_order_acq_rel)&&!scope_fault.load(std::memory_order_acquire))return true;
        out={};return false;
    }
};
// Thread-owned fixed scope stack. Unknown nested requests MUST enter nullptr,
// hiding a tracked outer request. Non-LIFO unwind/overflow poisons observation.
class NativeScopes201 {
    struct Entry { NativeProbe201* probe=nullptr;uint64_t ticket=0; };
    Entry stack_[8]{};size_t depth_=0;uint64_t serial_=0;bool failed_=false;
    void fault() noexcept { failed_=true;for(size_t i=0;i<depth_;++i)if(stack_[i].probe)stack_[i].probe->fail(); }
public:
    uint64_t enter(NativeProbe201* p) noexcept {
        if(failed_||depth_==8||serial_==UINT64_MAX){fault();return 0;}
        if(p&&!p->claim_scope())p=nullptr;
        stack_[depth_++]={p,++serial_};return serial_;
    }
    bool leave(uint64_t ticket) noexcept {
        if(failed_||!depth_||stack_[depth_-1].ticket!=ticket){fault();return false;}
        if(auto p=stack_[depth_-1].probe)p->release_scope();
        --depth_;return true;
    }
    NativeProbe201* current() noexcept { return !failed_&&depth_?stack_[depth_-1].probe:nullptr; }
    bool failed() const noexcept { return failed_; }
};
} // namespace a7
extern "C" bool a7_read_self201(uint64_t,void*,size_t) noexcept;
extern "C" uint64_t a7_probe_enter201(a7::NativeProbe201*) noexcept;
extern "C" uint64_t a7_probe_enter_request201(a7::NativeProbe201*,uint64_t) noexcept;
extern "C" bool a7_probe_leave201(uint64_t) noexcept;
extern "C" void a7_probe_locked_write201(const a7::ProbeRegisters201*) noexcept;
extern "C" int32_t a7_probe_write_return201(int32_t) noexcept;
