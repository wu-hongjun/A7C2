#include "../sony_identity.h"
static void put64(uint8_t* p,uint64_t x) { put32(p,uint32_t(x));put32(p+4,uint32_t(x>>32)); }
struct IdentityFixture {
    uint8_t record[0xd08]{},request[0x508]{},cookie[0x1680]{},payload[0x68c]{},file[FileInfoPrefixBytes201]{},response[FsysResultBytes201]{};
    IdentityFixture(uint32_t frame=73,uint32_t group=0,uint32_t media=0,uint32_t type=1) {
        const char* path=type==4 ? "A:/DCIM/100MSDCF/DSC00001.HIF" : "A:/DCIM/100MSDCF/DSC00001.JPG";
        // Prove that the optional DB prefix is ignored, not misread as SIDB ID.
        std::memset(cookie,0xa5,0x15e0);
        put32(payload+0x688,frame);put32(record+0x1f4+group*0x588,type);
        put32(record+0x20c+group*0x588,0);put32(record+0x130+media*4,909);
        std::strcpy(reinterpret_cast<char*>(record+0x23c+group*0x588+media*0x106),path);
        std::strcpy(reinterpret_cast<char*>(cookie+0x15f0),path);
        std::strcpy(reinterpret_cast<char*>(request),path);
        put32(request+0x410,0x206);put64(request+0x500,0x22220000);
        put32(cookie+0x15e0,media);put64(cookie+0x1670,0x11110000);put32(cookie+0x1678,group);
        put32(file+0x10,1);put64(file+0x18,0x33330000);put64(file+0x20,0x81);put64(file+0x40,0x44440000);
        std::strcpy(reinterpret_cast<char*>(file+0x60),path);std::strcpy(reinterpret_cast<char*>(response),path);
    }
    bool submit(IdentityLedger201& l) { return l.submit(0x11110000,record,sizeof(record),request,sizeof(request),0x22220000,cookie,sizeof(cookie)) && l.submission_return(0x22220000,0); }
    bool select(IdentityLedger201& l) { return l.select_record(payload,sizeof(payload),0x11110000) && l.record_return(le32(payload+0x688),0x11110000,0); }
    bool open(IdentityLedger201& l) { return l.opened(0x22220000,0x55550000,file,sizeof(file),0); }
};
static void identity_tests() {
    StillWrite201 d{};
    for (uint32_t type:{1u,4u}) for (uint32_t g:{0u,1u}) for (uint32_t m:{0u,1u,2u,3u}) {
        IdentityFixture f(73,g,m,type);IdentityLedger201 l;IdentityCandidate201 out{};
        CHECK(l.arm(73,700,9));CHECK(f.select(l));CHECK(f.submit(l));CHECK(f.open(l));
        CHECK(a7_decode_still_write201(0x11110000,f.record,sizeof(f.record),f.request,sizeof(f.request),0x22220000,f.cookie,sizeof(f.cookie),&d));
        CHECK(d.media==m && d.format_group==g && d.sidb_id==909 && d.cookie==0x22220000 && d.record==0x11110000);
        CHECK(d.kind==(type==1 ? PhotoKind::JPEG : PhotoKind::HIF));
        CHECK(!l.take(0x22220000,out));
        // Nonzero lock counter means common helper may fsync: no close emitted.
        put32(f.file+0x470,1);
        CHECK(l.completed(0x22220000,0,f.response,sizeof(f.response)));CHECK(!l.take(0x22220000,out));
        put32(f.file+0x470,0);CHECK(l.prfile_close_enter(0x55550000,f.file,sizeof(f.file)));
        CHECK(!l.take(0x22220000,out));CHECK(l.close_return(0x55550000,0));
        CHECK(l.take(0x22220000,out));CHECK(out.token==700 && out.frame==73 && out.generation==9 && out.open_handle==0x81);
        CHECK(!l.take(0x22220000,out));
    }
    { // Two independently armed frames complete in reverse order.
        IdentityFixture a(73),b(74,1,3,4);IdentityLedger201 l;IdentityCandidate201 out{};
        CHECK(l.arm(73,700,9));CHECK(l.arm(74,701,10));CHECK(a.select(l));
        CHECK(l.select_record(b.payload,sizeof(b.payload),0x11120000));CHECK(l.record_return(74,0x11120000,0));
        put64(b.cookie+0x1670,0x11120000);put64(b.request+0x500,0x22220001);
        put64(b.file+0x20,0xa1);CHECK(a.submit(l));
        CHECK(l.submit(0x11120000,b.record,sizeof(b.record),b.request,sizeof(b.request),0x22220001,b.cookie,sizeof(b.cookie)));CHECK(l.submission_return(0x22220001,0));
        CHECK(a.open(l));CHECK(l.opened(0x22220001,0x55550001,b.file,sizeof(b.file),0));
        CHECK(l.completed(0x22220000,0,a.response,sizeof(a.response)));
        CHECK(l.prfile_close_enter(0x55550001,b.file,sizeof(b.file)));CHECK(l.close_return(0x55550001,0));
        CHECK(l.completed(0x22220001,0,b.response,sizeof(b.response)));CHECK(l.take(0x22220001,out));
        CHECK(out.frame==74 && out.generation==10 && out.write.kind==PhotoKind::HIF && out.write.media==3);
        CHECK(!l.take(0x22220000,out));CHECK(l.prfile_close_enter(0x55550000,a.file,sizeof(a.file)));
        CHECK(l.close_return(0x55550000,0));CHECK(l.take(0x22220000,out));CHECK(out.frame==73 && out.generation==9);
    }
    { uint8_t command[16]{5,0,0x38,0x40};put32(command+4,73);uint32_t frame=0;
      IdentityLedger201 l;IdentityFixture f;CHECK(decode_capture_start201(command,16,frame) && frame==73);
      CHECK(l.arm_start(command,16,1,9));CHECK(f.select(l));
      CHECK(!decode_capture_start201(command,15,frame));command[2]=0x39;
      CHECK(!decode_capture_start201(command,16,frame));command[2]=0x38;put32(command+4,UINT32_MAX);
      CHECK(!decode_capture_start201(command,16,frame)); }
    for (int fail=0;fail<3;++fail) { // callbacks racing native function returns
        IdentityFixture f;IdentityLedger201 l;IdentityCandidate201 out{};
        CHECK(l.arm(73,7,9));CHECK(l.select_record(f.payload,sizeof(f.payload),0x11110000));
        CHECK(l.submit(0x11110000,f.record,sizeof(f.record),f.request,sizeof(f.request),0x22220000,f.cookie,sizeof(f.cookie)));
        CHECK(f.open(l));CHECK(l.prfile_close_enter(0x55550000,f.file,sizeof(f.file)));CHECK(l.close_return(0x55550000,0));
        CHECK(l.completed(0x22220000,0,f.response,sizeof(f.response)));CHECK(!l.take(0x22220000,out));
        CHECK(l.submission_return(0x22220000,fail==1 ? -1 : 0));CHECK(!l.take(0x22220000,out));
        CHECK(l.record_return(73,0x11110000,fail==2 ? 3 : 0));CHECK(l.take(0x22220000,out)==(fail==0));
    }
    // Each mutation must be rejected rather than allowing weak basename joins.
    for (int mutation=0;mutation<15;++mutation) {
        IdentityFixture f;IdentityLedger201 l;CHECK(l.arm(73,1,1));CHECK(f.select(l));
        switch (mutation) {
        case 0:put32(f.cookie+0x1678,2);break;
        case 1:put32(f.cookie+0x15e0,4);break;
        case 2:put64(f.cookie+0x1670,0x1234);break;
        case 3:put64(f.request+0x500,0x1234);break;
        case 4:put32(f.request+0x410,0);break;
        case 5:put32(f.record+0x1f4,2);break; // RAW cannot be promoted
        case 6:put32(f.record+0x20c,UINT32_MAX);break;
        case 7:f.cookie[0x15f0]='B';break;
        case 8:f.record[0x23c]='B';break;
        case 9:f.request[0]='B';break;
        case 10:std::memset(f.cookie+0x15f0,'x',128);break;
        case 11:std::memset(f.record+0x23c,'x',0x106);break;
        case 12:std::memset(f.request,'x',SonyPathBytes);break;
        case 13:put32(f.record+0x1f4,4);break; // HIF type with JPEG suffix
        case 14:f.cookie[0x15f0]=0;break;
        }
        CHECK(!f.submit(l) && l.faulted());
    }
    for (int short_span=0;short_span<3;++short_span) {
        IdentityFixture f;
        CHECK(!decode_still_write201(0x11110000,f.record,sizeof(f.record)-(short_span==0),f.request,sizeof(f.request)-(short_span==1),0x22220000,f.cookie,sizeof(f.cookie)-(short_span==2),d));
    }
    // Reverse ordering, close failure, async failure, duplicate, inactive slot,
    // backing/handle/owner reuse, callback path mismatch, and cancellation.
    for (int mode=0;mode<12;++mode) {
        IdentityFixture f;IdentityLedger201 l;IdentityCandidate201 out{};
        CHECK(l.arm(73,4,6));CHECK(f.select(l));CHECK(f.submit(l));CHECK(f.open(l));
        if (mode==1) put32(f.file+0x10,0);
        if (mode==2) put64(f.file+0x20,0xa1);
        if (mode==3) put64(f.file+0x40,0x7777);
        if (mode==4) put64(f.file+0x18,0x7777);
        if (mode==11) put32(f.file+0x470,1);
        if ((mode>=1 && mode<=4) || mode==11) CHECK(!l.prfile_close_enter(0x55550000,f.file,sizeof(f.file)) && l.faulted());
        else {
            CHECK(l.prfile_close_enter(0x55550000,f.file,sizeof(f.file)));
            if (mode==5) CHECK(!l.prfile_close_enter(0x55550000,f.file,sizeof(f.file)));
            else CHECK(l.close_return(0x55550000,mode==6 ? -1 : 0));
            if (mode==7) CHECK(l.completed(0x22220000,-1,nullptr,0));
            else if (mode!=5 && mode!=6) {
                if (mode==8) f.response[0]='B';
                bool ok=l.completed(0x22220000,0,f.response,sizeof(f.response));CHECK(ok==(mode!=8));
                if (mode==9) CHECK(!l.completed(0x22220000,0,f.response,sizeof(f.response)));
                if (mode==10) CHECK(l.cancel(73));
            }
        }
        CHECK(l.take(0x22220000,out)==(mode==0));
    }
    { IdentityFixture f;IdentityLedger201 l;CHECK(l.arm(73,1,0));CHECK(!l.arm(73,2,0));
      CHECK(!l.arm(74,1,0));CHECK(!l.arm(UINT32_MAX,3,0));CHECK(!f.submit(l));
      CHECK(!l.select_record(f.payload,sizeof(f.payload)-1,0x11110000));
      CHECK(!l.select_record(f.payload,sizeof(f.payload),0));CHECK(f.select(l));CHECK(!f.select(l) && l.faulted()); }
    { IdentityFixture f;IdentityLedger201 l;CHECK(l.arm(73,1,0));CHECK(f.select(l));CHECK(f.submit(l));
      f.file[0x60]='B';CHECK(f.open(l)); // translated backend path is not the external request path
    }
    { IdentityFixture f;IdentityLedger201 l;CHECK(l.arm(73,1,0));CHECK(f.select(l));CHECK(f.submit(l));
      CHECK(!f.submit(l) && l.faulted()); }
    { IdentityFixture f;IdentityLedger201 l;IdentityCandidate201 out{};
      CHECK(l.arm(73,1,0));CHECK(f.select(l));CHECK(f.submit(l));
      CHECK(!l.opened(0x22220000,0x55550000,f.file,sizeof(f.file),0x10));
      CHECK(!l.take(0x22220000,out)); }
    { IdentityLedger201 l;for (size_t i=0;i<BridgeCapacity;++i) CHECK(l.arm(uint32_t(i),i+1,0));CHECK(!l.arm(100,100,0)); }
}
