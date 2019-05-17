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

#include "../filtering/filter.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"

namespace pic {

/**
 * @brief The FilterSampler2DSub class
 */
class FilterSampler2DSub: public Filter
{
protected:
    ImageSamplerBilinear isb_default;
    ImageSampler *isb;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        float *vSrc1 = new float[dst->channels];

        float height1f = float(box->height - 1);
        float width1f = float(box->width - 1);

        for(int j = box->y0; j < box->y1; j++) {
            float y = float(j) / height1f;

            for(int i = box->x0; i < box->x1; i++) {
                float x = float(i) / width1f;

                float *out = (*dst )(i, j);

                isb->SampleImage(src[0], x, y, out);
                isb->SampleImage(src[1], x, y, vSrc1);

                for(int k = 0; k < dst->channels; k++) {
                    out[k] -= vSrc1[k];
                }
            }
        }

        delete[] vSrc1;
    }

public:

    /**
     * @brief FilterSampler2DSub
     * @param isb
     */
    FilterSampler2DSub(ImageSampler *isb) : Filter()
    {
        this->minInputImages = 2;

        if(isb != NULL) {
            this->isb = isb;
        } else {
            this->isb = &isb_default;
        }
    }

    ~FilterSampler2DSub()
    {
    }

    /**
     * @brief update
     * @param isb
     */
    void update(ImageSampler *isb)
    {
        if(isb != NULL) {
            this->isb = isb;
        }
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param isb
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, ImageSampler *isb)
    {
        FilterSampler2DSub filter(isb);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_2DSUB_HPP */

