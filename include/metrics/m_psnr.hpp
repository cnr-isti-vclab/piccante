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

#include "../base.hpp"

#include "../util/array.hpp"

#include "../image.hpp"
#include "../tone_mapping/get_all_exposures.hpp"
#include "../metrics/base.hpp"
#include "../metrics/mse.hpp"

namespace pic {

enum MULTI_EXPOSURE_TYPE{MET_HISTOGRAM, MET_MIN_MAX, MET_FROM_INPUT};

/**
 * @brief mPSNR computes the multiple-exposure peak signal-to-noise ratio (mPSNR) between two images.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param type.
 * @param minFstop is the minimum f-stop value of ori.
 * @param maxFstop is the maximum f-stop value of ori.
 * @return It returns the nMPSR error value between ori and cmp.
 */
PIC_INLINE double mPSNR(Image *ori, Image *cmp, MULTI_EXPOSURE_TYPE type, int minFstop = 0, int maxFstop = 0)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->isValid() || !cmp->isValid()) {
        return -3.0;
    }

    if(!ori->isSimilarType(cmp)) {
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

            int nExposures_v = 0;
            float *exposures_v = NULL;
            Arrayf::genRange(float(minFstop), 1.0f, float(maxFstop), exposures_v, nExposures_v);

            exposures.insert(exposures.begin(), exposures_v, exposures_v + nExposures_v);

        } break;

        case MET_FROM_INPUT: {
            for(int i = minFstop; i <= maxFstop; i++) {
                exposures.push_back(float(i));
            }

        } break;
    }

    if(exposures.empty()) {
        return -5.0;
    }

    #ifdef PIC_DEBUG
        printf("-------------------------------------------------------\n");
        printf("-- mPSNR:\n");
        printf("-- min F-stop: %d \t max F-stop: %d\n", minFstop, maxFstop);
    #endif

    int nBit = 8;
    float gamma = 2.2f; //typically 2.2
    auto n = exposures.size();
    double mse = 0.0;
    for(uint i = 0; i < n; i++) {
        double mse_i = MSE(ori, cmp, gamma, exposures[i], nBit);

        #ifdef PIC_DEBUG
            printf("-- Pass: %d \t MSE: %g\n", i, mse_i);
       #endif

        mse += mse_i;
    }

    mse /= double(n * ori->channels);

    int nValues = (1 << nBit) - 1;
    double nValuesd = double(nValues);
    double ret = 10.0 * log10((nValuesd * nValuesd) / mse);

    #ifdef PIC_DEBUG
        printf("-- value: %f\n", ret);
        printf("-------------------------------------------------------");
    #endif

    return ret;
}

} // end namespace pic

#endif /* PIC_METRICS_M_PSNR_HPP */

