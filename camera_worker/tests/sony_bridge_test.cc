#include "../sony_bridge.h"
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

using namespace a7;
static int checks=0;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr,"check %d at %d: %s\n",checks,__LINE__,#x); std::exit(1); } } while(0)
static void put32(uint8_t* p,uint32_t x) { for (int i=0;i<4;++i) p[i]=uint8_t(x>>(8*i)); }
static UiContext live{true,false,false,false};
static uint64_t serial=0;
static MenuReply tap(PresetMenu& menu,uint32_t code,UiContext c=live) {
    auto reply=menu.key(code,Edge::Press,++serial,c);
    auto release=menu.key(code,Edge::Release,++serial,c);
    if (reply.consume) CHECK(release.consume);
    return reply;
}
static int forwarded=0;
static const void* expected_result=nullptr;
static void original(void* c,int32_t status,const void* p) {
    CHECK(c==reinterpret_cast<void*>(77)); CHECK(status==0); CHECK(p==expected_result); ++forwarded;
}
static void result(uint8_t* p,const char* path) { std::memset(p,0,FsysResultBytes201); std::strcpy(reinterpret_cast<char*>(p),path); }

static void tests() {
    uint8_t raw[0x58]{};AppStillFinished201 app{};
    CHECK(!std::strcmp(a7_sony_bridge_version(),"a7-sony-observer-201-v2-offline"));
    CHECK(!a7_decode_app_still201(nullptr,0,&app));CHECK(!a7_decode_app_still201(raw,sizeof(raw),nullptr));
    CHECK(!a7_observe_fsys201(nullptr,0,0,nullptr,0));
    CHECK(!decode_app_still201(nullptr,sizeof(raw),app));CHECK(!decode_app_still201(raw,sizeof(raw)-1,app));
    CHECK(!decode_app_still201(raw,sizeof(raw),app));put32(raw+4,1);put32(raw+8,3);put32(raw+12,71);
    for (uint32_t count=0;count<=3;++count) {
        put32(raw+16,count);raw[0x30]=7;raw[0x50]=9;put32(raw+0x54,0x80000010);
        CHECK(decode_app_still201(raw,sizeof(raw),app)==(count<=2));
        if (count<=2) { CHECK(app.media==3 && app.opaque0c==71 && app.raw_error==0x80000010);
            CHECK(app.flags[0]==(count ? 7 : 0));CHECK(app.flags[1]==(count>1 ? 9 : 0)); }
    }
    PresetMenu menu(12,3,Custom);CHECK(!tap(menu,Right).consume);CHECK(!tap(menu,Custom,{false,false,false,false}).consume);
    CHECK(tap(menu,Custom).repaint && menu.visible());
    auto repeated=menu.key(Custom,Edge::Repeat,++serial,live);CHECK(!repeated.consume && menu.visible());
    CHECK(!menu.key(Right,Edge::Release,++serial,live).consume); // never swallow a Sony-owned release
    CHECK(!menu.key(Right,Edge::Press,serial,live).consume); // duplicate serial
    tap(menu,Left);CHECK(menu.draft().look==11);tap(menu,Right);CHECK(menu.draft().look==0);
    tap(menu,Right);tap(menu,Down);tap(menu,Right);CHECK(menu.draft().look==1 && menu.draft().lens==1);
    CHECK(!tap(menu,272).consume); // shutter remains Sony's responsibility, never arms a frame
    auto commit=tap(menu,Enter);CHECK(commit.commit && menu.pending() && !menu.visible());CHECK(menu.saved().look==0);
    CHECK(!menu.acknowledge(commit.ticket+1,true));CHECK(!tap(menu,Custom).consume);
    CHECK(menu.acknowledge(commit.ticket,true));CHECK(menu.saved().look==1 && menu.saved().lens==1 && menu.generation()==1);
    CHECK(!menu.acknowledge(commit.ticket,true));
    tap(menu,Custom);tap(menu,Right);tap(menu,Menu);CHECK(menu.saved().look==1 && !menu.visible());
    tap(menu,Custom);tap(menu,Right);commit=tap(menu,Enter);CHECK(menu.acknowledge(commit.ticket,false));CHECK(menu.saved().look==1);
    tap(menu,Custom);CHECK(!tap(menu,Right,{true,true,false,false}).consume && !menu.visible());
    tap(menu,Custom);CHECK(!tap(menu,Right,{true,false,true,false}).consume && !menu.visible());
    tap(menu,Custom);CHECK(!tap(menu,Right,{true,false,false,true}).consume && !menu.visible());
    tap(menu,Custom);CHECK(menu.update_context({false,false,false,false}) && !menu.visible());
    auto opened=menu.key(Custom,Edge::Press,++serial,live);CHECK(opened.consume);
    CHECK(menu.key(Custom,Edge::Press,serial,live).consume); // duplicate disposition, no second action
    CHECK(menu.key(Custom,Edge::Repeat,++serial,live).consume && menu.visible());
    CHECK(menu.key(Custom,Edge::Release,++serial,{true,true,false,false}).consume && !menu.visible());
    PresetMenu invalid(0,3,Custom);CHECK(!tap(invalid,Custom).consume);
    PresetMenu wrongkey(12,3,272);CHECK(!tap(wrongkey,272).consume);
    PresetMenu loaded(12,3,Custom2,{5,2},9);CHECK(loaded.saved().look==5 && loaded.generation()==9);tap(loaded,Custom2);tap(loaded,Down);tap(loaded,Right);CHECK(loaded.draft().lens==0);

    uint8_t r[FsysResultBytes201];result(r,"A:/DCIM/100MSDCF/DSC00001.JPG");
    CompletionMailbox box;CaptureTracker tracker;CaptureBinding out{};
    CHECK(!tracker.arm(0,"a",PhotoKind::JPEG,1));CHECK(!tracker.arm(1,"../a",PhotoKind::JPEG,1));
    CHECK(!tracker.arm(1,"raw",static_cast<PhotoKind>(2),1));
    CHECK(tracker.arm(1,"frame-a",PhotoKind::JPEG,1));CHECK(!tracker.arm(1,"again",PhotoKind::JPEG,1));
    CHECK(!tracker.arm(2,"frame-a",PhotoKind::JPEG,1));
    CHECK(!tracker.bind(1,77,"A:/DCIM/100MSDCF/DSC00001.JPG","DCIM/100MSDCF/DSC00001.JPG",false));
    for (const char* bad : {"/DCIM/DSC00001.JPG","../DSC00001.JPG","DCIM//DSC00001.JPG","DCIM/./DSC00001.JPG","DCIM/DSC00001.HIF","DCIM/OTHER.JPG"})
        CHECK(!tracker.bind(1,77,"A:/DCIM/100MSDCF/DSC00001.JPG",bad,true));
    CHECK(tracker.bind(1,77,"A:/DCIM/100MSDCF/DSC00001.JPG","DCIM/100MSDCF/DSC00001.JPG",true));
    expected_result=r;forward_fsys201(box,reinterpret_cast<void*>(77),0,r,sizeof(r),original);CHECK(forwarded==1);
    std::memset(r,0xa5,sizeof(r)); // result lifetime ends at callback return
    tracker.drain(box);CHECK(!tracker.take_candidate(1,out,box)); // write callback alone never releases
    CHECK(box.observe_close(77,0));tracker.drain(box);CHECK(tracker.take_candidate(1,out,box));
    CHECK(out.selection_generation==1 && !std::strcmp(out.relative_path,"DCIM/100MSDCF/DSC00001.JPG"));CHECK(!tracker.take_candidate(1,out,box));
    CHECK(tracker.arm(2,"frame-b",PhotoKind::HIF,2));CHECK(!tracker.bind(2,77,"A:/b.HIF","b.HIF",true)); // no cookie reuse
    CHECK(tracker.bind(2,78,"A:/b.HIF","b.HIF",true));result(r,"A:/b.HIF");
    CHECK(box.observe_close(78,0));tracker.drain(box);CHECK(!tracker.take_candidate(2,out,box));
    CHECK(box.observe_write201(78,0,r,sizeof(r)));tracker.drain(box);CHECK(tracker.take_candidate(2,out,box) && out.kind==PhotoKind::HIF);
    for (int mode=0;mode<5;++mode) {
        CompletionMailbox b;CaptureTracker t;
        CHECK(t.arm(10,"failed",PhotoKind::JPEG,1));CHECK(t.bind(10,100,"A:/f.JPG","f.JPG",true));result(r,mode==2 ? "A:/wrong.JPG" : "A:/f.JPG");
        CHECK(b.observe_write201(100,mode==0 ? -1 : 0,mode==0 ? nullptr : r,mode==0 ? 0 : sizeof(r)));
        if (mode==3) CHECK(b.observe_write201(100,0,r,sizeof(r)));
        if (mode==4) CHECK(t.cancel(10));
        CHECK(b.observe_close(100,mode==1 ? -2 : 0));t.drain(b);CHECK(!t.take_candidate(10,out,b));
    }
    { CompletionMailbox b;CaptureTracker t;result(r,"A:/f.JPG");CHECK(t.arm(11,"poison",PhotoKind::JPEG,1));CHECK(t.bind(11,101,"A:/f.JPG","f.JPG",true));
      b.observe_write201(101,0,r,sizeof(r));b.observe_close(101,0);t.drain(b);
      for (size_t i=0;i<=MailboxCapacity;++i) b.observe_close(500+i,0);
      CHECK(b.faulted() && !t.take_candidate(11,out,b)); }
    { CompletionMailbox b;std::memset(r,'x',sizeof(r));CHECK(!b.observe_write201(5,0,r,sizeof(r)) && b.faulted());
      expected_result=r;forward_fsys201(b,reinterpret_cast<void*>(77),0,r,sizeof(r),original);CHECK(forwarded==2); }
    { CompletionMailbox b;CHECK(!b.observe_write201(5,0,r,sizeof(r)-1) && b.faulted()); }
    { CompletionMailbox b;CHECK(!b.observe_write201(0,0,r,sizeof(r)) && !b.faulted()); }
    { CaptureTracker t;for (size_t i=0;i<BridgeCapacity;++i) {char id[32];std::snprintf(id,sizeof(id),"frame-%zu",i);CHECK(t.arm(i+1,id,PhotoKind::JPEG,1));}
      CHECK(!t.arm(1000,"overflow",PhotoKind::JPEG,1)); }
}

