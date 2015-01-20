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

#ifndef PIC_IMAGE_HPP
#define PIC_IMAGE_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <math.h>
#include <float.h>
#include <limits>

#include "base.hpp"
#include "util/compability.hpp"
#include "util/bbox.hpp"
#include "util/buffer.hpp"
#include "util/low_dynamic_range.hpp"

#include "util/math.hpp"

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
#include "io/exr.hpp"
#include "util/io.hpp"

namespace pic {

/**
 * @brief The Image class stores an image as buffer of float.
 */
class Image
{
protected:

    /**
     * @brief Destroy makes allocated buffers free.
     */
    void Destroy();

    /**
     * @brief Allocate allocates memory for the pixel buffer.
     * @param width is the number of horizontal pixels.
     * @param height is the number of vertical pixels.
     * @param channels is the number of color channels.
     * @param frames is the number of temporal pixels.
     */
    void Allocate(int width, int height, int channels, int frames);

    /**
     * @brief AllocateAux computes extra information after allocation;
     * e.g. strides.
     */
    void AllocateAux();

    /**
     * @brief SetNULL sets buffers values to NULL.
     */
    void SetNULL();

    //applied rendering values
    bool flippedEXR;
    int  readerCounter;
    bool notOwned;

    BBox fullBox;

    LDR_type typeLoad;

public:
    float exposure;
    int width, height, channels, frames, depth, alpha;

    int tstride, ystride, xstride;

    float widthf, width1f, heightf, height1f, framesf, frames1f;

    std::string nameFile;

    /**
     * @brief data is the main buffer where pixel values are stored.
     */
    float *data;

    /**
     * @brief dataTMP is a temporary buffer used for robust statistics.
     */
    float *dataTMP;

    /**
     * @brief dataUC is a buffer for rendering 8-bit images.
     */
    unsigned char *dataUC;

    /**
     * @brief dataRGBE is a buffer for rendering RGBE encoded images.
     */
    unsigned char *dataRGBE;

    //Half-precision encoding
#ifdef PIC_ENABLE_OPEN_EXR
    Imf::Rgba *dataEXR;
#endif

    /**
     * @brief the basic construct of an Image
     */
    Image();

    /**
     * @brief Image embeds an existing image in the new image.
     * @param imgIn is the input image to embed.
     * @param deepCopy enables a deep copy of img into this.
     */
    Image(Image *imgIn, bool deepCopy);

    /**
    * @brief Image loads an Image from a file on the disk.
    * @param nameFile is the file name.
    * @param typeLoad is an option for LDR images only:
    * LT_NOR means that the input image values will be normalized in [0,1].
    * LT_NOR_GAMMA means that the input image values will be normalized in [0,1], and
    * gamma correction 2.2 will be removed.
    * LT_NONE means that image values are not modified during the loading.
    *
    * The default value is LT_NOR_GAMMA assuming that
    * we are storing normalized and linearized values in Image.
    */
    Image(std::string nameFile, LDR_type typeLoad);

    /**
    * @brief Image creates an Image with a given size.
    * @param width is the horizontal size in pixels.
    * @param height is the vertical size in pixels.
    * @param channels is the number of color channels.
    */
    Image(int width, int height, int channels);

   /**
   * @brief Image embeds an array of float inside an Image.
   * @param color is the pointer to an array of float values.
   * @param channels is the color's number of elements.
   */
    Image(float *color, int channels);

    /**
     * @brief Image is a constructor which initializes an image defined by
     * the input properties.
     * @param frames is the number of temporal pixels.
     * @param width is the number of horizontal pixels.
     * @param height is the number of vertical pixels.
     * @param channels is the number of color channels.
     * @param data is a buffer of size frames * width * height * channels.
     * If it is empty (set to NULL) a new buffer will be created.
     */
    Image(int frames, int width, int height, int channels, float *data);

    /**
    * @brief Image destructor. This deallocates: data, dataUC, dataRGBE, dataTMP,
    */
    ~Image();

    /**
     * @brief CopySubImage copies imgIn in the current image.
     * The current image is written from (startX, startY).
     * @param imgIn the image to be copied.
     * @param startX is the horizontal coordinate in pixels.
     * @param startY is the vertical coordinate in pixels.
     */
    void CopySubImage(Image *imgIn, int startX, int startY);

    /**
     * @brief ScaleCosine multiplies the current image by the vertical cosine
     * assuming a longitude-latitude image.
     */
    void ScaleCosine();

    /**
     * @brief FlipH flips horizontally the current image.
     */
    void FlipH()
    {
        BufferFlipH<float>(data, width, height, channels, frames);
    }

