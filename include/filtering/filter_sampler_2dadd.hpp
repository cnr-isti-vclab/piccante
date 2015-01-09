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

#ifndef PIC_FILTERING_FILTER_SAMPLER_2DADD_HPP
#define PIC_FILTERING_FILTER_SAMPLER_2DADD_HPP

#include "filtering/filter.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"

namespace pic {

/**
 * @brief The FilterSampler2DAdd class
 */
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
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

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
    static Image *Execute(Image *imgIn, Image *imgOut, ImageSampler *isb)
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
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, isb);
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

void FilterSampler2DAdd::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{

    if(src.size() != 2) {
        return;
    }

    int channels = dst->channels;

    Image *src0 = src[0];
    Image *src1 = src[1];

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

