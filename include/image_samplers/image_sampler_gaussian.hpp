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

#include "image_samplers/image_sampler.hpp"

namespace pic {

/**
 * @brief The ImageSamplerGaussian class
 */
class ImageSamplerGaussian: public ImageSampler
{
protected:
    float	sigma;
    float	sigma2;
    int		halfSize;
    int		dirs[3];

public:
    /**
     * @brief ImageSamplerGaussian
     */
    ImageSamplerGaussian();

    /**
     * @brief ImageSamplerGaussian
     * @param sigma
     * @param direction
     */
    ImageSamplerGaussian(float sigma, int direction);

    /**
     * @brief Update
     * @param sigma
     * @param direction
     */
    void Update(float sigma, int direction);

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param vOut
     */
    void SampleImage(Image *img, float x, float y, float *vOut);

    /**
     * @brief SampleImage
     * @param img
     * @param x
     * @param y
     * @param t
     * @param vOut
     */
    void SampleImage(Image *img, float x, float y, float t, float *vOut) {}
};

PIC_INLINE ImageSamplerGaussian::ImageSamplerGaussian()
{
    Update(1.0f, 0);
}

PIC_INLINE ImageSamplerGaussian::ImageSamplerGaussian(float sigma,
        int direction)
{
    Update(sigma, direction);
}

PIC_INLINE void ImageSamplerGaussian::Update(float sigma, int direction)
{
    this->sigma = sigma;
    sigma2      = 2.0f * sigma * sigma;

    halfSize    = MAX(int(sigma * 2.5f), 1);

    dirs[ direction      % 3] = 1;
    dirs[(direction + 1) % 3] = 0;
    dirs[(direction + 2) % 3] = 0;
}

PIC_INLINE void ImageSamplerGaussian::SampleImage(Image *img, float x, float y,
        float *vOut)
{
    for(int k = 0; k < img->channels; k++) {
        vOut[k] = 0.0f;
    }

    int ix = int(x * img->widthf);
    int iy = int(y * img->heightf);

    //Gaussian
    float weight = 0.0f;

    for(int j = -halfSize; j <= halfSize; j++) {
        int ex = CLAMP(ix + j * dirs[0], img->width);
        int ey = CLAMP(iy + j * dirs[1], img->height);

        int ind = (ey * img->width + ex) * img->channels;

        float t = float(j) / float(halfSize);
        float tmpWeight = expf(-(t * t) / sigma2);

        for(int k = 0; k < img->channels; k++) {
            vOut[k] += img->data[ind] * tmpWeight;
            ind++;
        }

        weight += tmpWeight;
    }

    if(weight > 0.0f) {
        for(int k = 0; k < img->channels; k++) {
            vOut[k] /= weight;
        }
    }
}

} // end namespace pic

#endif /* PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_GAUSSIAN_HPP */