    /**
     * @brief FlipV flips vertically the current image.
     */
    void FlipV()
    {
        BufferFlipV<float>(data, width, height, channels, frames);
    }

    /**
     * @brief FlipHV flips horizontally and vertically the current image.
     */
    void FlipHV()
    {
        BufferFlipH<float>(data, width, height, channels, frames);
        BufferFlipV<float>(data, width, height, channels, frames);
    }

    /**
     * @brief FlipVH flips vertically and horizontally the current image.
     */
    void FlipVH()
    {
        BufferFlipV<float>(data, width, height, channels, frames);
        BufferFlipH<float>(data, width, height, channels, frames);
    }

    /**
     * @brief Rotate90CCW rotates 90 degrees counter-clockwise the current image.
     */
    void Rotate90CCW()
    {
        BufferRotate90CCW<float>(data, width, height, channels);
        AllocateAux();
    }

    /**
     * @brief Rotate90CW rotates 90 degrees clockwise the current image.
     */
    void Rotate90CW()
    {
        BufferRotate90CW<float>(data, width, height, channels);
        AllocateAux();
    }

    /**
     * @brief SetZero sets data to 0.0f.
     */
    void SetZero();

    /**
     * @brief SetRand sets data to random values.
     */
    void SetRand();

    /**
     * @brief isValid checks if the current image is valid, which means if they
     * have an allocated buffer or not.
     * @return This function return true if the current Image is allocated,
     * otherwise false.
     */
    bool isValid();

    /**
     * @brief SimilarType checks if the current image is similar to img;
     * i.e. if they have the same width, height, frames, and channels.
     * @param img is an input image
     * @return This function returns true if the two images are similar,
     * otherwise false.
     */
    bool SimilarType(const Image *img);

    /**
     * @brief Assign
     * @param imgIn
     */
    void Assign(const Image *imgIn);

    /**
     * @brief Blend
     * @param img
     * @param weight
     */
    void Blend(Image *img, Image *weight);

    /**
     * @brief Minimum is the minimum operator for Image.
     * @param img is a and Image and the operand. This
     * and the current Image need to have the same width,
     * height, and color channels.
     */
    void Minimum(Image *img);

    /**
     * @brief Maximum is the maximum operator for Image.
     * @param img is a and Image and the operand. This
     * and the current Image need to have the same width,
     * height, and color channels.
     */
    void Maximum(Image *img);

    /**
     * @brief ApplyFunction is an operator that applies
     * an input function to all values in data.
     */
    void ApplyFunction(float(*func)(float));

    /**
     * @brief getMaxVal computes the maximum value for the current Image.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getMaxVal(BBox *box, float *ret);

    /**
     * @brief getMinVal computes the minimum value for the current Image.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getMinVal(BBox *box, float *ret);

    /**
     * @brief getLogMeanVal computes the log mean for the current Image.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getLogMeanVal(BBox *box, float *ret);

    /**
     * @brief getSumVal sums values for the current Image.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getSumVal(BBox *box, float *ret);

    /**
     * @brief getMeanVal computes the mean for the current Image.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getMeanVal(BBox *box, float *ret);

    /**
     * @brief getMomentsVal computes the moments at pixel (x0, y0).
     * @param x0 is the horizontal coordinate.
     * @param y0 is the vertical coordinate.
     * @param radius is the radius of the patch.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getMomentsVal(int x0, int y0, int radius, float *ret);

    /**
     * @brief getVarianceVal computes the variance for the current Image.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getVarianceVal(float *meanVal, BBox *box, float *ret);

    /**
     * @brief getCovMtxVal computes the convariance matrix for the current Image.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getCovMtxVal(float *meanVal, BBox *box, float *ret);

    /**
     * @brief getMedVal computes the n-th value given a percentile.
     * @param perCent is the percentile.
     * @return This function returns the n-value given a percentile.
     */
    float getMedVal(float perCent);

    /**
     * @brief getGT finds the first value greater than val.
     * @param val is the reference value.
     * @return This function returns the first value greater than val.
     */
    float getGT(float val);

    /**
     * @brief sort sorts values in data and stores them into dataTMP.
     */
    void sort();

    /**
     * @brief getdataUC
     * @return
     */
    unsigned char *getdataUC()
    {
        return dataUC;
    }

    /**
     * @brief size computes the number of values.
     * @return This function returns the number of values.
     */
    int size() const
    {
        return height * width * channels * frames;
    }

    /**
     * @brief nPixels computes the number of pixels.
     * @return This function returns the number of pixels.
     */
    int nPixels() const
    {
        return height * width * frames;
    }

