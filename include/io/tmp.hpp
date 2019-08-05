/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_IO_TMP_HPP
#define PIC_IO_TMP_HPP

#include <stdio.h>
#include <string>

#include "../base.hpp"

namespace pic {

/**
 * @brief The TMP_IMG_HEADER struct is a header for a tmp image
 */
struct TMP_IMG_HEADER {
    int frames, width, height, channels;
};


/**
 * @brief ReadTMP reads a dump temp file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @param frames
 * @param bHeader
 * @return
 */
PIC_INLINE float *ReadTMP(std::string nameFile, float *data, int &width,
                          int &height, int &channels, int &frames, bool bHeader = true)
{
    FILE *file = fopen(nameFile.c_str(), "rb");

    if(file == NULL) {
        return NULL;
    }

    //read the header
    TMP_IMG_HEADER header;
    header.channels = -1;
    header.frames = -1;
    header.width = -1;
    header.height = -1;

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

/**
 * @brief WriteTMP writes a dump temp file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @param frames
 * @param bHeader
 * @return
 */
PIC_INLINE bool WriteTMP(std::string nameFile, float *data, int &width,
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

