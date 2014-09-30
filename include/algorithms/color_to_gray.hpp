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

#ifndef PIC_ALGORITHMS_COLOR_TO_GRAY_HPP
#define PIC_ALGORITHMS_COLOR_TO_GRAY_HPP

#include "image_raw.hpp"
#include "filtering/filter_channel.hpp"
#include "tone_mapping/exposure_fusion.hpp"

namespace pic {

/**
 * @brief ColorToGray
 * @param imgIn
 * @param imgOut
 * @return
 */
ImageRAW *ColorToGray(ImageRAW *imgIn, ImageRAW *imgOut)
{
    if(imgIn == NULL){
        return imgOut;
    }

    if(imgOut == NULL){
        imgOut = new ImageRAW(1, imgIn->width, imgIn->height, 1);
    }

    ImageRAWVec img_vec;
    ImageRAWVec input = Single(imgIn);

    FilterChannel flc(0);
    int channels = imgIn->channels;
    for(int i = 0; i < channels; i++) {
        flc.setChannel(i);
        img_vec.push_back(flc.ProcessP(input, NULL));
    }

    imgOut = ExposureFusion(img_vec, imgOut, 1.0f, 1.0f, 0.0f);

    for(int i=0; i<channels; i++) {
        delete img_vec[i];
    }
    return imgOut;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_COLOR_TO_GRAY_HPP */

