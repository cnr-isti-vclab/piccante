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

#ifndef PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_BILINEAR_HPP
#define PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_BILINEAR_HPP

#include "../image_samplers/image_sampler.hpp"

namespace pic {

/**
 * @brief The ImageSamplerBilinear class
 */
class ImageSamplerBilinear: public ImageSampler
{
public:
    ImageSamplerBilinear()
    {
    }

    /**
     * @brief SampleImage samples an image in normalized coordiantes (0,1).
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
        x *= img->width1f;
        y *= img->height1f;
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
        float xx, yy, dx, dy;
        int ind0, ind1, ind2, ind3;

        //Coordinates without fractions
        xx = floorf(x);
        yy = floorf(y);

        //Interpolation values
        dx = (x - xx);
        dy = (y - yy);

        //Integer coordinates
        int ix = int(xx);
        int iy = int(yy);
        int ix1 = CLAMP(ix + 1, img->width);
        int iy1 = CLAMP(iy + 1, img->height);

        //Bilinear interpolation indicies
        int t0 = iy  * img->width;
        int t1 = iy1 * img->width;

        ind0 = (ix  + t0) * img->channels;
        ind1 = (ix1 + t0) * img->channels;
        ind2 = (ix  + t1) * img->channels;
        ind3 = (ix1 + t1) * img->channels;

        for(int i = 0; i < img->channels; i++) {
            vOut[i] = Bilinear<float>(img->data[ind0 + i],
                                      img->data[ind1 + i],
                                      img->data[ind2 + i],
                                      img->data[ind3 + i],
                                      dx, dy);
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
        /*	float tmp = y;
            y = t;
            t = tmp;*/

        x = CLAMPi(x, 0.0f, 1.0f);
        y = CLAMPi(y, 0.0f, 1.0f);
        t = CLAMPi(t, 0.0f, 1.0f);

        x *= img->width1f;
        y *= img->height1f;
        t *= img->frames1f;

        float pOut2x = floorf(x);
        float pOut2y = floorf(y);
        float pOut2z = floorf(t);

        float deltax = x - pOut2x;
        float deltay = y - pOut2y;
        float deltaz = t - pOut2z;

        //Integer coordinates
        int ix = int(pOut2x);
        int iy = int(pOut2y);
        int iz = int(pOut2z);

        int ix1 = (ix + 1) % img->width;
        int iy1 = (iy + 1) % img->height;
        int iz1 = (iz + 1) % img->frames;

        float val[2];

        for(int i = 0; i < img->channels; i++) {
            val[0] = Bilinear<float>(
                         *((*img)(ix,	iy,	iz)  + i),
                         *((*img)(ix1,	iy,	iz)  + i),
                         *((*img)(ix,	iy,	iz1) + i),
                         *((*img)(ix1,	iy,	iz1) + i),
                         deltax, deltaz);

            val[1] = Bilinear<float>(
                         *((*img)(ix,	iy1,    iz)  + i),
                         *((*img)(ix1,	iy1,    iz)  + i),
                         *((*img)(ix,	iy1,    iz1) + i),
                         *((*img)(ix1,	iy1,    iz1) + i),
                         deltax, deltaz);

            vOut[i] = val[0] + deltay * (val[1] - val[0]);
        }
    }
};

} // end namespace pic

#endif /* PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_BILINEAR_HPP */

