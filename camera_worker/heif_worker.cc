/* Completed HIF worker. Offline candidate: no camera transport or installation.
 * Combined executable GPL-3.0-or-later; see NOTICE.md and codec sources.
 */
#include <libheif/heif.h>
#include <libheif/heif_properties.h>
#include "heif_core.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

static void require(bool b,const char* why){if(!b)throw std::runtime_error(why);}
static void check(heif_error e){if(e.code)throw std::runtime_error(std::string("codec: ")+e.message);}
#define OWNER(name,type,release) using name=std::unique_ptr<type,decltype(&release)>
OWNER(Context,heif_context,heif_context_free);
OWNER(Handle,heif_image_handle,heif_image_handle_release);
OWNER(Image,heif_image,heif_image_release);
OWNER(Nclx,heif_color_profile_nclx,heif_nclx_color_profile_free);
OWNER(Decode,heif_decoding_options,heif_decoding_options_free);
OWNER(Encode,heif_encoding_options,heif_encoding_options_free);
OWNER(Encoder,heif_encoder,heif_encoder_release);
static Context context(){Context c(heif_context_alloc(),heif_context_free);require(bool(c),"context_allocation");
    auto limits=*heif_context_get_security_limits(c.get());limits.max_image_size_pixels=40000000;
    limits.max_number_of_tiles=256;limits.max_items=2048;limits.max_color_profile_size=65536;
    limits.max_memory_block_size=256ULL<<20;limits.max_total_memory=384ULL<<20;
    check(heif_context_set_security_limits(c.get(),&limits));heif_context_set_max_decoding_threads(c.get(),0);return c;}
static std::vector<uint8_t> read_fd(int fd){struct stat st;require(!fstat(fd,&st)&&S_ISREG(st.st_mode)&&st.st_size>0&&st.st_size<=64*1024*1024,"input_bounds");
    std::vector<uint8_t> v(st.st_size);require(lseek(fd,0,SEEK_SET)==0,"input_seek");size_t done=0;
    while(done<v.size()){if(a7_cancelled())throw std::runtime_error("cancelled");auto n=read(fd,v.data()+done,v.size()-done);if(n<0&&errno==EINTR)continue;require(n>0,"input_read");done+=n;}return v;}
