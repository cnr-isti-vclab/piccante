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

#ifndef PIC_FILTERING_FILTER_SAMPLER_3D_HPP
#define PIC_FILTERING_FILTER_SAMPLER_3D_HPP

#include "filtering/filter.hpp"
#include "image_samplers/image_sampler.hpp"

namespace pic {

/**
 * @brief The FilterSampler3D class
 */
class FilterSampler3D: public Filter
{
protected:
    ImageSampler *isb;

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

public:
    /**
     * @brief FilterSampler3D
     * @param scale
     * @param isb
     */
    FilterSampler3D(float scale, ImageSampler *isb);

    /**
     * @brief Execute
     * @param in
     * @param isb
     * @param scale
     * @return
     */
    static Image *Execute(Image *in, ImageSampler *isb, float scale)
    {
        FilterSampler3D filterUp(scale, isb);
        Image *out = filterUp.Process(Single(in), NULL);
        return out;
    }
};

FilterSampler3D::FilterSampler3D(float scale, ImageSampler *isb)
{
    this->scale = scale;
    this->isb = isb;
}

Image *FilterSampler3D::SetupAux(ImageVec imgIn, Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = new Image( int(imgIn[0]->framesf * scale),
                            int(imgIn[0]->widthf  * scale),
                            int(imgIn[0]->heightf * scale),
                            imgIn[0]->channels);
    }

    return imgOut;
}

void FilterSampler3D::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    Image *source = src[0];

    for(int p = box->z0; p < box->z1; p++) {
        float t = float(p) / float(box->frames - 1);

        for(int j = box->y0; j < box->y1; j++) {
            float y = float(j) / float(box->height - 1);

            for(int i = box->x0; i < box->x1; i++) {
                float x = float(i) / float(box->width - 1);

                int c = p * source->tstride + j * source->ystride + i * source->xstride;

                isb->SampleImage(source, x, y, t, &dst->data[c]);
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_3D_HPP */

