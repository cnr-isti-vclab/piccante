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

#ifndef PIC_FILTERING_FILTER_SAMPLER_2D_HPP
#define PIC_FILTERING_FILTER_SAMPLER_2D_HPP

#include "../filtering/filter.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"
#include "../image_samplers/image_sampler_bsplines.hpp"
#include "../image_samplers/image_sampler_gaussian.hpp"
#include "../image_samplers/image_sampler_nearest.hpp"

namespace pic {

/**
 * @brief The FilterSampler2D class
 */
class FilterSampler2D: public Filter
{
protected:
    ImageSamplerNearest isb_default;
    ImageSampler *isb;
    float scaleX, scaleY;
    int width, height;
    bool swh;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterSample2D
     */
    FilterSampler2D() : Filter()
    {
        scaleX = -1.0f;
        scaleY = -1.0f;
        width = -1;
        height = -1;
        isb = NULL;
    }

    /**
     * @brief FilterSampler2D
     * @param scale
     * @param isb
     */
    FilterSampler2D(float scale, ImageSampler *isb);

    /**
     * @brief FilterSampler2D
     * @param scaleX
     * @param scaleY
     * @param isb
     */
    FilterSampler2D(float scaleX, float scaleY, ImageSampler *isb);

    /**
     * @brief FilterSampler2D
     * @param width
     * @param height
     * @param isb
     */
    FilterSampler2D(int width, int height, ImageSampler *isb);

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        if(imgIn.empty()) {
            width = -2;
            height = -2;
            channels = -2;
            frames = -2;
            return;
        }

        if(imgIn.size() == 1) {
            if(swh) {
                width  = int(imgIn[0]->widthf  * scaleX);
                height = int(imgIn[0]->heightf * scaleY);
            } else {
                width = this->width;
                height = this->height;
            }
        } else {
            width = imgIn[1]->width;
            height = imgIn[1]->height;
        }

        channels = imgIn[0]->channels;
        frames = imgIn[0]->frames;
    }

    /**
     * @brief update
     * @param width
     * @param height
     * @param isb
     */
    void update(int width, int height, ImageSampler *isb)
    {
        this->width  = width;
        this->height = height;

        this->swh = false;

        if(isb == NULL) {
            this->isb = new ImageSamplerNearest();
        } else {
            this->isb = isb;
        }
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param scale
     * @param isb
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float scale,
                             ImageSampler *isb = NULL)
    {
        FilterSampler2D filter(scale, isb);
        return filter.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param scaleX
     * @param scaleY
     * @param isb
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float scaleX,
                             float scaleY, ImageSampler *isb = NULL)
    {
        FilterSampler2D filter(scaleX, scaleY, isb);
        return filter.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param width
     * @param height
     * @param isb
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int width,
                             int height, ImageSampler *isb = NULL)
    {
        FilterSampler2D filter(width, height, isb);
        return filter.Process(Single(imgIn), imgOut);
    }

};

PIC_INLINE FilterSampler2D::FilterSampler2D(float scale,
        ImageSampler *isb = NULL): Filter()
{
    this->scale  = scale;
    this->scaleX = scale;
    this->scaleY = scale;

    this->swh = true;

    if(isb == NULL) {
        this->isb = new ImageSamplerNearest();
    } else {
        this->isb = isb;
    }
}

PIC_INLINE FilterSampler2D::FilterSampler2D(float scaleX, float scaleY,
        ImageSampler *isb = NULL): Filter()
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;

    this->swh = true;

    if(isb == NULL) {
        this->isb = &isb_default;
    } else {
        this->isb = isb;
    }
}

PIC_INLINE FilterSampler2D::FilterSampler2D(int width, int height,
        ImageSampler *isb = NULL): Filter()
{
    update(width, height, isb);
}

PIC_INLINE void FilterSampler2D::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    float height1f = float(box->height - 1);
    float width1f = float(box->width - 1);

    for(int j = box->y0; j < box->y1; j++) {
        float y = float(j) / height1f;

        for(int i = box->x0; i < box->x1; i++) {

            float x = float(i) / width1f;

            float *tmp_dst = (*dst)(i, j);

            isb->SampleImage(src[0], x, y, tmp_dst);
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_2D_HPP */