static void scenario() {
    PresetMenu menu(12,3,Custom);CompletionMailbox box;CaptureTracker tracker;
    tap(menu,Custom);tap(menu,Right);tap(menu,Down);tap(menu,Right);auto a=tap(menu,Enter);CHECK(a.commit);menu.acknowledge(a.ticket,true);
    CHECK(tracker.arm(9,"sony-jpeg",PhotoKind::JPEG,menu.generation()));
    CHECK(tracker.bind(9,99,"A:/DCIM/100MSDCF/DSC00001.JPG","DCIM/100MSDCF/DSC00001.JPG",true));
    tap(menu,Custom);tap(menu,Right);tap(menu,Down);tap(menu,Right);auto b=tap(menu,Enter);CHECK(b.commit);menu.acknowledge(b.ticket,true);
    CHECK(tracker.arm(10,"sony-hif",PhotoKind::HIF,menu.generation()));
    CHECK(tracker.bind(10,100,"A:/DCIM/100MSDCF/DSC00002.HIF","DCIM/100MSDCF/DSC00002.HIF",true));
    uint8_t raw[FsysResultBytes201];result(raw,"A:/DCIM/100MSDCF/DSC00002.HIF");
    box.observe_write201(100,0,raw,sizeof(raw));box.observe_close(100,0);
    result(raw,"A:/DCIM/100MSDCF/DSC00001.JPG");box.observe_close(99,0);box.observe_write201(99,0,raw,sizeof(raw));tracker.drain(box);
    CaptureBinding h{},j{};CHECK(tracker.take_candidate(10,h,box));CHECK(tracker.take_candidate(9,j,box));
    CHECK(j.selection_generation==1 && h.selection_generation==2);
    std::printf("{\"scope\":\"synthetic normalized bindings; no Sony hook installed\",\"menu_choices\":[[%u,%u],[%u,%u]],\"completion_order\":[\"%s\",\"%s\"],\"paths\":[\"%s\",\"%s\"]}\n",a.choice.look,a.choice.lens,b.choice.look,b.choice.lens,h.event_id,j.event_id,j.relative_path,h.relative_path);
}

#include "sony_identity_cases.h"
int main(int argc,char** argv) {
    tests();
    identity_tests();
    if (argc==2 && !std::strcmp(argv[1],"--scenario")) scenario();
    else std::printf("{\"checks\":%d,\"status\":\"passed\",\"camera_accessed\":false}\n",checks);
}
