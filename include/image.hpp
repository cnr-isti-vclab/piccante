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
#include <string>

#include "base.hpp"
#include "util/compability.hpp"
#include "util/bbox.hpp"
#include "util/buffer.hpp"
#include "util/dynamic_range.hpp"
#include "util/math.hpp"
#include "util/array.hpp"
#include "util/indexed_array.hpp"
#include "util/std_util.hpp"

//IO formats
#include "io/bmp.hpp"
#include "io/exr.hpp"
#include "io/exr_tiny.hpp"
#include "io/hdr.hpp"
#include "io/pfm.hpp"
#include "io/ppm.hpp"
#include "io/pgm.hpp"
#include "io/tmp.hpp"
#include "io/tga.hpp"
#include "io/vol.hpp"
#include "io/stb.hpp"
#include "io/exif.hpp"
#include "util/io.hpp"

namespace pic {

/**
 * @brief The Image class stores an image as buffer of float.
 */
class Image
{
protected:

    /**
     * @brief setNULL sets buffers values to NULL.
     */
    void setNULL();

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

    float widthf, width1f, heightf, height1f, channelsf, framesf, frames1f;

    std::string nameFile;

    /**
     * @brief data is the main buffer where pixel values are stored.
     */
    float *data;

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
    * @brief Image destructor. This deallocates: data, dataUC, and dataRGBE
    */
    ~Image();

    /**
     * @brief allocate allocates memory for the pixel buffer.
     * @param width is the number of horizontal pixels.
     * @param height is the number of vertical pixels.
     * @param channels is the number of color channels.
     * @param frames is the number of temporal pixels.
     */
    void allocate(int width, int height, int channels, int frames);

    /**
     * @brief allocateAux computes extra information after allocation;
     * e.g. strides.
     */
    void allocateAux();

    /**
     * @brief release frees allocated buffers.
     */
    void release();

    /**
     * @brief copySubImage copies imgIn in the current image.
     * The current image is written from (startX, startY).
     * @param imgIn the image to be copied.
     * @param startX is the horizontal coordinate in pixels.
     * @param startY is the vertical coordinate in pixels.
     */
    void copySubImage(Image *imgIn, int startX, int startY);

    /**
     * @brief scaleCosine multiplies the current image by the vertical cosine
     * assuming a longitude-latitude image.
     */
    void scaleCosine();

    /**
     * @brief FlipH flips horizontally the current image.
     */
    void flipH()
    {
        Buffer<float>::flipH(data, width, height, channels, frames);
    }

    /**
     * @brief FlipV flips vertically the current image.
     */
    void flipV()
    {
        Buffer<float>::flipV(data, width, height, channels, frames);
    }

    /**
     * @brief flipHV flips horizontally and vertically the current image.
     */
    void flipHV()
    {
        Buffer<float>::flipH(data, width, height, channels, frames);
        Buffer<float>::flipV(data, width, height, channels, frames);
    }

    /**
     * @brief flipVH flips vertically and horizontally the current image.
     */
    void flipVH()
    {
        Buffer<float>::flipV(data, width, height, channels, frames);
        Buffer<float>::flipH(data, width, height, channels, frames);
    }

    /**
     * @brief rotate90CCW rotates 90 degrees counter-clockwise the current image.
     */
    void rotate90CCW()
    {
        Buffer<float>::rotate90CCW(data, width, height, channels);
        allocateAux();
    }

    /**
     * @brief rotate90CW rotates 90 degrees clockwise the current image.
     */
    void rotate90CW()
    {
        Buffer<float>::rotate90CW(data, width, height, channels);
        allocateAux();
    }

    /**
     * @brief getDiagonalSize
     * @return
     */
    float getDiagonalSize()
    {
        return sqrtf(widthf * widthf + heightf * heightf);
    }

    /**
     * @brief setZero sets data to 0.0f.
     */
    void setZero();

    /**
     * @brief setRand
     * @param seed
     */
    void setRand(unsigned int seed);

    /**
     * @brief isValid checks if the current image is valid, which means if they
     * have an allocated buffer or not.
     * @return This function return true if the current Image is allocated,
     * otherwise false.
     */
    bool isValid();

