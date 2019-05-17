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

#include "../base.hpp"
#include "../image.hpp"
#include "../util/array.hpp"
#include "../metrics/base.hpp"
#include "../metrics/mse.hpp"

namespace pic {

/**
 * @brief PSNR computes the peak signal to noise ratio (PSNR) between two images.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param max_value is the maximum value of the domain of ori and cmp. If ori and comp
 * are normalized 8-bit LDR/SDR images max_value MUST BE 1.0!
 * @param bLargeDifferences, if true, skips big differences for stability.
 * @param type is the domain where to compute MSE (linear, logarithmic, and PU).
 * @return It returns the PSNR value between ori and cmp.
 */
PIC_INLINE double PSNR(Image *ori, Image *cmp, double max_value = -1.0, bool bLargeDifferences = false, METRICS_DOMAIN type = MD_LIN)
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

    if(max_value <= 0.0) {
        float *max_value_ori = ori->getMaxVal(NULL, NULL);
        float *max_value_cmp = cmp->getMaxVal(NULL, NULL);

        int ind;
        float m_ori = Arrayf::getMax(max_value_ori, ori->channels, ind);
        float m_cmp = Arrayf::getMax(max_value_cmp, cmp->channels, ind);

        max_value = double(MAX(m_ori, m_cmp));

        delete[] max_value_ori;
        delete[] max_value_cmp;
    }

    double rmse_value = RMSE(ori, cmp, bLargeDifferences, type);

    max_value = double(changeDomain(float(max_value), type));

    if(rmse_value > 0.0) {
        return 20.0 * log10(max_value / rmse_value);
    } else {
        return -3.0;
    }
}

} // end namespace pic

#endif /* PIC_METRICS_PSNR_HPP */

