// Offline reconstruction core for the pinned FSYS scratch-write path.
// No Sony pointer lookup, allocation, filesystem I/O or hook installation.
#pragma once
#include "sony_identity.h"
namespace a7 {
constexpr size_t EncodedPhotoLimit201=64u*1024u*1024u;
struct OwnedPhoto201 {
    IdentityCandidate201 identity{};
    const uint8_t* bytes=nullptr;
    size_t size=0;
};
// One instance per tracked write; controller-serialized. Storage is a dedicated
// caller-owned lease, allocated BEFORE capture and retained through consumption.
// Never hand a DMM address to chunk_enter: the future adapter must establish the
// readable CPU scratch span at libInfraFsys 0x2f088 in this request's scope.
class OwnedWriteImage201 {
    uint8_t* storage_;
    size_t capacity_,written_=0,pending_=0,logical_=0;
    uint64_t cookie_,handle_;
    bool failed_=false,started_=false,trimmed_=false,released_=false;
    bool fail() noexcept { failed_=true;return false; }
public:
    OwnedWriteImage201(void* storage,size_t capacity,uint64_t cookie,uint64_t handle) noexcept
      : storage_(static_cast<uint8_t*>(storage)),capacity_(capacity),cookie_(cookie),handle_(handle) {
        if (!storage || !capacity || capacity>EncodedPhotoLimit201 || !cookie || !handle) failed_=true;
    }
    bool failed() const noexcept { return failed_; }
    // Explicit same-open zero-position proof, never inferred from x6==0 or a
    // filename. Recovering this observation in Sony's backend is a deployment
    // gate; passing zero in an offline fixture does not establish it on camera.
    bool start_at(uint64_t cookie,uint64_t handle,uint64_t position,int32_t status) noexcept {
        if (failed_ || released_) return false;
        if (started_ || cookie!=cookie_ || handle!=handle_ || position || status) return fail();
        started_=true;return true;
    }
    // BEFORE the synchronous 0x2eaf4 call at 0x2f0a0. offset is x6 and
    // count is w27 from that exact loop, not a file position guessed by time.
    // Copy now: the scratch allocation is released at 0x2ef8c before callback.
    // data must be readable for exactly n bytes and must not alias our lease.
    bool chunk_enter(uint64_t cookie,uint64_t handle,size_t offset,const void* data,size_t n) noexcept {
        if (failed_ || released_) return false;
        if (!started_ || cookie!=cookie_ || handle!=handle_ || pending_ || trimmed_ || !n || !data ||
            offset!=written_ || n>capacity_-written_) return fail();
        auto begin=reinterpret_cast<uintptr_t>(data),own=reinterpret_cast<uintptr_t>(storage_);
        if (n>UINTPTR_MAX-begin || capacity_>UINTPTR_MAX-own ||
            (begin<own+capacity_ && own<begin+n)) return fail();
        std::memcpy(storage_+written_,data,n);pending_=n;return true;
    }
    // Native FSYS checks both sign and exact count at 0x2f0bc..0x2f0c4.
    bool chunk_return(uint64_t cookie,uint64_t handle,int32_t bytes_written) noexcept {
        if (failed_ || released_) return false;
        if (cookie!=cookie_ || handle!=handle_ || !pending_ || bytes_written<0 ||
            uint32_t(bytes_written)!=pending_) return fail();
        written_+=pending_;pending_=0;return true;
    }
    // AFTER native PrFILE_ftruncate at 0x2f128 succeeds. request+0x42c is
    // logical size; scratch transfer may include alignment padding beyond EOF.
    bool truncate_return(uint64_t cookie,uint64_t handle,size_t logical,int32_t status) noexcept {
        if (failed_ || released_) return false;
        if (cookie!=cookie_ || handle!=handle_ || pending_ || trimmed_ || status ||
            !logical || logical>written_) return fail();
        logical_=logical;trimmed_=true;return true;
    }
    // Only the ledger can supply success of RecordStill, submission, real
    // close AND async completion. Still no installation, sRGB assertion or
    // media/playback registration. Destination is the private worker workspace.
    bool take(IdentityLedger201& ledger,OwnedPhoto201& out) noexcept {
        out={};
        if (failed_ || released_ || pending_ || !trimmed_) return false;
        IdentityCandidate201 candidate{};
        if (!ledger.take(cookie_,candidate)) return false;
        if (candidate.write.cookie!=cookie_ || candidate.open_handle!=handle_) return fail();
        out={candidate,storage_,logical_};released_=true;return true;
    }
};
} // namespace a7