    /**
     * @brief checkCoordinates checks (x, y, z) coordinates) if they are valid or not.
     * @param x is the horizontal coordinate.
     * @param y is the vertical coordinate.
     * @param z is the temporal coordinate.
     * @return This function returns true if the coordinates are inside the bounding
     * box of the Image.
     */
    bool checkCoordinates(int x, int y, int z = 0)
    {
        return (x > -1) && (x < width) && (y > -1) && (y < height) &&
               (z > -1) && (z < frames);
    }

    /**
     * @brief ConvertFromMask converts a boolean mask into an Image. true is mapped
     * to 1.0f, and false is mapped to 0.0f.
     * @param mask is a buffer of boolean values with (width * height) elements.
     * @param width is the horizontal number of pixels.
     * @param height is the vertical number of pixels.
     */
    void ConvertFromMask(bool *mask, int width, int height);

    /**
     * @brief ConvertToMask converts an Image into a boolean mask.
     * @param color
     * @param threshold
     * @param cmp
     * @param mask
     * @return
     */
    bool *ConvertToMask(float *color, float threshold, bool cmp, bool *mask);

    /**
     * @brief getFlippedEXR returns the flippedEXR flag.
     * @return This function returns the flippedEXR flag.
     */
    bool getFlippedEXR()
    {
        return flippedEXR;
    }

    /**
     * @brief removeSpecials removes NaN and +/-Inf values and sets
     * them to 0.0f.
     */
    void removeSpecials();

    /**
     * @brief clamp set data values in the range [a,b]
     * @param a the minimum value.
     * @param b the maximum value.
     */
    void clamp(float a, float b);

    /**
     * @brief CalculateStrides computes the strides values
     * for pixels, lines and frames.
     */
    void CalculateStrides()
    {
        tstride = channels * height * width;
        ystride = width * channels;
        xstride = channels;
    }

    /**
     * @brief operator () returns a pointer to a pixel at (x, y, t)
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @param t is the temporal coordinate in pixels
     * @return This function returns a pointer to data at location (x, y, t).
     */
    float *operator()(int x, int y, int t)
    {
        return data + CLAMP(t, frames) * tstride +
                      CLAMP(x, width)  * xstride +
                      CLAMP(y, height) * ystride;
    }

    /**
     * @brief operator () returns a pointer to a pixel at (x, y)
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @return This function returns a pointer to data at location (x, y).
     */
    float *operator()(int x, int y)
    {
        return data + CLAMP(x, width)  * xstride +
                      CLAMP(y, height) * ystride;
    }

    /**
     * @brief operator () returns a pointer to a pixel at (x, y)
     * with normalized coordinates (values in [0, 1]).
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @return This function returns a pointer to data at location (x, y).
     */
    float *operator()(float x, float y)
    {
        int ix = CLAMP(int(floorf(x * width)), width);
        int iy = CLAMP(int(floorf(y * height)), height);
        return data + ix * xstride + iy * ystride;
    }

    /**
     * @brief NormalizedCoordinates computes normalized coordinates
     * (nx, ny) of (x, y).
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @param nx is the horizontal coordinate in [0, 1]
     * @param ny is the vertical coordinate in [0, 1]
     */
    void NormalizedCoordinates(int x, int y, float &nx, float &ny)
    {
        nx = float(x) / width1f;
        ny = float(y) / height1f;
    }

    /**
     * @brief Address calculates a memory address from (x, y)
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @return This function returns the memory address for (x, y)
     */
    int Address(int x, int y)
    {
        x = CLAMP(x, width);
        y = CLAMP(y, height);

        return x * xstride + y * ystride;
    }

    /**
     * @brief Address calculates a memory address from (x, y, t)
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @param t is the temporal coordinate in pixels
     * @return This function returns the memory address for (x, y, t)
     */
    int Address(int x, int y, int t)
    {
        x = CLAMP(x, width);
        y = CLAMP(y, height);
        t = CLAMP(t, frames);

        return x * xstride + y * ystride + t * tstride;
    }

    /**
     * @brief ReverseAddress computes (x, y) given a memory address
     * @param ind is the memory input address
     * @param x is the output horizontal coordinate for ind
     * @param y is the output vertical coordinate for ind
     */
    void ReverseAddress(int ind, int &x, int &y)
    {
        ind = ind / channels;
        y   = ind / width;
        x   = ind - (y * width);
    }

    /**
     * @brief AllocateSimilarOne creates an Image with similar size
     * of the calling instance.
     * @return This returns an Image with the same size of the calling instance.
     */
    Image *AllocateSimilarOne();

    /**
     * @brief Clone creates a deep copy of the calling instance.
     * @return This returns a deep copy of the calling instance.
     */
    Image *Clone() const;

