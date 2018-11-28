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

#include "../base.hpp"
#include "../util/string.hpp"
#include "../filtering/filter.hpp"
#include "../filtering/filter_bilateral_2ds.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_sigmoid_tmo.hpp"
#include "../tone_mapping/input_estimates.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

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
 * @brief The ReinhardTMO class
 */
class ReinhardTMO : public ToneMappingOperator
{
public:

    float alpha, whitePoint, phi;
    FilterSigmoidTMO flt_sigmoid;
    FilterBilateral2DS flt_bilateral;
    FilterLuminance flt_lum;

    /**
     * @brief ReinhardTMO
     * @param alpha
     * @param whitePoint
     * @param phi
     */
    ReinhardTMO(float alpha = 0.18f, float whitePoint = -1.0f, float phi = 8.0f)
    {
        images.push_back(NULL);
        images.push_back(NULL);
        images.push_back(NULL);
        update(alpha, whitePoint, phi);
    }

    ~ReinhardTMO()
    {
        release();
    }

    /**
     * @brief update
     * @param alpha
     * @param whitePoint
     * @param phi
     */
    void update(float alpha = 0.18f, float whitePoint = -1.0f, float phi = 8.0f)
    {
        this->alpha = alpha;
        this->whitePoint = whitePoint;
        this->phi = phi;

        flt_sigmoid.update(SIG_TMO, alpha, whitePoint, -1.0f, false);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(Image *imgIn, Image *imgOut)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->clone();
        }

        //luminance image
        images[0] = flt_lum.Process(Single(imgIn), images[0]);

        float LMin, LMax, LogAverage;
        images[0]->getMaxVal(NULL, &LMax);
        images[0]->getMinVal(NULL, &LMin);
        images[0]->getLogMeanVal(NULL, &LogAverage);

        bool bUpdate = false;
        if(alpha <= 0.0f) {
            alpha = estimateAlpha(LMin, LMax, LogAverage);
            bUpdate = true;
        }

        if(whitePoint <= 0.0f) {
            whitePoint = estimateWhitePoint(LMin, LMax);
            bUpdate = true;
        }

        //filter luminance in the sigmoid-space
        images[0]->applyFunction(&Sigmoid);

        float s_max = 8.0f;
        float sigma_s = 0.56f * powf(1.6f, s_max);

        float sigma_r = powf(2.0f, phi) * alpha / (s_max * s_max);

        flt_bilateral.update(ST_BRIDSON, sigma_s, sigma_r, 1);

        images[1] = flt_bilateral.Process(Single(images[0]), images[1]);

        images[0]->applyFunction(&SigmoidInv);

        images[1]->applyFunction(&SigmoidInv);

        //apply a sigmoid filter
        if(bUpdate) {
            flt_sigmoid.update(SIG_TMO, alpha, whitePoint, -1.0f, false);
        }

        images[2] = flt_sigmoid.Process(Double(images[0], images[1]), images[2]);

        //remove HDR luminance and replacing it with LDR one
        *imgOut /= *images[0];
        *imgOut *= *images[2];

        imgOut->removeSpecials();

        return imgOut;
    }
};


} // end namespace pic

#endif /* PIC_TONE_MAPPING_REINHARD_TMO_HPP */

