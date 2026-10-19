/* Native manual launcher + explicit completed-photo ingress. No firmware hooks,
 * service mode, device API, watcher, shell, auto-start or camera settings access. */
#include "heif_io.h"
#include "ingest_exif.h"
#include <sys/file.h>
#include <signal.h>
#include <sys/wait.h>
#include <cctype>
#include <limits>
#include <sys/statvfs.h>
#include <time.h>
extern char **environ;
using u8=uint8_t;using u32=uint32_t;using u64=uint64_t;
#include "../native/sha256.h"
struct Fd {
 int n=-1;explicit Fd(int x=-1):n(x){}~Fd(){if(n>=0)close(n);}Fd(const Fd&)=delete;Fd& operator=(const Fd&)=delete;
 Fd(Fd&& x) noexcept:n(x.n){x.n=-1;}Fd& operator=(Fd&& x) noexcept{if(n>=0)close(n);n=x.n;x.n=-1;return *this;}
};
static volatile sig_atomic_t stopping=0;
static void stop_launcher(int){stopping=1;}
static void active(){require(!stopping,"cancelled");}
static std::array<u8,32> digest(const std::vector<u8>& v){std::array<u8,32> d{};SHA256 h;sha_init(&h);require(v.size()<=UINT32_MAX,"hash_size");sha_add(&h,v.data(),v.size());sha_end(&h,d.data());return d;}
static std::string hex(const u8* d,size_t n=32){static const char* digits="0123456789abcdef";std::string s;for(size_t i=0;i<n;i++){s+=digits[d[i]>>4];s+=digits[d[i]&15];}return s;}
static uint64_t num64(const u8* p){return getnum(p,4)|(uint64_t(getnum(p+4,4))<<32);}
static double real64(const u8* p){double v;memcpy(&v,p,8);return v;}
static void set64(std::vector<u8>& v,size_t pos,uint64_t n){putnum(v,pos,n);putnum(v,pos+4,n>>32);}
static bool identifier(const std::string& s){if(s.empty()||s.size()>63)return false;for(char c:s)if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='-'||c=='_'))return false;return true;}
static std::string field(const u8* p,size_t n,bool allow_empty=false){size_t length=strnlen(reinterpret_cast<const char*>(p),n);require(length<n&&(length||allow_empty),"catalog_string");
 for(size_t i=0;i<length;i++)require(p[i]>=32&&p[i]<=126,"catalog_ASCII");for(size_t i=length;i<n;i++)require(p[i]==0,"catalog_padding");return {reinterpret_cast<const char*>(p),length};}
static Fd directory(const char* path){Fd f(open(path,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC));require(f.n>=0,"directory_open");return f;}
static void private_directory(int fd){struct stat st;require(!fstat(fd,&st)&&S_ISDIR(st.st_mode)&&st.st_uid==getuid()&&!(st.st_mode&077),"workspace_requires_private_owned_POSIX_directory");}
static Fd dirat(int parent,const std::string& name){Fd fd(openat(parent,name.c_str(),O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC));require(fd.n>=0,"subdirectory_open");return fd;}
static Fd regular(int dir,const std::string& name,bool executable=false){Fd fd(openat(dir,name.c_str(),O_RDONLY|O_NONBLOCK|O_NOFOLLOW|O_CLOEXEC));struct stat st;
 require(fd.n>=0&&!fstat(fd.n,&st)&&S_ISREG(st.st_mode)&&st.st_nlink==1&&(!executable||(st.st_mode&0111)),"regular_asset");return fd;}
static std::vector<u8> bytes(int fd,size_t maximum){struct stat st;require(!fstat(fd,&st)&&st.st_size>=0&&uint64_t(st.st_size)<=maximum,"asset_size");
 require(lseek(fd,0,SEEK_SET)==0,"asset_seek");std::vector<u8> v(st.st_size);size_t n=0;while(n<v.size()){active();auto got=read(fd,v.data()+n,v.size()-n);if(got<0&&errno==EINTR)continue;require(got>0,"asset_read");n+=got;}return v;}
