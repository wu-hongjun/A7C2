/* Fresh TIFF construction for native completed-photo ingestion. Source offsets,
 * GPS, MakerNote and thumbnails are never copied as opaque EXIF blocks. */
#pragma once
#include <map>
#include <array>
#include <set>
struct ExifValue {uint16_t type;uint32_t count;std::vector<uint8_t> bytes;};
using IFD=std::map<uint16_t,ExifValue>;
static uint32_t getnum(const uint8_t* p,int n,bool big=false){uint32_t v=0;for(int i=0;i<n;i++)v|=uint32_t(p[i])<<(8*(big?n-i-1:i));return v;}
static void putnum(std::vector<uint8_t>& v,size_t pos,uint32_t n,int bytes=4){require(pos+bytes<=v.size(),"TIFF_write_bounds");for(int i=0;i<bytes;i++)v[pos+i]=n>>(8*i);}
static ExifValue integer(uint32_t n,int type=4){ExifValue v{uint16_t(type),1,std::vector<uint8_t>(type==3?2:4)};putnum(v.bytes,0,n,type==3?2:4);return v;}
static ExifValue ascii(const std::string& s){ExifValue v{2,uint32_t(s.size()+1),{s.begin(),s.end()}};v.bytes.push_back(0);return v;}
static ExifValue rationals(const std::vector<double>& values){ExifValue v{5,uint32_t(values.size()),std::vector<uint8_t>(values.size()*8)};
 for(size_t i=0;i<values.size();i++){require(std::isfinite(values[i])&&values[i]>=0&&values[i]<=5000,"EXIF_rational");putnum(v.bytes,i*8,uint32_t(std::floor(values[i]*100000+.5)));putnum(v.bytes,i*8+4,100000);}return v;}
