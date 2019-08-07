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

#ifndef PIC_IO_EXR_TINY_HPP
#define PIC_IO_EXR_TINY_HPP

#include "../base.hpp"

#ifndef PIC_DISABLE_TINY_EXR

#define TINYEXR_IMPLEMENTATION

#include "../util/std_util.hpp"

#include "../externals/tinyexr/tinyexr.h"

namespace pic {

PIC_INLINE float *ReadEXR(std::string nameFile, float *data, int &width, int &height, int &channels)
{
    EXRImage image;
    InitEXRImage(&image);

    const char* err;
    int ret = ParseMultiChannelEXRHeaderFromFile(&image, nameFile.c_str(), &err);
    if (ret != 0) {
        #ifdef PIC_DEBUG
            printf("Parse EXR error: %s\n", err);
        #endif

        return NULL;
    }

    width = image.width;
    height = image.height;
    channels = image.num_channels;

    //Allocate into memory
    if(data == NULL) {
        data = new float[width * height * channels];
    }

    for (int i = 0; i < image.num_channels; i++) {
        if (image.pixel_types[i] == TINYEXR_PIXELTYPE_HALF) {
            image.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
        }
    }

    ret = LoadMultiChannelEXRFromFile(&image, nameFile.c_str(), &err);
    if (ret != 0) {
        #ifdef PIC_DEBUG
            printf("Load EXR error: %s\n", err);
        #endif
        return data;
    }

    float **images = (float**) image.images;

    int nPixels = width * height;
    for (int i = 0; i < nPixels; i++){
        int index = i * channels;

        data[index    ] = images[2][i];
        data[index + 1] = images[1][i];
        data[index + 2] = images[0][i];
    }

    FreeEXRImage(&image);
    return data;
}

/**
 * @brief WriteEXR
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @return
 */
PIC_INLINE bool WriteEXR(std::string nameFile, float *data, int width,
                         int height, int channels = 3)
{
    EXRImage image;
    InitEXRImage(&image);

     image.num_channels = channels;

     const char* channel_names[] = {"B", "G", "R"}; // "B", "G", "R", "A" for RGBA image

     std::vector< float* > images;
     for(int i = 0; i < channels; i++) {
         float *tmp = new float[width * height];
         images.push_back(tmp);
     }

     int nPixels = width * height;
     for (int i = 0; i < nPixels; i++){
         int index = i * channels;

         for(int j = 0; j < channels; j++) {
             images[j][i] = data[index + j];
         }
     }

     float *image_ptr[3];
     image_ptr[0] = &(images[2][0]); // B
     image_ptr[1] = &(images[1][0]); // G
     image_ptr[2] = &(images[0][0]); // R

     image.channel_names = channel_names;
     image.images = (unsigned char**)image_ptr;
     image.width = width;
     image.height = height;

     image.pixel_types = (int *)malloc(sizeof(int) * image.num_channels);
     image.requested_pixel_types = (int *)malloc(sizeof(int) * image.num_channels);
     for (int i = 0; i < image.num_channels; i++) {
       image.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
       image.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
     }

     const char* err;
     int ret = SaveMultiChannelEXRToFile(&image, nameFile.c_str(), &err);
     if (ret != 0) {
         printf("Save EXR err: %s\n", err);
         return false;
     }

     for(int i = 0; i < channels; i++) {
         delete_vec_s(images[i]);
     }
     images.clear();

     delete_vec_s(image.pixel_types);
     delete_vec_s(image.requested_pixel_types);

     return true;
}

}

#endif //PIC_DISABLE_TINY_EXR

#endif /* PIC_IO_EXR_TINY_HPP */