static std::vector<u8> asset(int dir,const std::string& name,size_t maximum){auto f=regular(dir,name);return bytes(f.n,maximum);}
static void syncdir(int dir){require(!fsync(dir),"directory_sync");}
static void writeall(int fd,const std::vector<u8>& v){size_t pos=0;while(pos<v.size()){active();auto n=write(fd,v.data()+pos,v.size()-pos);if(n<0&&errno==EINTR)continue;require(n>0,"asset_write");pos+=n;}}
static void fresh(int dir,const std::string& name,const std::vector<u8>& v){Fd f(openat(dir,name.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0600));require(f.n>=0,"exclusive_asset_create");writeall(f.n,v);require(!fsync(f.n),"asset_sync");}
static void atomic(int dir,const char* name,const std::vector<u8>& v){std::string tmp=std::string(name)+".next";fresh(dir,tmp,v);require(!renameat(dir,tmp.c_str(),dir,name),"state_rename");syncdir(dir);}
static std::vector<u8> textbytes(const std::string& s){return {s.begin(),s.end()};}
static void state(int dir,const char* s){atomic(dir,"state",textbytes(std::string(s)+"\n"));}
static std::string state(int dir){auto v=asset(dir,"state",64);return {v.begin(),v.end()};}
struct Look{std::string id,label;std::array<u8,32> hash;unsigned size;};
struct Catalog {
 std::vector<Look> looks;std::vector<Lens> lenses;std::vector<u8> raw;std::array<u8,32> hash;std::array<u8,32> jpeg,hif;
 explicit Catalog(int root){raw=asset(root,"catalog.bin",128+32*256+100*512);require(raw.size()>=128&&!memcmp(raw.data(),"A7CAT001",8)&&getnum(raw.data()+8,4)==1,"catalog_header");
  unsigned nl=getnum(raw.data()+12,4),nn=getnum(raw.data()+16,4);require(nl>=1&&nl<=32&&nn>=1&&nn<=100&&getnum(raw.data()+20,4)==nl*256+nn*512&&raw.size()==128+nl*256+nn*512,"catalog_bounds");
  auto payload=std::vector<u8>(raw.begin()+128,raw.end());auto sum=digest(payload);require(!memcmp(sum.data(),raw.data()+88,32),"catalog_digest");for(unsigned i=120;i<128;i++)require(raw[i]==0,"catalog_reserved");
  std::copy(raw.begin()+24,raw.begin()+56,jpeg.begin());std::copy(raw.begin()+56,raw.begin()+88,hif.begin());hash=digest(raw);
  for(unsigned i=0;i<nl;i++){auto p=raw.data()+128+256*i;Look l{field(p,64),field(p+64,128),{},getnum(p+224,4)};require(identifier(l.id),"look_id");
   std::copy(p+192,p+224,l.hash.begin());for(unsigned j=228;j<256;j++)require(p[j]==0,"look_reserved");require((l.id=="none"&&l.size==0)||(l.id!="none"&&l.size==426152),"look_size");
   for(auto& old:looks)require(old.id!=l.id,"duplicate_look_id");looks.push_back(l);}
  for(unsigned i=0;i<nn;i++){auto p=raw.data()+128+nl*256+512*i;Lens l{field(p,64),field(p+64,128),field(p+192,128,true),field(p+320,128,true),real64(p+448),real64(p+456)};
   require(identifier(l.id),"lens_id");for(unsigned j=464;j<512;j++)require(p[j]==0,"lens_reserved");
   require((l.id=="none"&&l.make.empty()&&l.model.empty()&&l.focal==0&&l.maximum==0)||(l.id!="none"&&!l.make.empty()&&!l.model.empty()&&std::isfinite(l.focal)&&l.focal>=1&&l.focal<=5000&&std::isfinite(l.maximum)&&l.maximum>=.5&&l.maximum<=128),"lens_bounds");
   for(auto& old:lenses)require(old.id!=l.id,"duplicate_lens_id");lenses.push_back(l);}
  require(looks[0].id=="none"&&lenses[0].id=="none","catalog_identity_slots");
 }
};
struct Selection{unsigned look,lens;double aperture;uint64_t generation;std::vector<u8> raw;};
static Selection selection(int dir,const Catalog& c){auto v=asset(dir,"selection.bin",128);require(v.size()==128&&!memcmp(v.data(),"A7SEL001",8)&&getnum(v.data()+8,4)==1,"selection_header");auto sum=digest({v.begin(),v.begin()+96});
 require(!memcmp(sum.data(),v.data()+96,32)&&!memcmp(c.hash.data(),v.data()+40,32),"selection_binding");Selection s{getnum(v.data()+12,4),getnum(v.data()+16,4),real64(v.data()+24),num64(v.data()+32),v};
 require(!getnum(v.data()+20,4)&&s.look<c.looks.size()&&s.lens<c.lenses.size()&&std::isfinite(s.aperture),"selection_bounds");for(unsigned i=72;i<96;i++)require(!v[i],"selection_reserved");
 require(s.aperture==0||(s.lens&&s.aperture>=c.lenses[s.lens].maximum&&s.aperture<=128),"aperture_bounds");return s;}
