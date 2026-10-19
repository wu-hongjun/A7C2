// Offline integration core. This does not register callbacks or patch Sony code.
// Caller owns lifecycle, readable spans and firmware/process identity validation.
#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace a7 {
constexpr size_t SonyPathBytes = 0x40f;
constexpr size_t FsysResultBytes201 = 0x4d8;
constexpr size_t BridgeCapacity = 32;
constexpr size_t MailboxCapacity = 128;
inline uint32_t le32(const uint8_t* p) {
    return uint32_t(p[0]) | uint32_t(p[1])<<8 | uint32_t(p[2])<<16 | uint32_t(p[3])<<24;
}
// These are UIBIZ enum values recovered from relocation-backed name records,
// NOT USB key codes, ux::KeyEvent values, or proof of physical C1/C2 mapping.
enum UiBiz : uint32_t { Up=2, Left=4, Down=6, Custom=14, Menu=19, Enter=20, Right=22, Custom2=223 };
enum class Edge { Press, Release, Repeat }; // normalized by a future verified input adapter
struct UiContext { bool still_live_view=false, modal=false, capturing=false, shutting_down=false; };
struct Choice { uint16_t look=0, lens=0; }; // index 0 means none/preserve; aperture stays unknown
struct MenuReply { bool consume=false, repaint=false, commit=false; Choice choice{}; uint64_t ticket=0; };
class PresetMenu {
    Choice saved_{}, draft_{};
    uint16_t looks_, lenses_, row_=0;
    uint32_t open_code_, owned_=0;
    uint64_t last_event_=0, ticket_=0, generation_=0;
    uint32_t last_code_=UINT32_MAX;
    Edge last_edge_=Edge::Press;
    bool valid_, visible_=false, pending_=false, last_consumed_=false;
public:
    PresetMenu(uint16_t looks, uint16_t lenses, uint32_t verified_open_code,
               Choice initial={},uint64_t generation=0)
      : saved_(initial), draft_(initial), looks_(looks), lenses_(lenses), open_code_(verified_open_code), generation_(generation),
        valid_(looks>0 && looks<=100 && lenses>0 && lenses<=100 &&
               initial.look<looks && initial.lens<lenses &&
               (verified_open_code==Custom || verified_open_code==Custom2)) {}
    bool visible() const { return visible_; }
    bool pending() const { return pending_; }
    uint16_t row() const { return row_; }
    Choice draft() const { return draft_; }
    Choice saved() const { return saved_; }
    uint64_t generation() const { return generation_; }
    // Also call on mode/focus/modal/shutdown notifications, without waiting for
    // another key. An already issued durable selection request still needs ack.
    bool update_context(UiContext c) noexcept {
        bool cancel=!c.still_live_view || c.modal || c.capturing || c.shutting_down;
        bool repaint=cancel && visible_;if (cancel) visible_=false;return repaint;
    }
    // UI-thread only. No disk I/O or process launch. AUTO is allowed by context.
    // Loss of context cancels drafts; shutter/unknown events always pass through.
    MenuReply key(uint32_t code, Edge edge, uint64_t serial, UiContext c) noexcept {
        MenuReply r{};
        r.repaint=update_context(c);
        uint32_t bit=code==Up ? 1 : code==Left ? 2 : code==Down ? 4 : code==Menu ? 8 :
                     code==Enter ? 16 : code==Right ? 32 : code==open_code_ ? 64 : 0;
        if (!serial || serial<=last_event_) {
            r.consume=serial && serial==last_event_ && code==last_code_ && edge==last_edge_ && last_consumed_;
            return r;
        }
        last_event_=serial;last_code_=code;last_edge_=edge;last_consumed_=false;
        if (bit && (owned_&bit)) {
            r.consume=last_consumed_=true;
            if (edge==Edge::Release) owned_&=~bit;
            return r;
        }
        if (!valid_ || !c.still_live_view || c.modal || c.capturing || c.shutting_down) {
            r.repaint=visible_; visible_=false; return r;
        }
        if (pending_) return r;
        if (!visible_) {
            if (code==open_code_ && edge==Edge::Press) {
                draft_=saved_; row_=0; visible_=true; r.consume=r.repaint=true;
                last_consumed_=true;
                owned_|=bit;
            }
            return r;
        }
        bool recognized=code==Up || code==Down || code==Left || code==Right || code==Enter || code==Menu || code==open_code_;
        if (!recognized || edge!=Edge::Press) return r;
        r.consume=last_consumed_=true;
        owned_|=bit;
        r.repaint=true;
        if (code==Menu || code==open_code_) { visible_=false; return r; }
        if (code==Up || code==Down) { row_^=1; return r; }
        if (code==Left || code==Right) {
            auto& n=row_ ? draft_.lens : draft_.look;
            uint16_t count=row_ ? lenses_ : looks_;
            n=uint16_t((n+count+(code==Right ? 1 : -1))%count); return r;
        }
        if (ticket_==UINT64_MAX || generation_==UINT64_MAX) { visible_=false; return r; }
        pending_=true; visible_=false; r.commit=true; r.choice=draft_; r.ticket=++ticket_; return r;
    }
    // A controller calls --select outside Sony's callback, then acknowledges its
    // actual durable success. Until then no new capture may use this draft.
    bool acknowledge(uint64_t ticket, bool success) noexcept {
        if (!pending_ || ticket!=ticket_) return false;
        pending_=false;
        if (success) { saved_=draft_; ++generation_; }
        return true;
    }
};

