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

#ifndef PIC_METRICS_PSNR_HPP
#define PIC_METRICS_PSNR_HPP

#include <math.h>
#include "image.hpp"
#include "metrics/base.hpp"
#include "metrics/mse.hpp"

namespace pic {

/**PSNR: Peak Signal to Noise Ratio for images in [0, 1]*/
double PSNR(Image *ori, Image *cmp, bool bLargeDifferences = false)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    double mse_value = MSE(ori, cmp, bLargeDifferences);

    if(mse_value > 0.0) {
        return 10.0 * log10(1.0 / mse_value);
    } else {
        return -3.0;
    }
}

/**PSNR: relative Peak Signal to Noise Ratio*/
double rPSNR(Image *ori, Image *cmp, bool bLargeDifferences = false)
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
        double valC = cmp->data[i];

        double delta = valO - valC;
        double maxOC = MAX(valO, valC);

        if(delta <= largeDifferences) {
            count++;

            if(maxOC > pic::C_SINGULARITY) {
                //to avoid singularities
                double tmp = delta / maxOC;
                acc += tmp * tmp;
            }
        }
    }

    return -10.0 * log10(acc / double(count));
}

} // end namespace pic

#endif /* PIC_METRICS_PSNR_HPP */

