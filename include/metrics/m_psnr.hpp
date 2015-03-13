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

#ifndef PIC_METRICS_M_PSNR_HPP
#define PIC_METRICS_M_PSNR_HPP

#include <math.h>
#include "image.hpp"
#include "metrics/base.hpp"
#include "util/indexed_array.hpp"
#include "metrics/mse.hpp"

namespace pic {

/**mPSNR: multiple-exposure peak signal-to-noise ratio*/
/**
 * @brief mPSNR computes the multiple-exposure peak signal-to-noise ratio (mPSNR) between two images.
 * @param ori
 * @param cmp
 * @param minFstop
 * @param maxFstop
 * @return
 */
double mPSNR(Image *ori, Image *cmp, int minFstop, int maxFstop)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    //TO DO: Calculate fstop
    if(minFstop == maxFstop) {

    }

    #ifdef PIC_DEBUG
        printf("-------------------------------------------------------\n");
        printf("-- mPSNR:\n");
        printf("-- min F-stop: %d \t max F-stop: %d\n", minFstop, maxFstop);
    #endif

    double aMSE = 0.0;

    for(int i = minFstop; i <= maxFstop; i++) {
        double tmp = MSE(ori, cmp, 2.2f, float(i));
        #ifdef PIC_DEBUG
            printf("-- Pass: %d \t MSE: %g\n", i, tmp);
       #endif
       aMSE += tmp;
    }

    aMSE /= double(maxFstop - minFstop + 1);

    double MSEconst = double(ori->channels) * 65025.0; //255*255;
    float ret = float(10.0 * log10(MSEconst / aMSE));

    #ifdef PIC_DEBUG
        printf("-- value: %f\n", ret);
        printf("-------------------------------------------------------");
    #endif

    return ret;
}

} // end namespace pic

#endif /* PIC_METRICS_M_PSNR_HPP */

