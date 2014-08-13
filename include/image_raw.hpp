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

#ifndef PIC_IMAGE_RAW_HPP
#define PIC_IMAGE_RAW_HPP

#include <fstream>

#include "image.hpp"

//IO formats
#include "io/bmp.hpp"
#include "io/exr.hpp"
#include "io/hdr.hpp"
#include "io/pfm.hpp"
#include "io/ppm.hpp"
#include "io/pgm.hpp"
#include "io/tmp.hpp"
#include "io/tga.hpp"
#include "io/vol.hpp"
#include "util/io.hpp"
#include "util/raw.hpp"

#include "algorithms/demosaic.hpp"

namespace pic {

/**
 * @brief The ImageRAW class provides an I/O interface for Image
 */
class ImageRAW: public Image
{
protected:
    LDR_type typeLoad;

public:

    /**
     * @brief ImageRAW is the basic constructor.
     */
    ImageRAW();

    /**
     * @brief ImageRAW loads an ImageRAW from a file on the disk.
     * @param nameFile is the file name.
     * @param typeLoad is an option for LDR images only:
     * LT_NOR means that the input image values will be normalized in [0,1].
     * LT_NOR_GAMMA means that the input image values will be normalized in [0,1], and
     * gamma correction 2.2 will be removed.
     * LT_NONE means that image values are not modified during the loading.
     *
     * The default value is LT_NOR_GAMMA assuming that
     * we are storing normalized and linearized values in ImageRAW.
     */
    ImageRAW(std::string nameFile, LDR_type typeLoad);

    /**
     * @brief ImageRAW creates an ImageRAW with a given size from a mask.
     * @param mask is a mask of boolean values.
     * @param width is the horizontal size in pixels of mask.
     * @param height is the vertical size in pixels of mask.
     */
    ImageRAW(bool *mask, int width, int height);

    /**
     * @brief ImageRAW creates an ImageRAW with a given size and pointer to float data.
     * If the pointer is NULL data is allocated.
     * @param frames is the temporal size in pixels.
     * @param width is the horizontal size in pixels.
     * @param height is the vertical size in pixels.
     * @param channels is the number of color channels.
     * @param data is a pointer to pixels values; it should have frames * width * height * channles values.
     */
    ImageRAW(int frames, int width, int height, int channels, float *data);

    /**
     * @brief ImageRAW creates an ImageRAW with a given size.
     * @param width is the horizontal size in pixels.
     * @param height is the vertical size in pixels.
     * @param channels is the number of color channels.
     */
    ImageRAW(int width, int height, int channels);

    /**
     * @brief ImageRAW embeds an array of float inside an ImageRAW.
     * @param color is the pointer to an array of float values.
     * @param channels is the color's number of elements.
     */
    ImageRAW(float *color, int channels);

    ~ImageRAW();

    /**
     * @brief AllocateSimilarOne creates an ImageRAW with similar size
     * of the calling instance.
     * @return This returns an ImageRAW with the same size of the calling instance.
     */
    ImageRAW *AllocateSimilarOne();

    /**
     * @brief Clone creates a deep copy of the calling instance.
     * @return This returns a deep copy of the calling instance.
     */
    ImageRAW *Clone();

    //QImage interop
#ifdef PIC_QT
    /**
     * @brief ConvertFromQImage converts a QImage into an ImageRAW.
     * @param img is a QImage.
     * @param typeLoad is a converting option for LDR images:
     * LT_NOR means that the input image values will be normalized in [0,1].
     * LT_NOR_GAMMA means that the input image values will be normalized in [0,1], and
     * gamma correction 2.2 will be removed.
     * LT_NONE means that image values are not modified.
     * @param readerCounter.
     */
    void ConvertFromQImage(const QImage *img, LDR_type typeLoad, int readerCounter);

    /**
     * @brief ConvertToQImage
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
    QImage *ConvertToQImage(QImage *image, LDR_type type, int writerCounter,
                            float gamma);
#endif

    /**
     * @brief Read opens an ImageRAW from a file on the disk.
     * @param nameFile is the file name.
     * @param typeLoad is an option for LDR images only:
     * LT_NOR means that the input image values will be normalized in [0,1].
     * LT_NOR_GAMMA means that the input image values will be normalized in [0,1], and
     * gamma correction 2.2 will be removed.
     * LT_NONE means that image values are not modified.
     *
     * The default is LT_NOR_GAMMA assuming that
     * we are storing normalized and linearized values in ImageRAW.
     *
     * @return This returns true if the reading succeeds, false otherwise.
     */
    bool Read (std::string nameFile, LDR_type typeLoad);

