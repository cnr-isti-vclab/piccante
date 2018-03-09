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

#ifndef PIC_TONE_MAPPING_DURAND_TMO_HPP
#define PIC_TONE_MAPPING_DURAND_TMO_HPP

#include "base.hpp"

#include "util/string.hpp"
#include "filtering/filter.hpp"
#include "filtering/filter_luminance.hpp"
#include "algorithms/bilateral_separation.hpp"

namespace pic {

/**
 * @brief DurandTMO
 * @param imgIn
 * @param imgOut
 * @param target_contrast
 * @return
 */
PIC_INLINE Image *DurandTMO(Image *imgIn, Image *imgOut = NULL, float target_contrast = 5.0f)
{
    if(imgIn == NULL) {
        return NULL;
    }

    if(imgOut == NULL) {
        imgOut = imgIn->clone();
    } else {
        *imgOut = imgIn;
    }

    if(target_contrast <= 0.0f) {
        target_contrast = 5.0f;
    }

    //luminance image
    Image *lum = FilterLuminance::Execute(imgIn, NULL, LT_CIE_LUMINANCE);

    //bilateral filter seperation
    ImageVec *sep = bilateralSeparation(lum);

    Image *base = sep->at(0);
    Image *detail = sep->at(1);


    base->applyFunction(log10fPlusEpsilon);
    detail->applyFunction(log10fPlusEpsilon);

    float max_log_base = base->getMaxVal()[0];
    float min_log_base = base->getMinVal()[0];

    float compression_factor =  log10fPlusEpsilon(target_contrast) / (max_log_base - min_log_base);
    float log_absoulte = compression_factor * max_log_base;

    *base *= compression_factor;
    *base += detail;
    *base -= log_absoulte;
    base->applyFunction(powf10fe);

    imgOut = imgIn->clone();
    *imgOut /= lum;
    *imgOut *= base;

    delete base;
    delete detail;
    delete lum;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_DURAND_TMO_HPP */