    //QImage interop
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
     * @brief Read opens an Image from a file on the disk.
     * @param nameFile is the file name.
     * @param typeLoad is an option for LDR images only:
     * LT_NOR means that the input image values will be normalized in [0,1].
     * LT_NOR_GAMMA means that the input image values will be normalized in [0,1], and
     * gamma correction 2.2 will be removed.
     * LT_NONE means that image values are not modified.
     *
     * The default is LT_NOR_GAMMA assuming that
     * we are storing normalized and linearized values in Image.
     *
     * @return This returns true if the reading succeeds, false otherwise.
     */
    bool Read (std::string nameFile, LDR_type typeLoad);

    /**
     * @brief Write saves an Image into a file on the disk.
     * @param nameFile is the file name.
     * @param typeWrite is an option for LDR images only:
     * LT_NOR means that Image ha normalized values and the output image
     * values will be multiplied by 255 to have values in [0,255].
     * LT_NOR_GAMMA means that Image ha normalized and linearized values. The output image
     * values will be gamma corrected (2.2) and multiplied by 255 to have values in [0,255].
     * LT_NONE means that Image values are the same of the output.
     *
     * The default is LT_NOR_GAMMA assuming that
     * we are storing normalized and linearized values in Image.
     *
     * @param writerCounter is the frame that we want to write on the disk in the case Image is a video.
     * The default writerCounter value is 0.
     * @return This returns true if the writing succeeds, false otherwise.
     */
    bool Write(std::string nameFile, LDR_type typeWrite, int writerCounter);


    /**
     * @brief ChangeOwnership
     * @param notOwned
     */
    void ChangeOwnership(bool notOwned)
    {
        this->notOwned = notOwned;
    }

    /**
     * @brief operator =
     * @param a
     */
    void operator =(const Image &a);

    /**
     * @brief operator =
     * @param a
     */
    void operator =(const float &a);

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const float &a);

    /**
     * @brief operator +
     * @param a
     * @return it returns (this + a)
     */
    Image operator +(const float &a) const;

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const Image &a);

    /**
     * @brief operator +
     * @param a
     * @return it returns (this + a)
     */
    Image operator +(const Image &a) const;

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const float &a);

    /**
     * @brief operator *
     * @param a
     * @return it returns (this * a)
     */
    Image operator *(const float &a) const;

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const Image &a);

    /**
     * @brief operator *
     * @param a
     * @return it returns (this * a)
     */
    Image operator *(const Image &a) const;

    /**
     * @brief operator -=
     * @param a
     */
    void operator -=(const float &a);

    /**
     * @brief operator -
     * @param a
     * @return it returns (this - a)
     */
    Image operator -(const float &a) const;

    /**
     * @brief operator -=
     * @param a
     */
    void operator -=(const Image &a);

    /**
     * @brief operator -
     * @param a
     * @return it returns (this - a)
     */
    Image operator -(const Image &a) const;

    /**
     * @brief operator /=
     * @param a
     */
    void operator /=(const float &a);

    /**
     * @brief operator /
     * @param a
     * @return it returns (this / a)
     */
    Image operator /(const float &a) const;

    /**
     * @brief operator /=
     * @param a
     */
    void operator /=(const Image &a);

    /**
     * @brief operator /
     * @param a
     * @return it returns (this / a)
     */
    Image operator /(const Image &a) const;
};

PIC_INLINE void Image::SetNULL()
{
    nameFile = "";
    notOwned = false;

    alpha = -1;
    tstride = -1;
    ystride = -1;
    xstride = -1;
    width = -1;
    height = -1;
    frames = -1;
    depth = -1;
    channels = -1;

    dataTMP = NULL;
    data = NULL;
    dataUC = NULL;
    dataRGBE = NULL;
    typeLoad = LT_NONE;

#ifdef PIC_ENABLE_OPEN_EXR
    dataEXR = NULL;
#endif

    flippedEXR = false;

    readerCounter = 0;
    exposure = 1.0f;
}

PIC_INLINE Image::Image()
{
    SetNULL();
}

PIC_INLINE Image::Image(Image *imgIn, bool deepCopy = true)
{
    SetNULL();
    
    if(imgIn == NULL) {
        return;
    }

    if(deepCopy) {
        Assign(imgIn);
    } else {
        width = imgIn->width;
        height = imgIn->height;
        channels = imgIn->channels;
        frames = imgIn->frames;
        data = imgIn->data;

        notOwned = true;
        exposure = imgIn->exposure;
        nameFile = imgIn->nameFile;
        flippedEXR = imgIn->flippedEXR;
        typeLoad = imgIn->typeLoad;

        AllocateAux();
    }

}

PIC_INLINE Image::Image(int width, int height, int channels = 3)
{
    SetNULL();
    Allocate(width, height, channels, 1);
}

