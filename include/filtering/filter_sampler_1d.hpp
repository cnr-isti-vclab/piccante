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

#ifndef PIC_FILTERING_FILTER_SAMPLER_1D_HPP
#define PIC_FILTERING_FILTER_SAMPLER_1D_HPP

#define X_DIRECTION 0
#define Y_DIRECTION 1
#define Z_DIRECTION 2

#include "filtering/filter.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"
#include "image_samplers/image_sampler_bsplines.hpp"
#include "image_samplers/image_sampler_gaussian.hpp"
#include "image_samplers/image_sampler_nearest.hpp"

namespace pic {

/**
 * @brief The FilterSampler1D class
 */
class FilterSampler1D: public Filter
{
protected:
    ImageSampler	*isb;
    int             dirs[3];
    int				size;
    bool			swh;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *SetupAux(ImageVec imgIn, Image *imgOut);

    /**
     * @brief SetDirection
     * @param direction
     */
    void SetDirection(int direction);

    /**
     * @brief SetImageSampler
     * @param isb
     */
    void SetImageSampler(ImageSampler *isb);

public:
    /**
     * @brief FilterSampler1D
     * @param scale
     * @param direction
     * @param isb
     */
    FilterSampler1D(float scale, int direction, ImageSampler *isb);

    /**
     * @brief FilterSampler1D
     * @param size
     * @param direction
     * @param isb
     */
    FilterSampler1D(int size, int direction, ImageSampler *isb);

    /**
     * @brief Update
     * @param scale
     * @param direction
     * @param isb
     */
    void Update(float scale, int direction, ImageSampler *isb);

    /**
     * @brief Update
     * @param size
     * @param direction
     * @param isb
     */
    void Update(int size, int direction, ImageSampler *isb);

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(Image *imgIn, int &width, int &height, int &channels, int &frames)
    {
        if(swh) {
            float scaleX = (dirs[X_DIRECTION] == 1) ? scale : 1.0f;
            float scaleY = (dirs[Y_DIRECTION] == 1) ? scale : 1.0f;

            width  = int(imgIn->width  * scaleX);
            height = int(imgIn->height * scaleY);
        } else {
            width  = (dirs[X_DIRECTION] == 1) ? size : imgIn->width;
            height = (dirs[Y_DIRECTION] == 1) ? size : imgIn->height;
        }

        channels    = imgIn->channels;
        frames      = imgIn->frames;
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param scale
     * @param direction
     * @param isb
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, float scale,
                             int direction, ImageSampler *isb)
    {
        FilterSampler1D filter(scale, direction, isb);
        return filter.ProcessP(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterSampler1D::FilterSampler1D(float scale, int direction = 0,
        ImageSampler *isb = NULL)
{
    this->isb = NULL;
    Update(scale, direction, isb);
}

PIC_INLINE FilterSampler1D::FilterSampler1D(int size, int direction = 0,
        ImageSampler *isb = NULL)
{
    this->isb = NULL;
    Update(size, direction, isb);
}

PIC_INLINE void FilterSampler1D::Update(float scale, int direction,
                                        ImageSampler *isb)
{
    this->scale = scale;
    this->swh   = true;

    SetDirection(direction);
    SetImageSampler(isb);
}

PIC_INLINE void FilterSampler1D::Update(int size, int direction,
                                        ImageSampler *isb)
{
    this->size = size;
    this->swh  = false;

    SetDirection(direction);
    SetImageSampler(isb);
}

PIC_INLINE void FilterSampler1D::SetDirection(int direction = 0)
{
    dirs[ direction      % 3] = 1;
    dirs[(direction + 1) % 3] = 0;
    dirs[(direction + 2) % 3] = 0;
}

PIC_INLINE void FilterSampler1D::SetImageSampler(ImageSampler *isb)
{
    if(isb == NULL) {
        if(this->isb == NULL) {
            this->isb = new ImageSamplerNearest();
        }
    } else {
        this->isb = isb;
    }
}

PIC_INLINE Image *FilterSampler1D::SetupAux(ImageVec imgIn,
        Image *imgOut)
{
    if(imgOut == NULL) {
        if(swh) {
            float scaleX = (dirs[X_DIRECTION] == 1) ? scale : 1.0f;
            float scaleY = (dirs[Y_DIRECTION] == 1) ? scale : 1.0f;

            imgOut = new Image(  imgIn[0]->frames,
                                    int(imgIn[0]->widthf  * scaleX),
                                    int(imgIn[0]->heightf * scaleY),
                                    imgIn[0]->channels);
        } else {
            int nWidth  = (dirs[X_DIRECTION] == 1) ? size : imgIn[0]->width;
            int nHeight = (dirs[Y_DIRECTION] == 1) ? size : imgIn[0]->height;

            imgOut = new Image(imgIn[0]->frames, nWidth, nHeight, imgIn[0]->channels);
        }
    }

    return imgOut;
}

PIC_INLINE void FilterSampler1D::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *source = src[0];

    float width1f  = float(box->width  - 1);
    float height1f = float(box->height - 1);

    for(int j = box->y0; j < box->y1; j++) {
        float y = float(j) / height1f;

        for(int i = box->x0; i < box->x1; i++) {
            float x = float(i) / width1f;

            float *tmp_data = (*dst)(i, j);
            isb->SampleImage(source, x, y, tmp_data);
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_1D_HPP */

