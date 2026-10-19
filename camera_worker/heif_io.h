/* Shared bounded HEIF reader; no camera APIs. */
#pragma once
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
