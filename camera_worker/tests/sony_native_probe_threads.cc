#include "../sony_native_probe.h"
#include <atomic>
#include <thread>
#include <cassert>
#include <cstdio>
int main() {
    std::atomic<unsigned> ready{0};
    auto test=[&] {
        a7::NativeProbe201 outer{},inner{};a7::ProbeRegisters201 frame{};
        auto first=a7_probe_enter201(&outer);assert(first);
        auto unknown=a7_probe_enter201(nullptr);assert(unknown);
        a7_probe_locked_write201(&frame);assert(!outer.failed);assert(a7_probe_leave201(unknown));
        auto nested=a7_probe_enter201(&inner);assert(nested);a7_probe_locked_write201(&frame);
        assert(inner.failed&&!outer.failed);assert(a7_probe_leave201(nested));
        ready.fetch_add(1);
        while(ready.load()!=2)std::this_thread::yield();
        a7_probe_locked_write201(&frame);assert(outer.failed);assert(a7_probe_leave201(first));
    };
    std::thread a(test),b(test);a.join();b.join();
    a7::NativeProbe201 shared{};std::atomic<bool> owner_ready{false},other_done{false};
    std::thread owner([&]{
        auto scope=a7_probe_enter201(&shared);assert(scope);shared.written=1234;owner_ready.store(true);
        while(!other_done.load())std::this_thread::yield();
        assert(a7_probe_leave201(scope));
    });
    std::thread duplicate([&]{
        while(!owner_ready.load())std::this_thread::yield();
        auto scope=a7_probe_enter201(&shared);assert(scope);a7::ProbeRegisters201 frame{};
        a7_probe_locked_write201(&frame);assert(a7_probe_leave201(scope));other_done.store(true);
    });
    owner.join();duplicate.join();
    assert(shared.scope_fault.load()&&shared.phase.load()==2&&shared.written==1234&&!shared.failed);
    a7::IdentityLedger201 ledger;a7::OwnedPhoto201 photo;assert(!shared.take(ledger,photo));
    std::puts("{\"threads\":2,\"TLS_nested_and_unknown_scopes\":\"passed\",\"duplicate_producer_and_quiescence\":\"passed\"}");
}