    /**
     * @brief Write saves an ImageRAW into a file on the disk.
     * @param nameFile is the file name.
     * @param typeWrite is an option for LDR images only:
     * LT_NOR means that ImageRAW ha normalized values and the output image
     * values will be multiplied by 255 to have values in [0,255].
     * LT_NOR_GAMMA means that ImageRAW ha normalized and linearized values. The output image
     * values will be gamma corrected (2.2) and multiplied by 255 to have values in [0,255].
     * LT_NONE means that ImageRAW values are the same of the output.
     *
     * The default is LT_NOR_GAMMA assuming that
     * we are storing normalized and linearized values in ImageRAW.
     *
     * @param writerCounter is the frame that we want to write on the disk in the case ImageRAW is a video.
     * The default writerCounter value is 0.
     * @return This returns true if the writing succeeds, false otherwise.
     */
    bool Write(std::string nameFile, LDR_type typeWrite, int writerCounter);

    /**
     * @brief ReadRAW opens an ImageRAW from a RAW file with bayering on the disk.
     * @param nameFile is the image file name.
     * @param nameFileBlack is the black image file name.
     * @param type is the kind of RAW:
     * RAW_U8_RGGB: 8-bit raw values with RGGB bayer pattern.
     * RAW_U16_RGGB: 16-bit raw values with RGGB bayer pattern.
     * RAW_SINGLE: float raw values in RGB pixel format.
     * RAW_DOUBLE: double raw values in RGB pixel format.
     *
     * @param width is the horizontal size of the RAW image in pixels.
     * @param height is the vertical size of the RAW image in pixels.
     * @param channels is the number of color channels.
     * @return
     */
    bool ReadRAW(std::string nameFile, std::string nameFileBlack, RAW_type type,
                 int width, int height, int channels);

    /**
     * @brief WriteRAW saves an ImageRAW into a RAW file.
     * @param nameFile is the image file name.
     * @param type is the kind of RAW:
     * RAW_U8_RGGB: 8-bit raw values with RGGB bayer pattern.
     * RAW_U16_RGGB: 16-bit raw values with RGGB bayer pattern.
     * RAW_SINGLE: float raw values in RGB pixel format.
     * RAW_DOUBLE: double raw values in RGB pixel format.
     * @return
     */
    bool WriteRAW(std::string nameFile, RAW_type type);
};

PIC_INLINE ImageRAW::ImageRAW()
{
#ifdef PIC_ENABLE_OPEN_EXR
    dataEXR = NULL;
#endif
    typeLoad = LT_NONE;
    SetNULL();
}

PIC_INLINE ImageRAW::ImageRAW(float *color, int channels)
{
#ifdef PIC_ENABLE_OPEN_EXR
    dataEXR = NULL;
#endif

    typeLoad = LT_NONE;
    SetNULL();

    if(color!=NULL)
    {
        Allocate(1, 1, channels, 1);
        for(int i=0;i<channels;i++)
            data[i] = color[i];
    }
}

PIC_INLINE ImageRAW::~ImageRAW()
{
#ifdef PIC_ENABLE_OPEN_EXR

    if(dataEXR != NULL) {
        delete[] dataEXR;
    }

#endif

    Destroy();
}

PIC_INLINE ImageRAW::ImageRAW(std::string nameFile,
                              LDR_type typeLoad = LT_NOR_GAMMA)
{
    SetNULL();
    Read(nameFile, typeLoad);
}

PIC_INLINE ImageRAW::ImageRAW(int width, int height, int channels = 3)
{
    SetNULL();
    Allocate(width, height, channels, 1);
}

PIC_INLINE ImageRAW::ImageRAW(int frames, int width, int height, int channels,
                              float *data = NULL)
{
    SetNULL();

    if(data == NULL) {
        Allocate(width, height, channels, frames);
    } else {
        this->frames   = frames;
        this->channels = channels;
        this->width    = width;
        this->height   = height;

        AllocateAux();

        this->notOwned = true;
        this->data = data;
    }
}

PIC_INLINE ImageRAW::ImageRAW(bool *mask, int width, int height)
{
    ConvertFromMask(mask, width, height);
}

#ifdef PIC_QT
PIC_INLINE void ImageRAW::ConvertFromQImage(const QImage *img,
        LDR_type typeLoad = LT_NONE, int readerCounter = 0)
{
    bool bAlpha = img->hasAlphaChannel();

    if(img->depth() == 1) {
        channels = 1;
    } else {
        channels = img->depth() / 8;
    }
    
    if(bAlpha) { //check for alpha
        Allocate(img->width(), img->height(), channels, 1);
        alpha = channels;
    } else {
        if(img->depth() == 32 ) {
            Allocate(img->width(), img->height(), 3, 1);
        } else {
            Allocate(img->width(), img->height(), channels, 1);
        }
    }

    int tmpInd = tstride * (readerCounter % frames);

    if(dataUC != NULL) {
        delete[] dataUC;
    }

    unsigned int n = width * height * channels;
    dataUC = new unsigned char[n];

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
            QRgb col = img->pixel(j, i);

            int A = (col & 0xFF000000) >> 24;
            int R = (col & 0x00FF0000) >> 16;
            int G = (col & 0x0000FF00) >> 8;
            int B = (col & 0x000000FF);
            int ind = tmpInd + i * ystride + j * xstride;
            
            dataUC[ind         ] = R;
            dataUC[ind + shiftG] = G;
            dataUC[ind + shiftB] = B;
            
            if(bAlpha) {
                dataUC[ind + shiftA] = A;
            }
        }
     }


    /*
    const unsigned char *tmp = img->bits();
    for(int i=0; i<n; i+=channels) {
        int j = (i * 4) / channels;

        dataUC[i    ] = tmp[j + 2];
        dataUC[i + 1] = tmp[j + 1];
        dataUC[i + 2] = tmp[j    ];
        
        if(bAlpha) {
            dataUC[i + 3] = tmp[j + 3];
        }
    }*/

    ConvertLDR2HDR(dataUC, &data[tmpInd], width * height * channels, typeLoad);
}

