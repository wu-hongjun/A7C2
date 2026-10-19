#include "sony_bridge.h"

// Linkable integration entry points, not a Sony loadable module or trampoline.
// Their C++ ABI is our own. Do not cast these to a Sony callback/delegate type.
extern "C" const char* a7_sony_bridge_version() noexcept { return "a7-sony-observer-201-v2-offline"; }
extern "C" bool a7_decode_app_still201(const void* p,size_t n,a7::AppStillFinished201* out) noexcept {
    return out && a7::decode_app_still201(p,n,*out);
}
extern "C" bool a7_observe_fsys201(a7::CompletionMailbox* box,uint64_t cookie,int32_t status,
                                  const void* result,size_t readable_size) noexcept {
    return box && box->observe_write201(cookie,status,result,readable_size);
}

#include "sony_identity.h"
extern "C" bool a7_decode_still_write201(uint64_t record,const void* r,size_t rn,const void* q,size_t qn,
                                        uint64_t cookie,const void* c,size_t cn,a7::StillWrite201* out) noexcept {
    return out && a7::decode_still_write201(record,r,rn,q,qn,cookie,c,cn,*out);
}