PIC_INLINE Image::Image(int frames, int width, int height, int channels,
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
        this->notOwned = true;
        this->data = data;

        AllocateAux();
    }
}

PIC_INLINE Image::Image(std::string nameFile, LDR_type typeLoad = LT_NOR_GAMMA)
{
    SetNULL();
    Read(nameFile, typeLoad);
}

PIC_INLINE Image::Image(float *color, int channels)
{
    typeLoad = LT_NONE;
    SetNULL();

    if(color != NULL) {
        Allocate(1, 1, channels, 1);
        memcpy(data, color, channels);
    }
}

PIC_INLINE Image::~Image()
{
    Destroy();
}

PIC_INLINE void Image::Destroy()
{
    //Destroy the allocated resources
    if(data != NULL && (!notOwned)) {
        delete[] data;
    }

    if(dataTMP != NULL) {
        delete[] dataTMP;
    }

    if(dataUC != NULL) {
        delete[] dataUC;
    }

    if(dataRGBE != NULL) {
        delete[] dataRGBE;
    }

    #ifdef PIC_ENABLE_OPEN_EXR
        if(dataEXR != NULL) {
            delete[] dataEXR;
        }
    #endif

    SetNULL();
}

PIC_INLINE void Image::Allocate(int width, int height, int channels, int frames)
{
    if(width < 1 || height < 1 || channels < 1 || frames < 1) {
        #ifdef PIC_DEBUG
            printf("Image::Allocate: not a valid image to be allocated.\n");
        #endif
        return;
    }

    if(this->width > 0 && this->height > 0 && this->channels > 0 &&
       this->frames > 0 && data != NULL) {
        #ifdef PIC_DEBUG
            printf("Image::Allocate: already allocated image.\n");
        #endif
        return;
    }

    this->frames = frames;
    this->channels = channels;
    this->width = width;
    this->height = height;
    this->notOwned = false;

    data = new float [height * width * channels * frames];

    AllocateAux();
}

PIC_INLINE void Image::AllocateAux()
{
    this->fullBox.SetBox(0, width, 0, height, 0, frames, width,
                         height, frames);

    this->depth    = frames;
    this->widthf   = float(width);
    this->width1f  = float(width - 1);
    this->heightf  = float(height);
    this->height1f = float(height - 1);
    this->framesf  = float(frames);
    this->frames1f = float(frames -1);

    CalculateStrides();
}

PIC_INLINE void Image::Assign(const Image *imgIn)
{
    if(imgIn == NULL) {
        return;
    }

    if(!SimilarType(imgIn)) {
        Destroy();
        Allocate(imgIn->width, imgIn->height, imgIn->channels, imgIn->frames);
    }

    exposure = imgIn->exposure;
    nameFile = imgIn->nameFile;
    flippedEXR = imgIn->flippedEXR;
    typeLoad = imgIn->typeLoad;

    memcpy(data, imgIn->data, frames * width * height * channels * sizeof(float));
}

PIC_INLINE void Image::clamp(float a = 0.0f, float b = 1.0f)
{
    int n = size();

    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        data[i] = CLAMPi(data[i], a, b);
    }
}

PIC_INLINE void Image::removeSpecials()
{
    int n = size();
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        float val = data[i];

        if(isnan(val) || isinf(val)) {
            data[i] = 0.0f;
        }
    }
}

PIC_INLINE bool Image::SimilarType(const Image *img)
{
    if(img == NULL) {
        return false;
    }

    bool ret =	(width		==	img->width) &&
                (height		==	img->height) &&
                (frames		==	img->frames) &&
                (channels	==	img->channels) &&
                (flippedEXR	==	img->flippedEXR);

#ifdef PIC_DEBUG

    if(!ret) {
        printf("\nImage::SimilarType: ERROR The two compared images are not similar.\n");
    }

#endif

    return ret;
}

PIC_INLINE bool Image::isValid()
{
    return (width > 0) && (height > 0) && (channels > 0) && (frames > 0) &&
           (data != NULL);
}

PIC_INLINE void Image::CopySubImage(Image *imgIn, int startX, int startY)
{
    if(imgIn == NULL) {
        return;
    }

    if(imgIn->channels != channels) {
        return;
    }

    //Checking bounds
    int sX, sY, eX, eY, dX, dY, shiftX, shiftY;

    //Start
    sX = MIN(startX, width);
    sX = MAX(sX, 0);

    sY = MIN(startY, height);
    sY = MAX(startY, 0);

    dX = sX - startX;

    if(dX < 0) {
        shiftX = dX;
    } else {
        shiftX = -sX;
    }

    //End
    eX = MIN(startX + imgIn->width, width);
    eX = MAX(eX, 0);

    eY = MIN(startY + imgIn->height, height);
    eY = MAX(eY, 0);

    dY = sY - startY;

    if(dY < 0) {
        shiftY = dY;
    } else {
        shiftY = -sY;
    }

    #pragma omp parallel for

    for(int j = sY; j < eY; j++) {

        for(int i = sX; i < eX; i++) {
            float *curr_data = (*this)(i, j);
            float *imgIn_data = (*imgIn)(i + shiftX, j + shiftY);

            for(int k = 0; k < channels; k++) {
                curr_data[k] = imgIn_data[k];
            }
        }
    }
}

