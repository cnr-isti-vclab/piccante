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

#ifndef PIC_IO_BMP_HPP
#define PIC_IO_BMP_HPP

#include <stdio.h>
#include <string>

#ifdef PIC_WIN32
#include <windows.h>
#endif

#include "../base.hpp"

namespace pic {

#ifndef PIC_WIN32

/**
 * @brief The BITMAPFILEHEADER struct
 */
struct BITMAPFILEHEADER {
    unsigned short  bfType;
    unsigned int    bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned int    bfOffBits;
};

/**
 * @brief The BITMAPINFOHEADER struct
 */
struct BITMAPINFOHEADER {
    unsigned int      biSize;
    int               biWidth;
    int               biHeight;
    unsigned short    biPlanes;
    unsigned short    biBitCount;
    unsigned int      biCompression;
    unsigned int      biSizeImage;
    int               biXPelsPerMeter;
    int               biYPelsPerMeter;
    unsigned int      biClrUsed;
    unsigned int      biClrImportant;
};

#define BI_RGB              0L

#endif

/**
 * @brief BitmapPadding (SYSTEM: X POS Y POS).
 * @param bpp
 * @param width
 * @return
 */
inline int BitmapPadding(int bpp, int width)
{
    int padding;

    if(((width * bpp / 8) % 4) != 0) {
        padding = 4 - ((width * bpp / 8) % 4);
    } else {
        padding = 0;
    }

    return padding;
}

/**
 * @brief ReadBMP reads an image as .bmp file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @return
 */
PIC_INLINE unsigned char *ReadBMP(std::string nameFile, unsigned char *data,
                                  int &width, int &height, int &channels)
{
    FILE *file = fopen(nameFile.c_str(), "rb");

    if(file == NULL) {
        return data;
    }

    BITMAPFILEHEADER    bmpfh;
    BITMAPINFOHEADER    bmpih;

    //reading the bitmap file header:
    //this structure is 14 bytes ==> no alignment
    //so issues for some compilers
    fread(&bmpfh.bfType, sizeof(unsigned short), 1, file);
    fread(&bmpfh.bfSize, sizeof(unsigned int), 1, file);
    fread(&bmpfh.bfReserved1, sizeof(unsigned short), 1, file);
    fread(&bmpfh.bfReserved2, sizeof(unsigned short), 1, file);
    fread(&bmpfh.bfOffBits, sizeof(unsigned int), 1, file);

    fread(&bmpih, sizeof(BITMAPINFOHEADER), 1, file);

    //24-bit images only!
    if(bmpih.biCompression != BI_RGB) {
        fclose(file);
        return data;
    }

    int bpp = bmpih.biBitCount;

    channels = bpp / 8;

    if(!(channels == 3 || channels == 1)) {
        fclose(file);
        return data;
    }

    fseek(file, bmpfh.bfOffBits, SEEK_SET);

    width  = bmpih.biWidth;
    height = bmpih.biHeight;

    if(data == NULL) {
        data = new unsigned char[width * height * channels];
    }

    //compute padding
    int padding = BitmapPadding(bpp, width);

    unsigned char *pads = NULL;

    if(padding > 0) {
        pads = new unsigned char[padding];
    }

    unsigned char tmp[3];

    for(int j = (height - 1); j > -1; j--) {
        int cj = j * width;

        if(channels == 3) {
            for(int i = 0; i < width; i++) {
                int c = (cj + i) * 3;
                fread(tmp, sizeof(unsigned char), 3, file);
                //from BGR to RGB
                data[c + 2] = tmp[0];
                data[c + 1] = tmp[1];
                data[c    ] = tmp[2];
            }
        }

        if(channels == 1) {
            fread(&data[cj], sizeof(unsigned char), width, file);
        }

        if(padding > 0) {
            fread(pads, sizeof(unsigned char), padding, file);
        }
    }

    fclose(file);
    return data;
}

/**
 * @brief WriteBMP writes an image as a .bmp file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @return
 */
PIC_INLINE bool WriteBMP(std::string nameFile, const unsigned char *data,
                         int width, int height, int channels)
{
    if(data == NULL) {
        return false;
    }

    //	4*(bbp/32*width)
    BITMAPFILEHEADER    bmpfh;
    BITMAPINFOHEADER    bmpih;

    //preparing the file header info
    bmpfh.bfType = 19778;
    //to avoid issues with 4-byte alignment
    bmpfh.bfOffBits = 54; //sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    bmpfh.bfReserved1 = 0L;
    bmpfh.bfReserved2 = 0L;
    bmpfh.bfSize = 1078;

    //preparing the bmp header info
    bmpih.biBitCount = 24;
    bmpih.biCompression = 0;
    bmpih.biHeight = height;
    bmpih.biWidth = width;
    bmpih.biClrUsed = 0;
    bmpih.biClrImportant = 0;
    bmpih.biXPelsPerMeter = 0;
    bmpih.biYPelsPerMeter = 0;
    bmpih.biSize = sizeof(bmpih);
    bmpih.biPlanes = 1;
    bmpih.biSizeImage = 0;

    FILE *file = fopen(nameFile.c_str(), "wb");

    if(file == NULL) {
        return false;
    }

    //writing the bitmap file header:
    //this structure is 14 bytes ==> no alignment
    //so issues for some compilers
    fwrite(&bmpfh.bfType, sizeof(unsigned short), 1, file);
    fwrite(&bmpfh.bfSize, sizeof(unsigned int), 1, file);
    fwrite(&bmpfh.bfReserved1, sizeof(unsigned short), 1, file);
    fwrite(&bmpfh.bfReserved2, sizeof(unsigned short), 1, file);
    fwrite(&bmpfh.bfOffBits, sizeof(unsigned int), 1, file);

    //writing the bitmap info header:
    //this is already 4-byte aligned so no issues
    //depending on the compiler
    fwrite(&bmpih, sizeof(BITMAPINFOHEADER), 1, file);

    //padding?
    int bpp = 24;
    int padding = BitmapPadding(bpp, width);

    unsigned char *pads = NULL;

    if(padding > 0) {
        pads = new unsigned char[padding];
    }

    unsigned char tmp[3];

    int shiftG = 1;
    int shiftB = 2;

    if(channels==1) {
        shiftG = 0;
        shiftB = 0;
    }

    for(int j = (height - 1); j > -1; j--) {
        int cj = j * width;

        for(int i = 0; i < width; i++) {
            int c = (cj + i) * channels;
            //From RGB to BGR
            tmp[0] = data[c + shiftB];
            tmp[1] = data[c + shiftG];
            tmp[2] = data[c    ];
            fwrite(tmp, sizeof(unsigned char), 3, file);
        }

        if(padding > 0) {
            fwrite(pads, sizeof(unsigned char), padding, file);
        }
    }

    fclose(file);
    return true;
}

} // end namespace pic

#endif /* PIC_IO_BMP_HPP */

