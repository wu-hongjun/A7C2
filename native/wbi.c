/* Native vendor sysfs sequence only: inspect or drop1 -> mode0.
 * No raw block writes, firmware/settings writes, shell or power commands.
 */
typedef unsigned long u64;typedef unsigned int u32;typedef long i64;typedef unsigned char u8;
static i64 sc(i64 no,i64 a,i64 b,i64 c,i64 d,i64 e,i64 f) {
    register i64 x0 __asm__("x0")=a,x1 __asm__("x1")=b,x2 __asm__("x2")=c;
    register i64 x3 __asm__("x3")=d,x4 __asm__("x4")=e,x5 __asm__("x5")=f,x8 __asm__("x8")=no;
    __asm__ volatile("svc 0" : "+r"(x0) : "r"(x1),"r"(x2),"r"(x3),"r"(x4),"r"(x5),"r"(x8) : "memory","cc");return x0;
}
static i64 op(const char *p,i64 flags) {return sc(56,-100,(i64)p,flags,0644,0,0);}
static int same(const u8*a,const u8*b,u32 n) {for(u32 i=0;i<n;i++)if(a[i]!=b[i])return 0;return 1;}
struct Request {u8 magic[8];u32 mode,reserved;};
struct Report {
    u8 magic[8];u32 version,mode;
    i64 status,cleanup,euid,pid,drop_write,drop_close,mode_write,mode_close,log_status;
    u8 before_mode[16],after_mode[16],before_dev[32],after_dev[32];
    u32 fresh_drop_log,reserved;
};
_Static_assert(sizeof(struct Report)==192,"WBI result ABI");
static struct Report r;static struct Request req;static u8 logbuf[8192];
static i64 read_text(const char *path,u8 *out,u32 size) {
    i64 fd=op(path,0);if(fd<0)return fd;
    i64 n=sc(63,fd,(i64)out,size-1,0,0,0);i64 c=sc(57,fd,0,0,0,0,0);
    if(n<0)return n;if(c)return c;if(n>=(i64)size-1)return -75;return n;
}
static void change(const char *path,char value,i64 *written,i64 *closed) {
    i64 fd=op(path,1);
    if(fd<0) {*written=fd;return;}
    *written=sc(64,fd,(i64)&value,1,0,0,0);*closed=sc(57,fd,0,0,0,0,0);
}
static void work(void) {
    i64 fd=op("/tmp/jiritsu/a7c2.wbi.request",0);
    if(fd<0) {r.status=fd;return;}
    i64 n=sc(63,fd,(i64)&req,sizeof(req),0,0,0),c=sc(57,fd,0,0,0,0,0);
    const u8 magic[]="A7WBI001";
    if(n!=(i64)sizeof(req)||c||!same(req.magic,magic,8)||req.mode>1||req.reserved) {r.status=-22;return;}
    r.mode=req.mode;
    if(r.euid) {r.status=-13;return;}
    n=read_text("/sys/power/image_mode",r.before_mode,sizeof(r.before_mode));
    if(n!=2||!same(r.before_mode,(const u8*)"2\n",2)) {r.status=n<0?n:-22;return;}
    n=read_text("/sys/power/image_dev",r.before_dev,sizeof(r.before_dev));
    if(n!=6||!same(r.before_dev,(const u8*)"*wbi4\n",6)) {r.status=n<0?n:-22;return;}
    if(!req.mode)return;
    i64 log=op("/dev/kmsg",0x800);
    if(log<0) {r.status=log;return;}
    n=sc(62,log,0,2,0,0,0);
    if(n<0) {r.status=n;sc(57,log,0,0,0,0,0);return;}
    change("/sys/power/image_drop",'1',&r.drop_write,&r.drop_close);
    if(r.drop_write==1) {
        /* Even if close reports an error, finish mode0 after accepted drop1. */
        change("/sys/power/image_mode",'0',&r.mode_write,&r.mode_close);
    }
    if(r.drop_write!=1||r.mode_write!=1||r.drop_close||r.mode_close)r.status=-5;
    n=read_text("/sys/power/image_mode",r.after_mode,sizeof(r.after_mode));
    if(n!=2||!same(r.after_mode,(const u8*)"0\n",2))r.status=n<0?n:-84;
    n=read_text("/sys/power/image_dev",r.after_dev,sizeof(r.after_dev));
    if(n!=6||!same(r.after_dev,r.before_dev,6))r.status=n<0?n:-84;
    const u8 marker[]="dropped wbi";
    for(u32 count=0;count<32;count++) {
        n=sc(63,log,(i64)logbuf,sizeof(logbuf),0,0,0);
        if(n==-11)break;
        if(n<0) {r.log_status=n;break;}
        if(!n)break;
        for(u32 i=0;i+sizeof(marker)-1<=(u32)n;i++)
            if(same(logbuf+i,marker,sizeof(marker)-1))r.fresh_drop_log=1;
    }
    c=sc(57,log,0,0,0,0,0);if(c)r.log_status=c;
    if((r.log_status||!r.fresh_drop_log)&&!r.status)r.status=-61;
}
__attribute__((visibility("default"))) void _init(void) {
    const u8 magic[]="A7WBR001";for(u32 i=0;i<8;i++)r.magic[i]=magic[i];
    r.version=1;r.euid=sc(175,0,0,0,0,0,0);r.pid=sc(172,0,0,0,0,0,0);
    r.cleanup=sc(35,-100,(i64)"/tmp/jiritsu/libJit.so",0,0,0,0);work();
    i64 fd=op("/tmp/jiritsu/a7c2.wbi.result",0x241);
    if(fd>=0) {sc(64,fd,(i64)&r,sizeof(r),0,0,0);sc(82,fd,0,0,0,0,0);sc(57,fd,0,0,0,0,0);}
}
