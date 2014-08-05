/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_IO_TGA_HPP
#define PIC_IO_TGA_HPP

#include <stdio.h>
#include <string>
#include <iostream>

#include "base.hpp"
#include "util/buffer.hpp"

namespace pic {

struct TGA_HEADER{
    unsigned char id_length;
    unsigned char colormap_type;
    unsigned char image_type;

    //colormap information
    short int     colormap_first_entry;
    short int     colormap_length;
    unsigned char colormap_entry_size;

    //image information
    short int     x_origin;
    short int     y_origin;
    short int     width;
    short int     height;
    unsigned char depth;

    unsigned char descriptor;
};

/**ReadTGA: reads an image in the .tga format*/
PIC_INLINE unsigned char *ReadTGA(std::string nameFile, unsigned char *data,
                                  int &width, int &height, int &channels)
{
    std::ifstream tga_in(nameFile.c_str(), std::ios::binary);

    if(!tga_in.is_open()) {
        return data;
    }

    //reading the header
    TGA_HEADER header;
    tga_in.read((char*)(&header.id_length), 1);
    tga_in.read((char*)(&header.colormap_type), 1);
    tga_in.read((char*)(&header.image_type), 1);

    tga_in.read((char*)(&header.colormap_first_entry), 2);
    tga_in.read((char*)(&header.colormap_length), 2);
    tga_in.read((char*)(&header.colormap_entry_size), 1);

    tga_in.read((char*)(&header.x_origin), 2);
    tga_in.read((char*)(&header.y_origin), 2);
    tga_in.read((char*)(&header.width), 2);
    tga_in.read((char*)(&header.height), 2);
    tga_in.read((char*)(&header.depth), 1);
    tga_in.read((char*)(&header.descriptor), 1);


    width  = (int)(header.width);
    height = (int)(header.height);

    //extra information from the developer
    for(int i=0; i<header.id_length; i++) {
        char tmp;
        tga_in.read(&tmp, 1);
    }

    //supporting only 8-bit RGB or RGBA
    if(!((header.depth==32) || (header.depth==24))) {
        tga_in.close();
        return data;
    }

    channels = header.depth / 8;

    int size = width * height * channels;
    if(data == NULL) {
        data = new unsigned char[size];
    }

    //reading uncompressed data
    if((header.image_type > 0) && (header.image_type < 4)) {
        tga_in.read((char*)(data), size);

        //values are stored as BGR
        BufferBGRtoRGB(data, width, height, channels, 1);

        //values are stored with a vertical flip
        BufferFlipV(data, width, height, channels, 1);

    } else {
        //reading RLE compressed data

    }

    tga_in.close();
    return data;
}

/**WriteTGA: writes an image in the .tga format*/
PIC_INLINE bool WriteTGA(std::string nameFile, const unsigned char *data,
                         int width, int height, int channels)
{
    std::ofstream tga_out(nameFile.c_str(), std::ios::binary);

    if(!tga_out.is_open()) {
        return false;
    }

    //setting the header
    TGA_HEADER header;
    header.id_length = 0;
    header.colormap_type = 0; //no color map included

    header.colormap_first_entry = 0;
    header.colormap_length = 0;
    header.colormap_entry_size = 0;

    header.x_origin = 0;
    header.y_origin = 0;
    header.width = width;
    header.height = height;
    header.depth = 8*channels;

    if(channels == 4) {
        header.descriptor = 3;
    } else {
        header.descriptor = 0;
    }

    if(channels == 1) {
        header.image_type = 3; //uncompressed gray scale
    } else {
        header.image_type = 2; //uncompressed RGB
    }

    tga_out.write((char*)(&header.id_length), 1);
    tga_out.write((char*)(&header.colormap_type), 1);
    tga_out.write((char*)(&header.image_type), 1);

    tga_out.write((char*)(&header.colormap_first_entry), 2);
    tga_out.write((char*)(&header.colormap_length), 2);
    tga_out.write((char*)(&header.colormap_entry_size), 1);

    tga_out.write((char*)(&header.x_origin), 2);
    tga_out.write((char*)(&header.y_origin), 2);
    tga_out.write((char*)(&header.width), 2);
    tga_out.write((char*)(&header.height), 2);
    tga_out.write((char*)(&header.depth), 1);
    tga_out.write((char*)(&header.descriptor), 1);

//    tga_out.write((char*)(&header.components), 4);
//    tga_out.write((char*)(&header.bytes), 4);

    tga_out.write((char*)(data), width * height * channels);

    return true;
}

} // end namespace pic

#endif /* PIC_IO_TMP_HPP */

