// Host-only harness for the original SDK RefRGBtoRGBTable3D function.
// The generated include is extracted verbatim from a pinned, user-supplied SDK.
// Adobe algorithm attribution and license: NOTICE.md, licenses/Adobe-DNG-SDK.txt.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>
using real32 = float;
using real64 = double;
using uint16 = uint16_t;
using uint32 = uint32_t;
using int32 = int32_t;
float Pin_real32(float x) { return std::clamp(x, 0.f, 1.f); }
int32 Pin_int32(int32 a, int32 x, int32 b) { return std::clamp(x, a, b); }
float Lerp_real32(float a, float b, float t) { return a*(1.f-t)+b*t; }
float EncodeOverrange(float) { throw std::runtime_error("Overrange not in tested contract"); }
float DecodeOverrange(float) { throw std::runtime_error("Overrange not in tested contract"); }
double enc(double x) { return x<=.0031308 ? x*12.92 : 1.055*std::pow(x,1./2.4)-.055; }
double dec(double x) { return x<=.0031308*12.92 ? x/12.92 : std::pow((x+.055)/1.055,2.4); }
struct dng_matrix {
    std::array<std::array<double,3>,3> m;
    const auto &operator[](int i) const { return m[i]; }
    auto &operator[](int i) { return m[i]; }
};
struct dng_1d_table {
    float data[4098];
    explicit dng_1d_table(bool inverse) {
        for(int i=0;i<=4096;++i) data[i]=float(inverse ? dec(double(i)/4096) : enc(double(i)/4096));
        data[4097]=data[4096];
    }
    float Interpolate(float x) const {
        if (!(x>=0.f && x<=1.f)) throw std::runtime_error("Invalid gamma input");
        float y=x*4096.f; int i=int(y); float t=y-float(i);
        return data[i]*(1.f-t)+data[i+1]*t;
    }
};
#include "adobe_ref.inc"

dng_matrix normalized(dng_matrix a) {
    double white[3]={.3457/.3585,1.,(1.-.3457-.3585)/.3585};
    for(int r=0;r<3;++r) {
        double sum=a[r][0]+a[r][1]+a[r][2];
        for(int c=0;c<3;++c) a[r][c]*=white[r]/sum;
    }
    return a;
}
dng_matrix inverse(dng_matrix a) {
    // Independent Gauss-Jordan inversion (Python converter uses cofactors).
    dng_matrix out{{{{1,0,0},{0,1,0},{0,0,1}}}};
    for(int col=0;col<3;++col) {
        int pivot=col;
        for(int r=col+1;r<3;++r) if(std::abs(a[r][col])>std::abs(a[pivot][col])) pivot=r;
        std::swap(a[col],a[pivot]); std::swap(out[col],out[pivot]);
        double v=a[col][col];
        for(int c=0;c<3;++c) { a[col][c]/=v; out[col][c]/=v; }
        for(int r=0;r<3;++r) if(r!=col) {
            double f=a[r][col];
            for(int c=0;c<3;++c) { a[r][c]-=f*a[col][c]; out[r][c]-=f*out[col][c]; }
        }
    }
    return out;
}
dng_matrix product(dng_matrix a,dng_matrix b) {
    dng_matrix out{};
    for(int r=0;r<3;++r) for(int c=0;c<3;++c) for(int k=0;k<3;++k) out[r][c]+=a[r][k]*b[k][c];
    return out;
}
std::array<float,3> map(dng_matrix a,std::array<float,3> p) {
    std::array<float,3> out{};
    for(int r=0;r<3;++r) out[r]=p[0]*float(a[r][0])+p[1]*float(a[r][1])+p[2]*float(a[r][2]);
    return out;
}
uint32 word(const std::vector<unsigned char> &v,size_t p) {
    return uint32(v.at(p))|(uint32(v.at(p+1))<<8)|(uint32(v.at(p+2))<<16)|(uint32(v.at(p+3))<<24);
}
int main(int argc,char **argv) {
    if(argc!=4) return 2;
    try {
        std::ifstream f(argv[1],std::ios::binary);
        std::vector<unsigned char> raw((std::istreambuf_iterator<char>(f)),{});
        if(raw.size()!=196652 || word(raw,0)!=1 || word(raw,4)!=1 || word(raw,8)!=3 || word(raw,12)!=32 ||
           word(raw,196624)!=3 || word(raw,196628)!=1 || word(raw,196632)!=0) return 3;
        std::vector<uint16> samples(32*32*32*4,0);
        size_t pos=16;
        for(int r=0;r<32;++r) for(int g=0;g<32;++g) for(int b=0;b<32;++b) {
            int coord[3]={r,g,b};
            for(int c=0;c<3;++c) {
                unsigned v=raw.at(pos)|(unsigned(raw.at(pos+1))<<8); pos+=2;
                samples[((r*32+g)*32+b)*4+c]=uint16(v+(coord[c]*65535+16)/31);
            }
        }
        auto srgb=normalized({{{{.4361,.3851,.1431},{.2225,.7169,.0606},{.0139,.0971,.7141}}}});
        auto p3=normalized({{{{.5151,.2920,.1571},{.2412,.6922,.0666},{-.0010,.0419,.7843}}}});
        auto pro=normalized({{{{.7977,.1352,.0313},{.2880,.7119,.0001},{0,0,.8249}}}});
        auto in=product(inverse(pro),srgb), out=product(inverse(srgb),pro);
        auto em=product(inverse(p3),pro), dm=product(inverse(pro),p3);
        dng_1d_table eg(false),dg(true);
        std::ifstream input(argv[2],std::ios::binary);
        std::ofstream output(argv[3],std::ios::binary|std::ios::trunc);
        std::array<float,3> rgb;
        while(input.read(reinterpret_cast<char*>(rgb.data()),12)) {
            for(auto &x:rgb) { if(!std::isfinite(x)||x<0||x>1) return 4; x=float(dec(x)); }
            rgb=map(in,rgb);
            RefRGBtoRGBTable3D(&rgb[0],&rgb[1],&rgb[2],nullptr,1,1,1,0,32,samples.data(),1.f,0,&em,&dm,&eg,&dg,false);
            rgb=map(out,rgb);
            for(auto &x:rgb) x=float(enc(Pin_real32(x)));
            output.write(reinterpret_cast<char*>(rgb.data()),12);
        }
        if(!output || input.gcount()!=0) return 5;
    } catch(...) { return 6; }
    return 0;
}
