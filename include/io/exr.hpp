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

#ifndef PIC_IO_EXR_HPP
#define PIC_IO_EXR_HPP

#ifdef PIC_ENABLE_OPEN_EXR

#include "base.hpp"

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


/**ReadEXR: reads EXR data from file*/
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

/**ReadEXR: reads EXR data from file*/
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

/**WriteEXR: writes an .exr file*/
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

#endif

#endif /* PIC_IO_EXR_HPP */

