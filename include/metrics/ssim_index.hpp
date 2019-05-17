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

#ifndef PIC_METRICS_SSIM_INDEX_HPP
#define PIC_METRICS_SSIM_INDEX_HPP

#include <math.h>

#include "../base.hpp"
#include "../image.hpp"
#include "../util/math.hpp"
#include "../metrics/base.hpp"

#include "../util/array.hpp"
#include "../util/std_util.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_ssim.hpp"

namespace pic {

class SSIMIndex
{
protected:
    float K0, K1, sigma_window, dynamic_range;
    bool bDownsampling;

    FilterLuminance flt_lum;
    FilterGaussian2D flt_gauss2D;
    FilterSSIM flt_ssim;

    METRICS_DOMAIN type;

public:

    SSIMIndex()
    {
        K0 = 0.01f;
        K1 = 0.03f;
        dynamic_range = -1.0f;
        sigma_window = 1.5f;
        type = MD_LIN;
        bDownsampling = true;
        flt_gauss2D.update(sigma_window);
    }

    /**
     * @brief update
     * @param K0
     * @param K1
     * @param sigma_window
     * @param dynamic_range
     * @param bDownsampling
     * @param type
     */
    void update(float K0 = 0.01f,
                float K1 = 0.03f,
                float sigma_window = 1.5f,
                float dynamic_range = -1.0f,
                bool bDownsampling = true,
                METRICS_DOMAIN type = MD_LIN)
    {
        this->K0 = K0 > 0.0f ? K0 : this->K0;
        this->K1 = K1 > 0.0f ? K1 : this->K0;
        this->sigma_window = sigma_window > 0.0f ? sigma_window : this->sigma_window;
        this->dynamic_range = dynamic_range > 0.0f ? dynamic_range : this->dynamic_range;
        this->bDownsampling = bDownsampling;
        this->type = type;

        flt_gauss2D.update(sigma_window);
    }

    /**
     * @brief execute
     * @param ori
     * @param cmp
     */
    Image *execute(ImageVec imgIn, float &ssim_index, Image *ssim_map = NULL)
    {
        ssim_index = -1.0f;

        bool bCheckInput = ImageVecCheck(imgIn, 2) && ImageVecCheckSimilarType(imgIn);

        if(!bCheckInput) {
            return ssim_map;
        }

        Image *ori = imgIn[0];
        Image *cmp = imgIn[1];

        Image *ori_d = NULL;
        Image *cmp_d = NULL;

        bool bAllocated = false;
        if(bDownsampling) {
            float f = MAX(1.0f, lround(MIN(ori->widthf, ori->heightf) / 256.0f));

            #ifdef PIC_DEBUG
                printf("\nDownsampling factor: %f\n", f);
            #endif

            if(f > 1.0f) {
                ori_d = FilterDownSampler2D::execute(ori, NULL, 1.0f / f);
                cmp_d = FilterDownSampler2D::execute(cmp, NULL, 1.0f / f);

                ori = ori_d;
                cmp = cmp_d;

                bAllocated = true;
            }
        }

        Image *L_ori = flt_lum.Process(Single(ori), NULL);
        Image *L_cmp = flt_lum.Process(Single(cmp), NULL);

        switch(type)
        {
            case MD_PU:
            {
                L_ori->applyFunction(PUEncode);
                L_cmp->applyFunction(PUEncode);
            } break;

            case MD_LOG10:
            {
                L_ori->applyFunction(log10fPlusEpsilon);
                L_cmp->applyFunction(log10fPlusEpsilon);
            } break;

            default:
            {

            } break;
        }

        if(dynamic_range <= 0.0f) {
            dynamic_range = L_ori->getDynamicRange(false, 1.0f);
        }

        float C0 = K0 * dynamic_range;
        C0 = C0 * C0;

        float C1 = K1 * dynamic_range;
        C1 = C1 * C1;

        Image *img_mu1 = flt_gauss2D.Process(Single(L_ori), NULL);
        Image *img_mu2 = flt_gauss2D.Process(Single(L_cmp), NULL);

        Image img_ori_cmp = (*L_ori) * (*L_cmp);

        L_ori->applyFunction(square);
        L_cmp->applyFunction(square);

        Image *img_sigma1_sq = flt_gauss2D.Process(Single(L_ori), NULL);
        Image *img_sigma2_sq = flt_gauss2D.Process(Single(L_cmp), NULL);
        Image *img_sigma1_sigma2 = flt_gauss2D.Process(Single(&img_ori_cmp), NULL);

        if(C0 > 0.0f && C1 > 0.0f) {
            flt_ssim.update(C0, C1);

            ImageVec src;
            src.push_back(img_mu1);
            src.push_back(img_mu2);
            src.push_back(img_sigma1_sq);
            src.push_back(img_sigma2_sq);
            src.push_back(img_sigma1_sigma2);

            ssim_map = flt_ssim.Process(src, ssim_map);

            if(ssim_map != NULL) {
                ssim_map->getMeanVal(NULL, &ssim_index);
            }

            stdVectorClear<Image>(src);
        }

        if(bAllocated) {
            auto vec = Double(ori_d, cmp_d);
            stdVectorClear<Image>(vec);
        }

        return ssim_map;
    }

};

} // end namespace pic

#endif /* PIC_METRICS_SSIM_INDEX_HPP */

