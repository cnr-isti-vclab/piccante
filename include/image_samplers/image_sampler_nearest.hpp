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

#ifndef PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_NEAREST_HPP
#define PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_NEAREST_HPP

#include "../image_samplers/image_sampler.hpp"

namespace pic {

/**
 * @brief The ImageSamplerNearest class
 */
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
    void SampleImage(Image *img, float x, float y, float *vOut)
    {
        x = CLAMPi(x, 0.0f, 1.0f);
        y = CLAMPi(y, 0.0f, 1.0f);

        //Coordiantes in [0,width-1]x[0,height-1]
        x = x * img->width1f;
        y = y * img->height1f;

        SampleImageUC(img, x, y, vOut);
    }

    /**
     * @brief SampleImageUC samples an image in unnormalized coordinates [0,width-1]x[0,height-1].
     * @param img
     * @param x
     * @param y
     * @param vOut
     */
    void SampleImageUC(Image *img, float x, float y, float *vOut)
    {
        //Integer coordinates
        int ix = CLAMP(int(x), img->width);
        int iy = CLAMP(int(y), img->height);

        //Bilinear interpolation indicies
        int ind = (ix * img->xstride + iy * img->ystride);

        for(int i = 0; i < img->channels; i++) {
            vOut[i] = img->data[ind + i];
        }
    }

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param t
     * @param vOut
     */
    void SampleImage(Image *img, float x, float y, float t, float *vOut)
    {
        x = CLAMPi(x, 0.0f, 1.0f);
        y = CLAMPi(y, 0.0f, 1.0f);
        t = CLAMPi(t, 0.0f, 1.0f);

        //coordiantes in [0,width-1] x [0,height-1] x [0,frames-1]
        x = x * img->width1f;
        y = y * img->height1f;
        t = t * img->frames1f;

        //integer coordinates
        int ix = int(x);
        int iy = int(y);
        int it = int(t);

        //indicies
        int ind = (ix * img->xstride + iy * img->ystride + it * img->tstride);

        for(int i = 0; i < img->channels; i++) {
            vOut[i] = img->data[ind + i];
        }
    }
};

} // end namespace pic

#endif /* PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_NEAREST_HPP */

