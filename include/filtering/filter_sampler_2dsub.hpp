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

#ifndef PIC_FILTERING_FILTER_SAMPLER_2DSUB_HPP
#define PIC_FILTERING_FILTER_SAMPLER_2DSUB_HPP

#include "filtering/filter.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"

namespace pic {

/**
 * @brief The FilterSampler2DSub class
 */
class FilterSampler2DSub: public Filter
{
protected:
    bool            bIsb;
    ImageSampler	*isb;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterSampler2DSub
     * @param isb
     */
    FilterSampler2DSub(ImageSampler *isb);

    ~FilterSampler2DSub();

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
        FilterSampler2DSub filter(isb);
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

FilterSampler2DSub::FilterSampler2DSub(ImageSampler *isb)
{
    if(isb != NULL) {
        bIsb = false;
        this->isb = isb;
    } else {
        bIsb = true;
        this->isb = new ImageSamplerBilinear();
    }
}

FilterSampler2DSub::~FilterSampler2DSub()
{
    if(bIsb) {
        delete isb;
    }
}

void FilterSampler2DSub::Update(ImageSampler *isb)
{
    if((this->isb != NULL) && (bIsb)) {
        delete this->isb;
    }

    this->isb = isb;
    bIsb = false;
}

void FilterSampler2DSub::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    if(src.size() != 2) {
        return;
    }

    int channels = dst->channels;

    Image *src0 = src[0];
    Image *src1 = src[1];

    float *tmp_mem = new float[channels * 2];
    float *vOut  = &tmp_mem[0];
    float *vsrc0 = &tmp_mem[channels];

    float inv_height1f = 1.0f / float(box->height - 1);
    float inv_width1f = 1.0f / float(box->width - 1);

    for(int j = box->y0; j < box->y1; j++) {
        float y = float(j) * inv_height1f;

        for(int i = box->x0; i < box->x1; i++) {
            float x = float(i) * inv_width1f;

            float *tmp_dst  = (*dst )(i, j);

            isb->SampleImage(src0, x, y, vsrc0);
            isb->SampleImage(src1, x, y, vOut);

            for(int k = 0; k < channels; k++) {
                tmp_dst[k] = vsrc0[k] - vOut[k];
            }
        }
    }

    delete[] tmp_mem;
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_2DSUB_HPP */

