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

#ifndef PIC_METRICS_MSE_HPP
#define PIC_METRICS_MSE_HPP

#include <math.h>

#include "../base.hpp"
#include "../image.hpp"
#include "../metrics/base.hpp"

namespace pic {

/**
 * @brief MSE computes the mean square error (MSE) between two images.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param bLargeDifferences, if true, skips big differences for stability.
 * @param type is the domain where to compute MSE (linear, logarithmic, and PU).
 * @return It returns the MSE value between ori and cmp.
 */
PIC_INLINE double MSE(Image *ori, Image *cmp, bool bLargeDifferences = false, METRICS_DOMAIN type = MD_LIN)
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


    int count = 0;

    float largeDifferences = C_LARGE_DIFFERENCESf;

    if(!bLargeDifferences) {
        largeDifferences = FLT_MAX;
    }

    double acc = 0.0;
    for(int i = 0; i < size; i++) {
        float o_val = changeDomain(ori->data[i], type);
        float c_val = changeDomain(cmp->data[i], type);

        double delta = double(o_val - c_val);

        if(delta <= largeDifferences) {
            acc += delta * delta;
            count++;
        }
    }

    return acc / double(count);
}

/**
 * @brief MSE computes the mean square error (MSE) between two HDR images with given exposure and gamma.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param gamma is the encoding gamma.
 * @param fstop is the f-stop value of the image.
 * @param nBit is the number of bits used for the discretization.
 * @return It returns the MSE value between ori and cmp.
 */
PIC_INLINE double MSE(Image *ori, Image *cmp, float gamma = 2.2f, float fstop = 0.0f, int nBit = 8)
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

    float invGamma = 1.0f / gamma;
    float exposure = powf(2.0f, fstop);

    int area = ori->width * ori->height;
    int size = area * ori->channels;

    unsigned long long acc = 0;

    int nValues = (1 << nBit) - 1;
    float nValuesf = float(nValues);

    for(int i = 0; i < size; i++) {
        int oriLDR = int(nValuesf * (powf(ori->data[i] * exposure, invGamma)));
        int cmpLDR = int(nValuesf * (powf(cmp->data[i] * exposure, invGamma)));

        oriLDR = CLAMPi(oriLDR, 0, nValues);
        cmpLDR = CLAMPi(cmpLDR, 0, nValues);

        int delta = cmpLDR - oriLDR;

        acc += delta * delta;
    }

    return (double(acc) / double(area));
}

/**
 * @brief RMSE computes the root mean squared error (RMSE) between two images.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @param bLargeDifferences, if true, skips big differences for stability.
 * @param type is the domain where to compute RMSE (linear, logarithmic, and PU).
 * @return It returns the MSE value between ori and cmp.
 */
PIC_INLINE double RMSE(Image *ori, Image *cmp, bool bLargeDifferences = false, METRICS_DOMAIN type = MD_LIN)
{
    return sqrt(MSE(ori, cmp, bLargeDifferences, type));
}

} // end namespace pic

#endif /* PIC_METRICS_MSE_HPP */

