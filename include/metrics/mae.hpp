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

#ifndef PIC_METRICS_MAE_HPP
#define PIC_METRICS_MAE_HPP

#include <math.h>
#include "image_raw.hpp"
#include "metrics/base.hpp"

namespace pic {

/**MAE: mean absolute error*/
double MAE(ImageRAW *ori, ImageRAW *cmp, bool bLargeDifferences = false)
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

    float largeDifferences = C_LARGE_DIFFERENCES;
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

