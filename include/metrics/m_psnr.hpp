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

#include "util/array.hpp"

#include "image.hpp"
#include "tone_mapping/get_all_exposures.hpp"
#include "metrics/base.hpp"
#include "metrics/mse.hpp"

namespace pic {

enum MULTI_EXPOSURE_TYPE{MET_HISTOGRAM, MET_MIN_MAX};

/**
 * @brief mPSNR computes the multiple-exposure peak signal-to-noise ratio (mPSNR) between two images.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param type.
 * @param minFstop is the minimum f-stop value of ori.
 * @param maxFstop is the maximum f-stop value of ori.
 * @return It returns the nMPSR error value between ori and cmp.
 */
double mPSNR(Image *ori, Image *cmp, MULTI_EXPOSURE_TYPE type, int minFstop = 0, int maxFstop = 0)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    std::vector<float> exposures;

    switch (type) {
        case MET_HISTOGRAM: {
            exposures = getAllExposures(ori);
        } break;

        case MET_MIN_MAX: {
            if(minFstop == maxFstop) {
                getMinMaxFstops(ori, minFstop, maxFstop);
            }

            Array<float>::genRange(float(minFstop), 1.0f, float(maxFstop), exposures);
        } break;
    }

    #ifdef PIC_DEBUG
        printf("-------------------------------------------------------\n");
        printf("-- mPSNR:\n");
        printf("-- min F-stop: %d \t max F-stop: %d\n", minFstop, maxFstop);
    #endif

    double aMSE = 0.0;
    float gamma = 2.2f; //typically 2.2
    for(unsigned int i=0; i<exposures.size(); i++) {
        double tmp = MSE(ori, cmp, gamma, exposures[i]);

        #ifdef PIC_DEBUG
            printf("-- Pass: %d \t MSE: %g\n", i, tmp);
       #endif

        aMSE += tmp;
    }

    aMSE /= double(maxFstop - minFstop + 1);

    double MSEconst = double(ori->channels) * 65025.0; //NOTE: 65025 = 255 * 255;
    float ret = float(10.0 * log10(MSEconst / aMSE));

    #ifdef PIC_DEBUG
        printf("-- value: %f\n", ret);
        printf("-------------------------------------------------------");
    #endif

    return ret;
}

} // end namespace pic

#endif /* PIC_METRICS_M_PSNR_HPP */

