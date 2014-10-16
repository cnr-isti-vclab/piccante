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

#ifndef PIC_FILTERING_FILTER_SAMPLER_2D_HPP
#define PIC_FILTERING_FILTER_SAMPLER_2D_HPP

#include "filtering/filter.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"
#include "image_samplers/image_sampler_bsplines.hpp"
#include "image_samplers/image_sampler_gaussian.hpp"
#include "image_samplers/image_sampler_nearest.hpp"

namespace pic {

class FilterSampler2D: public Filter
{
protected:
    ImageSampler	*isb;
    float			scaleX, scaleY;
    int				width, height;
    bool			swh;

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

    //SetupAux
    Image *SetupAux(ImageVec imgIn, Image *imgOut);

public:
    //Basic constructors
    FilterSampler2D(float scale, ImageSampler *isb);
    FilterSampler2D(float scaleX, float scaleY, ImageSampler *isb);

    FilterSampler2D(int width, int height, ImageSampler *isb);

    /**Output size*/
    void OutputSize(Image *imgIn, int &width, int &height, int &channels, int &frames)
    {
        if(swh) {
            width       = int(imgIn->widthf  * scaleX);
            height      = int(imgIn->heightf * scaleY);
        } else {
            width       = this->width;
            height      = this->height;
        }

        channels    = imgIn->channels;
        frames      = imgIn->frames;
    }

    static Image *Execute(Image *imgIn, Image *imgOut, float scale,
                             ImageSampler *isb)
    {
        FilterSampler2D filter(scale, isb);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(Image *imgIn, Image *imgOut, float scaleX,
                             float scaleY, ImageSampler *isb)
    {
        FilterSampler2D filter(scaleX, scaleY, isb);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(Image *imgIn, Image *imgOut, int width,
                             int height, ImageSampler *isb)
    {
        FilterSampler2D filter(width, height, isb);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static void Execute(std::string nameIn, std::string nameOut, float scale,
                        ImageSampler *isb)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, scale, isb);
        imgOut->Write(nameOut);
    }
};

PIC_INLINE FilterSampler2D::FilterSampler2D(float scale,
        ImageSampler *isb = NULL)
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
        ImageSampler *isb = NULL)
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;

    this->swh = true;

    if(isb == NULL) {
        this->isb = new ImageSamplerNearest();
    } else {
        this->isb = isb;
    }
}

PIC_INLINE FilterSampler2D::FilterSampler2D(int width, int height,
        ImageSampler *isb = NULL)
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

PIC_INLINE Image *FilterSampler2D::SetupAux(ImageVec imgIn,
        Image *imgOut)
{
    if(imgOut == NULL) {
        if(swh) {
            imgOut = new Image(  imgIn[0]->frames, 
                                    int(imgIn[0]->widthf  * scaleX),
                                    int(imgIn[0]->heightf * scaleY),
                                    imgIn[0]->channels);
        } else {
            imgOut = new Image(imgIn[0]->frames, width, height, imgIn[0]->channels);
        }
    }

    return imgOut;
}

PIC_INLINE void FilterSampler2D::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *source = src[0];

    float width1f  = float(box->width  - 1);
    float height1f = float(box->height - 1);

    for(int j = box->y0; j < box->y1; j++) {
        float y = float(j) / height1f;

        for(int i = box->x0; i < box->x1; i++) {
            float x = float(i) / width1f;

            float *tmp_dst = (*dst)(i, j);
            isb->SampleImage(source, x, y, tmp_dst);
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_2D_HPP */

