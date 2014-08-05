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

#ifndef PIC_FILTERING_FILTER_SAMPLER_2DADD_HPP
#define PIC_FILTERING_FILTER_SAMPLER_2DADD_HPP

#include "filtering/filter.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"

namespace pic {

class FilterSampler2DAdd: public Filter
{
protected:
    ImageSampler	*isb;
    bool            bIsb;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

public:
    /**
     * @brief FilterSampler2DAdd
     */
    FilterSampler2DAdd();

    /**
     * @brief FilterSampler2DAdd
     * @param isb
     */
    FilterSampler2DAdd(ImageSampler *isb);

    ~FilterSampler2DAdd();

    /**
     * @brief Update
     * @param isb
     */
    void Update(ImageSampler *isb);

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param isb
     * @return
     */
    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, ImageSampler *isb)
    {
        FilterSampler2DAdd filter(isb);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param isb
     */
    static void Execute(std::string nameIn, std::string nameOut, ImageSampler *isb)
    {
        ImageRAW imgIn(nameIn);
        ImageRAW *imgOut = Execute(&imgIn, NULL, isb);
        imgOut->Write(nameOut);
    }
};

FilterSampler2DAdd::FilterSampler2DAdd()
{
    bIsb = true;
    this->isb = new ImageSamplerBilinear();
}

FilterSampler2DAdd::FilterSampler2DAdd(ImageSampler *isb)
{
    bIsb = false;
    this->isb = isb;
}

FilterSampler2DAdd::~FilterSampler2DAdd()
{
    if(bIsb) {
        delete isb;
    }
}

void FilterSampler2DAdd::Update(ImageSampler *isb)
{
    if((this->isb!=NULL) && (bIsb)) {
        delete this->isb;
    }

    bIsb = false;
    this->isb = isb;
}

void FilterSampler2DAdd::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
{

    if(src.size() != 2) {
        return;
    }

    int channels = dst->channels;

    ImageRAW *src0 = src[0];
    ImageRAW *src1 = src[1];

    float *vOut1 = new float[channels];

    float height1f = float(box->height - 1);
    float width1f  = float(box->width  - 1);

    for(int j = box->y0; j < box->y1; j++) {
        float y = float(j) / height1f;

        for(int i = box->x0; i < box->x1; i++) {
            float x = float(i) / width1f;

            float *tmp_dst = (*dst)(i, j);

            //Convolution kernel
            isb->SampleImage(src0, x, y, tmp_dst);
            isb->SampleImage(src1, x, y, vOut1);

            for(int k = 0; k < channels; k++) {
                tmp_dst[k] += vOut1[k];
            }
        }
    }

    delete[] vOut1;
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_2DADD_HPP */

