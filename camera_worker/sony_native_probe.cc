#include "sony_native_probe.h"
#include <cerrno>
#ifdef __linux__
#include <sys/uio.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_vm.h>
#endif
// Read ONLY this process. Kernel validation contains unmapped/PROT_NONE reads;
// no external PID, /dev/mem, USB, camera path, G HUB or signal handler is used.
extern "C" bool a7_read_self201(uint64_t address,void* output,size_t n) noexcept {
    if(!output||!n||n>a7::EncodedPhotoLimit201||!a7::NativeProbe201::range(address,n))return false;
#ifdef __linux__
    iovec local{output,n},remote{reinterpret_cast<void*>(uintptr_t(address)),n};
    ssize_t got;do {got=process_vm_readv(getpid(),&local,1,&remote,1,0);}while(got<0&&errno==EINTR);
    return got>=0&&size_t(got)==n;
#elif defined(__APPLE__)
    mach_vm_size_t got=0;
    return mach_vm_read_overwrite(mach_task_self(),address,n,reinterpret_cast<mach_vm_address_t>(output),&got)==KERN_SUCCESS&&got==n;
#else
    return false;
#endif
}
static constinit thread_local a7::NativeScopes201 scopes;
extern "C" uint64_t a7_probe_enter201(a7::NativeProbe201* p) noexcept { return scopes.enter(p); }
// V2 worker 0x2f7e4 receives the original request in x2. Read its actual cookie,
// not translator core completion fields or a global last-request variable.
extern "C" uint64_t a7_probe_enter_request201(a7::NativeProbe201* p,uint64_t request) noexcept {
    int saved=errno;
    auto ticket=scopes.enter(p);
    if(ticket&&p&&scopes.current()==p) {
        uint8_t cookie[8]{},flags[4]{};
        if(!p->read||!a7::NativeProbe201::range(request,0x508)||
           !p->read(request+0x500,cookie,8)||a7::le64(cookie)!=p->cookie||
           !p->read(request+0x410,flags,4)||a7::le32(flags)!=0x206) {
            p->fail();
        }
    }
    errno=saved;return ticket;
}
extern "C" bool a7_probe_leave201(uint64_t ticket) noexcept { return scopes.leave(ticket); }
extern "C" void a7_probe_locked_write201(const a7::ProbeRegisters201* frame) noexcept {
    int saved=errno;
    if(auto p=scopes.current();p&&frame)p->locked_write(*frame);
    errno=saved;
}
extern "C" int32_t a7_probe_write_return201(int32_t status) noexcept {
    int saved=errno;if(auto p=scopes.current())p->write_return(status);errno=saved;return status;
}
