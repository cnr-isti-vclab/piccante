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
#include "image.hpp"
#include "metrics/base.hpp"

namespace pic {

/**MAE: mean absolute error*/
double MAE(Image *ori, Image *cmp, bool bLargeDifferences = false)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    int size = ori->size();

    double acc = 0.0;
    int count = 0;

    double largeDifferences = C_LARGE_DIFFERENCES;
    if(!bLargeDifferences) {
        largeDifferences = FLT_MAX;
    }

    for(int i = 0; i < size; i++) {
        double valO = ori->data[i];
        double valc = cmp->data[i];

        double delta = fabs(valO - valc);

        if(delta <= largeDifferences) {
            count++;
            acc += delta;
        }
    }

    acc /= double(count);

    return float(acc);
}

} // end namespace pic

#endif /* PIC_METRICS_MAE_HPP */

