#include "../sony_bridge.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

static std::atomic<unsigned> forwarded{0};
static void original(void*,int32_t,const void*) { forwarded.fetch_add(1,std::memory_order_relaxed); }
int main() {
    unsigned poisoned=0;
    for (unsigned round=0;round<100;++round) {
        a7::CompletionMailbox box;
        uint8_t raw[a7::FsysResultBytes201]{};std::strcpy(reinterpret_cast<char*>(raw),"A:/synthetic.JPG");
        std::atomic<bool> start{false};std::vector<std::thread> threads;
        std::atomic<unsigned> done{0};
        for (uintptr_t i=1;i<=4;++i) threads.emplace_back([&,i] {
            while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
            for (unsigned n=0;n<8;++n) {
                a7::forward_fsys201(box,reinterpret_cast<void*>(i),0,raw,sizeof(raw),original);
                box.observe_close(i,0);
            }
            done.fetch_add(1,std::memory_order_release);
        });
        start.store(true,std::memory_order_release);
        a7::Observation o{};unsigned n=0;bool bad=false;
        auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);
        while (done.load(std::memory_order_acquire)!=4 || n<64) {
            if (box.pop(o)) {
                if (o.cookie<1 || o.cookie>4 || (o.kind==a7::ObservationKind::Write && std::strcmp(o.path,"A:/synthetic.JPG"))) bad=true;
                ++n;
            } else std::this_thread::yield();
            if (box.faulted() || std::chrono::steady_clock::now()>deadline) { bad=true;break; }
        }
        for (auto& t:threads) t.join();
        if (bad) return 1;
        if (box.faulted()) { ++poisoned; return 4; }
        else if (n!=64) return 2;
    }
    if (forwarded.load()!=3200) return 3;
    std::printf("{\"rounds\":100,\"forwarded\":3200,\"observations_consumed\":6400,\"contention_fault_rounds\":%u,\"status\":\"passed\"}\n",poisoned);
}