PIC_INLINE QImage *ImageRAW::ConvertToQImage(QImage *image = NULL,
        LDR_type type = LT_NOR_GAMMA, int writerCoutner = 0, float gamma = 2.2f)
{

    QImage *ret = NULL;
    bool bAllocate = false;

    if(image != NULL) {
        bAllocate = (image->width() != width || image->height() != height);
    } else {
        bAllocate = true;
    }

    if(bAllocate) {
        ret = new QImage(width, height, QImage::Format_ARGB32);
    } else {
        ret = image;
    }

    float *tmpData = &data[writerCoutner % frames];

    dataUC = ConvertHDR2LDR(tmpData, dataUC, width * height * channels, type, gamma);

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

    #pragma omp parallel for

    for(int i = 0; i < height; i++) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            int c = (ind + j) * channels;
            ret->setPixel(j, i, qRgb(dataUC[c], dataUC[c + shifter[0]],
                                     dataUC[c + shifter[1]]));
        }
    }

    return ret;
}
#endif

PIC_INLINE bool ImageRAW::Read(std::string nameFile,
                               LDR_type typeLoad = LT_NOR_GAMMA)
{
    this->nameFile = nameFile;

    this->typeLoad = typeLoad;

    LABEL_IO_EXTENSION label;

    bool bReturn = false;

    //Reading an HDR format
    label = getLabelHDRExtension(nameFile);

    if(label != IO_NULL) {
        float *dataReader = NULL;
        float *tmp = NULL;

        if(data != NULL) {
            dataReader = &data[tstride * readerCounter];
#ifdef PIC_ENABLE_OPEN_EXR
            dataEXR = new Imf::Rgba[width * height];
#endif
        } else {
            channels = 3;
        }

        switch(label) {
        case IO_TMP:
            tmp = ReadTMP(nameFile, dataReader, width, height, channels, frames);
            break;

        case IO_HDR:
            tmp = ReadHDR(nameFile, dataReader, width, height);
            break;

        case IO_PFM:
            tmp = ReadPFM(nameFile, dataReader, width, height);
            break;

        case IO_EXR:
#ifdef PIC_ENABLE_OPEN_EXR
            tmp = ReadEXR(nameFile, dataReader, width, height, channels, dataEXR);
#endif
            break;

        case IO_VOL:
            tmp = ReadVOL(nameFile, dataReader, width, height, frames, channels);
            depth = frames;
            break;

        default:
            tmp = NULL;
        }

        if(tmp != NULL) {
            if(data == NULL) {
                data = tmp;

                if(frames <= 0) {
                    frames = 1;
                }
            }

            AllocateAux();
            bReturn = true;
        } else {
            bReturn = false;
        }
    } else {
        //Reading an LDR format
        label = getLabelLDRExtension(nameFile);
        unsigned char *dataReader = NULL;
        unsigned char *tmp = NULL;
        bool bExt = false;

        if(dataUC != NULL) {
            dataReader = dataUC;
        }

        switch(label) {
        case IO_BMP:
            tmp = ReadBMP(nameFile, dataReader, width, height, channels);
            break;

        case IO_PPM:
            tmp = ReadPPM(nameFile, dataReader, width, height, channels);
            break;

        case IO_PGM:
            tmp = ReadPGM(nameFile, dataUC, width, height, channels);
            break;

        case IO_TGA:
            tmp = ReadTGA(nameFile.c_str(), dataReader, width, height, channels);
            break;

        case IO_JPG:
            bExt = true;
            break;

        case IO_PNG:
            bExt = true;
            break;

        default:
            tmp = NULL;
        }

        if(bExt) { //External reader
#ifdef PIC_QT
            QImage tmpImg;
            bool ret = tmpImg.load(nameFile.c_str());

            if(!ret) {
                bReturn = false;
            } else {
                ConvertFromQImage(&tmpImg, typeLoad);
                bReturn = true;
            }
#else
            bReturn = false;
#endif
        } else {
            if(tmp != NULL) { //move the handle where it's trackable
                if(dataUC == NULL) {
                    dataUC = tmp;
                }
            }

            float *tmpFloat = NULL;

            if(data != NULL) {
                tmpFloat = &data[tstride * readerCounter];
            }

            float *tmpConv = ConvertLDR2HDR(tmp, tmpFloat, width * height * channels,
                                            typeLoad);

            if(tmpConv != NULL) {
                if(data == NULL) {
                    data = tmpConv;

                    if(frames <= 0) {
                        frames = 1;
                    }

                    AllocateAux();
                }

                bReturn = true;
            } else {
                bReturn = false;
            }
        }
    }

    readerCounter = (readerCounter + 1) % frames;
    return bReturn;
}