PIC_INLINE void Image::ScaleCosine()
{
    int half_h = height >> 1;

    #pragma omp parallel for

    for(int i = 0; i < height; i++) {
        float angle  = C_PI * float(i - half_h) / float(height);
        float cosAng = MAX(cosf(angle), 0.0f);

        for(int j = 0; j < width; j++) {
            float *tmp_data = (*this)(j, i);

            for(int k = 0; k < channels; k++) {
                tmp_data[k] *= cosAng;
            }
        }
    }
}

PIC_INLINE void Image::ApplyFunction(float(*func)(float))
{
    int size = width * height * channels;
    #pragma omp parallel for

    for(int i = 0; i < size; i++) {
        data[i] = (*func)(data[i]);
    }
}

PIC_INLINE void Image::sort()
{
    int size = frames * width * height * channels;

    if(dataTMP == NULL) {
        dataTMP = new float[size];
        memcpy(dataTMP, data, sizeof(float) * size);
    }

    std::sort(dataTMP, dataTMP + size);
}

PIC_INLINE float Image::getMedVal(float perCent = 0.5f)
{
    if(dataTMP == NULL) {
        sort();
    }

    int size = frames * width * height * channels;
    return dataTMP[int(perCent * float(size))];
}

PIC_INLINE float Image::getGT(float val)
{
    if(dataTMP == NULL) {
        sort();
    }

    int size = frames * width * height * channels;

    for(int i = 0; i < size; i++) {
        if(dataTMP[i] > val) {
            return dataTMP[i];
        }
    }

    return -1.0f;
}

PIC_INLINE void Image::Blend(Image *img, Image *weight)
{
    if(img == NULL || weight == NULL) {
        return;
    }

    if( (weight->channels != 1) &&
        (weight->channels != img->channels)) {
        return;
    }

    int size = height * width;

    #pragma omp parallel for

    for(int ind = 0; ind < size; ind++) {
        int i = ind * channels;

        int indx_w = ind * weight->channels;
        //int indx_w = i_w;// + (j % weight->channels);
        float w0 = weight->data[indx_w];
        float w1 = 1.0f - w0;

        for(int j = 0; j < channels; j++) {
            int indx = i + j;

            data[indx] = data[indx] * w0 + img->data[indx] * w1;
        }
    }
}

PIC_INLINE void Image::Minimum(Image *img)
{
    if(!SimilarType(img)) {
        return;
    }

    int size = height * width * channels;

    #pragma omp parallel for

    for(int i = 0; i < size; i++) {
        data[i] = data[i] > img->data[i] ? img->data[i] : data[i];
    }
}

PIC_INLINE void Image::Maximum(Image *img)
{
    if(!SimilarType(img)) {
        return;
    }

    int size = height * width * channels;

    #pragma omp parallel for

    for(int i = 0; i < size; i++) {
        data[i] = data[i] < img->data[i] ? img->data[i] : data[i];
    }
}

PIC_INLINE void Image::SetZero()
{
    int size = frames * height * width * channels;
//	memset(data, 0, size * sizeof(float));

    #pragma omp parallel for

    for(int i = 0; i < size; i++) {
        data[i] = 0.0f;
    }
}

PIC_INLINE void Image::SetRand()
{
    std::mt19937 m(rand() % 10000);
    int size = frames * height * width * channels;

    for(int i = 0; i < size; i++) {
        data[i] = float(m()) / 4294967295.0f;
    }
}

PIC_INLINE float *Image::getMaxVal(BBox *box = NULL, float *ret = NULL)
{
    if(box == NULL) {
        box = new BBox(width, height, frames);
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    for(int l = 0; l < channels; l++) {
        ret[l] = -FLT_MAX;
    }

    for(int k = box->z0; k < box->z1; k++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_data = (*this)(i, j, k);

                for(int l = 0; l < channels; l++) {
                    ret[l] = ret[l] < tmp_data[l] ? tmp_data[l] : ret[l];
                }
            }
        }
    }

    return ret;
}