struct AppStillFinished201 {
    uint32_t media=0, opaque0c=0, count=0, raw_error=0;
    uint8_t flags[2]{};
};
// Exact source dispatch at appFw 0x3573c2c. This event has NO pathname/frame ID.
inline bool decode_app_still201(const void* data, size_t size, AppStillFinished201& out) noexcept {
    out={};
    if (!data || size!=0x58) return false;
    auto p=static_cast<const uint8_t*>(data);
    if (le32(p+4)!=1) return false; // other branch is a synchronous reply, not this callback
    out.media=le32(p+8); out.opaque0c=le32(p+12); out.count=le32(p+16); out.raw_error=le32(p+0x54);
    if (out.count>2) return false;
    if (out.count) out.flags[0]=p[0x30];
    if (out.count>1) out.flags[1]=p[0x50];
    return true;
}

enum class ObservationKind { Write, Close };
struct Observation {
    ObservationKind kind=ObservationKind::Write;
    uint64_t cookie=0;
    int32_t status=0;
    char path[SonyPathBytes]{};
};
// Bounded append-only MPSC mailbox. One lock-free atomic ticket; no application
// retry loop or software mutex. Atomic RMW latency is not a wait-free guarantee.
// Slots are never recycled within a session; no ABA or producer overwrite.
// Overflow poisons the session: no further candidate can be released.
// The consumer must stop processing and retain its jobs for review on fault.
class CompletionMailbox {
    static_assert(std::atomic<bool>::is_always_lock_free);
    static_assert(std::atomic<size_t>::is_always_lock_free);
    struct Slot { Observation event{}; std::atomic<bool> published{false}; };
    std::atomic<bool> fault_{false};
    Slot slots_[MailboxCapacity]{};
    size_t read_=0; // consumer only
    std::atomic<size_t> next_{0};
    bool push(const Observation& e) noexcept {
        if (fault_.load(std::memory_order_acquire)) return false;
        size_t ticket=next_.fetch_add(1,std::memory_order_relaxed);
        if (ticket>=MailboxCapacity) { fault_.store(true,std::memory_order_release); return false; }
        slots_[ticket].event=e;
        slots_[ticket].published.store(true,std::memory_order_release);
        return !faulted();
    }
public:
    bool faulted() const noexcept { return fault_.load(std::memory_order_acquire); }
    bool pop(Observation& e) noexcept {
        if (faulted() || read_>=MailboxCapacity || !slots_[read_].published.load(std::memory_order_acquire)) return false;
        e=slots_[read_++].event;return !faulted();
    }
    // Invoke before forwarding Sony's original callback, while result is alive.
    // The trampoline must establish readable result bounds; this span cannot
    // validate arbitrary pointers. cookie is copied, never dereferenced.
    bool observe_write201(uint64_t cookie, int32_t status, const void* result, size_t size) noexcept {
        Observation e{}; e.cookie=cookie; e.status=status;
        if (!cookie) return false;
        if (!status) {
            if (!result || size!=FsysResultBytes201) { fault_.store(true,std::memory_order_release); return false; }
            auto p=static_cast<const char*>(result);
            auto end=static_cast<const char*>(std::memchr(p,0,SonyPathBytes));
            if (!end || end==p) { fault_.store(true,std::memory_order_release); return false; }
            std::memcpy(e.path,p,size_t(end-p));
        }
        return push(e);
    }
    // Separate assertion from an independently matched, actual close observer.
    // A fsync/flush notification must never call this method.
    bool observe_close(uint64_t cookie, int32_t status) noexcept {
        Observation e{};e.kind=ObservationKind::Close;e.cookie=cookie;e.status=status;
        return cookie && push(e);
    }
};
using SonyFsysFunction = void(void*,int32_t,const void*);
inline void forward_fsys201(CompletionMailbox& box, void* cookie, int32_t status,
                            const void* result, size_t readable_size, SonyFsysFunction& original) noexcept {
    box.observe_write201(reinterpret_cast<uintptr_t>(cookie),status,result,readable_size);
    // Required non-null original is supplied by a future validated trampoline.
    // No global callback slot is replaced by this core.
    original(cookie,status,result);
}

