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

#ifndef PIC_METRICS_RELATIVE_ERROR_HPP
#define PIC_METRICS_RELATIVE_ERROR_HPP

#include <math.h>

#include "../base.hpp"
#include "../image.hpp"

#include "../metrics/base.hpp"

namespace pic {

/**
 * @brief RelativeError computes the relative error between two images.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param bLargeDifferences, if true, skips big differences for stability.
 * @param type is the domain where to compute MSE (linear, logarithmic, and PU).
 * @return It returns the relative error value between ori and cmp.
 */
PIC_INLINE double RelativeError(Image *ori, Image *cmp, bool bLargeDifferences = false, METRICS_DOMAIN type = MD_LIN)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->isValid() || !cmp->isValid()) {
        return -4.0;
    }

    if(!ori->isSimilarType(cmp)) {
        return -1.0;
    }

    int size = ori->size();

    double relErr = 0.0f;
    int count = 0;

    float largeDifferences = C_LARGE_DIFFERENCESf;
    if(!bLargeDifferences) {
        largeDifferences = FLT_MAX;
    }

    for(int i = 0; i < size; i++) {
        double o_val = double(changeDomain(ori->data[i], type));
        double c_val = double(changeDomain(cmp->data[i], type));

        double delta = fabs(o_val - c_val);

        if(delta <= largeDifferences) {
            count++;

            if(o_val > C_SINGULARITY) { //to avoid singularities
                relErr += delta / o_val;
            }
        }
    }

    if(count > 0) {
        return relErr / double(count);
    } else {
        return -3.0;
    }
}

} // end namespace pic

#endif /* PIC_METRICS_RELATIVE_ERROR_HPP */

