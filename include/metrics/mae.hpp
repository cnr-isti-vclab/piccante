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

#ifndef PIC_METRICS_MAE_HPP
#define PIC_METRICS_MAE_HPP

#include <math.h>

#include "../base.hpp"
#include "../image.hpp"
#include "../metrics/base.hpp"

namespace pic {

/**
 * @brief MAE computes the mean abosulute errore (MAE) between two images.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param bLargeDifferences, if true, skips big differences for stability.
 * @return It returns the MAE value between ori and cmp.
 */
PIC_INLINE double MAE(Image *ori, Image *cmp, bool bLargeDifferences = false)
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

    double largeDifferences = C_LARGE_DIFFERENCES;

    if(!bLargeDifferences) {
        largeDifferences = FLT_MAX;
    }

    double acc = 0.0;
    int count = 0;
    for(int i = 0; i < size; i++) {
        double valO = ori->data[i];
        double valc = cmp->data[i];

        double delta = fabs(valO - valc);

        if(delta < largeDifferences) {
            acc += delta;
            count++;
        }
    }

    return acc / double(count);
}

} // end namespace pic

#endif /* PIC_METRICS_MAE_HPP */