    /**
     * @brief isSimilarType checks if the current image is similar to img;
     * i.e. if they have the same width, height, frames, and channels.
     * @param img is an input image
     * @return This function returns true if the two images are similar,
     * otherwise false.
     */
    bool isSimilarType(const Image *img);    

    /**
     * @brief assign
     * @param imgIn
     */
    void assign(const Image *imgIn);

    /**
     * @brief blend
     * @param img
     * @param weight
     */
    void blend(Image *img, Image *weight);

    /**
     * @brief minimum is the minimum operator for Image.
     * @param img is a and Image and the operand. This
     * and the current Image need to have the same width,
     * height, and color channels.
     */
    void minimum(Image *img);

    /**
     * @brief maximum is the maximum operator for Image.
     * @param img is a and Image and the operand. This
     * and the current Image need to have the same width,
     * height, and color channels.
     */
    void maximum(Image *img);

    /**
     * @brief applyFunction is an operator that applies
     * an input function to all values in data.
     */
    void applyFunction(float(*func)(float));

    /**
     * @brief applyFunctionParam
     * @param param
     */
    void applyFunctionParam(float(*func)(float, std::vector<float>&), std::vector<float> &param);

    /**
     * @brief getFullBox computes a full BBox for this image.
     * @return This function returns a full BBox for this image.
     */
    BBox getFullBox();

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
     * @brief getPercentileVal computes the n-th value given a percentile.
     * @param percentile is the percentile.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param percentile is the percentile.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getPercentileVal(float percentile, BBox *box, float *ret);


    /**
     * @brief getPercentileVal computes the median value value given a percentile.
     * @param box is the bounding box where to compute the function. If it
     * is set to NULL the function will be computed on the entire image.
     * @param percentile is the percentile.
     * @param ret is an array where the function computations are stored. If it
     * is set to NULL an array will be allocated.
     * @return This function returns an array where the function computations
     * are stored.
     */
    float *getMedVal( BBox *box, float *ret);

    /**
     * @brief getDynamicRange computes the dynamic range of the image.
     * @param bRobust is a value that enables robust computation of the dynamic range using percentile.
     * @param percentile is the percentile value used when computing the dynamic range in a robust way.
     * @return
     */
    float getDynamicRange(bool bRobust, float percentile);

    /**
     * @brief getdataUC
     * @return
     */
    unsigned char *getdataUC()
    {
        return dataUC;
    }

    /**
     * @brief getColorSamples
     * @param samples
     * @param percentage
     * @return
     */
    float *getColorSamples(float *samples, int &nSamples, float percentage);

    /**
     * @brief size computes the number of values.
     * @return This function returns the number of values of the entire image.
     */
    int size() const
    {
        return height * width * channels * frames;
    }

