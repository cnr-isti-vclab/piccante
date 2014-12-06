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

class FilterSampler3D: public Filter
{
protected:
    ImageSampler *isb;
    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

    //SetupAux
    Image *SetupAux(ImageVec imgIn, Image *imgOut);

public:
    //Basic constructors
    FilterSampler3D(float scale, ImageSampler *isb);

    static Image *Execute(Image *in, ImageSampler *isb, float scale)
    {
        FilterSampler3D filterUp(scale, isb);
        Image *out = filterUp.Process(Single(in), NULL);
        return out;
    }
};

//Basic constructor
FilterSampler3D::FilterSampler3D(float scale, ImageSampler *isb)
{
    this->scale = scale;
    this->isb = isb;
}

//SetupAux
Image *FilterSampler3D::SetupAux(ImageVec imgIn, Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = new Image(  int(imgIn[0]->framesf * scale),
                                int(imgIn[0]->widthf  * scale),
                                int(imgIn[0]->heightf * scale),
                                imgIn[0]->channels);
    }

    return imgOut;
}

//Process in a box
void FilterSampler3D::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int channels = dst->channels;

    Image *source = src[0];

    int i, j, k, p, c;
    float x, y, t;
    float *vOut = new float[channels];

    for(p = box->z0; p < box->z1; p++) {
        t = float(p) / float(box->frames - 1);

        for(j = box->y0; j < box->y1; j++) {
            y = float(j) / float(box->height - 1);

            for(i = box->x0; i < box->x1; i++) {
                x = float(i) / float(box->width - 1);
                //Convolution kernel
                c = p * source->tstride + j * source->ystride + i * source->xstride;
                isb->SampleImage(source, x, y, t, vOut);

                for(k = 0; k < channels; k++) {
                    dst->data[c + k] = vOut[k];
                }
            }
        }
    }

    delete[] vOut;
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLER_3D_HPP */

