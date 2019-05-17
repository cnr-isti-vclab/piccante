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
#include "../util/math.hpp"
#include "../filtering/filter.hpp"
#include "../filtering/filter_bilateral_2ds.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_sigmoid_tmo.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The ReinhardTMO class
 */
class ReinhardTMO : public ToneMappingOperator
{
protected:

    /**
     * @brief sigmoidParam
     * @param x
     * @param param
     * @return
     */
    static float sigmoidParam(float x, std::vector< float > &param)
    {
        float x_s = x * param[0];

        return x_s / (x_s + param[1]);
    }

    /**
     * @brief sigmoidInvParam
     * @param y
     * @param param
     * @return
     */
    static float sigmoidInvParam(float y, std::vector< float > &param)
    {
        float x_s = y *  param[1] / (1.0f -  y);

        return x_s / param[0];
    }

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessAux(ImageVec imgIn, Image *imgOut)
    {
        //luminance image
        images[0] = flt_lum.Process(imgIn, images[0]);

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

        if(bUpdate) {
            flt_sigmoid.update(this->sig_mode, this->alpha, this->whitePoint, -1.0f, false);
        }

        //filter luminance in the sigmoid-space
        if(phi > 0.0f) {
            float s_max = 8.0f;
            float value = powf(2.0f, phi) * alpha / (s_max * s_max);

            std::vector<float> param;
            param.push_back(alpha / LogAverage);
            param.push_back(value);

            float pEpsilon = 0.05f; //threshold
            images[0]->applyFunctionParam(sigmoidParam, param);//applyFunction(&sigmoid);


            flt_bilateral.update(1.6f, pEpsilon / 2.0f);

            images[1] = flt_bilateral.Process(Single(images[0]), images[1]);

            images[0]->applyFunctionParam(sigmoidInvParam, param);
            images[1]->applyFunctionParam(sigmoidInvParam, param);

            images[2] = flt_sigmoid.Process(Double(images[0], images[1]), images[2]);
        } else {
            images[2] = flt_sigmoid.Process(Single(images[0]), images[2]);
        }

        //remove HDR luminance and replacing it with LDR one
        *imgOut = *imgIn[0];
        *imgOut /= *images[0];
        *imgOut *= *images[2];

        imgOut->removeSpecials();

        return imgOut;
    }

    SIGMOID_MODE sig_mode;
    float alpha, whitePoint, phi;
    FilterSigmoidTMO flt_sigmoid;
    FilterBilateral2DS flt_bilateral;
    FilterLuminance flt_lum;

public:

    /**
     * @brief ReinhardTMO
     * @param alpha
     * @param whitePoint
     * @param phi
     * @param sig_mode
     */
    ReinhardTMO(float alpha = 0.18f, float whitePoint = -1.0f, float phi = 8.0f, SIGMOID_MODE sig_mode = SIG_TMO)
    {
        images.push_back(NULL);
        images.push_back(NULL);
        images.push_back(NULL);
        update(alpha, whitePoint, phi, sig_mode);
    }

    ~ReinhardTMO()
    {
        release();
    }

    /**
     * @brief estimateAlpha
     * @param LMin
     * @param LMax
     * @param logAverage
     * @return
     */
    static float estimateAlpha(float LMin, float LMax, float logAverage)
    {
        float log2f       = logf(2.0f);
        float log2Max     = logf(LMax       + 1e-9f) / log2f;
        float log2Min     = logf(LMin       + 1e-9f) / log2f;
        float log2Average = logf(logAverage + 1e-9f) / log2f;

        float tmp = (2.0f * log2Average - log2Min - log2Max) / (log2Max - log2Min);

        return 0.18f * powf(4.0f, tmp);
    }

    /**
     * @brief estimateWhitePoint
     * @param LMin
     * @param LMax
     * @return
     */
    static float estimateWhitePoint(float LMin, float LMax)
    {
        float log2f       = logf(2.0f);
        float log2Max     = logf(LMax + 1e-9f) / log2f;
        float log2Min     = logf(LMin + 1e-9f) / log2f;

        return 1.5f * powf(2.0f, (log2Max - log2Min - 5.0f));
    }

    /**
     * @brief update
     * @param alpha
     * @param whitePoint
     * @param phi
     * @param sig_mode
     */
    void update(float alpha = 0.18f, float whitePoint = 1e6f, float phi = 8.0f, SIGMOID_MODE sig_mode = SIG_TMO)
    {
        this->alpha = alpha;
        this->whitePoint = whitePoint;
        this->phi = phi;
        this->sig_mode = sig_mode;

        flt_sigmoid.update(SIG_TMO, this->alpha, this->whitePoint, -1.0f, false);
    }

    /**
     * @brief executeGlobal1
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image* executeGlobal1(Image *imgIn, Image *imgOut)
    {
        ReinhardTMO rtmo(-1.0f, -1.0f, -8.0f, SIG_TMO);
        return rtmo.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief executeGlobal2
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image* executeGlobal2(Image *imgIn, Image *imgOut)
    {
        ReinhardTMO rtmo(-1.0f, -1.0f, -8.0f, SIG_TMO_WP);
        return rtmo.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief executeLocal1
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image* executeLocal1(Image *imgIn, Image *imgOut)
    {
        ReinhardTMO rtmo(-1.0f, -1.0f, 8.0f, SIG_TMO);
        return rtmo.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief executeLocal2
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image* executeLocal2(Image *imgIn, Image *imgOut)
    {
        ReinhardTMO rtmo(-1.0f, -1.0f, 8.0f, SIG_TMO_WP);
        return rtmo.Process(Single(imgIn), imgOut);
    }
};


} // end namespace pic

#endif /* PIC_TONE_MAPPING_REINHARD_TMO_HPP */