PIC_INLINE float *Image::getMinVal(BBox *box = NULL, float *ret = NULL)
{
    if(box == NULL) {
        box = new BBox(width, height, frames);
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    for(int l = 0; l < channels; l++) {
        ret[l] = FLT_MAX;
    }

    for(int k = box->z0; k < box->z1; k++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_data = (*this)(i, j, k);

                for(int l = 0; l < channels; l++) {
                    ret[l] = ret[l] > tmp_data[l] ? tmp_data[l] : ret[l];
                }
            }
        }
    }

    return ret;
}

PIC_INLINE float *Image::getSumVal(BBox *box = NULL, float *ret = NULL)
{
    if(box == NULL) {
        box = new BBox(width, height, frames);
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    for(int l = 0; l < channels; l++) {
        ret[l] = 0.0f;
    }

    for(int k = box->z0; k < box->z1; k++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_data = (*this)(i, j, k);

                for(int l = 0; l < channels; l++) {
                    ret[l] += tmp_data[l];
                }
            }
        }
    }

    return ret;
}

PIC_INLINE float *Image::getMeanVal(BBox *box = NULL, float *ret = NULL)
{
    if(box == NULL) {
        box = &fullBox;
    }

    ret = getSumVal(box, ret);

    float totf = float(box->Size());

    for(int l = 0; l < channels; l++) {
        ret[l] /= totf;
    }

    return ret;
}

PIC_INLINE float *Image::getMomentsVal(int x0, int y0, int radius, float *ret = NULL)
{
    int channels_2 = channels * 2;

    if(ret == NULL) {
        ret = new float[channels_2];
    }

    for(int l = 0; l < channels_2; l++) {
        ret[l] = 0.0f;
    }

    for(int j = -radius; j <= radius; j++) {
        int y = y0 + j;

        for(int i = -radius; i <= radius; i++) {
            int x = x0 + i;

            float *tmp_data = (*this)(x, y);

            for(int l = 0; l < channels_2; l += 2) {
                ret[l    ] += j * tmp_data[l];
                ret[l + 1] += i * tmp_data[l];
            }
        }
    }

    return ret;
}

PIC_INLINE float *Image::getVarianceVal(float *meanVal = NULL, BBox *box = NULL,
                                        float *ret = NULL)
{
    if(box == NULL) {
        box = &fullBox;
    }

    if(meanVal == NULL) {
        meanVal = getMeanVal(box);
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    for(int l = 0; l < channels; l++) {
        ret[l] = 0.0f;
    }

    for(int k = box->z0; k < box->z1; k++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_data = (*this)(i, j, k);

                for(int l = 0; l < channels; l++) {
                    float tmp = tmp_data[l] - meanVal[l];
                    ret[l] += tmp * tmp;
                }
            }
        }
    }

    float totf = float(box->Size() - 1);

    for(int l = 0; l < channels; l++) {
        ret[l] /= totf;
    }

    return ret;
}


PIC_INLINE float *Image::getCovMtxVal(float *meanVal, BBox *box, float *ret)
{
    if(box == NULL) {
        box = &fullBox;
    }

    if(meanVal == NULL) {
        meanVal = getMeanVal(box);
    }

    int n = channels * channels;

    if(ret == NULL) {
        ret = new float[n];
    }

    for(int l = 0; l < n; l++) {
        ret[l] = 0.0f;
    }

    for(int k = box->z0; k < box->z1; k++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_data = (*this)(i, j, k);

                for(int l = 0; l < channels; l++) {
                    float tmp1 = (tmp_data[l] - meanVal[l]);

                    for(int m = 0; m < channels; m++) {
                        float tmp2 = (tmp_data[m] - meanVal[m]);

                        ret[l * channels + m] += tmp1 * tmp2;
                    }
                }
            }
        }
    }

    float totf = float(box->Size() - 1);

    for(int l = 0; l < n; l++) {
        ret[l] /= totf;
    }

    return ret;
}

PIC_INLINE float *Image::getLogMeanVal(BBox *box = NULL, float *ret = NULL)
{
    if(box == NULL) {
        box = &fullBox;
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    for(int l = 0; l < channels; l++) {
        ret[l] = 0.0f;
    }

    for(int k = box->z0; k < box->z1; k++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_data = (*this)(i, j, k);

                for(int l = 0; l < channels; l++) {
                    ret[l] += logf(tmp_data[l] + 1e-6f);
                }
            }
        }
    }

    float tot = float(box->Size());

    for(int l = 0; l < channels; l++) {
        ret[l] = expf(ret[l] / tot);
    }

    return ret;
}

