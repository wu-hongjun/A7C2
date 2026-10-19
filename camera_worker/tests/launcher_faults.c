/* Mac-only test interposer. Never linked into a target/bundle. */
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#define INTERPOSE(replacement, original) \
 __attribute__((used)) static struct {const void* replacement;const void* original;} interpose_##original \
 __attribute__((section("__DATA,__interpose")))={(const void*)&replacement,(const void*)&original}
static int fault(const char* name){const char* p=getenv("A7_LAUNCHER_TEST_FAULT");return p&&!strcmp(p,name);}
static int suffix(int fd,const char* ending){char p[4096];if(fcntl(fd,F_GETPATH,p))return 0;size_t a=strlen(p),b=strlen(ending);return a>=b&&!strcmp(p+a-b,ending);}
static ssize_t test_write(int fd,const void* p,size_t n){
 if(fault("snapshot-enospc")&&(suffix(fd,"/input.jpg")||suffix(fd,"/input.HIF"))){errno=ENOSPC;return -1;}
 return write(fd,p,n);
}
static ssize_t test_read(int fd,void* p,size_t n){
 ssize_t got=read(fd,p,n);
 if(fault("source-changed")&&n==65536&&suffix(fd,"/large.JPG")){struct timeval t[2];gettimeofday(&t[0],0);t[1]=t[0];t[1].tv_sec+=30;futimes(fd,t);}
 return got;
}
static pid_t test_fork(void){if(fault("fork-failed")){errno=EAGAIN;return -1;}return fork();}
static int test_execve(const char* path,char*const argv[],char*const envp[]){if(fault("exec-failed")){errno=ENOEXEC;return -1;}return execve(path,argv,envp);}
INTERPOSE(test_write,write);INTERPOSE(test_read,read);INTERPOSE(test_fork,fork);INTERPOSE(test_execve,execve);
