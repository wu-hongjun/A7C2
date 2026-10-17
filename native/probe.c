/* One-shot Jiritsu diagnostic plugin for the pinned ILCE-7CM2 2.01 AP.
 * Loaded by SET_FILE2's native libJit.so dlopen/dlclose path. No preload,
 * child process, clock, firmware, settings, mount or arbitrary memory writes.
 * Reads only this process's maps/mounts/cmdline and the pinned model index.
 */
typedef unsigned long u64;
typedef unsigned int u32;
typedef long i64;
typedef unsigned char u8;
static i64 sc(i64 no,i64 a,i64 b,i64 c,i64 d,i64 e,i64 f) {
    register i64 x0 __asm__("x0")=a, x1 __asm__("x1")=b;
    register i64 x2 __asm__("x2")=c, x3 __asm__("x3")=d;
    register i64 x4 __asm__("x4")=e, x5 __asm__("x5")=f;
    register i64 x8 __asm__("x8")=no;
    __asm__ volatile("svc 0" : "+r"(x0) : "r"(x1),"r"(x2),"r"(x3),"r"(x4),"r"(x5),"r"(x8) : "memory","cc");
    return x0;
}
static i64 op(const char *p,i64 flags) {return sc(56,-100,(i64)p,flags,0644,0,0);}
static void cl(i64 fd) {if(fd>=0)sc(57,fd,0,0,0,0,0);}
static u8 maps[524288], mounts[32768], cmdline[256];
struct Report {
    u8 magic[8]; u32 version, index;
    i64 status, euid, pid, cleanup; u64 base;
    u32 maps_len, mounts_len, cmdline_len, reserved;
};
static struct Report report;
static const u8 magic[8]={'A','7','P','R','B','0','0','1'};
static i64 read_all(const char *path,u8 *out,u32 size) {
    i64 fd=op(path,0);if(fd<0)return fd;
    u32 done=0, retries=0;
    while(done<size) {
        i64 n=sc(63,fd,(i64)(out+done),size-done,0,0,0);
        if(n==-4 && retries++<3)continue;
        if(n<0) {cl(fd);return n;}
        if(!n) {cl(fd);return done;}
        if((u64)n>size-done) {cl(fd);return -5;}
        done+=(u32)n;
    }
    cl(fd);return -75; /* reject truncation rather than parse partial maps */
}
static int write_all(i64 fd,const void *data,u32 size) {
    u32 done=0,retries=0;
    while(done<size) {
        i64 n=sc(64,fd,(i64)((const u8*)data+done),size-done,0,0,0);
        if(n==-4 && retries++<3)continue;
        if(n<=0 || (u64)n>size-done)return -5;
        done+=(u32)n;
    }
    return 0;
}
static int hex(const u8 *buf,u32 end,u32 *at,u64 *value) {
    u32 begin=*at;*value=0;
    while(*at<end) {
        u8 c=buf[*at];u32 v;
        if(c>='0'&&c<='9')v=c-'0';
        else if(c>='a'&&c<='f')v=c-'a'+10;
        else break;
        if(*at-begin>=16)return 0;
        *value=(*value<<4)|v;(*at)++;
    }
    return *at>begin;
}
static int app_mapping(u32 begin,u32 end,u64 *start,u64 *stop,u64 *offset) {
    const u8 suffix[]="/appFw.so";
    if(end-begin<sizeof(suffix)-1)return 0;
    for(u32 i=0;i<sizeof(suffix)-1;i++)
        if(maps[end-(sizeof(suffix)-1)+i]!=suffix[i])return 0;
    u32 at=begin;
    if(!hex(maps,end,&at,start)||at>=end||maps[at++]!='-')return 0;
    if(!hex(maps,end,&at,stop)||end-at<7||maps[at++]!=' ')return 0;
    if(maps[at]!='r')return 0;
    at+=4;
    if(at>=end||maps[at++]!=' ')return 0;
    return hex(maps,end,&at,offset);
}
static void find_model(void) {
    u32 begin=0,count=0;
    for(u32 i=0;i<report.maps_len;i++)if(maps[i]=='\n') {
        u64 start,stop,offset;
        if(app_mapping(begin,i,&start,&stop,&offset) && !offset) {
            report.base=start;count++;
        }
        begin=i+1;
    }
    if(count!=1) {report.status=-2;return;}
    const u64 address=report.base+0x5bc35b0UL;
    if(address<report.base) {report.status=-22;return;}
    begin=0;count=0;
    for(u32 i=0;i<report.maps_len;i++)if(maps[i]=='\n') {
        u64 start,stop,offset;
        if(app_mapping(begin,i,&start,&stop,&offset) && address>=start && address+4<=stop)count++;
        begin=i+1;
    }
    if(count!=1) {report.status=-14;return;}
    i64 fd=op("/proc/self/mem",0);
    if(fd<0) {report.status=fd;return;}
    i64 n=sc(67,fd,(i64)&report.index,4,address,0,0);cl(fd);
    if(n!=4)report.status=n<0?n:-5;
    else if(report.index>25)report.status=-22;
}
__attribute__((visibility("default"))) void _init(void) {
    for(u32 i=0;i<8;i++)report.magic[i]=magic[i];
    report.version=1;report.index=0xffffffffU;
    report.euid=sc(175,0,0,0,0,0,0);
    report.pid=sc(172,0,0,0,0,0,0);
    report.cleanup=sc(35,-100,(i64)"/tmp/jiritsu/libJit.so",0,0,0,0);
    i64 n=read_all("/proc/self/maps",maps,sizeof(maps));
    if(n<0)report.status=n;
    else {report.maps_len=(u32)n;find_model();}
    n=read_all("/proc/mounts",mounts,sizeof(mounts));
    if(n>=0)report.mounts_len=(u32)n;else if(!report.status)report.status=n;
    n=read_all("/proc/self/cmdline",cmdline,sizeof(cmdline));
    if(n>=0)report.cmdline_len=(u32)n;else if(!report.status)report.status=n;
    i64 fd=op("/tmp/jiritsu/a7c2.probe.result",0x241);
    if(fd>=0) {
        if(!write_all(fd,&report,sizeof(report)) &&
           !write_all(fd,maps,report.maps_len) &&
           !write_all(fd,mounts,report.mounts_len))write_all(fd,cmdline,report.cmdline_len);
        sc(82,fd,0,0,0,0,0);cl(fd);
    }
    /* Return normally to dlopen; never exit or modify the hosting process. */
}
