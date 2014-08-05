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

class FilterSampler1D: public Filter
{
protected:
    ImageSampler	*isb;
    int             dirs[3];
    int				size;
    bool			swh;

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

    //SetupAux
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut);

    void SetDirection(int direction);
    void SetImageSampler(ImageSampler *isb);

public:
    //Basic constructors
    FilterSampler1D(float scale, int direction, ImageSampler *isb);
    FilterSampler1D(int size, int direction, ImageSampler *isb);

    void Update(float scale, int direction, ImageSampler *isb);
    void Update(int size, int direction, ImageSampler *isb);

    /**Output size*/
    void OutputSize(ImageRAW *imgIn, int &width, int &height, int &channels, int &frames)
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

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, float scale,
                             int direction, ImageSampler *isb)
    {
        FilterSampler1D filter(scale, direction, isb);
        return filter.ProcessP(Single(imgIn), imgOut);
    }
};

//Basic constructor
PIC_INLINE FilterSampler1D::FilterSampler1D(float scale, int direction = 0,
        ImageSampler *isb = NULL)
{
    this->isb = NULL;
    Update(scale, direction, isb);
}

//Basic constructor
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
    dirs[ direction         ] = 1;
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

//SetupAux
PIC_INLINE ImageRAW *FilterSampler1D::SetupAux(ImageRAWVec imgIn,
        ImageRAW *imgOut)
{
    if(imgOut == NULL) {
        if(swh) {
            float scaleX = (dirs[X_DIRECTION] == 1) ? scale : 1.0f;
            float scaleY = (dirs[Y_DIRECTION] == 1) ? scale : 1.0f;

            imgOut = new ImageRAW(imgIn[0]->frames, imgIn[0]->width * scaleX,
                                  imgIn[0]->height * scaleY, imgIn[0]->channels);
        } else {
            int nWidth  = (dirs[X_DIRECTION] == 1) ? size : imgIn[0]->width;
            int nHeight = (dirs[Y_DIRECTION] == 1) ? size : imgIn[0]->height;

            imgOut = new ImageRAW(imgIn[0]->frames, nWidth, nHeight, imgIn[0]->channels);
        }
    }

    return imgOut;
}

//Process in a box
PIC_INLINE void FilterSampler1D::ProcessBBox(ImageRAW *dst, ImageRAWVec src,
        BBox *box)
{
    ImageRAW *source = src[0];

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

