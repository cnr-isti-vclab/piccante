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

#ifndef PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_NEAREST_HPP
#define PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_NEAREST_HPP

#include "image_samplers/image_sampler.hpp"

namespace pic {

class ImageSamplerNearest: public ImageSampler
{
public:
    /**
     * @brief ImageSamplerNearest
     */
    ImageSamplerNearest() {}

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param vOut
     */
    void SampleImage(Image *img, float x, float y, float *vOut);

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param t
     * @param vOut
     */
    void SampleImage(Image *img, float x, float y, float t, float *vOut);
};

PIC_INLINE void ImageSamplerNearest::SampleImage(Image *img, float x, float y,
        float *vOut)
{
    x = CLAMPi(x, 0.0f, 1.0f);
    y = CLAMPi(y, 0.0f, 1.0f);

    //Coordiantes in [0,width-1]x[0,height-1]
    x = lround(x * float(img->width - 1));
    y = lround(y * float(img->height - 1));
    //Integer coordinates
    int ix = int(x);
    int iy = int(y);
    //Bilinear interpolation indicies
    float *data = (*img)(ix, iy);

    for(int i = 0; i < img->channels; i++) {
        vOut[i] = data[i];
    }
}

PIC_INLINE void ImageSamplerNearest::SampleImage(Image *img, float x, float y,
        float t, float *vOut)
{
    x = CLAMPi(x, 0.0f, 1.0f);
    y = CLAMPi(y, 0.0f, 1.0f);
    t = CLAMPi(t, 0.0f, 1.0f);

    //Coordiantes in [0,width-1]x[0,height-1]x[0,frames-1]
    x = lround(x * float(img->width - 1));
    y = lround(y * float(img->height - 1));
    t = lround(t * float(img->frames - 1));
    //Integer coordinates
    int ix = int(x);
    int iy = int(y);
    int it = int(t);
    //Bilinear interpolation indicies
    int ind = (ix * img->xstride + iy * img->ystride + it * img->tstride);

    for(int i = 0; i < img->channels; i++) {
        vOut[i] = img->data[ind + i];
    }
}

} // end namespace pic

#endif /* PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_NEAREST_HPP */

