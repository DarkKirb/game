#include <png.hpp>
#include <stdlib.h>
#include <iostream>
#include <cstring>

PNG::PNG(std::string &fname) {
    unsigned char header[8];
    FILE *fp = fopen(fname.c_str(), "rb");
    if(!fp) {
        std::cerr << "could not open" << fname << std::endl;
        abort();
    }
    fread(header, 1, 8, fp);
    if(png_sig_cmp(header, 0, 8)) {
        std::cerr << fname << " is not a valid png" << std::endl;
        abort();
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    info_ptr = png_create_info_struct(png_ptr);
    if(setjmp(png_jmpbuf(png_ptr)))
        abort();
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    number_of_passes = png_set_interlace_handling(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (png_get_valid(png_ptr, info_ptr,
        PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
          png_set_gray_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_BEFORE);
#ifdef _3DS
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        png_set_bgr(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        png_set_swap_alpha(png_ptr);
#endif



    png_read_update_info(png_ptr, info_ptr);
    if (setjmp(png_jmpbuf(png_ptr)))
        abort();
    row_pointers = new png_bytep[height];
    for (int y=0; y<height; y++)
        row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];
    png_read_image(png_ptr, row_pointers);
    fclose(fp);
}

PNG::~PNG() {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    for(int i=0;i<height;i++)
        delete[] row_pointers[i];
    delete[] row_pointers;
}

void PNG::read_to(char* p) {
    auto t = png_get_rowbytes(png_ptr, info_ptr);
    for(int i = height-1; i>=0; i--) {
        memcpy(p, row_pointers[i], t);
        p+=t;
    }
}
