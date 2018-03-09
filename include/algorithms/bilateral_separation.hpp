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

#ifndef PIC_ALGORITHMS_BILATERAL_SEPARATION_HPP
#define PIC_ALGORITHMS_BILATERAL_SEPARATION_HPP

#include "image.hpp"
#include "filtering/filter_bilateral_2ds.hpp"
#include "util/math.hpp"

namespace pic {

/**
 * @brief powf10fe
 * @param x
 * @return
 */
PIC_INLINE float powf10fe(float x)
{
    return MAX(powf(10.0f, x) - 1e-6f, 0.0f);
}

/**
 * @brief bilateralSeparation
 * @param imgIn
 * @param sigma_s
 * @param simga_r
 * @return
 */
PIC_INLINE ImageVec* bilateralSeparation(Image *imgIn, float sigma_s = -1.0f, float sigma_r = 0.4f)
{
    if(imgIn == NULL) {
        return NULL;
    }

    ImageVec *out = new ImageVec;

    if(sigma_s <= 0.0f) {
        sigma_s = MAX(imgIn->widthf, imgIn->heightf) * 0.02f;
    }

    if(sigma_r <= 0.0f) {
        sigma_r = 0.4f;
    }

    Image *img_tmp = imgIn->clone();

    img_tmp->applyFunction(log10fPlusEpsilon);

    Image *img_flt = FilterBilateral2DS::Execute(img_tmp, NULL, sigma_s, sigma_r);

    img_flt->applyFunction(powf10fe);

    Image *img_detail = imgIn->clone();

    *img_detail /= *img_flt;
    img_detail->removeSpecials();

    out->push_back(img_flt);
    out->push_back(img_detail);

    delete img_tmp;

    return out;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_BILATERAL_SEPARATION_HPP */