PIC_INLINE bool ImageRAW::Write(std::string nameFile, LDR_type typeWrite = LT_NOR_GAMMA,
                                int writerCounter = 0)
{
    if(!isValid()) {
        return false;
    }

    LABEL_IO_EXTENSION label;

    //Reading an HDR format
    label = getLabelHDRExtension(nameFile);

    if(label != IO_NULL) {
        float *dataWriter = NULL;

        if((writerCounter > 0) && (writerCounter < frames)) {
            dataWriter = &data[tstride * writerCounter];
        } else {
            dataWriter = data;
        }

        bool ret = false;

        switch(label) {
        case IO_TMP:
            ret = WriteTMP(nameFile, dataWriter, width, height, channels, frames);
            break;

        case IO_HDR:
            ret = WriteHDR(nameFile, dataWriter, width, height, channels);
            break;

        case IO_PFM:
            ret = WritePFM(nameFile, dataWriter, width, height, channels);
            break;

        case IO_EXR:
#ifdef PIC_ENABLE_OPEN_EXR
            ret = WriteEXR(nameFile, dataWriter, width, height, channels);
#endif
            break;

        case IO_VOL:
            ret = WriteVOL(nameFile, dataWriter, width, height, frames, channels);
            break;

        default:
            ret = false;
        }

        return ret;
    } else {
        //Writing an LDR format
        label = getLabelLDRExtension(nameFile);

        bool bExt = (label == IO_JPG) || (label == IO_PNG);

        if(bExt) {
#ifdef PIC_QT
            QImage *tmpImg = ConvertToQImage(NULL, typeWrite);
            tmpImg->save(nameFile.c_str());

            if(tmpImg != NULL) {
                delete tmpImg;
            }

            return true;
#else
            return false;
#endif
        } else {
            float *dataWriter = NULL;

            if((writerCounter > 0) && (writerCounter < frames)) {
                dataWriter = &data[tstride * writerCounter];
            } else {
                dataWriter = data;
            }

            unsigned char *tmp = ConvertHDR2LDR(dataWriter, dataUC,
                                                width * height * channels, typeWrite);

            if(dataUC == NULL) {
                dataUC = tmp;
            }

            switch(label) {
            case IO_BMP:
                return WriteBMP(nameFile, dataUC, width, height, channels);
                break;

            case IO_TGA:
                //values are stored with a vertical flip
                BufferFlipV(dataUC, width, height, channels, 1);

                //values needs to be stored as BGR
                BufferBGRtoRGB(dataUC, width, height, channels, 1);

                return WriteTGA(nameFile, dataUC, width, height, channels);
                break;

            case IO_PPM:
                return WritePPM(nameFile, dataUC, width, height, channels);
                break;

            case IO_PGM:
                return WritePGM(nameFile, dataUC, width, height, channels);
                break;

            default:
                return false;
            }
        }
    }
}