static void select(int root,const Catalog& c,const char* look,const char* lens,const char* aperture){auto old=selection(root,c);Selection s{};bool a=false,b=false;
 for(unsigned i=0;i<c.looks.size();i++)if(c.looks[i].id==look){s.look=i;a=true;}for(unsigned i=0;i<c.lenses.size();i++)if(c.lenses[i].id==lens){s.lens=i;b=true;}require(a&&b,"unknown_preset_id");
 s.aperture=0;if(strcmp(aperture,"unknown")){char* end=nullptr;s.aperture=strtod(aperture,&end);require(end&&*end==0&&end!=aperture&&s.lens&&std::isfinite(s.aperture)&&s.aperture>=c.lenses[s.lens].maximum&&s.aperture<=128,"invalid_actual_aperture");}
 require(old.generation<UINT64_MAX,"selection_generation_overflow");std::vector<u8> v(128);memcpy(v.data(),"A7SEL001",8);putnum(v,8,1);putnum(v,12,s.look);putnum(v,16,s.lens);memcpy(v.data()+24,&s.aperture,8);set64(v,32,old.generation+1);std::copy(c.hash.begin(),c.hash.end(),v.begin()+40);auto sum=digest({v.begin(),v.begin()+96});std::copy(sum.begin(),sum.end(),v.begin()+96);atomic(root,"selection.bin",v);printf("selected generation=%llu look=%s lens=%s aperture=%s\n",(unsigned long long)(old.generation+1),look,lens,aperture);}
static Fd source_file(int base,const std::string& relative){require(!relative.empty()&&relative.size()<=1024&&relative[0]!='/',"relative_capture_path");Fd dir(dup(base));size_t pos=0;
 while(true){size_t end=relative.find('/',pos);std::string part=relative.substr(pos,end==std::string::npos?end:end-pos);require(!part.empty()&&part!="."&&part!=".."&&part.size()<=255,"capture_component");
  if(end==std::string::npos)return regular(dir.n,part);dir=dirat(dir.n,part);pos=end+1;}}