    /**
     * @brief size computes the number of values.
     * @return This function returns the number of values of a frame.
     */
    int sizeFrame() const
    {
        return height * width * channels;
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
     * @brief convertFromMask converts a boolean mask into an Image. true is mapped
     * to 1.0f, and false is mapped to 0.0f.
     * @param mask is a buffer of boolean values with (width * height) elements.
     * @param width is the horizontal number of pixels.
     * @param height is the vertical number of pixels.
     */
    void convertFromMask(bool *mask, int width, int height);

    /**
     * @brief convertToMask converts an Image into a boolean mask.
     * @param color
     * @param threshold
     * @param cmp
     * @param mask
     * @return
     */
    bool *convertToMask(float *color, float threshold, bool cmp, bool *mask);

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
     * @brief calculateStrides computes the strides values
     * for pixels, lines and frames.
     */
    void calculateStrides()
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
     * @brief getLL returns a pointer to a pixel given a normalized
     * direction and assuming longituted-latitude mapping of the image.
     * @param x the x-coordinate of the unit-vector
     * @param y the y-coordinate of the unit-vector
     * @param z the z-coordinate of the unit-vector
     * @return This function returns a pointer to data at location (x, y, z)
     * that is normalized; i.e., sqrt(x^2+y^2+z^2) == 1.
     */
    float *getLL(float x, float y, float z)
    {
        float xf = 1.0f - ((atan2f(z, -x) * C_INV_PI) * 0.5f + 0.5f);
        float yf = (acosf(y) * C_INV_PI);

        int ix = CLAMP(int(floorf(xf * width)), width);
        int iy = CLAMP(int(floorf(yf * height)), height);

        return data + ix * xstride + iy * ystride;
    }

    /**
     * @brief getNormalizedCoords computes normalized coordinates
     * (nx, ny) of (x, y).
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @param nx is the horizontal coordinate in [0, 1]
     * @param ny is the vertical coordinate in [0, 1]
     */
    void getNormalizedCoords(int x, int y, float &nx, float &ny)
    {
        nx = float(x) / width1f;
        ny = float(y) / height1f;
    }

    /**
     * @brief getAddress calculates a memory address from (x, y)
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @return This function returns the memory address for (x, y)
     */
    int getAddress(int x, int y)
    {
        x = CLAMP(x, width);
        y = CLAMP(y, height);

        return x * xstride + y * ystride;
    }

    /**
     * @brief getAddress calculates a memory address from (x, y, t)
     * @param x is the horizontal coordinate in pixels
     * @param y is the vertical coordinate in pixels
     * @param t is the temporal coordinate in pixels
     * @return This function returns the memory address for (x, y, t)
     */
    int getAddress(int x, int y, int t)
    {
        x = CLAMP(x, width);
        y = CLAMP(y, height);
        t = CLAMP(t, frames);

        return x * xstride + y * ystride + t * tstride;
    }

    /**
     * @brief reverseAddress computes (x, y) given a memory address
     * @param ind is the memory input address
     * @param x is the output horizontal coordinate for ind
     * @param y is the output vertical coordinate for ind
     */
    void reverseAddress(int ind, int &x, int &y)
    {
        ind = ind / channels;
        y   = ind / width;
        x   = ind - (y * width);
    }

    /**
     * @brief allocateSimilarOne creates an Image with similar size
     * of the calling instance.
     * @return This returns an Image with the same size of the calling instance.
     */
    Image *allocateSimilarOne();

    /**
     * @brief allocateSimilarTo allocate an Image with similar size
     * of the passed by.
     */
    void allocateSimilarTo(Image *img);

    /**
     * @brief Clone creates a deep copy of the calling instance.
     * @return This returns a deep copy of the calling instance.
     */
    Image *clone() const;

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
     * @brief changeOwnership
     * @param notOwned
     */
    void changeOwnership(bool notOwned)
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

    /**
     * @brief sort
     * @return
     */
    float *sort()
    {
        if(!isValid()) {
            return NULL;
        }

        int size_i = size();
        float *tmp = new float[size_i];
        memcpy(tmp, data, sizeof(float) * size_i);
        std::sort(tmp, tmp + size_i);
        return tmp;
    }
};

PIC_INLINE void Image::setNULL()
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

    channelsf = -1.0f;
    widthf = -1.0f;
    heightf = -1.0f;
    width1f = -1.0f;
    height1f = -1.0f;
    framesf = -1.0f;
    frames1f = -1.0f;

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
    setNULL();
}

PIC_INLINE Image::Image(Image *imgIn, bool deepCopy = true)
{
    setNULL();
    
    if(imgIn == NULL) {
        return;
    }

    if(deepCopy) {
        assign(imgIn);
    } else {
        width = imgIn->width;
        height = imgIn->height;
        channels = imgIn->channels;
        frames = imgIn->frames;

        widthf = imgIn->widthf;
        heightf = imgIn->heightf;
        channelsf = imgIn->channelsf;
        framesf = imgIn->framesf;

        data = imgIn->data;
        dataUC = imgIn->dataUC;
        dataRGBE = imgIn->dataRGBE;

    #ifdef PIC_ENABLE_OPEN_EXR
        dataRGBE = imgIn->dataRGBE;
    #endif

        notOwned = true;
        exposure = imgIn->exposure;
        nameFile = imgIn->nameFile;
        flippedEXR = imgIn->flippedEXR;
        typeLoad = imgIn->typeLoad;

        allocateAux();
    }

}

PIC_INLINE Image::Image(int width, int height, int channels = 3)
{
    setNULL();
    allocate(width, height, channels, 1);
}

PIC_INLINE Image::Image(int frames, int width, int height, int channels,
                        float *data = NULL)
{
    setNULL();

    if(data == NULL) {
        allocate(width, height, channels, frames);
    } else {
        this->frames   = frames;
        this->channels = channels;
        this->width    = width;
        this->height   = height;
        this->notOwned = true;
        this->data = data;

        allocateAux();
    }
}

PIC_INLINE Image::Image(std::string nameFile, LDR_type typeLoad = LT_NOR_GAMMA)
{
    setNULL();
    Read(nameFile, typeLoad);
}

PIC_INLINE Image::Image(float *color, int channels)
{
    typeLoad = LT_NONE;
    setNULL();

    if(color != NULL) {
        allocate(1, 1, channels, 1);
        memcpy(data, color, channels);
    }
}

PIC_INLINE Image::~Image()
{
    release();
}

PIC_INLINE void Image::release()
{
    //release all allocated resources
    if(!notOwned) {
        data = delete_vec_s(data);
        dataUC = delete_vec_s(dataUC);
        dataRGBE = delete_vec_s(dataRGBE);

        #ifdef PIC_ENABLE_OPEN_EXR
            delete_vec_s(dataEXR);
        #endif
    }
}

PIC_INLINE void Image::allocate(int width, int height, int channels, int frames)
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

