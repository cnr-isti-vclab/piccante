/*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

#ifndef IMAGE_QIMAGE_INTEROP_HPP
#define IMAGE_QIMAGE_INTEROP_HPP

#include "qt_includes.hpp"

#include "piccante.hpp"

#ifdef PIC_QT

/**
 * @brief ConvertFromQImage converts a QImage into an Image.
 * @param img is a QImage.
 * @param typeLoad is a converting option for LDR images:
 * LT_NOR means that the input image values will be normalized in [0,1].
 * LT_NOR_GAMMA means that the input image values will be normalized in [0,1], and
 * gamma correction 2.2 will be removed.
 * LT_NONE means that image values are not modified.
 * @param readerCounter.
 */
pic::Image *ImageConvertFromQImage(QImage *imgIn,
                                   pic::Image *imgOut,
                                   pic::LDR_type typeLoad = pic::LT_NONE,
                                   int readerCounter = 0)
{
    bool bAlpha = imgIn->hasAlphaChannel();

    int channels;

    if(imgIn->depth() == 1) {
        channels = 1;
    } else {
        channels = imgIn->depth() / 8;
    }

    int width = imgIn->width();
    int height = imgIn->height();

    //compute channels
    if(!bAlpha) {
        if(imgIn->depth() == 32 ) {
            channels = 3;
        }
    }

    if(imgOut == NULL) {
        imgOut = new pic::Image(width, height, channels);
    } else {
        imgOut->allocate(width, height, channels, 1);
    }

    int frames = imgOut->frames;

    int tmpInd = imgOut->tstride * (readerCounter % frames);

    if(imgOut->dataUC != NULL) {
        delete[] imgOut->dataUC;
    }

    unsigned int n = width * height * channels;
    imgOut->dataUC = new unsigned char[n];

    //NOTE: this code works but it is slow!
    int shiftG = 0;
    int shiftB = 0;
    int shiftA = 1;

    if(channels == 3 || channels == 4) {
        shiftG = 1;
        shiftB = 2;
        shiftA = 3;
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            QRgb col = imgIn->pixel(j, i);

            int A = (col & 0xFF000000) >> 24;
            int R = (col & 0x00FF0000) >> 16;
            int G = (col & 0x0000FF00) >> 8;
            int B = (col & 0x000000FF);
            int ind = tmpInd + i * imgOut->ystride + j * imgOut->xstride;

            imgOut->dataUC[ind         ] = R;
            imgOut->dataUC[ind + shiftG] = G;
            imgOut->dataUC[ind + shiftB] = B;

            if(bAlpha) {
                imgOut->dataUC[ind + shiftA] = A;
            }
        }
     }

    pic::convertLDR2HDR(imgOut->dataUC, &imgOut->data[tmpInd], n, typeLoad);

    return imgOut;
}

/**
 * @brief ImageConvertToQImage
 * @param image
 * @param typeLoad is an option for LDR images only:
 * LT_NOR means that the input image values will be normalized in [0,1].
 * LT_NOR_GAMMA means that the input image values will be normalized in [0,1], and
 * gamma correction 2.2 will be removed.
 * LT_NONE means that image values are not modified.
 * @param writerCounter.
 * @param gamma.
 * @return
 */
QImage *ImageConvertToQImage(pic::Image *imgIn,
                             QImage *imgOut = NULL,
                             pic::LDR_type type = pic::LT_NOR_GAMMA,
                             int writerCoutner = 0, float gamma = 2.2f)
{
    if(imgIn == NULL) {
        return imgOut;
    }

    QImage *ret = NULL;
    bool bAllocate = false;

    int width = imgIn->width;
    int height = imgIn->height;
    int channels = imgIn->channels;
    int frames = imgIn->frames;

    if(imgOut != NULL) {
        bAllocate = (imgOut->width() != width || imgOut->height() != height);
    } else {
        bAllocate = true;
    }

    if(bAllocate) {
        ret = new QImage(width, height, QImage::Format_ARGB32);
    } else {
        ret = imgOut;
    }

    float *tmpData = &imgIn->data[writerCoutner % frames];

    imgIn->dataUC = pic::convertHDR2LDR(tmpData, imgIn->dataUC, width * height * channels, type, gamma);

    int shifter[2];
    shifter[0] = 1;
    shifter[1] = 2;

    switch(channels) {
    case 1: {
        shifter[0] = 0;
        shifter[1] = 0;
    }
    break;

    case 2: {
        shifter[0] = 1;
        shifter[1] = 1;
    }
    break;
    }

    for(int i = 0; i < height; i++) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            int c = (ind + j) * channels;
            ret->setPixel(j, i, qRgb(imgIn->dataUC[c],
                                     imgIn->dataUC[c + shifter[0]],
                                     imgIn->dataUC[c + shifter[1]]));
        }
    }

    return ret;
}

/**
 * @brief ImageWrite
 * @param imgIn
 * @param nameFile
 * @param typeWrite
 * @return
 */
bool ImageWrite(pic::Image *imgIn, std::string nameFile, pic::LDR_type typeWrite = pic::LT_NOR_GAMMA)
{
    if(imgIn == NULL) {
        return false;
    }

    bool bWrite = imgIn->Write(nameFile, typeWrite, 0);

    if(!bWrite) {
        QImage *tmpImg = ImageConvertToQImage(imgIn, NULL, typeWrite);
        tmpImg->save(nameFile.c_str());

        if(tmpImg != NULL) {
            delete tmpImg;
        }

        return true;
    } else {
        return bWrite;
    }
}

/**
 * @brief ImageRead
 * @param nameFile
 * @param typeLoad
 * @return
 */
pic::Image *ImageRead(std::string nameFile, pic::Image *imgOut, pic::LDR_type typeLoad = pic::LT_NOR_GAMMA)
{
    if(imgOut != NULL) {
        bool bRead = imgOut->Read(nameFile, typeLoad);

        if(!bRead) {
            QImage imgIn;
            imgIn.load(nameFile.c_str());
            imgOut = ImageConvertFromQImage(&imgIn, imgOut, typeLoad);
        }
    }

    return imgOut;
}

#endif /* PIC_QT */


#endif /* IMAGE_QIMAGE_INTEROP_HPP */
