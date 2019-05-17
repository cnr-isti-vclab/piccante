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

#ifndef PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_GAUSSIAN_HPP
#define PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_GAUSSIAN_HPP

#include "../image_samplers/image_sampler.hpp"
#include "../util/precomputed_gaussian.hpp"

namespace pic {

/**
 * @brief The ImageSamplerGaussian class
 */
class ImageSamplerGaussian: public ImageSampler
{
protected:
    PrecomputedGaussian *pg;

public:
    /**
     * @brief ImageSamplerGaussian
     */
    ImageSamplerGaussian()
    {
        pg = NULL;
    }

    /**
     * @brief ImageSamplerGaussian
     * @param sigma
     * @param direction
     */
    ImageSamplerGaussian(float sigma, unsigned int direction)
    {
        update(sigma, direction);
    }

    /**
     * @brief update
     * @param sigma
     * @param direction
     */
    void update(float sigma, unsigned int direction)
    {
        if(pg != NULL) {
            delete pg;
            pg = NULL;
        }

        pg = new PrecomputedGaussian(sigma);

        SetDirection(direction);
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
        for(int k = 0; k < img->channels; k++) {
            vOut[k] = 0.0f;
        }

        int ix = int(x * img->widthf);
        int iy = int(y * img->heightf);

        for(int i = 0; i < pg->kernelSize ; i++) {
            int ex = CLAMP(ix + i * dirs[0], img->width);
            int ey = CLAMP(iy + i * dirs[1], img->height);

            int ind = (ey * img->width + ex) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                vOut[k] += img->data[ind] * pg->coeff[i];
                ind++;
            }
        }
    }
};

} // end namespace pic

#endif /* PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_GAUSSIAN_HPP */