    data = new float [size()];

    allocateAux();
}

PIC_INLINE void Image::allocateAux()
{
    this->fullBox = getFullBox();

    this->depth    = frames;
    this->widthf   = float(width);
    this->width1f  = float(width - 1);
    this->heightf  = float(height);
    this->height1f = float(height - 1);
    this->channelsf = float(channels);
    this->framesf  = float(frames);
    this->frames1f = float(frames -1);

    calculateStrides();
}

PIC_INLINE BBox Image::getFullBox()
{
    BBox fullBox;
    fullBox.setBox(0, width, 0, height, 0, frames, width,
                             height, frames);

    return fullBox;
}

PIC_INLINE void Image::assign(const Image *imgIn)
{
    if(imgIn == NULL) {
        return;
    }

    if(!isSimilarType(imgIn)) {
        release();
        allocate(imgIn->width, imgIn->height, imgIn->channels, imgIn->frames);
    }

    exposure = imgIn->exposure;
    nameFile = imgIn->nameFile;
    typeLoad = imgIn->typeLoad;
    flippedEXR = imgIn->flippedEXR;

    memcpy(data, imgIn->data, size() * sizeof(float));
}

PIC_INLINE void Image::clamp(float a = 0.0f, float b = 1.0f)
{
    int size_i = size();

    #pragma omp parallel for
    for(int i = 0; i < size_i; i++) {
        data[i] = CLAMPi(data[i], a, b);
    }
}

PIC_INLINE void Image::removeSpecials()
{
    int size_i = size();

    #pragma omp parallel for
    for(int i = 0; i < size_i; i++) {
        float val = data[i];

        if(isnan(val) || isinf(val)) {
            data[i] = 0.0f;
        }
    }
}

PIC_INLINE bool Image::isSimilarType(const Image *img)
{
    if(img == NULL) {
        return false;
    }

    bool ret =	(width      ==  img->width) &&
                (height     ==  img->height) &&
                (frames     ==  img->frames) &&
                (channels   ==  img->channels) &&
                (flippedEXR ==  img->flippedEXR);

#ifdef PIC_DEBUG

    if(!ret) {
        printf("\nImage::isSimilarType: ERROR The two compared images are not similar.\n");
    }

#endif

    return ret;
}

PIC_INLINE bool Image::isValid()
{
    return (width > 0) && (height > 0) && (channels > 0) && (frames > 0) &&
           (data != NULL);
}

PIC_INLINE void Image::copySubImage(Image *imgIn, int startX, int startY)
{
    Buffer<float>::copySubBuffer(imgIn->data, imgIn->width, imgIn->height, imgIn->channels,
                                 startX, startY,
                                 data, width, height, channels);
}

PIC_INLINE void Image::scaleCosine()
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

PIC_INLINE void Image::applyFunction(float(*func)(float))
{
    if(!isValid()) {
        return;
    }

    int size_i = size();

    #pragma omp parallel for
    for(int i = 0; i < size_i; i++) {
        data[i] = (*func)(data[i]);
    }
}

