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

#ifndef PIC_IO_TMP_HPP
#define PIC_IO_TMP_HPP

#include <stdio.h>
#include <string>

#include "base.hpp"

namespace pic {

/**Header for a tmp image*/
struct TMP_IMG_HEADER {
    int frames, width, height, channels;
};

/**ReadTMP: reads a dump temp file*/
PIC_INLINE float *ReadTMP(std::string nameFile, float *data, int &width,
                          int &height, int &channels, int &frames, bool bHeader = true)
{
    FILE *file = fopen(nameFile.c_str(), "rb");

    if(file == NULL) {
        return NULL;
    }

    //reading the header
    TMP_IMG_HEADER header;

    if(bHeader) {
        fread(&header, sizeof(TMP_IMG_HEADER), 1, file);

        if(header.channels < 1 && header.frames < 1 && header.height < 1 &&
           header.width < 1) { //invalid image!
            return NULL;
        }
    }

    if(data == NULL) {
        data = new float[width * height * channels * frames];
    }

    if(bHeader) {
        width    = header.width;
        height   = header.height;
        channels = header.channels;
        frames   = header.frames;
    }

    fread(data, sizeof(float), frames * width * height * channels, file);

    fclose(file);

    return data;
}

/**WriteTMP: writes a dump temp file*/
PIC_INLINE bool WriteTMP(std::string nameFile, const float *data, int &width,
                         int &height, int &channels, int &frames, bool bHeader = true)
{

    TMP_IMG_HEADER header;

    if(bHeader) {
        header.frames = frames;
        header.width = width;
        header.height = height;
        header.channels = channels;
    }

    FILE *file = fopen(nameFile.c_str(), "wb");

    if(file == NULL) {
        return false;
    }

    int size = frames * width * height * channels;

     if(size < 1)
         return false;

    if(bHeader) {
        fwrite(&header, sizeof(TMP_IMG_HEADER), 1, file);
    }
  
    fwrite(data, sizeof(float), size, file);

    fclose(file);

    return true;
}

} // end namespace pic

#endif /* PIC_IO_TMP_HPP */

