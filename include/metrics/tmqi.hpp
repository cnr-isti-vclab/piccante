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

#ifndef PIC_METRICS_TMQI_HPP
#define PIC_METRICS_TMQI_HPP

#include <math.h>

#include "../base.hpp"
#include "../image.hpp"
#include "../metrics/base.hpp"
#include "../util/indexed_array.hpp"
#include "../util/array.hpp"
#include "../util/math.hpp"
#include "../util/tile_list.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_ssim.hpp"

namespace pic {

/**
 * @brief The TMQI class
 */
class TMQI
{
public:
    float a, invA, alpha, beta;
    std::vector<float> weights;
    FilterLuminance flt_lum;
    FilterGaussian2D flt_gauss2D;
    FilterSSIM flt_ssim;

    /**
     * @brief TMQI
     */
    TMQI()
    {
        a = 0.8012f;
        invA = 1.0f - a;
        alpha = 0.3046f;
        beta = 0.7088f;

        weights.push_back(0.0448f);
        weights.push_back(0.2856f);
        weights.push_back(0.3001f);
        weights.push_back(0.2363f);
        weights.push_back(0.1333f);
    }

    /**
     * @brief statisticalNaturalness
     * @return
     */
    float statisticalNaturalness(Image *L_LDR)
    {
        if(L_LDR == NULL) {
            return FLT_MAX;
        }

        if(L_LDR->channels != 1) {
            return FLT_MAX;
        }

        float u;
        L_LDR->getMeanVal(NULL, &u);

        TileList tl(11, L_LDR->width, L_LDR->height);

        float sig = 0.0f;
        int n = int(tl.size());
        for(int i = 0; i < n; i++) {
            auto j = tl.getNext();

            auto box = tl.getBBox(j);

            float var_i;
            L_LDR->getVarianceVal(NULL, &box, &var_i);

            sig += sqrtf(var_i);
        }

        sig /= float(n);

        float p_hat[] ={4.4f, 10.1f};
        float beta_mode = (p_hat[0] - 1.0f) / (p_hat[0] + p_hat[1] - 2.0f);

        float C_0 = betaPDF(beta_mode, p_hat[0], p_hat[1]);
        float C = betaPDF(sig / 64.29f, p_hat[0], p_hat[1]);
        float pc = C / C_0;

        float mu_hat = 115.94f;
        float sigma_hat = 27.99f;

        float B = normalDistribution(u, mu_hat, sigma_hat);
        float B_0 = normalDistribution(mu_hat, mu_hat, sigma_hat);

        float pb = B / B_0;

        return pb * pc;
    }

    /**
     * @brief structuralFidelity
     * @param L_HDR
     * @param L_LDR
     * @param S
     * @param s_map
     * @return
     */
    Image* structuralFidelity(Image *L_HDR, Image *L_LDR, float &S, Image *s_map = NULL)
    {
        return s_map;
    }

    /**
     * @brief execute
     * @param img_HDR
     * @param img_LDR
     * @param Q
     * @param N
     * @param S
     * @param tmqi_map
     * @return
     */
    Image *execute(Image *img_HDR, Image *img_LDR, float &Q, float &N, float &S, Image *tmqi_map = NULL)
    {
        N = -1.0f;
        S = -1.0f;
        Q = -1.0f;

        if(img_HDR == NULL || img_LDR == NULL) {
            return tmqi_map;
        }

        if(!img_HDR->isValid() || !img_LDR->isValid()) {
            return tmqi_map;
        }

        if(!img_HDR->isSimilarType(img_LDR)) {
            return tmqi_map;
        }

        float max_img_LDR;
        img_LDR->getMaxVal(NULL, &max_img_LDR);

        if(max_img_LDR <= 1.0f) {
            return tmqi_map;
        }

        Image *L_HDR = flt_lum.Process(Single(img_HDR), NULL);
        Image *L_LDR = flt_lum.Process(Single(img_LDR), NULL);

        float min_L_HDR, max_L_HDR;

        L_HDR->getMinVal(NULL, &min_L_HDR);
        L_HDR->getMaxVal(NULL, &max_L_HDR);

        *L_HDR -= min_L_HDR;

        float scale = (powf(2.0f, 32.0f) - 1.0f) / (max_L_HDR - min_L_HDR);
        *L_HDR *= scale;

        N = statisticalNaturalness(L_LDR);
        tmqi_map = structuralFidelity(L_HDR, L_LDR, S, tmqi_map);

        Q = a * powf(S, alpha) + invA * powf(N, beta);

        return tmqi_map;
    }

};

} // end namespace pic

#endif /* PIC_METRICS_TMQI_HPP */