PIC_INLINE void Image::applyFunctionParam(float(*func)(float, std::vector<float>&), std::vector<float> &param)
{
    if(!isValid()) {
        return;
    }

    int size_i = size();

    #pragma omp parallel for
    for(int i = 0; i < size_i; i++) {
        data[i] = (*func)(data[i], param);
    }
}

PIC_INLINE float *Image::getPercentileVal(float percentile, BBox *box, float *ret)
{
    if(!isValid()) {
        return ret;
    }

    if(box == NULL) {
        box = &fullBox;
    }

    if(ret == NULL) {
        ret = new float[channels << 1];
    }

    int size_i = box->Size();
    float *dataTMP = new float[size_i];

    for(int ch = 0; ch < channels; ch++) {
        int counter = 0;
        for(int k = box->z0; k < box->z1; k++) {
            for(int j = box->y0; j < box->y1; j++) {
                for(int i = box->x0; i < box->x1; i++) {
                    dataTMP[counter] = (*this)(i, j, k)[ch];
                    counter++;
                }
            }
        }

        std::sort(dataTMP, dataTMP + size_i);

        int size_i_t = size_i - 1;
        int index_r = ch << 1;

        float index_f;
        int index;

        float size_i_t_f = float(size_i_t);

        index_f = percentile * size_i_t_f;
        index = CLAMPi(int(index_f), 0, size_i_t);
        ret[index_r    ] = dataTMP[index];

        index_f = (1.0f - percentile) * size_i_t_f;
        index = CLAMPi(int(index_f), 0, size_i_t);
        ret[index_r + 1] = dataTMP[index];
    }

    delete[] dataTMP;

    return ret;
}

PIC_INLINE float *Image::getMedVal(BBox *box, float *ret)
{
    return getPercentileVal(0.5f, box, ret);
}

PIC_INLINE float Image::getDynamicRange(bool bRobust = false, float percentile = 0.99f)
{
    if(!isValid()) {
        return -1.0f;
    }

    if(bRobust) {
        if(percentile <= 0.5f) {
            percentile = 0.99f;
        }

        float percentile_low = 1.0f - percentile;

        float *values = getPercentileVal(percentile_low, NULL, NULL);
        float min_val = values[0];
        float max_val = values[1];

        if(min_val > 0.0f) {
            return max_val / min_val;
        } else {
            if(percentile > 0.5f) {
                return getDynamicRange(true, percentile * 0.99f);
            } else {
                return 0.0f;
            }
        }
    } else {
        float ret = -1.0f;

        float *min_val_v = getMinVal(NULL, NULL);
        float *max_val_v = getMaxVal(NULL, NULL);

        int ind;
        float min_val = Arrayf::getMin(min_val_v, channels, ind);
        float max_val = Arrayf::getMax(max_val_v, channels, ind);

        if(min_val <= 0.0f) {
            IntCoord coord;
            IndexedArray<float>::findSimple(data, size(), IndexedArray<float>::bFuncNotNeg, coord);
            min_val = IndexedArray<float>::min(data, coord);

            if(min_val != max_val) {
                if(max_val > min_val) {
                    ret = max_val / min_val;
                } else {
                    ret = -2.0f;
                }
            } else {
                ret = 0.0f;
            }
        }

        delete_vec_s(min_val_v);
        delete_vec_s(max_val_v);

        return ret;
    }
}

PIC_INLINE void Image::blend(Image *img, Image *weight)
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

PIC_INLINE void Image::minimum(Image *img)
{
    if(!isValid() || !isSimilarType(img)) {
        return;
    }

    int n = size();

    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        data[i] = data[i] > img->data[i] ? img->data[i] : data[i];
    }
}

PIC_INLINE void Image::maximum(Image *img)
{
    if(!isValid() || !isSimilarType(img)) {
        return;
    }

    int n = size();

    #pragma omp parallel for
    for(int i = 0; i < n; i++) {
        data[i] = data[i] < img->data[i] ? img->data[i] : data[i];
    }
}

PIC_INLINE void Image::setZero()
{
    if(!isValid()) {
        return;
    }

    Buffer<float>::assign(data, size(), 0.0f);
}