static bool contains(const std::vector<uint16_t>& tags,uint16_t t){return std::find(tags.begin(),tags.end(),t)!=tags.end();}
static std::array<IFD,2> read_exif(const std::vector<uint8_t>& data){
 std::array<IFD,2> out;if(data.empty())return out;require(data.size()>=8&&data.size()<=1024*1024,"source_EXIF_bounds");
 auto p=data.data();bool big=p[0]=='M';require((big?memcmp(p,"MM",2):memcmp(p,"II",2))==0&&getnum(p+2,2,big)==42,"source_TIFF_header");
 uint32_t off=getnum(p+4,4,big),sub=0;std::vector<uint16_t> roots{0x010f,0x0110,0x0132,0x013b,0x8298,0x0112,0x8769};
 std::vector<uint16_t> photos{0x829a,0x829d,0x8822,0x8827,0x8830,0x8832,0x9003,0x9004,0x9010,0x9011,0x9012,0x9201,0x9202,0x9204,0x9205,0x9207,0x9208,0x9209,0x920a,0x9290,0x9291,0x9292,0xa402,0xa403,0xa405,0xa432,0xa433,0xa434,0xa001};
 for(int level=0;level<2;level++){
  if(level&&sub==0)break;if(level)off=sub;
  require(off>=8&&off<=data.size()-2,"source_IFD_offset");unsigned count=getnum(p+off,2,big);require(count<=512&&off+2+12ULL*count+4<=data.size(),"source_IFD_bounds");
  std::set<uint16_t> seen;
  for(unsigned i=0;i<count;i++){
   auto e=p+off+2+12*i;uint16_t tag=getnum(e,2,big),type=getnum(e+2,2,big);if(!contains(level?photos:roots,tag))continue;
   require(seen.insert(tag).second,"duplicate_source_EXIF");uint32_t n=getnum(e+4,4,big);unsigned unit=(type==1||type==2||type==7)?1:type==3?2:(type==4||type==9)?4:(type==5||type==10)?8:0;
   require(unit&&n&&n<=4096,"source_EXIF_type_or_count");uint64_t size=uint64_t(unit)*n;unsigned where=size<=4?unsigned(e+8-p):getnum(e+8,4,big);
   require(where<=data.size()&&size<=data.size()-where,"source_EXIF_value");ExifValue v{type,n,{p+where,p+where+size}};
   if(type==2)require(v.bytes.back()==0,"source_EXIF_ASCII");
   // Normalize numeric units to little endian, including the rational halves.
   unsigned atom=(type==5||type==10)?4:unit;
   if(big&&atom>1)for(size_t j=0;j<size;j+=atom)std::reverse(v.bytes.begin()+j,v.bytes.begin()+j+atom);
   if(type==5||type==10)for(unsigned j=0;j<n;j++)require(getnum(v.bytes.data()+j*8+4,4)!=0,"source_EXIF_denominator");
   if(tag==0x8769){require(!level&&type==4&&n==1,"source_subIFD");sub=getnum(v.bytes.data(),4);continue;}
   out[level][tag]=std::move(v);
  }
 }
 return out;
}
struct Lens {std::string id,label,make,model;double focal=0,maximum=0;};
static std::vector<uint8_t> make_exif(const std::vector<uint8_t>& source,unsigned width,unsigned height,unsigned orientation,const Lens& lens,double aperture){
 auto fields=read_exif(source);auto& root=fields[0];auto& photo=fields[1];
 if(photo.count(0xa001)){auto v=photo[0xa001];require(v.type==3&&v.count==1&&getnum(v.bytes.data(),2)==1,"source_not_sRGB");}
 root[0x0112]=integer(orientation,3);root[0x0131]=ascii("A7C2 Photo Launcher 0.1.0");
 photo[0xa001]=integer(1,3);photo[0xa002]=integer(width);photo[0xa003]=integer(height);
 if(lens.id!="none"){
  for(auto t:{0x829d,0x9202,0x9205,0x920a,0xa405,0xa432,0xa433,0xa434})photo.erase(t);
  photo[0x920a]=rationals({lens.focal});photo[0xa432]=rationals({lens.focal,lens.focal,lens.maximum,lens.maximum});
  photo[0xa433]=ascii(lens.make);photo[0xa434]=ascii(lens.model);
  if(lens.maximum>=1)photo[0x9205]=rationals({2*std::log2(lens.maximum)});
  if(aperture){photo[0x829d]=rationals({aperture});if(aperture>=1)photo[0x9202]=rationals({2*std::log2(aperture)});}
 }
 require(orientation>=1&&orientation<=8,"output_orientation");
 root[0x8769]=integer(0);size_t sub=8+2+12*root.size()+4,total=sub+2+12*photo.size()+4;
 require(total<=65527,"output_EXIF_size");size_t used=total;std::vector<uint8_t> tiff(65527);tiff[0]=tiff[1]='I';putnum(tiff,2,42,2);putnum(tiff,4,8);root[0x8769]=integer(sub);
 size_t base=8;
 for(auto* ifd:{&root,&photo}){putnum(tiff,base,ifd->size(),2);size_t i=0;
  for(auto& [tag,v]:*ifd){size_t entry=base+2+12*i++;putnum(tiff,entry,tag,2);putnum(tiff,entry+2,v.type,2);putnum(tiff,entry+4,v.count);
   if(v.bytes.size()<=4)std::copy(v.bytes.begin(),v.bytes.end(),tiff.begin()+entry+8);
   else{if(used%2)used++;require(used<=tiff.size()&&v.bytes.size()<=tiff.size()-used,"output_EXIF_size");putnum(tiff,entry+8,used);memcpy(tiff.data()+used,v.bytes.data(),v.bytes.size());used+=v.bytes.size();}}
  base=sub;
 }
 std::vector<uint8_t> exif(used+6);memcpy(exif.data(),"Exif\0\0",6);memcpy(exif.data()+6,tiff.data(),used);return exif;
}
struct JpegInfo {unsigned width=0,height=0,orientation=1;std::vector<uint8_t> tiff;};
static JpegInfo jpeg_info(const std::vector<uint8_t>& v){JpegInfo out;require(v.size()>4&&v[0]==255&&v[1]==216,"JPEG_SOI");size_t pos=2;bool sof=false,sos=false;
 while(pos<v.size()){
  require(v[pos++]==255,"JPEG_marker");while(pos<v.size()&&v[pos]==255)pos++;require(pos<v.size(),"JPEG_marker_end");unsigned m=v[pos++];
  require(m!=0&&m!=216&&m!=217&&!(m>=0xd0&&m<=0xd7),"JPEG_header_marker");require(pos+2<=v.size(),"JPEG_length");unsigned n=(v[pos]<<8)|v[pos+1];require(n>=2&&pos+n<=v.size(),"JPEG_segment");auto p=v.data()+pos+2;unsigned size=n-2;
  if(m==0xe1&&size>=6&&!memcmp(p,"Exif\0\0",6)){require(out.tiff.empty(),"duplicate_JPEG_EXIF");out.tiff.assign(p+6,p+size);}
  if(m==0xe2&&size>=12&&!memcmp(p,"ICC_PROFILE\0",12))throw std::runtime_error("JPEG_ICC_requires_host_conversion");
  if(m>=0xc0&&m<=0xcf&&m!=0xc4&&m!=0xc8&&m!=0xcc){require(m==0xc0&&!sof&&size==15&&p[0]==8&&p[5]==3,"requires_baseline_RGB8_JPEG");sof=true;out.height=(p[1]<<8)|p[2];out.width=(p[3]<<8)|p[4];}
  pos+=n;if(m==0xda){sos=true;break;}
 }
 require(sof&&sos&&out.width&&out.height&&out.width<=8192&&out.height<=8192&&uint64_t(out.width)*out.height<=40000000,"JPEG_dimensions_or_scan");
 auto fields=read_exif(out.tiff);if(fields[0].count(0x0112)){auto x=fields[0][0x0112];require(x.type==3&&x.count==1,"source_orientation_type");out.orientation=getnum(x.bytes.data(),2);require(out.orientation>=1&&out.orientation<=8,"source_orientation");}
 return out;
}