struct Source {
    std::vector<uint8_t> bytes;Context ctx=context();Handle handle{nullptr,heif_image_handle_release};
    Nclx profile{nullptr,heif_nclx_color_profile_free};heif_image_tiling tiles{};
    heif_orientation orientation=heif_orientation_normal;unsigned width=0,height=0;
    Source(std::vector<uint8_t> b):bytes(std::move(b)){
        check(heif_context_read_from_memory_without_copy(ctx.get(),bytes.data(),bytes.size(),nullptr));
        require(heif_context_get_number_of_top_level_images(ctx.get())==1,"multiple_top_level_images");
        heif_image_handle* h=nullptr;check(heif_context_get_primary_image_handle(ctx.get(),&h));handle.reset(h);
        width=heif_image_handle_get_width(h);height=heif_image_handle_get_height(h);
        require(width&&height&&width<=8192&&height<=8192&&uint64_t(width)*height<=40000000,"dimensions");
        require(!heif_image_handle_has_alpha_channel(h)&&!heif_image_handle_is_premultiplied_alpha(h),"alpha_not_supported");
        require(heif_image_handle_get_luma_bits_per_pixel(h)==10&&heif_image_handle_get_chroma_bits_per_pixel(h)==10,"requires_10_bit");
        heif_colorspace space;heif_chroma chroma;check(heif_image_handle_get_preferred_decoding_colorspace(h,&space,&chroma));
        require(space==heif_colorspace_YCbCr&&(chroma==heif_chroma_420||chroma==heif_chroma_422),"requires_420_or_422");
        heif_color_profile_nclx* n=nullptr;check(heif_image_handle_get_nclx_color_profile(h,&n));profile.reset(n);
        require(n->color_primaries==1&&n->transfer_characteristics==13&&(n->matrix_coefficients==1||n->matrix_coefficients==5||n->matrix_coefficients==6),"requires_sRGB_SDR_nclx");
        require(heif_image_handle_get_raw_color_profile_size(h)==0,"ICC_not_supported");
        check(heif_image_handle_get_image_tiling(h,0,&tiles));
        require(tiles.num_columns&&tiles.num_rows&&uint64_t(tiles.num_columns)*tiles.num_rows<=256&&tiles.tile_width&&tiles.tile_height&&
          uint64_t(tiles.tile_width)*tiles.tile_height<=6000000&&!tiles.left_offset&&!tiles.top_offset&&!tiles.number_of_extra_dimensions,"tile_bounds");
        require(uint64_t(tiles.num_columns-1)*tiles.tile_width<tiles.image_width&&uint64_t(tiles.num_columns)*tiles.tile_width>=tiles.image_width&&
          uint64_t(tiles.num_rows-1)*tiles.tile_height<tiles.image_height&&uint64_t(tiles.num_rows)*tiles.tile_height>=tiles.image_height,"grid_geometry");
        heif_item_id id;check(heif_context_get_primary_image_ID(ctx.get(),&id));
        int count=heif_item_get_transformation_properties(ctx.get(),id,nullptr,0);require(count>=0&&count<=16,"transform_count");
        heif_property_id props[16];require(heif_item_get_transformation_properties(ctx.get(),id,props,16)==count,"transforms");
        for(int i=0;i<count;i++){
            auto type=heif_item_get_property_type(ctx.get(),id,props[i]);heif_orientation next=heif_orientation_normal;
            if(type==heif_item_property_type_transform_rotation){int angle=heif_item_get_property_transform_rotation_ccw(ctx.get(),id,props[i]);
                require(angle>=0&&angle%90==0,"rotation");next=angle==90?heif_orientation_rotate_270_cw:angle==180?heif_orientation_rotate_180:angle==270?heif_orientation_rotate_90_cw:heif_orientation_normal;}
            else if(type==heif_item_property_type_transform_mirror){auto m=heif_item_get_property_transform_mirror(ctx.get(),id,props[i]);
                require(m!=heif_transform_mirror_direction_invalid,"mirror");next=m==heif_transform_mirror_direction_vertical?heif_orientation_flip_vertically:heif_orientation_flip_horizontally;}
            else {int l,t,r,b;heif_item_get_property_transform_crop_borders(ctx.get(),id,props[i],tiles.image_width,tiles.image_height,&l,&t,&r,&b);
                require(type==heif_item_property_type_transform_crop&&!l&&!t&&!r&&!b,"nontrivial_crop_not_supported");}
            orientation=heif_orientation_concat(orientation,next);
        }
        bool swap=orientation>=5;
        require(width==(swap?tiles.image_height:tiles.image_width)&&height==(swap?tiles.image_width:tiles.image_height),"transform_dimensions");
    }
    std::vector<uint8_t> exif(){auto h=handle.get();int count=heif_image_handle_get_number_of_metadata_blocks(h,"Exif");require(count<=1,"duplicate_exif");if(!count)return {};
        heif_item_id id;require(heif_image_handle_get_list_of_metadata_block_IDs(h,"Exif",&id,1)==1,"exif_id");size_t size=heif_image_handle_get_metadata_size(h,id);
        require(size>=12&&size<=1024*1024,"exif_size");std::vector<uint8_t> v(size);check(heif_image_handle_get_metadata(h,id,v.data()));
        uint32_t off=(uint32_t(v[0])<<24)|(uint32_t(v[1])<<16)|(uint32_t(v[2])<<8)|v[3];require(off<=size-12,"exif_offset");
        return std::vector<uint8_t>(v.begin()+4+off,v.end());}
    Image tile(unsigned x,unsigned y){Decode opt(heif_decoding_options_alloc(),heif_decoding_options_free);require(bool(opt),"decode_options");
        opt->ignore_transformations=1;opt->strict_decoding=1;opt->convert_hdr_to_8bit=0;opt->num_codec_threads=1;
        // No repair/relabel: accepted input is explicitly sRGB SDR with its declared matrix/range.
        opt->output_image_nclx_profile_passthrough=1;opt->autocorrect_broken_input=0;
        heif_image* p=nullptr;check(heif_image_handle_decode_image_tile(handle.get(),&p,heif_colorspace_RGB,heif_chroma_interleaved_RRGGBB_LE,opt.get(),x,y));
        Image img(p,heif_image_release);require(heif_image_get_primary_width(p)==int(tiles.tile_width)&&heif_image_get_primary_height(p)==int(tiles.tile_height)&&
            heif_image_get_bits_per_pixel_range(p,heif_channel_interleaved)==10,"decoded_tile_shape_or_depth");
        require(heif_image_get_decoding_warnings(p,0,nullptr,0)==0,"decode_warning");return img;}
    void describe(){printf("{\"width\":%u,\"height\":%u,\"raw_width\":%u,\"raw_height\":%u,\"orientation\":%d,\"bits\":10,\"tiles\":[%u,%u],\"tile_size\":[%u,%u],\"nclx\":[%d,%d,%d,%d]}\n",width,height,tiles.image_width,tiles.image_height,int(orientation),tiles.num_columns,tiles.num_rows,tiles.tile_width,tiles.tile_height,int(profile->color_primaries),int(profile->transfer_characteristics),int(profile->matrix_coefficients),int(profile->full_range_flag));}
};
static heif_error write_output(heif_context*,const void* data,size_t size,void* user){int fd=*static_cast<int*>(user);auto p=static_cast<const uint8_t*>(data);
    while(size){if(a7_cancelled())return {heif_error_Encoding_error,heif_suberror_Unspecified,"cancelled"};auto n=write(fd,p,size);if(n<0&&errno==EINTR)continue;
        if(n<=0)return {heif_error_Encoding_error,heif_suberror_Unspecified,"output_write"};p+=n;size-=n;}return {heif_error_Ok,heif_suberror_Unspecified,"Success"};}