PIC_INLINE void Image::ConvertFromMask(bool *mask, int width, int height)
{
    if((mask == NULL) || (width < 1) || (height < 1)) {
        return;
    }

    Allocate(width, height, 1, 1);

    int size = (width * height);

    #pragma omp parallel for

    for(int i = 0; i < size; i++) {
        data[i] = mask[i] ? 1.0f : 0.0f;
    }
}

PIC_INLINE bool *Image::ConvertToMask(float *color = NULL, float threshold = 0.5f,
                                      bool cmp = true,  bool *mask = NULL)
{
    if(!isValid() || (color == NULL)) {
        return NULL;
    }
    
    bool bColorAllocated = false;

    if(color == NULL) {
        bColorAllocated = true;
        color = new float[channels];

        for(int i=0;i<channels;i++) {
            color[i] = 0.5f;
        }
    }

    int n = width * height;

    if(mask == NULL) {
        mask = new bool[n];
    }

    float tmpThreshold = threshold * float(channels);

    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        int ind = i * channels;

        float val = 0.0f;

        for(int k = 0; k < channels; k++) {
            val += fabsf(data[ind + k] - color[k]);
        }

        if(cmp) {
            mask[i] = val > tmpThreshold;
        } else {
            mask[i] = val < tmpThreshold;
        }
    }

    if(bColorAllocated) {
        delete[] color;
    }

    return mask;
}


#ifdef PIC_QT
PIC_INLINE void Image::ConvertFromQImage(const QImage *img,
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

PIC_INLINE QImage *Image::ConvertToQImage(QImage *image = NULL,
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

PIC_INLINE bool Image::Read(std::string nameFile,
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
            tmp = ReadPFM(nameFile, dataReader, width, height, channels);
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

PIC_INLINE bool Image::Write(std::string nameFile, LDR_type typeWrite = LT_NOR_GAMMA,
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

PIC_INLINE Image *Image::AllocateSimilarOne()
{
    Image *ret = new Image(frames, width, height, channels);
    ret->flippedEXR = flippedEXR;
    return ret;
}

PIC_INLINE Image *Image::Clone() const
{
    Image *ret = new Image(frames, width, height, channels);
    ret->flippedEXR = flippedEXR;
    ret->exposure = exposure;
    ret->nameFile = nameFile;
    ret->alpha = alpha;
    ret->typeLoad = typeLoad;

    memcpy(ret->data, data, width * height * channels * sizeof(float));
    return ret;
}

PIC_INLINE void Image::operator =(const Image &a)
{
    this->Assign(&a);
}

PIC_INLINE void Image::operator =(const float &a)
{
    BufferAssign(data, size(), a);
}

PIC_INLINE void Image::operator +=(const float &a)
{
    BufferAdd(data, size(), a);
}

PIC_INLINE Image Image::operator +(const float &a) const
{
    Image *out = this->Clone();
    *out += a;
    return Image(out, false);
}

PIC_INLINE void Image::operator +=(const Image &a)
{
    if(SimilarType(&a)) {
        BufferAdd(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            BufferAddS(data, a.data, nPixels(), channels);
        }
    }

}

PIC_INLINE Image Image::operator +(const Image &a) const
{
    Image *out = this->Clone();
    *out += a;
    return Image(out, false);
}

PIC_INLINE void Image::operator *=(const float &a)
{
    BufferMul(data, size(), a);
}

PIC_INLINE Image Image::operator *(const float &a) const
{
    Image *out = this->Clone();
    *out *= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator *=(const Image &a)
{
    if(SimilarType(&a)) {
        BufferMul(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            BufferMulS(data, a.data, nPixels(), channels);
        }
    }
}

PIC_INLINE Image Image::operator *(const Image &a) const
{
    Image *out = this->Clone();
    *out *= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator -=(const float &a)
{
    BufferSub(data, size(), a);
}

PIC_INLINE Image Image::operator -(const float &a) const
{
    Image *out = this->Clone();
    *out -= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator -=(const Image &a)
{
    if(SimilarType(&a)) {
        BufferSub(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            BufferSubS(data, a.data, nPixels(), channels);
        }
    }
}

PIC_INLINE Image Image::operator -(const Image &a) const
{
    Image *out = this->Clone();
    *out -= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator /=(const float &a)
{
    BufferDiv(data, size(), a);
}

PIC_INLINE Image Image::operator /(const float &a) const
{
    Image *out = this->Clone();
    *out /= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator /=(const Image &a)
{
    if(SimilarType(&a)) {
        BufferDiv(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            BufferDivS(data, a.data, nPixels(), channels);
        }
    }
}

PIC_INLINE Image Image::operator /(const Image &a) const
{
    Image *out = this->Clone();
    *out /= a;
    return Image(out, false);
}
} // end namespace pic

#endif /* PIC_IMAGE_HPP */

