/* Read only: compare six fixed AP regions in process memory and on disk.
 * Reuse the audited maps/model reader; no application or settings writes.
 */
#define _init probe_init
#include "probe.c"
#undef _init
struct Runtime {
    u8 magic[8];u32 version,count;i64 status,euid,pid;u64 base;
    u32 index,reserved;u8 data[6][2][128];
    u8 image_mode[16],image_dev[32],uptime[128],boot_id[64];
};
static struct Runtime runtime;
static const u64 offsets[6]={0x3528804,0x4b2ed19,0x59892b8,0x5989370,0x4b4a5e4,0x4b3b544};
static const u32 lengths[6]={112,2,40,40,12,12};
__attribute__((visibility("default"))) void _init(void) {
    probe_init();
    const u8 magic[]="A7RUN001";for(u32 i=0;i<8;i++)runtime.magic[i]=magic[i];
    runtime.version=1;runtime.count=6;runtime.status=report.status;runtime.euid=report.euid;
    runtime.pid=report.pid;runtime.base=report.base;runtime.index=report.index;
    if(!runtime.status) {
        i64 mem=op("/proc/self/mem",0),disk=op("/usr/lib/appFw.so",0);
        if(mem<0||disk<0)runtime.status=mem<0?mem:disk;
        else for(u32 i=0;i<6;i++) {
            u64 va=runtime.base+offsets[i]+(offsets[i]>=0x51a7718?0x10000:0);
            i64 n=sc(67,mem,(i64)runtime.data[i][0],lengths[i],va,0,0);
            if(n!=(i64)lengths[i]) {runtime.status=n<0?n:-5;break;}
            n=sc(67,disk,(i64)runtime.data[i][1],lengths[i],offsets[i],0,0);
            if(n!=(i64)lengths[i]) {runtime.status=n<0?n:-5;break;}
        }
        cl(mem);cl(disk);
    }
    read_all("/sys/power/image_mode",runtime.image_mode,sizeof(runtime.image_mode));
    read_all("/sys/power/image_dev",runtime.image_dev,sizeof(runtime.image_dev));
    read_all("/proc/uptime",runtime.uptime,sizeof(runtime.uptime));
    read_all("/proc/sys/kernel/random/boot_id",runtime.boot_id,sizeof(runtime.boot_id));
    i64 out=op("/tmp/jiritsu/a7c2.runtime.result",0x241);
    if(out>=0) {sc(64,out,(i64)&runtime,sizeof(runtime),0,0,0);sc(82,out,0,0,0,0,0);cl(out);}
}
