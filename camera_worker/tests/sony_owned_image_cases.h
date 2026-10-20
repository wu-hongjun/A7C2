#include "../sony_owned_image.h"
#include <vector>
static void complete_owned_fixture(IdentityFixture& f,IdentityLedger201& l) {
    CHECK(l.prfile_close_enter(0x55550000,f.file,sizeof(f.file)));
    CHECK(l.close_return(0x55550000,0));
    CHECK(l.completed(0x22220000,0,f.response,sizeof(f.response)));
}
static void owned_image_tests() {
    for (uint32_t type:{1u,4u}) {
        IdentityFixture f(73,1,3,type);IdentityLedger201 l;uint8_t buffer[32]{},src[16];
        for (size_t i=0;i<16;++i) src[i]=uint8_t(i);
        OwnedWriteImage201 image(buffer,sizeof(buffer),0x22220000,0x81);OwnedPhoto201 out{};
        CHECK(image.start_at(0x22220000,0x81,0,0));
        CHECK(l.arm(73,7,9));CHECK(f.select(l));CHECK(f.submit(l));CHECK(f.open(l));
        CHECK(!image.take(l,out));CHECK(image.chunk_enter(0x22220000,0x81,0,src,16));
        std::memset(src,0xff,16); // native scratch lifetime can end after call
        CHECK(image.chunk_return(0x22220000,0x81,16));
        CHECK(image.chunk_enter(0x22220000,0x81,16,src,16));CHECK(image.chunk_return(0x22220000,0x81,16));
        CHECK(image.truncate_return(0x22220000,0x81,19,0));CHECK(!image.take(l,out));
        complete_owned_fixture(f,l);CHECK(image.take(l,out));CHECK(out.bytes==buffer&&out.size==19);
        CHECK(out.identity.frame==73&&out.identity.generation==9&&out.identity.write.media==3);
        for (size_t i=0;i<16;++i) CHECK(out.bytes[i]==i);
        CHECK(out.bytes[16]==0xff);CHECK(!image.take(l,out));
    }
    for (int mode=0;mode<15;++mode) {
        uint8_t buffer[32]{},data[16]{};OwnedWriteImage201 image(buffer,32,1,2);
        CHECK(image.start_at(1,2,0,0));
        switch(mode) {
        case 0:CHECK(!image.chunk_enter(9,2,0,data,16));break;
        case 1:CHECK(!image.chunk_enter(1,9,0,data,16));break;
        case 2:CHECK(!image.chunk_enter(1,2,1,data,16));break;
        case 3:CHECK(!image.chunk_enter(1,2,0,data,0));break;
        case 4:CHECK(!image.chunk_enter(1,2,0,nullptr,16));break;
        case 5:CHECK(!image.chunk_enter(1,2,0,data,33));break;
        case 6:CHECK(!image.chunk_enter(1,2,0,buffer,16));break;
        case 7:CHECK(!image.chunk_return(1,2,16));break;
        default:
          CHECK(image.chunk_enter(1,2,0,data,16));
          if(mode==8){CHECK(!image.chunk_enter(1,2,0,data,16));break;}
          if(mode==9){CHECK(!image.chunk_return(1,2,15));break;}
          if(mode==10){CHECK(!image.chunk_return(1,2,-1));break;}
          if(mode==11){CHECK(!image.truncate_return(1,2,16,0));break;}
          CHECK(image.chunk_return(1,2,16));
          if(mode==12)CHECK(!image.truncate_return(1,2,17,0));
          if(mode==13)CHECK(!image.truncate_return(1,2,0,0));
          if(mode==14)CHECK(!image.truncate_return(1,2,16,-1));
        }
        CHECK(image.failed());CHECK(!image.chunk_enter(1,2,0,data,16));
    }
    { uint8_t data[2]{};OwnedWriteImage201 a(nullptr,1,1,2),b(data,0,1,2),c(data,EncodedPhotoLimit201+1,1,2),d(data,2,0,2);CHECK(a.failed()&&b.failed()&&c.failed()&&d.failed()); }
    for (int mode=0;mode<4;++mode) {
        IdentityFixture f;IdentityLedger201 l;uint8_t data[2]{},storage[2]{};OwnedPhoto201 out{};
        OwnedWriteImage201 image(storage,2,0x22220000,mode==3?0xa1:0x81);
        CHECK(image.start_at(0x22220000,mode==3?0xa1:0x81,0,0));
        CHECK(l.arm(73,7,9));CHECK(f.select(l));CHECK(f.submit(l));CHECK(f.open(l));
        CHECK(image.chunk_enter(0x22220000,mode==3?0xa1:0x81,0,data,2));CHECK(image.chunk_return(0x22220000,mode==3?0xa1:0x81,2));
        CHECK(image.truncate_return(0x22220000,mode==3?0xa1:0x81,2,0));
        if(mode==0)CHECK(l.completed(0x22220000,0,f.response,sizeof(f.response))); // no close
        if(mode==1){CHECK(l.prfile_close_enter(0x55550000,f.file,sizeof(f.file)));CHECK(l.close_return(0x55550000,-1));}
        if(mode>=2){complete_owned_fixture(f,l);if(mode==2)CHECK(l.cancel(73));}
        CHECK(!image.take(l,out));
    }
    { uint8_t storage[4]{},data[2]{};OwnedWriteImage201 image(storage,4,1,2);
      CHECK(image.start_at(1,2,0,0));
      CHECK(image.chunk_enter(1,2,0,data,2));CHECK(image.chunk_return(1,2,2));CHECK(image.truncate_return(1,2,2,0));
      CHECK(!image.chunk_enter(1,2,2,data,2)&&image.failed()); }
    for (int mode=0;mode<5;++mode) {
        uint8_t storage[4]{},data[2]{};OwnedWriteImage201 image(storage,4,1,2);
        if(mode==0)CHECK(!image.chunk_enter(1,2,0,data,2));
        if(mode==1)CHECK(!image.start_at(1,2,1,0));
        if(mode==2)CHECK(!image.start_at(1,2,0,-1));
        if(mode==3)CHECK(!image.start_at(1,3,0,0));
        if(mode==4){CHECK(image.start_at(1,2,0,0));CHECK(!image.start_at(1,2,0,0));}
        CHECK(image.failed());
    }
}
// Test-only driver: fixtures are generated local JPEG/HIF files, never Sony
// memory or mounted media. Export owned bytes AFTER all ledger gates succeed.
static void owned_replay(const char* jpeg,const char* hif,const char* directory,uint64_t generation) {
    IdentityFixture fs[2]={IdentityFixture(73,0,0,1),IdentityFixture(73,1,3,4)};
    // Both writes belong to ONE selected RecordStill and frozen frame, with
    // independent cookies, generated handles, slots and destination columns.
    std::memcpy(fs[0].record+0x77c,fs[1].record+0x77c,0x588);
    put32(fs[0].record+0x13c,909);std::memcpy(fs[1].record,fs[0].record,sizeof(fs[0].record));
    IdentityLedger201 l;CHECK(l.arm(73,7,generation));CHECK(fs[0].select(l));
    std::printf("{\"photos\":[");
    for (int k=0;k<2;++k) {
        FILE* input=std::fopen(k?hif:jpeg,"rb");CHECK(input);
        CHECK(!std::fseek(input,0,SEEK_END));long n=std::ftell(input);CHECK(n>0&&n<8*1024*1024);std::rewind(input);
        std::vector<uint8_t> original(size_t(n),0);CHECK(std::fread(original.data(),1,original.size(),input)==original.size());CHECK(!std::fclose(input));
        size_t padded=(original.size()+4095)/4096*4096;std::vector<uint8_t> scratch(padded,0xa5),owned(padded);
        std::memcpy(scratch.data(),original.data(),original.size());
        auto& f=fs[k];OwnedPhoto201 out{};uint64_t cookie=0x22220000+k,handle=0x81+k*0x20,slot=0x55550000+k;
        put64(f.request+0x500,cookie);put64(f.file+0x20,handle);
        CHECK(l.submit(0x11110000,f.record,sizeof(f.record),f.request,sizeof(f.request),cookie,f.cookie,sizeof(f.cookie)));
        CHECK(l.submission_return(cookie,0));CHECK(l.opened(cookie,slot,f.file,sizeof(f.file),0));
        OwnedWriteImage201 image(owned.data(),owned.size(),cookie,handle);
        CHECK(image.start_at(cookie,handle,0,0)); // synthetic same-open position
        for(size_t off=0;off<padded;) {
            size_t count=std::min(size_t(4096),padded-off);
            CHECK(image.chunk_enter(cookie,handle,off,scratch.data()+off,count));
            CHECK(image.chunk_return(cookie,handle,int32_t(count)));off+=count;
        }
        std::memset(scratch.data(),0xee,scratch.size()); // prove no late source dependency
        CHECK(image.truncate_return(cookie,handle,original.size(),0));CHECK(!image.take(l,out));
        CHECK(l.prfile_close_enter(slot,f.file,sizeof(f.file)));CHECK(l.close_return(slot,0));
        CHECK(l.completed(cookie,0,f.response,sizeof(f.response)));
        CHECK(image.take(l,out));CHECK(out.size==original.size()&&!std::memcmp(out.bytes,original.data(),out.size));
        std::string path=std::string(directory)+(k?"/owned-hif.HIF":"/owned-jpeg.JPG");
        FILE* output=std::fopen(path.c_str(),"wbx");CHECK(output);CHECK(std::fwrite(out.bytes,1,out.size,output)==out.size);CHECK(!std::fclose(output));
        std::printf("%s{\"frame\":%u,\"token\":%llu,\"generation\":%llu,\"group\":%u,\"media\":%u,\"bytes\":%zu,\"path\":\"%s\"}",k?",":"",out.identity.frame,
          (unsigned long long)out.identity.token,(unsigned long long)out.identity.generation,out.identity.write.format_group,out.identity.write.media,out.size,k?"owned-hif.HIF":"owned-jpeg.JPG");
    }
    std::puts("],\"owned_snapshots\":2,\"scratch_destroyed_before_completion\":true,\"padding_removed\":true,\"scope\":\"synthetic owned-byte replay, no Sony hooks\"}");
}