static void run(Source& src){require(src.width==a7_width()&&src.height==a7_height(),"job_dimensions");
    int fd=a7_reserve();require(fd>=0,a7_error());Context output=context();heif_encoder* e=nullptr;
    check(heif_context_get_encoder_for_format(output.get(),heif_compression_HEVC,&e));Encoder encoder(e,heif_encoder_release);
    check(heif_encoder_set_lossy_quality(e,a7_quality()));check(heif_encoder_set_parameter_string(e,"preset","ultrafast"));
    // Fixed 4:2:2 output retains chroma precision even for 4:2:0 input. No RGB8 or JPEG intermediary.
    check(heif_encoder_set_parameter_string(e,"chroma","422"));
    for(auto pair:{std::pair{"x265:pools","none"},std::pair{"x265:frame-threads","1"},std::pair{"x265:wpp","0"}})
        check(heif_encoder_set_parameter_string(e,pair.first,pair.second));
    check(heif_encoder_set_logging_level(e,0));
    Nclx n(heif_nclx_color_profile_alloc(),heif_nclx_color_profile_free);require(bool(n),"nclx_allocation");
    n->color_primaries=heif_color_primaries_ITU_R_BT_709_5;n->transfer_characteristics=heif_transfer_characteristic_IEC_61966_2_1;
    n->matrix_coefficients=heif_matrix_coefficients_ITU_R_BT_601_6;n->full_range_flag=1;
    Encode options(heif_encoding_options_alloc(),heif_encoding_options_free);require(bool(options),"encode_options");
    options->image_orientation=src.orientation;options->output_nclx_profile=n.get();options->save_alpha_channel=0;
    unsigned tw=src.tiles.tile_width,th=src.tiles.tile_height;
    while(uint64_t(tw)*th>1000000){if(tw>=th&&tw%2==0)tw/=2;else if(th%2==0)th/=2;else if(tw%2==0)tw/=2;else throw std::runtime_error("cannot_bound_encoder_tile");}
    require(tw>=64&&th>=64&&tw%2==0,"encoder_tile_geometry");
    unsigned cols=(src.tiles.image_width+tw-1)/tw,rows=(src.tiles.image_height+th-1)/th;
    require(uint64_t(cols)*rows<=256,"output_tile_count");
    heif_image_handle* g=nullptr;check(heif_context_add_grid_image(output.get(),src.tiles.image_width,src.tiles.image_height,cols,rows,options.get(),&g));
    Handle grid(g,heif_image_handle_release);
    for(unsigned y=0;y<src.tiles.num_rows;y++)for(unsigned x=0;x<src.tiles.num_columns;x++){
        require(!a7_cancelled(),"cancelled");auto img=src.tile(x,y);int stride=0;auto pixels=heif_image_get_plane(img.get(),heif_channel_interleaved,&stride);
        require(pixels&&stride>=int(src.tiles.tile_width*6),"RGB16_stride");
        for(unsigned row=0;row<src.tiles.tile_height;row++){require(!a7_cancelled(),"cancelled");auto p=reinterpret_cast<uint16_t*>(pixels+row*stride);
            for(unsigned col=0;col<src.tiles.tile_width;col++){float f[3];for(int c=0;c<3;c++){require(p[3*col+c]<=1023,"RGB10_sample");f[c]=p[3*col+c]/1023.f;}
                a7_apply(f);for(int c=0;c<3;c++)p[3*col+c]=uint16_t(std::floor(std::clamp(f[c],0.f,1.f)*1023.f+.5f));}}
        // Decode once per source tile; subdivide for bounded x265 frame allocations.
        for(unsigned sy=0;sy<src.tiles.tile_height/th;sy++)for(unsigned sx=0;sx<src.tiles.tile_width/tw;sx++){
            unsigned ox=x*(src.tiles.tile_width/tw)+sx,oy=y*(src.tiles.tile_height/th)+sy;
            if(ox>=cols||oy>=rows)continue;
            heif_image* part=nullptr;check(heif_image_create(tw,th,heif_colorspace_RGB,heif_chroma_interleaved_RRGGBB_LE,&part));Image tile(part,heif_image_release);
            check(heif_image_add_plane(part,heif_channel_interleaved,tw,th,10));int ps;auto dest=heif_image_get_plane(part,heif_channel_interleaved,&ps);
            require(dest&&ps>=int(tw*6),"encoder_plane");
            for(unsigned ry=0;ry<th;ry++)memcpy(dest+ry*ps,pixels+(sy*th+ry)*stride+sx*tw*6,tw*6);
            check(heif_image_set_nclx_color_profile(part,n.get()));check(heif_context_add_image_tile(output.get(),g,ox,oy,part,e));
        }
    }
    check(heif_context_set_primary_image(output.get(),g));check(heif_context_add_exif_metadata(output.get(),g,a7_exif()+6,a7_exif_size()-6));
    heif_writer writer{};writer.writer_api_version=1;writer.write=write_output;check(heif_context_write(output.get(),&writer,&fd));require(!fsync(fd),"output_sync");
    // Verify durable bytes, all tiles, geometry, color and exact rewritten metadata before publication.
    grid.reset();encoder.reset();output.reset();Source verified(read_fd(fd));require(verified.width==src.width&&verified.height==src.height&&verified.orientation==src.orientation,"output_geometry");
    require(verified.profile->matrix_coefficients==6&&verified.profile->full_range_flag==1,"output_color");auto exif=verified.exif();
    require(exif.size()==a7_exif_size()-6&&!memcmp(exif.data(),a7_exif()+6,exif.size()),"output_exif");
    for(unsigned y=0;y<verified.tiles.num_rows;y++)for(unsigned x=0;x<verified.tiles.num_columns;x++){require(!a7_cancelled(),"cancelled");auto img=verified.tile(x,y);}
    int published=a7_publish();require(!published,a7_error());printf("{\"status\":\"ok\",\"published\":true,\"bits\":10,\"width\":%u,\"height\":%u,\"tiles_verified\":%u}\n",src.width,src.height,verified.tiles.num_rows*verified.tiles.num_columns);
}
int main(int argc,char**argv){
    if(argc==2&&!strcmp(argv[1],"--version")){puts("A7C2 HIF Worker 0.1.0; 10-bit HEVC/4:2:2; hardware untested; GPL-3.0-or-later");return 0;}
    if(argc!=3||(strcmp(argv[1],"--run")&&strcmp(argv[1],"--check")&&strcmp(argv[1],"--inspect")&&strcmp(argv[1],"--metadata")&&strcmp(argv[1],"--vectors"))){fputs("Usage: hif-worker --run|--check|--vectors JOBDIR | --inspect|--metadata LOCAL.HIF\n",stderr);return 2;}
#ifdef __linux__
    rlimit mem{512ULL<<20,512ULL<<20},cpu{1800,1800},file{128ULL<<20,128ULL<<20};
    if(setrlimit(RLIMIT_AS,&mem)||setrlimit(RLIMIT_CPU,&cpu)||setrlimit(RLIMIT_FSIZE,&file)){fputs("resource_limits\n",stderr);return 1;}
#endif
    bool loaded=false;int status=0;
    try{
        require(!heif_init(nullptr).code,"heif_init");
        if(!strcmp(argv[1],"--inspect")||!strcmp(argv[1],"--metadata")){
            int fd=open(argv[2],O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);require(fd>=0,"input_open");std::vector<uint8_t> bytes;
            try{bytes=read_fd(fd);}catch(...){close(fd);throw;}close(fd);Source src(std::move(bytes));
            if(!strcmp(argv[1],"--inspect"))src.describe();else{auto exif=src.exif();require(fwrite(exif.data(),1,exif.size(),stdout)==exif.size(),"metadata_write");}
        }else{
            loaded=true;int code=a7_load(argv[2]);require(!code,a7_error());
            if(!strcmp(argv[1],"--vectors")){float p[3];while(fread(p,sizeof(p),1,stdin)==1){for(auto f:p)require(std::isfinite(f)&&f>=0&&f<=1,"vectors_bounds");a7_apply(p);require(fwrite(p,sizeof(p),1,stdout)==1,"vectors_write");}require(!ferror(stdin),"vectors_read");}
            else {auto bytes=read_fd(a7_input());require(!a7_verify_snapshot(bytes.data(),bytes.size()),"snapshot_hash");Source src(std::move(bytes));if(!strcmp(argv[1],"--check")){require(src.width==a7_width()&&src.height==a7_height(),"job_dimensions");src.describe();}else run(src);}
        }
    }catch(const std::exception& e){fprintf(stderr,"HIF worker: %s; published=%s\n",e.what(),a7_published()?"true":"false");status=1;}
    if(loaded)a7_cleanup();heif_deinit();return status;
}
