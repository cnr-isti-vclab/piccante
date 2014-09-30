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

#ifndef PIC_METRICS_MSE_HPP
#define PIC_METRICS_MSE_HPP

#include <math.h>
#include "image_raw.hpp"
#include "metrics/base.hpp"

namespace pic {

/**MSE: mean squared error*/
double MSE(ImageRAW *ori, ImageRAW *cmp, bool bLargeDifferences=false)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    int size = ori->width * ori->height * ori->channels;
    double delta = 0.0;
    double acc   = 0.0;

    int count = 0;

    float largeDifferences = C_LARGE_DIFFERENCESf;
    if(!bLargeDifferences) {
        largeDifferences = FLT_MAX;
    }

    for(int i = 0; i < size; i++) {
        delta = ori->data[i] - cmp->data[i];

        if(delta <= largeDifferences) {
            acc += delta * delta;
            count++;
        }
    }

    return acc / double(count);
}

/** MSE: single exposure MSE at 8-bit*/
double MSE(ImageRAW *ori, ImageRAW *cmp, float gamma, float fstop)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    float invGamma = 1.0f / gamma;
    float exposure = powf(2.0f, fstop);

    int area = ori->width * ori->height;
    int size = area * ori->channels;

    unsigned long long acc = 0;

    for(int i = 0; i < size; i++) {
        int oriLDR = int(255.0f * (powf(ori->data[i] * exposure, invGamma)));
        int cmpLDR = int(255.0f * (powf(cmp->data[i] * exposure, invGamma)));

        oriLDR = CLAMPi(oriLDR, 0, 255);
        cmpLDR = CLAMPi(cmpLDR, 0, 255);

        int delta = cmpLDR - oriLDR;

        acc += delta * delta;
    }

    return (double(acc) / double(area));
}

/** RMSE: root mean squared error*/
double RMSE(ImageRAW *ori, ImageRAW *cmp, bool bLargeDifferences = false)
{
    return sqrt(MSE(ori, cmp, bLargeDifferences));
}

} // end namespace pic

#endif /* PIC_METRICS_MSE_HPP */

