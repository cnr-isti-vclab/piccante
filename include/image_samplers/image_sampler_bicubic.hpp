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

#ifndef PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_BICUBIC_HPP
#define PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_BICUBIC_HPP

#include "../image_samplers/image_sampler.hpp"

namespace pic {

/**
 * @brief The ImageSamplerBicubic class
 */
class ImageSamplerBicubic: public ImageSampler
{
public:
    ImageSamplerBicubic()
    {

    }

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param vOut
     */
    void SampleImage(Image *img, float x, float y, float *vOut)
    {
        float xx, yy, dx, dy;

        //Coordiantes in [0,width-1]x[0,height-1]
        x *= img->width1f;
        y *= img->height1f;

        //Coordinates without fractions
        xx = floorf(x);
        yy = floorf(y);

        //Interpolation values
        dx = (x - xx);
        dy = (y - yy);

        //Integer coordinates
        int ix = int(xx);
        int iy = int(yy);

        for(int k = 0; k < img->channels; k++) {
            vOut[k] = 0.0f;
        }

        //Bicubic interpolation
        float rx, ry;
        int ey, ex;
        for(int j = -1; j < 3; j++) {
            ry = Bicubic(float(j) - dy);
            ey = CLAMP(iy + j, img->height);

            for(int i = -1; i < 3; i++) {
                rx = Bicubic(-(float(i) - dx));
                ex = CLAMP(ix + i, img->width);
                int ind = (ey * img->width + ex) * img->channels;

                rx *= ry;
                for(int k = 0; k < img->channels; k++) {
                    vOut[k] += img->data[ind + k] * rx;
                }
            }
        }
    }
};

} // end namespace pic

#endif /* PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_BICUBIC_HPP */

