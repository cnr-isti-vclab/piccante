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

#ifndef PIC_TONE_MAPPING_REINHARD_TMO_HPP
#define PIC_TONE_MAPPING_REINHARD_TMO_HPP

#include "util/string.hpp"
#include "filtering/filter.hpp"
#include "filtering/filter_bilateral_2ds.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_sigmoid_tmo.hpp"

namespace pic {

/**
 * @brief Sigmoid
 * @param x
 * @return
 */
inline float Sigmoid(float x)
{
    return x / (x + 1.0f);
}

/**
 * @brief SigmoidInv
 * @param x
 * @return
 */
inline float SigmoidInv(float x)
{
    return x / (1.0f - x);
}

/**
 * @brief EstimateAlpha
 * @param LMax
 * @param LMin
 * @param logAverage
 * @return
 */
inline float EstimateAlpha(float LMax, float LMin, float logAverage)
{

    float log2f       = logf(2.0f);
    float log2Max     = logf(LMax      + 1e-9f) / log2f;
    float log2Min     = logf(LMin      + 1e-9f) / log2f;
    float log2Average = logf(logAverage + 1e-9f) / log2f;

    float tmp = (2.0f * log2Average - log2Min - log2Max) / (log2Max - log2Min);

    return 0.18f * powf(4.0f, tmp);
}

/**
 * @brief EstimateWhitePoint
 * @param LMax
 * @param LMin
 * @return
 */
inline float EstimateWhitePoint(float LMax, float LMin)
{

    float log2f       = logf(2.0f);
    float log2Max     = logf(LMax + 1e-9f) / log2f;
    float log2Min     = logf(LMin + 1e-9f) / log2f;

    return 1.5f * powf(2.0f, (log2Max - log2Min - 5.0f));
}

/**
 * @brief ReinhardTMO
 * @param imgIn
 * @param imgOut
 * @param alpha
 * @param whitePoint
 * @param phi
 * @return
 */
Image *ReinhardTMO(Image *imgIn, Image *imgOut = NULL, float alpha = 0.18f,
                      float whitePoint = -1.0f, float phi = 8.0f)
{
    if(imgIn == NULL) {
        return NULL;
    }

    if(imgOut == NULL) {
        imgOut = imgIn->Clone();
    }

    //luminance image
    Image *lum = FilterLuminance::Execute(imgIn, NULL, LT_CIE_LUMINANCE);

    float LMax = lum->getMaxVal()[0];
    float LMin = lum->getMinVal()[0];
    float LogAverage = lum->getLogMeanVal()[0];

    if(alpha <= 0.0f) {
        alpha = EstimateAlpha(LMax, LMin, LogAverage);
    }

    if(whitePoint <= 0.0f) {
        whitePoint = EstimateWhitePoint(LMax, LMin);
    }

    //Filtering luminance in the sigmoid-space
    lum->ApplyFunction(&Sigmoid);

    float s_max = 8.0f;
    float sigma_s = 0.56f * powf(1.6f, s_max);

    float sigma_r = powf(2.0f, phi) * alpha / (s_max * s_max);

    Image *filteredLum = FilterBilateral2DS::Execute(lum, NULL, sigma_s,
                            sigma_r);

    lum->ApplyFunction(&SigmoidInv);

    filteredLum->ApplyFunction(&SigmoidInv);

    //Applying a sigmoid filter
    FilterSigmoidTMO fSTMO(SIG_TMO, alpha, whitePoint, LogAverage);
    Image *tonemapped = fSTMO.Process(Double(lum, filteredLum), NULL);

    //Removing HDR luminance and replacing it with LDR one
    *imgOut /= *lum;
    *imgOut *= *tonemapped;

    imgOut->removeSpecials();

    //Freeing memory
    delete filteredLum;
    delete lum;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_REINHARD_TMO_HPP */

