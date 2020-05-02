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

#include "../base.hpp"

#ifdef PIC_ENABLE_OPEN_EXR

//include for OpenEXR 1.xx

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
#endif //PIC_ENABLE_OPEN_EXR

#endif /* PIC_IO_EXR_HPP */

