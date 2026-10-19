// Offline source-bound identity model for the pinned ILCE-7CM2 2.01 libraries.
// No Sony hooks. All spans must be copied while their actual owner is alive.
#pragma once
#include "sony_bridge.h"
namespace a7 {
constexpr size_t FileInfoPrefixBytes201=0x474; // through lock counter, not sizeof(Sony struct)
inline uint64_t le64(const uint8_t* p) noexcept { return le32(p) | uint64_t(le32(p+4))<<32; }
struct StillWrite201 {
    uint64_t record=0, cookie=0;
    uint32_t format_group=0, media=0, sidb_id=0;
    PhotoKind kind=PhotoKind::JPEG;
    char path[128]{};
};
// Call only for the actual mpr submit at 0xf20ec, with the selected RecordStill
// object, not a NULL FIFO sentinel or a cached/pseudo branch. request+0x500 is
// the original cookie. Its first 0x15e0 bytes may be uninitialized: never read it.
inline bool decode_still_write201(uint64_t record_address, const void* record, size_t rn,
                                  const void* request, size_t qn, uint64_t cookie_address,
                                  const void* cookie, size_t cn, StillWrite201& out) noexcept {
    out={};
    if (!record_address || !cookie_address || !record || !request || !cookie ||
        rn!=0xd08 || qn!=0x508 || cn!=0x1680) return false;
    auto r=static_cast<const uint8_t*>(record), q=static_cast<const uint8_t*>(request), c=static_cast<const uint8_t*>(cookie);
    uint32_t group=le32(c+0x1678), media=le32(c+0x15e0);
    if (group>1 || media>3 || le64(c+0x1670)!=record_address ||
        le64(q+0x500)!=cookie_address || le32(q+0x410)!=0x206) return false;
    uint32_t type=le32(r+0x1f4+group*0x588);
    if ((type!=1 && type!=4) || le32(r+0x20c+group*0x588)==UINT32_MAX) return false;
    const char* path=reinterpret_cast<const char*>(c+0x15f0);
    size_t n=strnlen(path,128);
    const char* rp=reinterpret_cast<const char*>(r+0x23c+group*0x588+media*0x106);
    if (!n || n>=127 || strnlen(rp,0x106)!=n || strnlen(reinterpret_cast<const char*>(q),SonyPathBytes)!=n ||
        std::memcmp(path,rp,n+1) || std::memcmp(path,q,n+1)) return false;
    // Actual full-original type and suffix must agree; RAW/sidecar/thumbnail
    // events cannot be promoted merely by having a matching basename.
    if (n<4 || std::strcmp(path+n-4,type==1 ? ".JPG" : ".HIF")) return false;
    out.record=record_address;out.cookie=cookie_address;out.format_group=group;out.media=media;
    out.sidb_id=le32(r+0x130+media*4);out.kind=type==1 ? PhotoKind::JPEG : PhotoKind::HIF;
    std::memcpy(out.path,path,n+1);return true;
}
struct OpenFile201 { uint64_t handle=0, drive=0, backing=0; };
// Snapshot BEFORE the real underlying fclose: common close helper clears these
// fields even on failure and can return 0 without performing any fclose.
inline bool decode_open_file201(const void* file,size_t n,OpenFile201& out) noexcept {
    out={};if (!file || n!=FileInfoPrefixBytes201) return false;
    auto p=static_cast<const uint8_t*>(file);
    if (!(le32(p+0x10)&1)) return false;
    out={le64(p+0x20),le64(p+0x18),le64(p+0x40)};
    return out.handle && out.drive && out.backing;
}
// Observed AP payload from 0x35332f0, while 0x34acd48 retains it across
// mpr_RecordStill at 0x34aceb0. The identifier is not in MPR's late public
// completion callback. No dereference of a live Sony pointer occurs here.
inline bool decode_frame201(const void* payload,size_t n,uint32_t& frame) noexcept {
    if (!payload || n!=0x68c) return false;
    frame=le32(static_cast<const uint8_t*>(payload)+0x688);
    return frame!=UINT32_MAX;
}
// AP set_capture_start serializer 0x3539f44: 16 bytes, header 5/0x4038,
// ID at +4. Acceptance/exposure timing across drive modes remains unproved.
inline bool decode_capture_start201(const void* packet,size_t n,uint32_t& frame) noexcept {
    if (!packet || n!=16) return false;
    auto p=static_cast<const uint8_t*>(packet);
    if (p[0]!=5 || p[1] || p[2]!=0x38 || p[3]!=0x40) return false;
    frame=le32(p+4);return frame!=UINT32_MAX;
}
struct IdentityCandidate201 {
    uint64_t token=0, generation=0, open_handle=0;
    uint32_t frame=0;
    StillWrite201 write{};
};
// Serialized controller model, not callable concurrently by Sony hook threads.
// A future hook must capture the selected record *inside* the AP RecordStill
// dynamic scope under Sony's own FIFO mutex. It must propagate the V2 request
// scope into its synchronous writer/open. No global "last request" is valid.
class IdentityLedger201 {
    struct Frame { uint32_t id=0; uint64_t token=0,generation=0,record=0; bool failed=false,return_seen=false,accepted=false; };
    struct Write { StillWrite201 info{}; OpenFile201 open{}; uint64_t slot=0;
                   bool opened=false,close_started=false,closed=false,wrote=false,failed=false,taken=false,submit_seen=false,accepted=false; };
    Frame frames_[BridgeCapacity]{};
    Write writes_[BridgeCapacity]{};
    size_t nf_=0,nw_=0;bool fault_=false;
    Frame* frame(uint32_t id) noexcept { for (size_t i=0;i<nf_;++i) if (frames_[i].id==id) return &frames_[i];return nullptr; }
    Write* write(uint64_t cookie) noexcept { for (size_t i=0;i<nw_;++i) if (writes_[i].info.cookie==cookie) return &writes_[i];return nullptr; }
    bool poison() noexcept { fault_=true;return false; }
public:
    bool faulted() const noexcept { return fault_; }
    // AFTER durable launcher arming, BEFORE passing the ID-bearing imaging
    // request to Sony. ID wrap/reuse is rejected for this bounded session.
    bool arm(uint32_t id,uint64_t token,uint64_t generation) noexcept {
        if (fault_ || id==UINT32_MAX || !token || nf_==BridgeCapacity || frame(id)) return false;
        for (size_t i=0;i<nf_;++i) if (frames_[i].token==token) return false;
        frames_[nf_++]={id,token,generation,0,false};return true;
    }
    bool arm_start(const void* packet,size_t n,uint64_t token,uint64_t generation) noexcept {
        uint32_t id=0;return decode_capture_start201(packet,n,id) && arm(id,token,generation);
    }
    bool select_record(const void* payload,size_t n,uint64_t record) noexcept {
        uint32_t id=0;if (fault_ || !record || !decode_frame201(payload,n,id)) return false;
        auto f=frame(id);if (!f || f->failed) return false;
        if (f->record) return poison(); // duplicate/ambiguous FIFO observation
        for (size_t i=0;i<nf_;++i) if (frames_[i].record==record) return poison();
        f->record=record;return true;
    }
    // Observe the same scoped native RecordStill return; status 0 is success.
    // Async write/close may arrive earlier; no candidate can bypass this gate.
    bool record_return(uint32_t id,uint64_t record,int32_t status) noexcept {
        auto f=frame(id);if (fault_ || !f || !record || f->record!=record) return false;
        if (f->return_seen) return poison();
        f->return_seen=true;f->accepted=!status;if (status) f->failed=true;return true;
    }
    bool submit(uint64_t record,const void* r,size_t rn,const void* q,size_t qn,
                uint64_t cookie,const void* c,size_t cn) noexcept {
        if (fault_) return false;
        Frame* f=nullptr;for (size_t i=0;i<nf_;++i) if (frames_[i].record==record && record) f=&frames_[i];
        if (!f || f->failed) return false;
        StillWrite201 decoded{};
        if (!decode_still_write201(record,r,rn,q,qn,cookie,c,cn,decoded)) return poison();
        if (nw_==BridgeCapacity || write(cookie)) return poison();
        for (size_t i=0;i<nw_;++i) {
            const auto& p=writes_[i].info;
            if (p.record==record && p.format_group==decoded.format_group && p.media==decoded.media) return poison();
        }
        writes_[nw_++].info=decoded;return true;
    }
    bool submission_return(uint64_t cookie,int32_t status) noexcept {
        auto w=write(cookie);if (fault_ || !w) return false;
        if (w->submit_seen) return poison();
        w->submit_seen=true;w->accepted=!status;if (status) w->failed=true;return true;
    }
    // Capture same synchronous V2 writer's successful open, from file-info
    // validated by fsys_GetFileInfo(handle), not a POSIX fd or inferred pathname.
    bool opened(uint64_t cookie,uint64_t slot,const void* info,size_t n,uint16_t filesystem) noexcept {
        auto w=write(cookie);OpenFile201 open{};
        if (fault_ || !w || w->failed || !slot || w->opened) return false;
        // TNtfs uses a separate global backing handle. That lease is unproved.
        if (filesystem==0x10) { w->failed=true;return false; }
        if (!decode_open_file201(info,n,open)) return poison();
        // PrFILE's stored pathname came through fsys_CreatePathInfo. It need
        // not equal the external Sony pathname; identity comes from the exact
        // synchronous open scope and generated handle, never a basename join.
        for (size_t i=0;i<nw_;++i) if (writes_[i].opened &&
            (writes_[i].slot==slot || writes_[i].open.handle==open.handle)) return poison();
        w->slot=slot;w->open=open;w->opened=true;return true;
    }
    // Only at the actual PrFILE_fclose call after active-bit test.
    // TNtfs is excluded by opened; do not route its underlying call here.
    // fsync/common-helper entry must NOT call this. Copy before slot is cleared.
    bool prfile_close_enter(uint64_t slot,const void* info,size_t n) noexcept {
        if (fault_) return false;
        OpenFile201 open{};if (!decode_open_file201(info,n,open) || le32(static_cast<const uint8_t*>(info)+0x470)) return poison();
        for (size_t i=0;i<nw_;++i) { auto& w=writes_[i];if (w.slot!=slot || !w.opened) continue;
            if (w.failed || w.close_started || w.open.handle!=open.handle || w.open.drive!=open.drive || w.open.backing!=open.backing) return poison();
            w.close_started=true;return true;
        }
        return false; // unrelated Sony open
    }
    bool close_return(uint64_t slot,int32_t status) noexcept {
        if (fault_) return false;
        for (size_t i=0;i<nw_;++i) { auto& w=writes_[i];if (w.slot!=slot || !w.opened) continue;
            if (!w.close_started || w.closed || w.failed) return poison();
            if (status) w.failed=true;else w.closed=true;return true;
        }return false;
    }
    bool completed(uint64_t cookie,int32_t status,const void* result,size_t n) noexcept {
        auto w=write(cookie);if (fault_ || !w || w->failed) return false;
        if (w->wrote) return poison();
        if (status) { w->failed=true;return true; }
        if (!result || n!=FsysResultBytes201) return poison();
        auto p=static_cast<const char*>(result);
        if (strnlen(p,SonyPathBytes)!=std::strlen(w->info.path) || std::strcmp(p,w->info.path)) return poison();
        w->wrote=true;return true;
    }
    bool cancel(uint32_t id) noexcept { auto f=frame(id);if (!f) return false;f->failed=true;return true; }
    // Metadata candidate ONLY. Never authorizes a late pathname reopen. A
    // producer-owned same-open byte snapshot/digest is still mandatory before
    // the worker can accept a job. This library provides no camera byte reader.
    bool take(uint64_t cookie,IdentityCandidate201& out) noexcept {
        auto w=write(cookie);if (fault_ || !w || w->failed || w->taken || !w->accepted || !w->closed || !w->wrote) return false;
        Frame* f=nullptr;for (size_t i=0;i<nf_;++i) if (frames_[i].record==w->info.record) f=&frames_[i];
        if (!f || f->failed || !f->accepted) return false;
        out={f->token,f->generation,w->open.handle,f->id,w->info};w->taken=true;return true;
    }
};
} // namespace a7
extern "C" bool a7_decode_still_write201(uint64_t,const void*,size_t,const void*,size_t,uint64_t,const void*,size_t,a7::StillWrite201*) noexcept;