static bool unchanged(const struct stat& a,const struct stat& b){
#ifdef __APPLE__
 return a.st_dev==b.st_dev&&a.st_ino==b.st_ino&&a.st_size==b.st_size&&a.st_mtimespec.tv_sec==b.st_mtimespec.tv_sec&&a.st_mtimespec.tv_nsec==b.st_mtimespec.tv_nsec&&a.st_ctimespec.tv_sec==b.st_ctimespec.tv_sec&&a.st_ctimespec.tv_nsec==b.st_ctimespec.tv_nsec;
#else
 return a.st_dev==b.st_dev&&a.st_ino==b.st_ino&&a.st_size==b.st_size&&a.st_mtim.tv_sec==b.st_mtim.tv_sec&&a.st_mtim.tv_nsec==b.st_mtim.tv_nsec&&a.st_ctim.tv_sec==b.st_ctim.tv_sec&&a.st_ctim.tv_nsec==b.st_ctim.tv_nsec;
#endif
}
static std::vector<u8> look_asset(int root,const Catalog& c,const Selection& s){auto& l=c.looks[s.look];if(!l.size){require(digest({})==l.hash,"identity_hash");return {};}
 auto dir=dirat(root,"looks");auto v=asset(dir.n,l.id+".bin",426152);require(v.size()==l.size&&digest(v)==l.hash,"selected_LUT_hash");require(!memcmp(v.data(),"A7LUT001",8)&&getnum(v.data()+8,4)==1&&!getnum(v.data()+12,4),"selected_LUT_header");
 for(size_t i=16;i<v.size();i+=4){float x;memcpy(&x,v.data()+i,4);require(std::isfinite(x)&&x>=(i<160?-16:0)&&x<=(i<160?16:1),"selected_LUT_numeric");}return v;}
