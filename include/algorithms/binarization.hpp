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

#ifndef PIC_ALGORITHMS_BINARIZATION_HPP
#define PIC_ALGORITHMS_BINARIZATION_HPP

#include "../base.hpp"

#include "../image.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_threshold.hpp"

namespace pic {

/**
 * @brief binarization
 * @param imgIn
 * @param bAdaptive
 * @return
 */
PIC_INLINE Image *binarization(Image *imgIn, Image *imgOut = NULL, bool bAdaptive = false)
{
    if(imgIn == NULL) {
        return NULL;
    }

    Image *imgIn_lum = FilterLuminance::execute(imgIn, NULL);

    if(bAdaptive) {
        FilterThreshold flt_thr(0.0f, true);

        FilterGaussian2D flt_gauss(MIN(imgIn->widthf, imgIn->heightf) * 0.2f);
        Image *imgIn_lum_flt = flt_gauss.Process(Single(imgIn_lum), NULL);

        imgOut = flt_thr.Process(Double(imgIn_lum, imgIn_lum_flt), imgOut);

        delete imgIn_lum_flt;
    } else {
        float mean_lum;

        imgIn_lum->getMeanVal(NULL, &mean_lum);

        FilterThreshold flt_thr(mean_lum, false);

        imgOut = flt_thr.Process(Single(imgIn_lum), imgOut);
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_BINARIZATION_HPP */

