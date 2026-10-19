/* Synthetic HEIF generator/10-bit RGB dumper; host tests only. */
#define main hif_worker_entry_for_tests
#include "../heif_worker.cc"
#undef main
int main(int argc,char**argv){try{
    if(argc==3&&!strcmp(argv[1],"dump")){int fd=open(argv[2],O_RDONLY|O_NOFOLLOW);require(fd>=0,"fixture_open");auto bytes=read_fd(fd);close(fd);Source src(std::move(bytes));
        std::vector<uint16_t> row(src.tiles.image_width*3);
        for(unsigned ty=0;ty<src.tiles.num_rows;ty++){
            std::vector<Image> tiles;for(unsigned tx=0;tx<src.tiles.num_columns;tx++)tiles.push_back(src.tile(tx,ty));
            for(unsigned y=0;y<src.tiles.tile_height&&ty*src.tiles.tile_height+y<src.tiles.image_height;y++){
                for(unsigned tx=0;tx<src.tiles.num_columns;tx++){int stride;auto p=heif_image_get_plane_readonly(tiles[tx].get(),heif_channel_interleaved,&stride);
                    unsigned count=std::min(src.tiles.tile_width,src.tiles.image_width-tx*src.tiles.tile_width);memcpy(row.data()+tx*src.tiles.tile_width*3,p+y*stride,count*6);}
                require(fwrite(row.data(),6,src.tiles.image_width,stdout)==src.tiles.image_width,"dump_write");}}
        return 0;
    }
    require(argc==7,"fixture PATH ORIENTATION TRANSFER BITS CHROMA MODE");
    auto ctx=context();heif_encoder* e=nullptr;check(heif_context_get_encoder_for_format(ctx.get(),heif_compression_HEVC,&e));Encoder enc(e,heif_encoder_release);
    check(heif_encoder_set_lossless(e,1));check(heif_encoder_set_parameter_string(e,"preset","ultrafast"));check(heif_encoder_set_logging_level(e,0));
    check(heif_encoder_set_parameter_string(e,"chroma",argv[5]));check(heif_encoder_set_parameter_string(e,"x265:pools","none"));check(heif_encoder_set_parameter_string(e,"x265:frame-threads","1"));check(heif_encoder_set_parameter_string(e,"x265:wpp","0"));
    int bits=atoi(argv[4]);Nclx n(heif_nclx_color_profile_alloc(),heif_nclx_color_profile_free);n->color_primaries=heif_color_primaries_ITU_R_BT_709_5;
    n->transfer_characteristics=static_cast<heif_transfer_characteristics>(atoi(argv[3]));n->matrix_coefficients=heif_matrix_coefficients_ITU_R_BT_601_6;n->full_range_flag=1;
    Encode opts(heif_encoding_options_alloc(),heif_encoding_options_free);opts->image_orientation=static_cast<heif_orientation>(atoi(argv[2]));opts->output_nclx_profile=n.get();
    heif_image_handle* g=nullptr;check(heif_context_add_grid_image(ctx.get(),1024,128,2,2,opts.get(),&g));Handle grid(g,heif_image_handle_release);
    for(int ty=0;ty<2;ty++)for(int tx=0;tx<2;tx++){
        heif_image* p=nullptr;check(heif_image_create(512,64,heif_colorspace_RGB,heif_chroma_interleaved_RRGGBB_LE,&p));Image img(p,heif_image_release);
        check(heif_image_add_plane(p,heif_channel_interleaved,512,64,bits));check(heif_image_set_nclx_color_profile(p,n.get()));int stride;auto data=heif_image_get_plane(p,heif_channel_interleaved,&stride);
        for(int y=0;y<64;y++){auto row=reinterpret_cast<uint16_t*>(data+y*stride);for(int x=0;x<512;x++)for(int c=0;c<3;c++){
            int v=x+tx*512;if(strcmp(argv[6],"gray"))v=(v+((c==1?y+ty*64: c==2?255:0)*3))%1024;
            row[x*3+c]=v>>(10-bits);}}
        check(heif_context_add_image_tile(ctx.get(),g,tx,ty,p,e));}
    check(heif_context_set_primary_image(ctx.get(),g));check(heif_context_write_to_file(ctx.get(),argv[1]));return 0;
}catch(const std::exception& e){fprintf(stderr,"%s\n",e.what());return 1;}}
