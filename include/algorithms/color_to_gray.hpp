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

#ifndef PIC_ALGORITHMS_COLOR_TO_GRAY_HPP
#define PIC_ALGORITHMS_COLOR_TO_GRAY_HPP

#include "image.hpp"
#include "filtering/filter_channel.hpp"
#include "tone_mapping/exposure_fusion.hpp"

namespace pic {

/**
 * @brief ColorToGray
 * @param imgIn
 * @param imgOut
 * @return
 */
Image *ColorToGray(Image *imgIn, Image *imgOut)
{
    if(imgIn == NULL){
        return imgOut;
    }

    if(imgOut == NULL){
        imgOut = new Image(1, imgIn->width, imgIn->height, 1);
    }

    ImageVec img_vec;
    ImageVec input = Single(imgIn);

    FilterChannel flt(0);
    int channels = imgIn->channels;
    for(int i = 0; i < channels; i++) {
        img_vec.push_back(flt.ProcessP(input, NULL));
        flt.setChannel(i + 1);
    }

    imgOut = ExposureFusion(img_vec, 1.0f, 1.0f, 0.0f, imgOut);

    for(int i=0; i<channels; i++) {
        delete img_vec[i];
    }
    return imgOut;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_COLOR_TO_GRAY_HPP */

