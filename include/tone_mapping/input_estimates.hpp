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

#ifndef PIC_TONE_MAPPING_INPUT_ESTIMATES_HPP_
#define PIC_TONE_MAPPING_INPUT_ESTIMATES_HPP_

namespace pic {

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

} // end namespace pic

#endif /* PIC_TONE_MAPPING_INPUT_ESTIMATES_HPP_ */