enum class PhotoKind { JPEG, HIF };
enum class CaptureState { Unused, Armed, Bound, Candidate, Taken, Failed };
struct CaptureBinding {
    uint64_t token=0, cookie=0, selection_generation=0;
    PhotoKind kind=PhotoKind::JPEG;
    CaptureState state=CaptureState::Unused;
    bool wrote=false, closed=false;
    char event_id[65]{}, sony_path[SonyPathBytes]{}, relative_path[256]{};
};
class CaptureTracker {
    CaptureBinding entries_[BridgeCapacity]{};
    size_t used_=0;
    bool fault_=false;
    static bool copy(char* dest,size_t n,const char* s) noexcept {
        if (!s) return false;
        size_t len=strnlen(s,n);
        if (!len || len==n) return false;
        std::memcpy(dest,s,len+1); return true;
    }
    static bool event_id(const char* s) noexcept {
        if (!s) return false;
        size_t n=strnlen(s,65); if (!n || n>64) return false;
        for (size_t i=0;i<n;++i) if (!((s[i]>='a'&&s[i]<='z') || (s[i]>='A'&&s[i]<='Z') ||
            (s[i]>='0'&&s[i]<='9') || s[i]=='-' || s[i]=='_')) return false;
        return true;
    }
    static bool relative(const char* s,PhotoKind k) noexcept {
        if (!s) return false;
        size_t n=strnlen(s,256); if (n<5 || n==256 || s[0]=='/' || s[n-1]=='/') return false;
        size_t component=0;
        for (size_t i=0;i<=n;++i) {
            if (i==n || s[i]=='/') {
                if (!component || (component==1 && s[i-1]=='.') ||
                    (component==2 && s[i-1]=='.' && s[i-2]=='.')) return false;
                component=0;
            } else {
                unsigned char c=s[i]; if (c<32 || c>126 || c=='\\' || c==':') return false;
                ++component;
            }
        }
        const char* ext=s+n-4;
        return std::strcmp(ext,k==PhotoKind::JPEG ? ".JPG" : ".HIF")==0;
    }
public:
    bool faulted() const noexcept { return fault_; }
    // Controller-only methods: serialized off the Sony callback threads. Arm
    // must follow the launcher's durable --arm BEFORE the corresponding capture.
    bool arm(uint64_t token,const char* id,PhotoKind kind,uint64_t generation) noexcept {
        if (fault_ || !token || used_==BridgeCapacity || !event_id(id) ||
            (kind!=PhotoKind::JPEG && kind!=PhotoKind::HIF)) return false;
        for (size_t i=0;i<used_;++i) if (entries_[i].token==token || !std::strcmp(entries_[i].event_id,id)) return false;
        auto& e=entries_[used_++];e.token=token;e.selection_generation=generation;e.kind=kind;e.state=CaptureState::Armed;
        copy(e.event_id,sizeof(e.event_id),id); return true;
    }
    // Caller proves exact full-original request -> armed frame association;
    // filename/order/timing guesses are forbidden. No pointer reuse per session.
    bool bind(uint64_t token,uint64_t cookie,const char* sony,const char* relative_path,bool full_original) noexcept {
        if (fault_ || !cookie || !full_original) return false;
        for (size_t i=0;i<used_;++i) if (entries_[i].cookie==cookie) return false;
        for (size_t i=0;i<used_;++i) {
            auto& e=entries_[i]; if (e.token!=token || e.state!=CaptureState::Armed) continue;
            char path[SonyPathBytes]{};
            if (!relative(relative_path,e.kind) || !copy(path,sizeof(path),sony)) return false;
            // Both names must describe the same basename; drive/root mapping is
            // still an explicit responsibility of the trusted producer.
            const char* tail=path;
            for (const char* p=path;*p;++p) if (*p=='/' || *p=='\\') tail=p+1;
            const char* reltail=std::strrchr(relative_path,'/');reltail=reltail ? reltail+1 : relative_path;
            if (std::strcmp(tail,reltail)) return false;
            e.cookie=cookie;std::memcpy(e.sony_path,path,sizeof(path));copy(e.relative_path,sizeof(e.relative_path),relative_path);
            e.state=CaptureState::Bound; return true;
        }
        return false;
    }
    void drain(CompletionMailbox& box) noexcept {
        if (box.faulted()) fault_=true;
        if (fault_) return;
        Observation o{};
        while (box.pop(o)) {
            for (size_t i=0;i<used_;++i) {
                auto& e=entries_[i]; if (e.cookie!=o.cookie) continue;
                // Terminal entries never become runnable again on duplicates.
                if (e.state==CaptureState::Taken || e.state==CaptureState::Failed) break;
                bool duplicate=o.kind==ObservationKind::Write ? e.wrote : e.closed;
                if (duplicate || o.status || (o.kind==ObservationKind::Write && std::strcmp(e.sony_path,o.path))) {
                    e.state=CaptureState::Failed; break;
                }
                if (o.kind==ObservationKind::Write) e.wrote=true; else e.closed=true;
                if (e.wrote && e.closed) e.state=CaptureState::Candidate;
                break;
            }
        }
        if (box.faulted()) fault_=true;
    }
    bool cancel(uint64_t token) noexcept {
        for (size_t i=0;i<used_;++i) if (entries_[i].token==token && entries_[i].state!=CaptureState::Taken) {
            entries_[i].state=CaptureState::Failed; return true;
        }
        return false;
    }
    // This is NOT permission to enqueue a worker. The controller still needs a
    // same-file identity seal/digest from a trusted producer and scheduling.
    // A digest obtained from a possibly reused pathname after this event does
    // not establish that identity. No such producer is installed by this core.
    bool take_candidate(uint64_t token,CaptureBinding& out,const CompletionMailbox& box) noexcept {
        if (box.faulted()) fault_=true;
        if (fault_) return false;
        for (size_t i=0;i<used_;++i) if (entries_[i].token==token && entries_[i].state==CaptureState::Candidate) {
            out=entries_[i];entries_[i].state=CaptureState::Taken;return true;
        }
        return false;
    }
};
} // namespace a7

extern "C" const char* a7_sony_bridge_version() noexcept;
extern "C" bool a7_decode_app_still201(const void*,size_t,a7::AppStillFinished201*) noexcept;
extern "C" bool a7_observe_fsys201(a7::CompletionMailbox*,uint64_t,int32_t,const void*,size_t) noexcept;
