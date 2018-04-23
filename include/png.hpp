#pragma once
#include <string>
#define PNG_DEBUG 3
#include <png.h>

struct PNG {
    int height, width;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;


    PNG(std::string &fname);
    ~PNG();
    void read_to(char*);
};