static Fd open_job(int root,const char* id){require(identifier(id),"event_id");auto jobs=dirat(root,"jobs");auto job=dirat(jobs.n,id);private_directory(job.n);return job;}
static void space(int dir,uint64_t needed){struct statvfs v;require(!fstatvfs(dir,&v)&&v.f_frsize&&v.f_bavail>=needed/v.f_frsize+(needed%v.f_frsize!=0),"insufficient_workspace_space");}
static void arm(int root,const Catalog& c,const char* id){
 require(identifier(id),"event_id");auto selected=selection(root,c);(void)look_asset(root,c,selected);space(root,16ULL<<20);auto jobs=dirat(root,"jobs");
 require(!mkdirat(jobs.n,id,0700),"event_already_exists_or_unavailable");syncdir(jobs.n);auto job=dirat(jobs.n,id);
 fresh(job.n,"selection.bin",selected.raw);fresh(job.n,"catalog.sha256",textbytes(hex(c.hash.data())+"\n"));state(job.n,"armed");
 printf("armed event=%s generation=%llu look=%s lens=%s\n",id,(unsigned long long)selected.generation,c.looks[selected.look].id.c_str(),c.lenses[selected.lens].id.c_str());
}
static void enqueue(int root,const Catalog& c,const char* capture_root,const std::string& relative,const char* id,const char* expected){require(identifier(id),"event_id");require(strlen(expected)==64,"expected_source_SHA256");for(size_t i=0;i<64;i++)require((expected[i]>='0'&&expected[i]<='9')||(expected[i]>='a'&&expected[i]<='f'),"expected_source_SHA256");std::string ext;auto dot=relative.rfind('.');if(dot!=std::string::npos)ext=relative.substr(dot);for(auto& x:ext)x=std::tolower(static_cast<unsigned char>(x));
 bool hif=ext==".hif"||ext==".heif"||ext==".heic";require(hif||ext==".jpg"||ext==".jpeg","photo_extension");auto captures=directory(capture_root);auto input=source_file(captures.n,relative);struct stat initial,after;require(!fstat(input.n,&initial)&&initial.st_size>0&&initial.st_size<=64*1024*1024,"capture_size");
 space(root,uint64_t(initial.st_size)+(144ULL<<20));auto job=open_job(root,id);require(state(job.n)=="armed\n","completion_requires_unique_armed_event");auto selected=selection(job.n,c);auto look=look_asset(root,c,selected);state(job.n,"preparing");
 try{
  fresh(job.n,"format",textbytes(hif?"HIF\n":"JPEG\n"));
  const char* input_name=hif?"input.HIF":"input.jpg";Fd copy(openat(job.n,input_name,O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0600));require(copy.n>=0,"snapshot_create");
  SHA256 hash;sha_init(&hash);std::vector<u8> block(65536);uint64_t remain=initial.st_size;
  while(remain){active();size_t want=std::min<uint64_t>(block.size(),remain);auto got=read(input.n,block.data(),want);if(got<0&&errno==EINTR)continue;require(got>0,"snapshot_read");std::vector<u8> piece(block.begin(),block.begin()+got);writeall(copy.n,piece);sha_add(&hash,piece.data(),piece.size());remain-=got;}
  std::array<u8,32> input_hash{};sha_end(&hash,input_hash.data());require(hex(input_hash.data())==expected,"completed_source_identity_mismatch");require(!fstat(input.n,&after)&&unchanged(initial,after),"source_changed_during_snapshot");require(!fsync(copy.n),"snapshot_sync");
  auto verify=bytes(input.n,64*1024*1024);require(digest(verify)==input_hash,"source_changed_after_snapshot");verify.clear();verify.shrink_to_fit();require(!fstat(input.n,&after)&&unchanged(initial,after),"source_changed_after_hash");
  auto photo=bytes(copy.n,64*1024*1024);require(digest(photo)==input_hash,"snapshot_hash");unsigned width,height,orientation=1;std::vector<u8> source_exif;
  if(hif){Source source(std::move(photo));width=source.width;height=source.height;source_exif=source.exif();}
  else{auto info=jpeg_info(photo);width=info.width;height=info.height;orientation=info.orientation;source_exif=std::move(info.tiff);}
  auto exif=make_exif(source_exif,width,height,orientation,c.lenses[selected.lens],selected.aperture);fresh(job.n,"metadata.exif",exif);if(!look.empty())fresh(job.n,"look.bin",look);
  std::vector<u8> head(176);memcpy(head.data(),hif?"A7HIFS01":"A7JOBS01",8);putnum(head,8,1);putnum(head,12,!look.empty());putnum(head,16,95);putnum(head,20,width);putnum(head,24,height);putnum(head,28,exif.size());putnum(head,32,look.size());set64(head,40,initial.st_size);
  std::copy(input_hash.begin(),input_hash.end(),head.begin()+48);auto sum=digest(exif);std::copy(sum.begin(),sum.end(),head.begin()+80);sum=digest(look);std::copy(sum.begin(),sum.end(),head.begin()+112);sum=digest({head.begin(),head.begin()+144});std::copy(sum.begin(),sum.end(),head.begin()+144);
  std::string receipt="A7COMPLETED1\nevent="+std::string(id)+"\nsource_sha256="+hex(input_hash.data())+"\nselection_sha256="+hex(digest(selected.raw).data())+"\ngeneration="+std::to_string(selected.generation)+"\ncompletion=explicit_closed_srgb_contract\nselection_time=explicit_arm_event\n";
  fresh(job.n,"completion.txt",textbytes(receipt));
  std::vector<u8> binding(160);memcpy(binding.data(),"A7READY1",8);auto bound=digest(selected.raw);std::copy(bound.begin(),bound.end(),binding.begin()+8);bound=digest(head);std::copy(bound.begin(),bound.end(),binding.begin()+40);std::copy(c.hash.begin(),c.hash.end(),binding.begin()+72);bound=digest({binding.begin(),binding.begin()+128});std::copy(bound.begin(),bound.end(),binding.begin()+128);fresh(job.n,"binding.bin",binding);
  fresh(job.n,"job.bin",head);syncdir(job.n);state(job.n,"ready");printf("ready event=%s generation=%llu look=%s lens=%s\n",id,(unsigned long long)selected.generation,c.looks[selected.look].id.c_str(),c.lenses[selected.lens].id.c_str());
 }catch(...){try{state(job.n,"failed");}catch(...){}throw;}
}
static void run(int root,int control,const Catalog& c,const char* id){auto lease=regular(root,"worker.lock");require(!flock(lease.n,LOCK_EX|LOCK_NB),"worker_busy");auto job=open_job(root,id);require(state(job.n)=="ready\n","job_not_ready_no_automatic_retry");auto selected=selection(job.n,c);auto format=asset(job.n,"format",16);bool hif=format==textbytes("HIF\n");require(hif||format==textbytes("JPEG\n"),"job_format");
 auto dir=dirat(root,"bin");const char* name=hif?"hif-worker":"jpeg-worker";auto exe=regular(dir.n,name,true);auto executable=bytes(exe.n,16*1024*1024);require(digest(executable)==(hif?c.hif:c.jpeg),"worker_hash");
 const char* result=hif?"result.HIF":"result.jpg";struct stat st;require(fstatat(job.n,result,&st,AT_SYMLINK_NOFOLLOW)<0&&errno==ENOENT,"result_already_present_needs_review");
 // Freeze the job's selected LUT even if global selection has changed.
 auto wanted=look_asset(root,c,selected);auto header=asset(job.n,"job.bin",176);require(header.size()==176,"ready_header");auto binding=asset(job.n,"binding.bin",160);require(binding.size()==160&&!memcmp(binding.data(),"A7READY1",8),"completion_binding");for(size_t i=104;i<128;i++)require(!binding[i],"completion_reserved");auto bound=digest({binding.begin(),binding.begin()+128});require(!memcmp(bound.data(),binding.data()+128,32),"completion_binding_hash");bound=digest(selected.raw);require(!memcmp(bound.data(),binding.data()+8,32),"completion_selection_binding");bound=digest(header);require(!memcmp(bound.data(),binding.data()+40,32)&&!memcmp(c.hash.data(),binding.data()+72,32),"completion_job_binding");
 auto lookhash=digest(wanted);require(!memcmp(header.data()+112,lookhash.data(),32),"job_selection_LUT_binding");
 space(job.n,144ULL<<20);state(job.n,"running");sigset_t blocked,previous;sigemptyset(&blocked);sigaddset(&blocked,SIGINT);sigaddset(&blocked,SIGTERM);require(!sigprocmask(SIG_BLOCK,&blocked,&previous),"signal_mask_needs_review");pid_t child=fork();if(child<0){sigprocmask(SIG_SETMASK,&previous,nullptr);state(job.n,"needs-review");throw std::runtime_error("fork_failed_no_retry");}
 if(child==0){
  // The worker lease survives exec and parent death; other descriptors use CLOEXEC.
  struct sigaction defaults{};defaults.sa_handler=SIG_DFL;sigemptyset(&defaults.sa_mask);sigaction(SIGINT,&defaults,nullptr);sigaction(SIGTERM,&defaults,nullptr);sigprocmask(SIG_SETMASK,&previous,nullptr);
  if(stopping||fcntl(lease.n,F_SETFD,0)<0||fchdir(job.n))_exit(125);char* args[]={const_cast<char*>(name),const_cast<char*>("--run"),const_cast<char*>("."),nullptr};char* env[]={const_cast<char*>("LC_ALL=C"),nullptr};
#ifdef __linux__
  fexecve(exe.n,args,env);
#else
  char path[4096];if(fcntl(exe.n,F_GETPATH,path)==0)execve(path,args,env);
#endif
  _exit(126);
 }
 sigprocmask(SIG_SETMASK,&previous,nullptr);require(!flock(control,LOCK_UN),"control_unlock");
 int status=0;bool cancelled=false;
 while(true){if(stopping&&!cancelled){kill(child,SIGTERM);cancelled=true;}pid_t rc=waitpid(child,&status,WNOHANG);if(rc==child)break;
  if(rc<0&&errno==EINTR)continue;if(rc<0)throw std::runtime_error("wait_failed_no_retry");timespec delay{0,50000000};nanosleep(&delay,nullptr);}
 while(flock(control,LOCK_EX)){if(errno==EINTR)continue;throw std::runtime_error("control_relock_needs_review");}
 if(stopping||cancelled||!WIFEXITED(status)||WEXITSTATUS(status)!=0){state(job.n,"needs-review");throw std::runtime_error("worker_failed_or_cancelled_no_retry");}
 try{auto f=regular(job.n,result);auto data=bytes(f.n,128*1024*1024);auto sum=digest(data);fresh(job.n,"result.sha256",textbytes(hex(sum.data())+"\n"));state(job.n,"done");printf("done event=%s result=%s sha256=%s\n",id,result,hex(sum.data()).c_str());}
 catch(...){try{state(job.n,"needs-review");}catch(...){}throw;}
}
int main(int argc,char**argv){if(argc==2&&!strcmp(argv[1],"--version")){puts("A7C2 Photo Launcher 0.1.0; explicit completion; offline-tested candidate; hardware untested");return 0;}
 bool syntax=(argc==3&&!strcmp(argv[1],"--list"))||(argc==6&&!strcmp(argv[1],"--select"))||(argc==7&&!strcmp(argv[1],"--enqueue-completed-srgb"))||(argc==4&&(!strcmp(argv[1],"--run")||!strcmp(argv[1],"--status")||!strcmp(argv[1],"--arm")));
 if(!syntax){fputs("Usage: photo-launcher --list ROOT | --select ROOT LOOK LENS APERTURE_OR_unknown | --enqueue-completed-srgb ROOT CAPTURE_ROOT RELATIVE_PHOTO EVENT_ID EXPECTED_SHA256 | --arm|--run|--status ROOT EVENT_ID\n",stderr);return 2;}
 struct sigaction action{};action.sa_handler=stop_launcher;sigemptyset(&action.sa_mask);sigaction(SIGINT,&action,nullptr);sigaction(SIGTERM,&action,nullptr);
#ifdef __linux__
 rlimit mem{512ULL<<20,512ULL<<20},cpu{1800,1800},file{128ULL<<20,128ULL<<20};if(setrlimit(RLIMIT_AS,&mem)||setrlimit(RLIMIT_CPU,&cpu)||setrlimit(RLIMIT_FSIZE,&file)){fputs("resource_limits\n",stderr);return 1;}
#endif
 try{auto root=directory(argv[2]);private_directory(root.n);auto lock=regular(root.n,"lock");require(!flock(lock.n,LOCK_EX|LOCK_NB),"launcher_busy");Catalog c(root.n);
  if(!strcmp(argv[1],"--list")){auto s=selection(root.n,c);for(auto& l:c.looks)printf("look %s %s\n",l.id.c_str(),l.label.c_str());for(auto& l:c.lenses)printf("lens %s %s\n",l.id.c_str(),l.label.c_str());printf("selected generation=%llu look=%s lens=%s aperture=%.6g\n",(unsigned long long)s.generation,c.looks[s.look].id.c_str(),c.lenses[s.lens].id.c_str(),s.aperture);}
  else if(!strcmp(argv[1],"--select"))select(root.n,c,argv[3],argv[4],argv[5]);
  else if(!strcmp(argv[1],"--enqueue-completed-srgb"))enqueue(root.n,c,argv[3],argv[4],argv[5],argv[6]);
  else if(!strcmp(argv[1],"--arm"))arm(root.n,c,argv[3]);
  else if(!strcmp(argv[1],"--run"))run(root.n,lock.n,c,argv[3]);else{auto job=open_job(root.n,argv[3]);auto s=state(job.n);printf("%s",s.c_str());}
 }catch(const std::exception& e){fprintf(stderr,"photo-launcher: %s\n",e.what());return 1;}return 0;}
