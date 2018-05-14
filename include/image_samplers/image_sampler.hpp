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

#ifndef PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_HPP
#define PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_HPP

#include "../image.hpp"
#include "../util/image_sampler.hpp"

namespace pic {

/**
 * @brief The ImageSampler class
 */
class ImageSampler
{
protected:
    int dirs[3];

public:

    /**
     * @brief ImageSampler
     */
    ImageSampler() {}

    ~ImageSampler() {}

    void SetDirection(unsigned int direction)
    {
        dirs[ direction      % 3] = 1;
        dirs[(direction + 1) % 3] = 0;
        dirs[(direction + 2) % 3] = 0;
    }

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param vOut
     */
    virtual void SampleImage(Image *img, float x, float y, float *vOut) {}

    /**
     * @brief SampleImageUC samples an image in unnormalized coordinates [0,width-1]x[0,height-1].
     * @param img
     * @param x
     * @param y
     * @param vOut
     */
    virtual void SampleImageUC(Image *img, float x, float y, float *vOut) {}

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param t
     * @param vOut
     */
    virtual void SampleImage(Image *img, float x, float y, float t, float *vOut) {}
};

} // end namespace pic

#endif /* PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_HPP */
