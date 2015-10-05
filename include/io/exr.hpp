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

#ifndef PIC_IO_EXR_HPP
#define PIC_IO_EXR_HPP

#include "base.hpp"

#ifdef PIC_ENABLE_OPEN_EXR

#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

#ifdef PIC_WIN32
#pragma comment( lib, "Iex_dll" )
#pragma comment( lib, "Half_dll" )
#pragma comment( lib, "IlmImf_dll" )
#pragma comment( lib, "IlmThread_dll" )
#pragma comment( lib, "Imath_dll" )
#pragma comment( lib, "zlib" )
#endif

//SYSTEM: X POS Y NEG
namespace pic {

/**
 * @brief ReadEXR reads .exr data from file
 * @param nameFile
 * @param width
 * @param height
 * @param channels
 * @param pixelBuffer
 * @return
 */
PIC_INLINE Imf::Rgba *ReadEXR(std::string nameFile, int &width, int &height,
                              int &channels, Imf::Rgba *pixelBuffer = NULL)
{
    try {
        Imf::RgbaInputFile in(nameFile.c_str());
        Imath::Box2i win = in.dataWindow();

        Imath::V2i dim(win.max.x - win.min.x + 1, win.max.y - win.min.y + 1);

        //printf("%d %d %d %d\n",win.max.x,win.min.x,win.max.y,win.min.y);

        if(pixelBuffer == NULL) {
            pixelBuffer = new Imf::Rgba[dim.x * dim.y];
        }

        int dx = win.min.x;
        int dy = win.min.y;

        in.setFrameBuffer(pixelBuffer - dx - dy * dim.x, 1, dim.x);
        in.readPixels(win.min.y, win.max.y);

        width  = dim.x;
        height = dim.y;
        channels = 3;

        return pixelBuffer;

    } catch(Iex::BaseExc &e) {
        std::cerr << e.what() << std::endl;
        return NULL;
    }  
}

/**
 * @brief ReadEXR reads .exr data from file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @param pixelBuffer
 * @return
 */
PIC_INLINE float *ReadEXR(std::string nameFile, float *data, int &width,
                          int &height, int &channels, Imf::Rgba *pixelBuffer = NULL)
{
    try {
        Imf::RgbaInputFile in(nameFile.c_str());
        Imath::Box2i win = in.dataWindow();

        Imath::V2i dim(win.max.x - win.min.x + 1, win.max.y - win.min.y + 1);

        //printf("%d %d %d %d\n",win.max.x,win.min.x,win.max.y,win.min.y);

        bool bPixelBufferNULL = false;

        if(pixelBuffer == NULL) {
            pixelBuffer = new Imf::Rgba[dim.x * dim.y];
            bPixelBufferNULL = true;
        }

        int dx = win.min.x;
        int dy = win.min.y;

        in.setFrameBuffer(pixelBuffer - dx - dy * dim.x, 1, dim.x);
        in.readPixels(win.min.y, win.max.y);

        //Allocate into memory
        if(data == NULL) {
            data = new float[dim.x * dim.y * 3];
        }

        width  = dim.x;
        height = dim.y;
        channels = 3;

        int tot = width * height * channels;

        for(int i = 0; i < tot; i += channels) { //swizzle
            int j = i / 3;
            data[i    ] = pixelBuffer[j].r;
            data[i + 1] = pixelBuffer[j].g;
            data[i + 2] = pixelBuffer[j].b;
        }

        if(bPixelBufferNULL) {
            delete[] pixelBuffer;
        }

        return data;
    } catch(Iex::BaseExc &e) {
        std::cerr << e.what() << std::endl;
        return NULL;
    }
}

/**
 * @brief WriteEXR writes an .exr file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @param pixelBuffer
 * @return
 */
PIC_INLINE bool WriteEXR(std::string nameFile, const float *data, int width,
                         int height, int channels = 3, Imf::Rgba *pixelBuffer = NULL)
{
    Imath::Box2i win;
    win.max.x = width - 1;
    win.max.y = height - 1;
    win.min.x = 0;
    win.min.y = 0;
    Imf::RgbaOutputFile outC(nameFile.c_str(), win, win, Imf::WRITE_RGBA);

    //Copy data
    int tot = width * height;
    bool bPixelBufferNULL = false;

    if(pixelBuffer == NULL) {
        pixelBuffer = new Imf::Rgba[tot];
        bPixelBufferNULL = true;
    }

    int j = 0;

    for(int i = 0; i < tot; i++) {
        pixelBuffer[i].r = data[j];
        j++;
        pixelBuffer[i].g = data[j];
        j++;
        pixelBuffer[i].b = data[j];
        j++;
        pixelBuffer[i].a = 1.0f;
    }

    outC.setFrameBuffer(pixelBuffer, 1, width);
    outC.writePixels(height);

    if(bPixelBufferNULL) {
        delete[] pixelBuffer;
    }

    return true;
}

} // end namespace pic
#else

#ifndef PIC_DISABLE_TINY_EXR

#define TINYEXR_IMPLEMENTATION
#include "externals/tinyexr/tinyexr.h"

namespace pic {

/**
 * @brief ReadEXR
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @return
 */
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
        if (image.pixel_types[i] = TINYEXR_PIXELTYPE_HALF) {
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
PIC_INLINE bool WriteEXR(std::string nameFile, const float *data, int width,
                         int height, int channels = 3)
{
    EXRImage image;
    InitEXRImage(&image);

     image.num_channels = channels;

     const char* channel_names[] = {"B", "G", "R"}; // "B", "G", "R", "A" for RGBA image

     std::vector<float> images[3];
     images[0].resize(width * height);
     images[1].resize(width * height);
     images[2].resize(width * height);

     int nPixels = width * height;
     for (int i = 0; i < nPixels; i++){
         int index = i * channels;

         images[0][i] = data[index    ];
         images[1][i] = data[index + 1];
         images[2][i] = data[index + 2];
     }

     float* image_ptr[3];
     image_ptr[0] = &(images[2].at(0)); // B
     image_ptr[1] = &(images[1].at(0)); // G
     image_ptr[2] = &(images[0].at(0)); // R

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

     free(image.pixel_types);
     free(image.requested_pixel_types);

     return true;
}

}

#endif //PIC_DISABLE_TINY_EXR

#endif //PIC_ENABLE_OPEN_EXR

#endif /* PIC_IO_EXR_HPP */