PIC_INLINE void Image::setRand(unsigned int seed = 1)
{
    if(!isValid()) {
        return;
    }

    std::mt19937 m(seed);
    int n = size();

    for(int i = 0; i < n; i++) {
        data[i] = float(m()) / 4294967295.0f;
    }
}

PIC_INLINE float *Image::getMaxVal(BBox *box = NULL, float *ret = NULL)
{
    if(!isValid()) {
        return ret;
    }

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
    if(!isValid()) {
        return ret;
    }

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
    if(!isValid()) {
        return ret;
    }

    if(box == NULL) {
        box = &fullBox;
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    Arrayf::assign(0.0f, ret, channels);

    for(int k = box->z0; k < box->z1; k++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_data = (*this)(i, j, k);

                Arrayf::add(tmp_data, channels, ret);
            }
        }
    }

    return ret;
}

PIC_INLINE float *Image::getMeanVal(BBox *box = NULL, float *ret = NULL)
{
    if(!isValid()) {
        return ret;
    }

    if(box == NULL) {
        box = &fullBox;
    }

    ret = getSumVal(box, ret);

    float totf = float(box->Size());

    Arrayf::div(ret, channels, totf);

    return ret;
}

PIC_INLINE float *Image::getMomentsVal(int x0, int y0, int radius, float *ret = NULL)
{
    if(!isValid()) {
        return ret;
    }

    int channels_2 = channels << 1;

    if(ret == NULL) {
        ret = new float[channels_2];
    }

    Arrayf::assign(0.0f, ret, channels_2);

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

PIC_INLINE float *Image::getVarianceVal(float *meanVal = NULL,
                                        BBox *box = NULL,
                                        float *ret = NULL)
{
    if(!isValid()) {
        return ret;
    }

    if(box == NULL) {
        box = &fullBox;
    }

    bool bDeleteMeanVal = false;

    if(meanVal == NULL) {
        meanVal = getMeanVal(box, NULL);
        bDeleteMeanVal = true;
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    Arrayf::assign(0.0f, ret, channels);

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

    Arrayf::div(ret, channels, totf);

    if(bDeleteMeanVal) {
        delete[] meanVal;
    }

    return ret;
}


PIC_INLINE float *Image::getCovMtxVal(float *meanVal, BBox *box, float *ret)
{
    if(!isValid()) {
        return ret;
    }

    if(box == NULL) {
        box = &fullBox;
    }

    bool bMeanValAllocated = false;

    if(meanVal == NULL) {
        meanVal = getMeanVal(box, NULL);
        bMeanValAllocated = true;
    }

    int n = channels * channels;

    if(ret == NULL) {
        ret = new float[n];
    }

    Arrayf::assign(0.0f, ret, n);

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

    Arrayf::div(ret, n, totf);

    if(bMeanValAllocated) {
        delete[] meanVal;
    }

    return ret;
}

PIC_INLINE float *Image::getLogMeanVal(BBox *box = NULL, float *ret = NULL)
{
    if(!isValid()) {
        return ret;
    }

    if(box == NULL) {
        box = &fullBox;
    }

    if(ret == NULL) {
        ret = new float[channels];
    }

    Arrayf::assign(0.0f, ret, channels);

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

    float totf = float(box->Size());

    for(int l = 0; l < channels; l++) {
        ret[l] = expf(ret[l] / totf);
    }

    return ret;
}

PIC_INLINE void Image::convertFromMask(bool *mask, int width, int height)
{
    if((mask == NULL) || (width < 1) || (height < 1)) {
        return;
    }

    if(!isValid() || this->width != width || this->height != height) {
        allocate(width, height, 1, 1);
    }

    int size = (width * height);

    #pragma omp parallel for
    for(int i = 0; i < size; i++) {
        data[i] = mask[i] ? 1.0f : 0.0f;
    }
}

PIC_INLINE bool *Image::convertToMask(float *color = NULL, float threshold = 0.25f,
                                      bool cmp = true,  bool *mask = NULL)
{
    if(!isValid()) {
        return NULL;
    }
    
    bool bColorAllocated = false;

    if(color == NULL) {
        bColorAllocated = true;
        color = new float[channels];

        Arrayf::assign(0.0f, color, channels);
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

        bool bMask = val > tmpThreshold;
        mask[i] = cmp ? bMask : !bMask;

        /*
        if(cmp) {
            mask[i] = val > tmpThreshold;
        } else {
            mask[i] = val < tmpThreshold;
        }*/
    }

    if(bColorAllocated) {
        delete[] color;
    }

    return mask;
}

PIC_INLINE bool Image::Read(std::string nameFile,
                               LDR_type typeLoad = LT_NOR_GAMMA)
{
    this->nameFile = nameFile;

    this->typeLoad = typeLoad;

    LABEL_IO_EXTENSION label;

    bool bReturn = false;

    //read the image in an HDR format
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
#else
#ifndef PIC_DISABLE_TINY_EXR
            tmp = ReadEXR(nameFile, dataReader, width, height, channels);
#endif
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

            allocateAux();
            bReturn = true;
        } else {
            bReturn = false;
        }
    } else {
        //read the image using an LDR codec
        label = getLabelLDRExtension(nameFile);
        unsigned char *dataReader = NULL;
        unsigned char *tmp = NULL;
        bool bExt = false;

        if(dataUC != NULL) {
            dataReader = dataUC;
        }

        switch(label) {
        case IO_BMP: {
            tmp = ReadBMP(nameFile, dataReader, width, height, channels);
        } break;

        case IO_PPM: {
            tmp = ReadPPM(nameFile, dataReader, width, height, channels);
        } break;

        case IO_PGM: {
            tmp = ReadPGM(nameFile, dataUC, width, height, channels);
        } break;

        case IO_TGA: {
            tmp = ReadTGA(nameFile, dataReader, width, height, channels);
        } break;

        case IO_JPG: {
            bExt = true;

            EXIFInfo info;
            readEXIF(nameFile, info);
            exposure = estimateAverageLuminance(info.exposureTime, info.aperture, info.iso);

        } break;

        case IO_PNG: {
            bExt = true;
        } break;

        default: {
            tmp = NULL;
        } break;

        }

         if(bExt) {
             tmp = ReadSTB(nameFile, width, height, channels);
         }

         if(tmp != NULL) { //move the handle where it's trackable
             if(dataUC == NULL) {
                 dataUC = tmp;
             }
         }

         if(typeLoad == LT_LDR) {
             return dataUC != NULL;
         }

         float *tmpFloat = NULL;

         if(data != NULL) {
             tmpFloat = &data[tstride * readerCounter];
         }

         float *tmpConv = convertLDR2HDR(tmp, tmpFloat, sizeFrame(), typeLoad);

         dataUC = delete_vec_s(dataUC);

         if(tmpConv != NULL) {
             if(data == NULL) {
                 data = tmpConv;                 
                 frames = frames > 0 ? frames : 1;
                 allocateAux();
             }

             bReturn = true;
         } else {
             bReturn = false;
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

    //read an image in an HDR format
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
#else
#ifndef PIC_DISABLE_TINY_EXR
            ret = WriteEXR(nameFile, dataWriter, width, height, channels);
#endif
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
        //write the image into an LDR format
        label = getLabelLDRExtension(nameFile);

        bool bExt = (label == IO_JPG) || (label == IO_PNG);

        //allocate memory: begin
        float *dataWriter = NULL;

        if((writerCounter > 0) && (writerCounter < frames)) {
            dataWriter = &data[tstride * writerCounter];
        } else {
            dataWriter = data;
        }

        dataUC = convertHDR2LDR(dataWriter, dataUC, sizeFrame(), typeWrite);

        /*
        if(dataUC == NULL) {
            dataUC = tmp;
        } else {
            dataUC = delete_vec_s(dataUC);
            dataUC = tmp;
        }
        //allocate memory: end
        */

        if(bExt) {
            return WriteSTB(nameFile, dataUC, width, height, channels);
        } else {
            switch(label) {
            case IO_BMP:
                return WriteBMP(nameFile, dataUC, width, height, channels);
                break;

            case IO_TGA:
                //values are stored with a vertical flip
                Buffer<unsigned char>::flipV(dataUC, width, height, channels, 1);

                //values needs to be stored as BGR
                Buffer<unsigned char>::BGRtoRGB(dataUC, width, height, channels, 1);

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

PIC_INLINE Image *Image::allocateSimilarOne()
{
    Image *ret = new Image(frames, width, height, channels);
    ret->flippedEXR = flippedEXR;
    ret->exposure = exposure;
    ret->alpha = alpha;
    return ret;
}

PIC_INLINE void Image::allocateSimilarTo(Image *img)
{
    if(img != NULL) {
        if(img->isValid()) {
            allocate(img->width, img->height, img->channels, img->frames);
        }
    }
}

PIC_INLINE Image *Image::clone() const
{
    Image *ret = new Image(frames, width, height, channels);

    ret->fullBox = fullBox;
    ret->flippedEXR = flippedEXR;
    ret->exposure = exposure;
    ret->nameFile = nameFile;
    ret->alpha = alpha;
    ret->typeLoad = typeLoad;

    memcpy(ret->data, data, width * height * channels * sizeof(float));

    return ret;
}

PIC_INLINE float* Image::getColorSamples(float *samples,
                                         int &nSamples,
                                         float percentage = 1.0f)
{    
    percentage = CLAMPi(percentage, 0.0f, 1.0f);

    int nTot = nPixels();
    nSamples = int(ceilf(float(nTot) * percentage));

    if(samples == NULL) {
        samples = new float[nSamples * channels];
    }

    int shift = 1;

    if(nSamples < nTot) {
        shift = nTot / nSamples;
    }

    for(int i = 0; i < nSamples; i++) {
        int index = i * channels;
        int index_d = index * shift;

        for(int j = 0; j < channels; j++) {
            samples[index + j] = data[index_d + j];
        }
    }

    return samples;
}

PIC_INLINE void Image::operator =(const Image &a)
{
    this->assign(&a);
}

PIC_INLINE void Image::operator =(const float &a)
{
    Buffer<float>::assign(data, size(), a);
}

PIC_INLINE void Image::operator +=(const float &a)
{
    Buffer<float>::add(data, size(), a);
}

PIC_INLINE Image Image::operator +(const float &a) const
{
    Image *out = this->clone();
    *out += a;
    return Image(out, false);
}

PIC_INLINE void Image::operator +=(const Image &a)
{
    if(isSimilarType(&a)) {
        Buffer<float>::add(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            Buffer<float>::addS(data, a.data, nPixels(), channels);
        }
    }

}

PIC_INLINE Image Image::operator +(const Image &a) const
{
    Image *out = this->clone();
    *out += a;
    return Image(out, false);
}

PIC_INLINE void Image::operator *=(const float &a)
{
    Buffer<float>::mul(data, size(), a);
}

PIC_INLINE Image Image::operator *(const float &a) const
{
    Image *out = this->clone();
    *out *= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator *=(const Image &a)
{
    if(isSimilarType(&a)) {
        Buffer<float>::mul(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            Buffer<float>::mulS(data, a.data, nPixels(), channels);
        }
    }
}

PIC_INLINE Image Image::operator *(const Image &a) const
{
    Image *out = this->clone();
    *out *= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator -=(const float &a)
{
    Buffer<float>::sub(data, size(), a);
}

PIC_INLINE Image Image::operator -(const float &a) const
{
    Image *out = this->clone();
    *out -= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator -=(const Image &a)
{
    if(isSimilarType(&a)) {
        Buffer<float>::sub(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            Buffer<float>::subS(data, a.data, nPixels(), channels);
        }
    }
}

PIC_INLINE Image Image::operator -(const Image &a) const
{
    Image *out = this->clone();
    *out -= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator /=(const float &a)
{
    Buffer<float>::div(data, size(), a);
}

PIC_INLINE Image Image::operator /(const float &a) const
{
    Image *out = this->clone();
    *out /= a;
    return Image(out, false);
}

PIC_INLINE void Image::operator /=(const Image &a)
{
    if(isSimilarType(&a)) {
        Buffer<float>::div(data, a.data, size());
    } else {
        if((nPixels() == a.nPixels()) && (a.channels == 1)) {
            Buffer<float>::divS(data, a.data, nPixels(), channels);
        }
    }
}

PIC_INLINE Image Image::operator /(const Image &a) const
{
    Image *out = this->clone();
    *out /= a;
    return Image(out, false);
}

} // end namespace pic

#endif /* PIC_IMAGE_HPP */

