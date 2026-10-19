/* Standalone completed-JPEG worker. No camera SDK, transport, service entry,
 * constructor, firmware/settings access, watcher, thread or background process.
 * Adobe color algorithm attribution: NOTICE.md / licenses/Adobe-DNG-SDK.txt.
 */
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/syscall.h>
#endif
#ifndef A7_HIF_CORE
#include <jpeglib.h>
#endif
typedef uint8_t u8; typedef uint32_t u32; typedef uint64_t u64;
#include "../native/sha256.h"
#include "srgb_profile.h"
#define LUT_FLOATS (36+4097*2+32*32*32*3)
#define LUT_BYTES (16+LUT_FLOATS*4)
#define JPEG_LIMIT (64ULL*1024*1024)
#ifdef A7_HIF_CORE
static u8 hif_input_hash[32];
#endif
static volatile sig_atomic_t cancelled;
static jmp_buf failure;
static const char *reason="unknown";
static struct {
    int dirfd,infd,outfd,created,dec_live,enc_live,published;
    FILE *infile,*outfile;

#ifndef A7_HIF_CORE
    struct jpeg_decompress_struct dec;
    struct jpeg_compress_struct enc;
    struct jpeg_error_mgr err;
#endif
    u8 *exif,*look,*row; float *floats;
    u32 width,height,quality,exif_size,look_size; u64 input_size;
    char temp[64];
} S;
static void stop(int sig) {(void)sig;cancelled=1;}
static void die(const char *why) {reason=why;longjmp(failure,1);}
static u32 le32(const u8 *p) {return (u32)p[0]|(u32)p[1]<<8|(u32)p[2]<<16|(u32)p[3]<<24;}
static u64 le64(const u8 *p) {return le32(p)|(u64)le32(p+4)<<32;}
static int ordinary(int dir,const char *name,u64 size) {
    int fd=openat(dir,name,O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
    if(fd<0)die("asset_open");
    struct stat st;
    if(fstat(fd,&st)||!S_ISREG(st.st_mode)||(u64)st.st_size!=size) {close(fd);die("asset_type_or_size");}
    return fd;
}
static void exact_read(int fd,u8 *out,size_t n) {
    while(n) {
        if(cancelled)die("cancelled");
        ssize_t got=read(fd,out,n);
        if(got<0&&errno==EINTR)continue;
        if(got<=0)die("short_read");
        out+=got;n-=(size_t)got;
    }
}
static void hash_bytes(const u8 *raw,size_t n,u8 digest[32]) {
    SHA256 h;sha_init(&h);sha_add(&h,raw,(u32)n);sha_end(&h,digest);
}
static void hash_fd(int fd,u64 size,const u8 expected[32]) {
    u8 data[16384],digest[32];SHA256 h;sha_init(&h);
    if(lseek(fd,0,SEEK_SET)<0)die("seek");
    for(u64 remain=size;remain;) {
        u32 n=remain>sizeof(data)?sizeof(data):(u32)remain;
        exact_read(fd,data,n);sha_add(&h,data,n);remain-=n;
    }
    sha_end(&h,digest);
    if(memcmp(digest,expected,32))die("asset_hash");
    if(lseek(fd,0,SEEK_SET)<0)die("seek");
}
static u8 *asset(const char *name,u32 size,const u8 expected[32]) {
    int fd=ordinary(S.dirfd,name,size);u8 *raw=malloc(size?size:1);
    if(!raw){close(fd);die("allocation");}
    /* No longjmp from an untracked allocation/read: all read errors close/free. */
    size_t done=0;
    while(done<size) {
        ssize_t n=read(fd,raw+done,size-done);
        if(n<0&&errno==EINTR)continue;
        if(n<=0||cancelled){free(raw);close(fd);die("asset_read");}
        done+=(size_t)n;
    }
    close(fd);u8 got[32];hash_bytes(raw,size,got);
    if(memcmp(got,expected,32)){free(raw);die("asset_hash");}
    return raw;
}
static u32 tiff(const u8 *p,int bytes,int big) {
    u32 v=0;for(int i=0;i<bytes;i++)v|=(u32)p[i]<<(8*(big?bytes-1-i:i));return v;
}
static void validate_exif(void) {
    const u8 *p=S.exif+6;u32 n=S.exif_size-6;
    if(memcmp(S.exif,"Exif\0\0",6)||n<8)die("exif_header");
    int big=p[0]=='M';
    if((big?memcmp(p,"MM",2):memcmp(p,"II",2))||tiff(p+2,2,big)!=42)die("exif_tiff");
    u32 off=tiff(p+4,4,big),sub=0;int orientation=0,color=0,dimensions=0;
    static const u32 roots[]={0x010f,0x0110,0x0132,0x013b,0x8298,0x0112,0x0131,0x8769};
    static const u32 photos[]={0x829a,0x829d,0x8822,0x8827,0x8830,0x8832,0x9003,0x9004,
      0x9010,0x9011,0x9012,0x9201,0x9202,0x9204,0x9205,0x9207,0x9208,0x9209,0x920a,
      0x9290,0x9291,0x9292,0xa402,0xa403,0xa405,0xa432,0xa433,0xa434,0xa001,0xa002,0xa003};
    for(int level=0;level<2;level++) {
        if(off<8||off>n-2)die("exif_offset");
        u32 count=tiff(p+off,2,big),seen[64];
        if(count>64||off+2+12*count+4>n)die("exif_count");
        for(u32 i=0;i<count;i++) {
            const u8 *e=p+off+2+12*i;u32 tag=tiff(e,2,big),type=tiff(e+2,2,big),items=tiff(e+4,4,big);
            int allowed=0;const u32 *tags=level?photos:roots;u32 nt=level?sizeof(photos)/4:sizeof(roots)/4;
            for(u32 j=0;j<nt;j++)if(tags[j]==tag)allowed=1;
            if(!allowed||items>4096||!items)die("exif_tag");
            for(u32 j=0;j<i;j++)if(seen[j]==tag)die("exif_duplicate");seen[i]=tag;
            u32 width=type==1||type==2||type==7?1:type==3?2:type==4||type==9?4:type==5||type==10?8:0;
            if(!width)die("exif_type");u32 bytes=items*width;
            u32 where=bytes<=4?(u32)(e+8-p):tiff(e+8,4,big);
            if(where>n||bytes>n-where)die("exif_value_offset");
            const u8 *value=p+where;
            if(type==2&&value[bytes-1]!=0)die("exif_ascii");
            if(type==5||type==10)for(u32 j=0;j<items;j++)if(!tiff(value+8*j+4,4,big))die("exif_rational");
            if(tag==0x8769){if(level||type!=4||items!=1)die("exif_ifd");sub=tiff(value,4,big);}
            if(tag==0x0112){if(type!=3||items!=1||(tiff(value,2,big)<1||tiff(value,2,big)>8))die("orientation");orientation=1;}
#ifdef A7_HIF_CORE
            if(tag==0x0112&&tiff(value,2,big)!=1)die("hif_exif_orientation");
#endif
            if(tag==0xa001){if(type!=3||items!=1||tiff(value,2,big)!=1)die("color_space");color=1;}
            if(tag==0xa002||tag==0xa003) {
                if((type!=3&&type!=4)||items!=1||tiff(value,(int)width,big)!=(tag==0xa002?S.width:S.height))die("exif_dimensions");
                dimensions|=tag==0xa002?1:2;
            }
        }
        if(tiff(p+off+2+12*count,4,big))die("exif_thumbnail_or_chain");
        off=sub;
    }
    if(!orientation||!color||dimensions!=3)die("exif_required_fields");
}
static void load(const char *directory) {
    S.dirfd=open(directory,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
    if(S.dirfd<0)die("job_directory");
    u8 header[176],digest[32];int fd=ordinary(S.dirfd,"job.bin",sizeof(header));
    /* Fixed header read can't retain a file descriptor on failure. */
    ssize_t got=read(fd,header,sizeof(header));close(fd);
    if(got!=(ssize_t)sizeof(header))die("job_header_read");
    hash_bytes(header,144,digest);
#ifdef A7_HIF_CORE
    const char *magic="A7HIFS01";
#else
    const char *magic="A7JOBS01";
#endif
    if(memcmp(header,magic,8)||le32(header+8)!=1||memcmp(digest,header+144,32))die("job_header");
    u32 flags=le32(header+12);S.quality=le32(header+16);S.width=le32(header+20);S.height=le32(header+24);
    S.exif_size=le32(header+28);S.look_size=le32(header+32);S.input_size=le64(header+40);
    if(flags>1||le32(header+36)||!S.quality||S.quality>100||!S.width||!S.height||S.width>8192||S.height>8192||
      (u64)S.width*S.height>40000000||S.exif_size<14||S.exif_size>65533||!S.input_size||S.input_size>JPEG_LIMIT||
      S.look_size!=(flags?LUT_BYTES:0))die("job_bounds");
    S.exif=asset("metadata.exif",S.exif_size,header+80);validate_exif();
    if(flags) {
        S.look=asset("look.bin",S.look_size,header+112);
        if(memcmp(S.look,"A7LUT001",8)||le32(S.look+8)!=1||le32(S.look+12))die("lut_header");
        S.floats=(float*)(S.look+16);
        for(u32 i=0;i<LUT_FLOATS;i++) {
            float f=S.floats[i];float lo=i<36?-16.f:0.f,hi=i<36?16.f:1.f;
            if(!isfinite(f)||f<lo||f>hi)die("lut_values");
        }
    } else {hash_bytes((const u8*)"",0,digest);if(memcmp(digest,header+112,32))die("empty_lut_hash");}
#ifdef A7_HIF_CORE
    const char *input_name="input.HIF";
#else
    const char *input_name="input.jpg";
#endif
    S.infd=ordinary(S.dirfd,input_name,S.input_size);hash_fd(S.infd,S.input_size,header+48);
#ifdef A7_HIF_CORE
    memcpy(hif_input_hash,header+48,32);
#endif
}
static float clip(float x) {return x<0?0:x>1?1:x;}
static float gamma_at(const float *p,float x) {
    x=clip(x)*4096.f;int i=(int)x;if(i==4096)return p[4096];float t=x-(float)i;
    return p[i]*(1.f-t)+p[i+1]*t;
}
static void matrix(float p[3],const float *m) {
    float r=p[0],g=p[1],b=p[2];for(int c=0;c<3;c++)p[c]=r*m[3*c]+g*m[3*c+1]+b*m[3*c+2];
}
static void apply(float p[3]) {
    if(!S.floats)return;
    const float *m=S.floats,*dec=m+36,*enc=dec+4097,*lut=enc+4097;
    for(int c=0;c<3;c++)p[c]=gamma_at(dec,p[c]);
    matrix(p,m);matrix(p,m+9);
    int base[3],order[3]={0,1,2};float f[3];
    for(int c=0;c<3;c++) {
        float t=gamma_at(enc,p[c])*31.f;base[c]=(int)t;if(base[c]>30)base[c]=30;f[c]=t-base[c];
    }
    /* Sort fractions to select the four corners of the enclosing tetrahedron. */
    for(int a=0;a<2;a++)for(int b=a+1;b<3;b++)if(f[order[b]]>f[order[a]]){int t=order[a];order[a]=order[b];order[b]=t;}
    int stride[3]={32*32*3,32*3,3};int index=(base[0]*32*32+base[1]*32+base[2])*3;
    int o1=stride[order[0]],o2=o1+stride[order[1]],o3=32*32*3+32*3+3;
    float a=f[order[0]],b=f[order[1]],c=f[order[2]];
    for(int k=0;k<3;k++)p[k]=gamma_at(dec,clip((1.f-a)*lut[index+k]+(a-b)*lut[index+o1+k]+(b-c)*lut[index+o2+k]+c*lut[index+o3+k]));
    matrix(p,m+18);for(int k=0;k<3;k++)p[k]=clip(p[k]);
    matrix(p,m+27);for(int k=0;k<3;k++)p[k]=gamma_at(enc,p[k]);
}
#ifndef A7_HIF_CORE
static void jpeg_failure(j_common_ptr p) {(void)p;die("jpeg_error");}
static void jpeg_warning(j_common_ptr p,int level) {(void)p;if(level<0)die("jpeg_warning_or_truncation");}
static void header_check(void) {
    if(S.dec.image_width!=S.width||S.dec.image_height!=S.height||S.dec.data_precision!=8||S.dec.num_components!=3||
      (S.dec.jpeg_color_space!=JCS_YCbCr&&S.dec.jpeg_color_space!=JCS_RGB)||S.dec.progressive_mode||S.dec.arith_code||
      S.dec.Ss!=0||S.dec.Se!=63||S.dec.Ah!=0||S.dec.Al!=0||jpeg_has_multiple_scans(&S.dec))die("unsupported_jpeg");
}
static void decoder(FILE *input) {
    S.dec.err=&S.err;S.dec_live=1;jpeg_create_decompress(&S.dec);S.dec.mem->max_memory_to_use=4*1024*1024;
    jpeg_stdio_src(&S.dec,input);if(jpeg_read_header(&S.dec,TRUE)!=JPEG_HEADER_OK)die("jpeg_header");header_check();
    S.dec.out_color_space=JCS_RGB;S.dec.dct_method=JDCT_ISLOW;S.dec.do_fancy_upsampling=TRUE;
}
#endif
#ifdef A7_HIF_CORE
#define RESULT_NAME "result.HIF"
#define TEMP_PREFIX ".hif-worker"
#else
#define RESULT_NAME "result.jpg"
#define TEMP_PREFIX ".jpeg-worker"
#endif
static void reserve_output(void) {
    struct stat st;
    if(fstatat(S.dirfd,RESULT_NAME,&st,AT_SYMLINK_NOFOLLOW)==0||errno!=ENOENT)die("output_exists_or_unavailable");
    for(unsigned i=0;i<100;i++) {
        snprintf(S.temp,sizeof(S.temp),TEMP_PREFIX "-%ld-%u.tmp",(long)getpid(),i);
        S.outfd=openat(S.dirfd,S.temp,O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0600);
        if(S.outfd>=0){S.created=1;return;}if(errno!=EEXIST)die("output_reservation");
    }die("output_reservation");
}
static void publish(void) {
    if(cancelled)die("cancelled");
#ifdef __linux__
    if(syscall(SYS_renameat2,S.dirfd,S.temp,S.dirfd,RESULT_NAME,1))die("no_replace_publication");
#else
    if(linkat(S.dirfd,S.temp,S.dirfd,RESULT_NAME,0))die("no_replace_publication");
    S.published=1;
    if(unlinkat(S.dirfd,S.temp,0))die("published_temp_cleanup");
#endif
    S.published=1;S.created=0;
    if(fsync(S.dirfd))die("published_directory_sync_unconfirmed");
}
#ifndef A7_HIF_CORE
static void process(void) {
    S.infile=fdopen(S.infd,"rb");if(!S.infile)die("input_stream");S.infd=-1;
    decoder(S.infile);reserve_output();S.row=malloc(S.width*3);if(!S.row)die("row_allocation");
    S.outfile=fdopen(S.outfd,"w+b");if(!S.outfile)die("output_stream");S.outfd=-1;
    S.enc.err=&S.err;S.enc_live=1;jpeg_create_compress(&S.enc);S.enc.mem->max_memory_to_use=4*1024*1024;
    jpeg_stdio_dest(&S.enc,S.outfile);S.enc.image_width=S.width;S.enc.image_height=S.height;
    S.enc.input_components=3;S.enc.in_color_space=JCS_RGB;jpeg_set_defaults(&S.enc);
    jpeg_set_quality(&S.enc,(int)S.quality,TRUE);S.enc.optimize_coding=FALSE;S.enc.dct_method=JDCT_ISLOW;
    for(int i=0;i<3;i++)S.enc.comp_info[i].h_samp_factor=S.enc.comp_info[i].v_samp_factor=1;
    jpeg_start_decompress(&S.dec);jpeg_start_compress(&S.enc,TRUE);jpeg_write_marker(&S.enc,JPEG_APP0+1,S.exif,S.exif_size);
    jpeg_write_marker(&S.enc,JPEG_APP0+2,srgb_marker,sizeof(srgb_marker));
    JSAMPROW row=S.row;
    while(S.dec.output_scanline<S.height) {
        if(cancelled)die("cancelled");
        if(jpeg_read_scanlines(&S.dec,&row,1)!=1)die("jpeg_row_read");
        for(u32 x=0;x<S.width;x++) {
            float p[3];for(int c=0;c<3;c++)p[c]=row[3*x+c]/255.f;apply(p);
            for(int c=0;c<3;c++)row[3*x+c]=(u8)floorf(clip(p[c])*255.f+.5f);
        }
        if(jpeg_write_scanlines(&S.enc,&row,1)!=1)die("jpeg_row_write");
    }
    if(!jpeg_finish_decompress(&S.dec))die("jpeg_finish_input");
    /* Reject trailing bytes rather than silently accepting another concatenated image. */
    if(S.dec.src->bytes_in_buffer||fgetc(S.infile)!=EOF||ferror(S.infile))die("jpeg_trailing_bytes");
    jpeg_finish_compress(&S.enc);jpeg_destroy_compress(&S.enc);S.enc_live=0;
    jpeg_destroy_decompress(&S.dec);S.dec_live=0;
    if(fflush(S.outfile)||fsync(fileno(S.outfile)))die("output_write_sync");
    if(fseek(S.outfile,0,SEEK_SET))die("output_seek");
    decoder(S.outfile);jpeg_start_decompress(&S.dec);
    while(S.dec.output_scanline<S.height){if(cancelled)die("cancelled");if(jpeg_read_scanlines(&S.dec,&row,1)!=1)die("output_verify");}
    if(!jpeg_finish_decompress(&S.dec))die("output_verify_finish");jpeg_destroy_decompress(&S.dec);S.dec_live=0;
    FILE *closed=S.outfile;S.outfile=NULL;if(fclose(closed))die("output_close");publish();
}
static void vectors(void) {
    struct stat st;int fd=openat(S.dirfd,"vectors.in",O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
    if(fd<0||fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_size<12||st.st_size>2*1024*1024||st.st_size%12) {if(fd>=0)close(fd);die("vectors_input");}
    S.infile=fdopen(fd,"rb");if(!S.infile){close(fd);die("vectors_stream");}
    S.outfd=openat(S.dirfd,"vectors.out",O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0600);
    if(S.outfd<0)die("vectors_output");
    float p[3];
    while(fread(p,12,1,S.infile)==1) {
        if(cancelled)die("cancelled");for(int i=0;i<3;i++)if(!isfinite(p[i])||p[i]<0||p[i]>1)die("vectors_bounds");
        apply(p);if(write(S.outfd,p,12)!=12)die("vectors_write");
    }
    if(ferror(S.infile))die("vectors_read");
}
#endif
static void cleanup(void) {
#ifndef A7_HIF_CORE
    if(S.enc_live)jpeg_destroy_compress(&S.enc);if(S.dec_live)jpeg_destroy_decompress(&S.dec);
#endif
    if(S.infile)fclose(S.infile);if(S.outfile)fclose(S.outfile);
    if(S.infd>=0)close(S.infd);if(S.outfd>=0)close(S.outfd);
    if(S.created&&!S.published)unlinkat(S.dirfd,S.temp,0);if(S.dirfd>=0)close(S.dirfd);
    free(S.exif);free(S.look);free(S.row);
}
#ifndef A7_HIF_CORE
int main(int argc,char **argv) {
    S.dirfd=S.infd=S.outfd=-1;
    if(argc==2&&!strcmp(argv[1],"--version")){puts("A7C2 JPEG Worker native 0.1.0; static ARM64 candidate; offline-only validation");return 0;}
    if(argc!=3||(strcmp(argv[1],"--run")&&strcmp(argv[1],"--check")&&strcmp(argv[1],"--vectors"))){fputs("Usage: jpeg-worker --check|--run|--vectors JOB_DIRECTORY\n",stderr);return 2;}
#ifdef __linux__
    struct rlimit mem={64*1024*1024,64*1024*1024},cpu={120,120},file={128*1024*1024,128*1024*1024};
    if(setrlimit(RLIMIT_AS,&mem)||setrlimit(RLIMIT_CPU,&cpu)||setrlimit(RLIMIT_FSIZE,&file)){fputs("resource_limits\n",stderr);return 1;}
#endif
    signal(SIGINT,stop);signal(SIGTERM,stop);
    S.err=*jpeg_std_error(&S.err);S.err.error_exit=jpeg_failure;S.err.emit_message=jpeg_warning;
    if(setjmp(failure)) {fprintf(stderr,"{\"status\":\"%s\",\"published\":%s}\n",reason,S.published?"true":"false");cleanup();return 1;}
    load(argv[2]);
    if(!strcmp(argv[1],"--run"))process();
    else if(!strcmp(argv[1],"--vectors"))vectors();
    else {S.infile=fdopen(S.infd,"rb");if(!S.infile)die("input_stream");S.infd=-1;decoder(S.infile);}
    printf("{\"status\":\"ok\",\"width\":%u,\"height\":%u,\"published\":%s,\"image_row_bytes\":%u}\n",S.width,S.height,S.published?"true":"false",S.width*3);
    cleanup();return 0;
}

#else
/* Every longjmp ends within its own C wrapper; never crosses C++ frames. */
#include "heif_core.h"
int a7_load(const char *directory) {
    S.dirfd=S.infd=S.outfd=-1;signal(SIGINT,stop);signal(SIGTERM,stop);
    if(setjmp(failure))return -1;
    load(directory);return 0;
}
const char *a7_error(void){return reason;}
void a7_cleanup(void){cleanup();}
int a7_input(void){return S.infd;}
int a7_verify_snapshot(const unsigned char *data,unsigned long long size){
    u8 got[32];if(size!=S.input_size)return -1;hash_bytes(data,(size_t)size,got);return memcmp(got,hif_input_hash,32)?-1:0;
}
unsigned a7_width(void){return S.width;}
unsigned a7_height(void){return S.height;}
unsigned a7_quality(void){return S.quality;}
const unsigned char *a7_exif(void){return S.exif;}
unsigned a7_exif_size(void){return S.exif_size;}
int a7_cancelled(void){return cancelled;}
void a7_apply(float p[3]){apply(p);}
int a7_reserve(void){if(setjmp(failure))return -1;reserve_output();return S.outfd;}
int a7_publish(void){if(setjmp(failure))return -1;publish();return 0;}
int a7_published(void){return S.published;}
#endif
