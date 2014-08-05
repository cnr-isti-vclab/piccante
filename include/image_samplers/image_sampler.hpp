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

#ifndef PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_HPP
#define PIC_IMAGE_SAMPLERS_IMAGE_SAMPLER_HPP

#include "image.hpp"
#include "util/image_sampler.hpp"

namespace pic {

class ImageSampler
{
public:

    /**
     * @brief ImageSampler
     */
    ImageSampler() {}

    ~ImageSampler() {}

    /**
     * @brief SampleImage samples an image in uniform coordiantes.
     * @param img
     * @param x
     * @param y
     * @param vOut
     */
    virtual void SampleImage(Image *img, float x, float y, float *vOut) {}

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
