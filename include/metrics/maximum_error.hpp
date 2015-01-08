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

#ifndef PIC_METRICS_MAXIMUM_ERROR_HPP
#define PIC_METRICS_MAXIMUM_ERROR_HPP

#include <math.h>
#include "image.hpp"
#include "metrics/base.hpp"

namespace pic {

/**MaximumError: maximum error*/
double MaximumError(Image *ori, Image *cmp, bool bLargeDifferences = false)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    int size = ori->size();

    double maxVal = 0.0;

    float largeDifferences = C_LARGE_DIFFERENCESf;
    if(!bLargeDifferences) {
        largeDifferences = FLT_MAX;
    }

    for(int i = 0; i < size; i++) {
        double delta = fabs(ori->data[i] - cmp->data[i]);

        if((delta < C_LARGE_DIFFERENCES) && (maxVal < delta)) {
            maxVal = delta;
        }
    }

    return maxVal;
}

} // end namespace pic

#endif /* PIC_METRICS_MAXIMUM_ERROR_HPP */

