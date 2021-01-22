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
#include "../image_vec.hpp"

#include "../metrics/base.hpp"

#include "../util/indexed_array.hpp"
#include "../util/array.hpp"
#include "../util/math.hpp"
#include "../util/std_util.hpp"
#include "../util/tile_list.hpp"
#include "../util/string.hpp"

#include "../algorithms/pyramid.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_down_pp.hpp"

#include "../filtering/filter_tmqi.hpp"

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
    FilterTMQI flt_tmqi;

    /**
     * @brief TMQI
     */
    TMQI()
    {
        a = 0.8012f;
        invA = 1.0f - a;
        alpha = 0.3046f;
        beta = 0.7088f;

        flt_gauss2D.update(1.5f);

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
     * @brief localStructuralFidelity
     * @param L_HDR
     * @param L_LDR
     * @param S
     * @param s_map
     * @return
     */
    Image* localStructuralFidelity(Image *L_HDR, Image *L_LDR, float sf, float &S, Image *s_map = NULL)
    {        
        Image *img1 = L_HDR->clone();
        Image *img2 = L_LDR->clone();

        Image *mu1 = flt_gauss2D.Process(Single(img1), NULL);
        Image *mu2 = flt_gauss2D.Process(Single(img2), NULL);

        Image img12 = (*L_HDR) * (*L_LDR);

        delete L_HDR;
        delete L_LDR;

        Image mu12 = (*mu1) * (*mu2);

        mu1->applyFunction(square);
        mu2->applyFunction(square);
        img1->applyFunction(square);
        img2->applyFunction(square);

        Image *sigma1 = flt_gauss2D.Process(Single(img1), NULL);
        *sigma1 -= *mu1;
        delete mu1;
        sigma1->applyFunction(sqrtf_s);

        Image *sigma2 = flt_gauss2D.Process(Single(img2), NULL);
        *sigma2 -= *mu2;
        delete mu2;
        sigma2->applyFunction(sqrtf_s);

        Image *sigma12 = flt_gauss2D.Process(Single(&img12), NULL);
        *sigma12 -= mu12;
        mu12.release();

        flt_tmqi.update(sf);

        ImageVec vec = Triple(sigma1, sigma2, sigma12);
        s_map = flt_tmqi.Process(vec, s_map);

        s_map->getMeanVal(NULL, &S);

        stdVectorClear<Image>(vec);
        delete_s(img1);
        delete_s(img2);

        return s_map;
    }

    /**
     * @brief structuralFidelity
     * @param L_HDR
     * @param L_LDR
     * @return
     */
    float structuralFidelity(Image *L_HDR, Image *L_LDR)
    {
        float S = 1.0f;
        float f = 32.0f;

        Image *t_HDR = L_HDR;
        Image *t_LDR = L_LDR;
        for(uint i = 0; i < weights.size(); i++) {
            f /= 2.0f;

            if(t_HDR != NULL && t_LDR != NULL) {
                float S_i;
                localStructuralFidelity(t_HDR, t_LDR, f, S_i, NULL);

                S *= powf(S_i, weights[i]);

                int width = t_HDR->width >> 1;
                int height = t_HDR->height >> 1;
                t_HDR = FilterSampler2D::execute(t_HDR, NULL, width, height);
                t_LDR = FilterSampler2D::execute(t_LDR, NULL, width, height);
            } else {
                break;
            }
        }

        return S;
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
    Image *execute(ImageVec imgIn, float &Q, float &N, float &S, Image *tmqi_map = NULL)
    {
        N = -1.0f;
        S = -1.0f;
        Q = -1.0f;

        bool bCheckInput = ImageVecCheck(imgIn, 2) && ImageVecCheckSimilarType(imgIn);

        if(!bCheckInput) {
            return tmqi_map;
        }

        float max_img_LDR;
        imgIn[1]->getMaxVal(NULL, &max_img_LDR);

        if(max_img_LDR <= 1.0f) {
            return tmqi_map;
        }

        Image *L_HDR = flt_lum.Process(Single(imgIn[0]), NULL);
        Image *L_LDR = flt_lum.Process(Single(imgIn[1]), NULL);

        float min_L_HDR, max_L_HDR;

        L_HDR->getMinVal(NULL, &min_L_HDR);
        L_HDR->getMaxVal(NULL, &max_L_HDR);

        *L_HDR -= min_L_HDR;

        float scale = (powf(2.0f, 32.0f) - 1.0f) / (max_L_HDR - min_L_HDR);
        *L_HDR *= scale;

        N = statisticalNaturalness(L_LDR);

        S = structuralFidelity(L_HDR, L_LDR);

        Q = a * powf(S, alpha) + invA * powf(N, beta);

        return tmqi_map;
    }

};

} // end namespace pic

#endif /* PIC_METRICS_TMQI_HPP */