PIC_INLINE ImageRAW *ImageRAW::AllocateSimilarOne()
{
    ImageRAW *ret = new ImageRAW(frames, width, height, channels);
    ret->flippedEXR = flippedEXR;
    return ret;
}

PIC_INLINE ImageRAW *ImageRAW::Clone()
{
    ImageRAW *ret = new ImageRAW(frames, width, height, channels);
    ret->flippedEXR = flippedEXR;
    memcpy(ret->data, data, width * height * channels * sizeof(float));
    return ret;
}

PIC_INLINE bool ImageRAW::ReadRAW(std::string nameFile,
                                  std::string nameFileBlack, RAW_type type, int width, int height,
                                  int channels = 1)
{

    this->flippedEXR = true;

    int tot = width * height * channels;

    switch(type) {
    //Unsigned 16-bit RAW data using Bayer filter (RGGB)
    case RAW_U16_RGGB: {
        //Read RAW file
        RAW<unsigned short> dataU16(nameFile, tot);

        if(!nameFileBlack.empty()) {
            #ifdef PIC_DEBUG
                printf("Removing sensor noise\n");
            #endif

            RAW<unsigned short> black(nameFileBlack, tot);
            dataU16.Subtraction(black);
        }

        //Convert into float + normalisation
        float *dataRAW = new float[tot];
        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            dataRAW[i] = float(dataU16.data[i]) / (65535.0f * exposure);
        }

        //Debayering
        Allocate(width, height, 3, 1);

        Demosaic(new Image(1, width, height, 1, dataRAW), this);

        //deallocate tmp unsigned short
        dataU16.Release();
        delete[] dataRAW;
    }
    break;

    //Unsigned 8-bit RAW data using Bayer filter (RGGB)
    case RAW_U8_RGGB: {
        //Read RAW file
        RAW<unsigned char> dataU8(nameFile, tot);

        if(!nameFileBlack.empty()) {
            #ifdef PIC_DEBUG
                printf("Removing sensor noise\n");
            #endif
            RAW<unsigned char> black(nameFileBlack, tot);
            dataU8.Subtraction(black);
        }

        //Convert into float + normalisation
        float *dataRAW = new float[tot];
        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            dataRAW[i] = float(dataU8.data[i]) / (255.0f * exposure);
        }

        //Debayering
        Allocate(width, height, 3, 1);
        Demosaic(new Image(1, width, height, 1, dataRAW), this);
        //deallocate tmp unsigned short
        dataU8.Release();
        delete[] dataRAW;
    }
    break;

    //float RAW data in RGB format
    case RAW_FLOAT: {
        RAW<float> dataSingle(nameFile, tot);
        Allocate(width, height, channels, 1);

        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            data[i] = float(dataSingle.data[i]);
        }
    } break;

    //double RAW data in RGB format
    case RAW_DOUBLE: {
        RAW<double> dataDouble(nameFile, tot);
        Allocate(width, height, channels, 1);

        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            data[i] = float(dataDouble.data[i]);
        }

        dataDouble.Release();
    }
    break;
    }

    return true;
}

bool ImageRAW::WriteRAW(std::string nameFile, RAW_type type)
{
    if(data == NULL){
        return false;
    }

    int tot = width * height * channels;

    switch(type) {
    //Unsigned 16-bit RAW data using Bayer filter (RGGB)
    case RAW_U16_RGGB: {
        RAW<unsigned short> dataU16(tot);

        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            dataU16.data[i] = int(data[i] * 65535.0f);
        }

        dataU16.Write(nameFile);
    }
    break;

    //Unsigned 8-bit RAW data using Bayer filter (RGGB)
    case RAW_U8_RGGB: {
        RAW<unsigned char> dataU8(tot);

        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            dataU8.data[i] = int(data[i] * 255.0f);
        }

        dataU8.Write(nameFile);
    }
    break;

    //float RAW data in RGB format
    case RAW_FLOAT: {
        RAW<float> dataFloat(tot);

        dataFloat.Write(nameFile);
    } break;

    //double RAW data in RGB format
    case RAW_DOUBLE: {
        RAW<double> dataDouble(tot);

        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            dataDouble.data[i] = double(data[i]);
        }

        dataDouble.Write(nameFile);
    }
    break;
    }

    return true;
}

} // end namespace pic

#endif /* PIC_IMAGE_RAW_HPP */

