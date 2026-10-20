#include "../sony_native_probe.h"
#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>
struct ProbeFixture201 {
    IdentityFixture identity{};
    uint8_t drive[0x66a8]{},pf[64]{},frame[0x108]{},stream[0x78]{},data[8192]{},owned[8192]{};
    OwnedWriteImage201 image{owned,sizeof(owned),0x22220000,0x81};
    NativeProbe201 probe{};ProbeRegisters201 registers{};
    bool mutate_stream=false;unsigned header_reads=0;
    static thread_local ProbeFixture201* current;
    static bool read(uint64_t address,void* output,size_t n) noexcept {
        if(!current||address<reinterpret_cast<uintptr_t>(current)||n>sizeof(*current)||
           address-reinterpret_cast<uintptr_t>(current)>sizeof(*current)-n)return false;
        std::memcpy(output,reinterpret_cast<const void*>(uintptr_t(address)),n);errno=ERANGE;
        if(address==reinterpret_cast<uintptr_t>(current->stream)&&n==0x38&&current->mutate_stream&&++current->header_reads==2)
            static_cast<uint8_t*>(output)[0]^=1;
        return true;
    }
    ProbeFixture201() {
        current=this;
        probe.image=&image;probe.read=read;probe.cookie=0x22220000;probe.handle=0x81;
        probe.slot=reinterpret_cast<uintptr_t>(identity.file);probe.drive=reinterpret_cast<uintptr_t>(drive);probe.backing=reinterpret_cast<uintptr_t>(pf);
        put64(identity.file+0x18,probe.drive);put64(identity.file+0x40,probe.backing);
        put32(pf,1);put64(pf+0x18,0x12340000);put64(pf+0x38,0);
        uint64_t descriptor=reinterpret_cast<uintptr_t>(stream);
        put64(drive+0x66a0,descriptor);put32(stream,2);put32(stream+4,2);
        put64(stream+8,descriptor+0x38);put64(stream+0x10,descriptor+0x58);
        put64(stream+0x18,1);put64(stream+0x20,8193);
        put64(stream+0x38,0x30000000);put64(stream+0x40,4096);
        put64(stream+0x48,0x30001000);put64(stream+0x50,4096);
        probe.windows[0]={0x30000000,reinterpret_cast<uintptr_t>(data),sizeof(data)};
        registers.x[25]=probe.backing;registers.x[29]=reinterpret_cast<uintptr_t>(frame);registers.pc=0x7af08;
        put64(frame+0xe0,8192);put64(frame+0xe8,1);put64(frame+0xf0,1);
        for(size_t i=0;i<sizeof(data);++i)data[i]=uint8_t(i*7);
    }
    bool lease(){return probe.capture_stream(reinterpret_cast<uintptr_t>(stream));}
};
thread_local ProbeFixture201* ProbeFixture201::current=nullptr;
static void native_probe_tests() {
    { ProbeFixture201 f;CHECK(f.lease());IdentityLedger201 ledger;OwnedPhoto201 out{};
      CHECK(ledger.arm(73,7,9));CHECK(f.identity.select(ledger));CHECK(f.identity.submit(ledger));
      CHECK(ledger.opened(f.probe.cookie,f.probe.slot,f.identity.file,sizeof(f.identity.file),0));
      uint64_t scope=a7_probe_enter201(&f.probe);CHECK(scope);errno=EDOM;
      a7_probe_locked_write201(&f.registers);CHECK(errno==EDOM&&!f.probe.failed&&f.probe.pending);
      CHECK(!f.probe.take(ledger,out));CHECK(f.probe.write_return(8192));uint8_t expected[8192];std::memcpy(expected,f.data,sizeof(expected));std::memset(f.data,0xee,sizeof(f.data));
      CHECK(f.image.truncate_return(f.probe.cookie,f.probe.handle,7001,0));
      CHECK(ledger.prfile_close_enter(f.probe.slot,f.identity.file,sizeof(f.identity.file)));CHECK(ledger.close_return(f.probe.slot,0));
      CHECK(ledger.completed(f.probe.cookie,0,f.identity.response,sizeof(f.identity.response)));
      CHECK(!f.probe.take(ledger,out));CHECK(a7_probe_leave201(scope));CHECK(f.probe.take(ledger,out)&&out.size==7001);
      CHECK(!std::memcmp(out.bytes,expected,out.size)); }
    { ProbeFixture201 f;CHECK(f.lease());put64(f.frame+0xe0,4096);uint64_t scope=a7_probe_enter201(&f.probe);
      a7_probe_locked_write201(&f.registers);CHECK(f.probe.pending);CHECK(f.probe.write_return(4096));
      put64(f.frame+0xf0,4097);put64(f.pf+0x38,4096);a7_probe_locked_write201(&f.registers);
      CHECK(!f.probe.failed&&f.probe.write_return(4096));CHECK(!std::memcmp(f.data,f.owned,sizeof(f.data)));CHECK(a7_probe_leave201(scope)); }
    for(int mode=0;mode<22;++mode) {
        ProbeFixture201 f;
        if(mode==0)put32(f.stream,1); // unproved mode, never raw-cast cursor
        if(mode==1)put32(f.stream+4,521);
        if(mode==2)put64(f.stream+8,1);
        if(mode==3)put64(f.stream+0x18,0x1000);
        if(mode==4)put64(f.stream+0x40,0);
        if(mode==5)put64(f.stream+0x20,9000);
        if(mode==6)f.probe.windows[1]=f.probe.windows[0];
        bool leased=f.lease();CHECK(leased==(mode>6));
        if(leased) {
            if(mode==7)put64(f.pf+0x38,1);
            if(mode==8)f.registers.x[25]+=8;
            if(mode==9)f.registers.pc+=4;
            if(mode==10)put64(f.identity.file+0x20,0xa1);
            if(mode==11)put64(f.drive+0x66a0,0);
            if(mode==12)put64(f.frame+0xe8,2);
            if(mode==13)put64(f.frame+0xf0,4097);
            if(mode==14)put64(f.frame+0xe0,8193);
            if(mode==15)put64(f.pf+0x18,0);
            if(mode==16)put32(f.pf,0);
            if(mode==17)f.probe.windows[0].length=4096; // second physical extent outside mapping
            if(mode==18)f.probe.windows[0].cpu=1;
            if(mode==19)f.probe.windows[0].cpu=reinterpret_cast<uintptr_t>(f.owned);
            if(mode==20)put64(f.stream+0x38,0x30000001);
            if(mode==21)put64(f.stream+0x20,8194);
            auto scope=a7_probe_enter201(&f.probe);CHECK(scope);
            CHECK(!f.probe.locked_write(f.registers)&&f.probe.failed);CHECK(a7_probe_leave201(scope));
        }
    }
    { ProbeFixture201 f;CHECK(f.lease());NativeScopes201 stack;auto outer=stack.enter(&f.probe);auto unknown=stack.enter(nullptr);
      CHECK(!stack.current());CHECK(stack.leave(unknown)&&stack.current()==&f.probe);CHECK(stack.leave(outer)&&!stack.current()); }
    { ProbeFixture201 f;NativeScopes201 stack;auto outer=stack.enter(&f.probe);CHECK(stack.enter(nullptr));
      CHECK(!stack.leave(outer)&&stack.failed()&&f.probe.failed&&!stack.current()); }
    { ProbeFixture201 f;NativeScopes201 stack;for(int i=0;i<8;++i)CHECK(stack.enter(&f.probe));
      CHECK(!stack.enter(&f.probe)&&stack.failed()&&f.probe.failed&&!stack.current()); }
    { ProbeFixture201 f;f.mutate_stream=true;CHECK(!f.lease()&&f.probe.failed); }
    for(int mode=0;mode<3;++mode) {
        ProbeFixture201 f;
        if(mode==1)put64(f.identity.request+0x500,99);
        if(mode==2)put32(f.identity.request+0x410,0);
        errno=EDOM;auto scope=a7_probe_enter_request201(&f.probe,reinterpret_cast<uintptr_t>(f.identity.request));
        CHECK(scope&&errno==EDOM);CHECK(f.probe.failed==(mode!=0));CHECK(a7_probe_leave201(scope));
    }
    for(int32_t status:{0,-1,8192}) {
        ProbeFixture201 f;auto scope=a7_probe_enter201(&f.probe);errno=EDOM;
        CHECK(a7_probe_write_return201(status)==status&&errno==EDOM&&f.probe.failed);CHECK(a7_probe_leave201(scope));
    }
    { uint8_t owned[4]{};OwnedWriteImage201 image(owned,4,1,2);CHECK(image.start_at(1,2,0,0));
      CHECK(!image.chunk_enter(1,2,0,reinterpret_cast<void*>(uintptr_t(1)),1)&&image.failed()); }
    { uint8_t output[4]{};CHECK(!a7_read_self201(1,output,sizeof(output)));CHECK(!a7_read_self201(UINT64_MAX-1,output,4)); }
}
static void native_self_read_tests() {
    long page=sysconf(_SC_PAGESIZE);CHECK(page>0);
    void* memory=mmap(nullptr,size_t(page)*2,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);CHECK(memory!=MAP_FAILED);
    auto data=static_cast<uint8_t*>(memory);std::memset(data,0x37,size_t(page)*2);
    uint8_t output[32]{};CHECK(a7_read_self201(reinterpret_cast<uintptr_t>(data),output,32));CHECK(output[0]==0x37);
    CHECK(!mprotect(data+page,size_t(page),PROT_NONE));
    CHECK(!a7_read_self201(reinterpret_cast<uintptr_t>(data+page),output,1));
    CHECK(!a7_read_self201(reinterpret_cast<uintptr_t>(data+page-16),output,32));
    CHECK(!munmap(memory,size_t(page)*2));CHECK(!a7_read_self201(reinterpret_cast<uintptr_t>(data),output,1));
    std::puts("{\"self_read_guard_cases\":4,\"camera_accessed\":false,\"ghub_accessed\":false}");
}
