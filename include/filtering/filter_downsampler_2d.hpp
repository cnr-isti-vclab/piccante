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

#ifndef PIC_FILTERING_FILTER_DOWNSAMPLER_2D_HPP
#define PIC_FILTERING_FILTER_DOWNSAMPLER_2D_HPP

#include "../util/std_util.hpp"
#include "../filtering/filter_npasses.hpp"
#include "../filtering/filter_sampler_1d.hpp"
#include "../image_samplers/image_sampler_nearest.hpp"
#include "../image_samplers/image_sampler_gaussian.hpp"

namespace pic {

/**
 * @brief The FilterDownSampler2D class
 */
class FilterDownSampler2D: public FilterNPasses
{
protected:
    ImageSamplerGaussian *isg[2];
    FilterSampler1D *flt[2];

    bool swh;
    float scale[2];
    int width, height;

    /**
     * @brief allocate
     */
    void allocate()
    {
        for(int i = 0; i < 2; i++) {
            if(isg[i] == NULL) {
                isg[i] = new ImageSamplerGaussian();
            }

            if(flt[i] == NULL) {
                flt[i] = new FilterSampler1D(scale[i], i, isg[i]);
            } else {
                flt[i]->update(scale[i], i, isg[i]);
            }

            insertFilter(flt[i]);
        }
    }

public:

    /**
     * @brief FilterDownSampler2D
     * @param scaleX
     * @param scaleY
     */
    FilterDownSampler2D(float scaleX, float scaleY);

    /**
     * @brief FilterDownSampler2D
     * @param width
     * @param height
     */
    FilterDownSampler2D(int width, int height);

    ~FilterDownSampler2D();

    /**
     * @brief release
     */
    void release();
    
    /**
     * @brief PreProcess
     * @param imgIn
     * @param imgOut
     */
    void PreProcess(ImageVec imgIn, Image *imgOut);

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param width
     * @param height
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int width,
                             int height)
    {
        FilterDownSampler2D flt(width, height);
        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param scaleX
     * @param scaleY
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float scaleX,
                             float scaleY)
    {
        FilterDownSampler2D flt(scaleX, scaleY);
        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param scaleXY
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float scaleXY)
    {
        FilterDownSampler2D flt(scaleXY, scaleXY);
        return flt.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterDownSampler2D::FilterDownSampler2D(float scaleX, float scaleY = -1.0f) : FilterNPasses()
{
    for(int i = 0; i < 2; i++) {
        this->isg[i] = NULL;
        this->flt[i] = NULL;
        this->scale[i] = 1.0f;
    }

    if(scaleX > 0.0f) {
        this->scale[0] = scaleX;
        this->scale[1] = scaleY > 0.0f ? scaleY : scaleX;
    }

    width  = -1;
    height = -1;

    allocate();

    swh = true;
}

PIC_INLINE FilterDownSampler2D::FilterDownSampler2D(int width, int height) : FilterNPasses()
{
    for(int i = 0; i < 2; i++) {
        this->isg[i] = NULL;
        this->flt[i] = NULL;
        this->scale[i] = 1.0f;
    }

    if(width > 0) {
        this->width  = width;
    }

    if(height > 0) {
        this->height = height;
    }

    allocate();

    swh = (width < 1 ||  height < 1);
}

PIC_INLINE FilterDownSampler2D::~FilterDownSampler2D()
{
    release();
}

PIC_INLINE void FilterDownSampler2D::release()
{
    for (int i = 0; i < 2; i++) {
        flt[i] = delete_s(flt[i]);
        isg[i] = delete_s(isg[i]);
    }
}

PIC_INLINE void FilterDownSampler2D::PreProcess(ImageVec imgIn,
        Image *imgOut)
{
    if(!swh) {
        scale[0] = float(width)  / imgIn[0]->widthf;
        scale[1] = float(height) / imgIn[0]->heightf;
    }

    for(int i = 0; i < 2; i++) {
        isg[i]->update(1.0f / (5.0f * scale[i]), i);
        flt[i]->update(scale[i], i, isg[i]);
    }

}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DOWNSAMPLER_2D_HPP */

