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
#include "../metrics/base.hpp"
#include "../util/indexed_array.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_ssim.hpp"

namespace pic {

/**
 * @brief SSIMIndex
 * @param ori
 * @param cmp
 * @param ssim_index
 * @param ssim_map
 * @param K0
 * @param K1
 * @param sigma_window
 * @param dynamic_range
 * @param bDownsampling
 * @return
 */
PIC_INLINE Image* SSIMIndex(Image *ori, Image *cmp, float &ssim_index, Image *ssim_map = NULL, float K0 = 0.01f, float K1 = 0.03f,
                 float sigma_window = 1.5f, float dynamic_range = -1.0f, bool bDownsampling = false)
{
    if(ori == NULL || cmp == NULL) {
        return NULL;
    }

    if(!ori->isSimilarType(cmp)) {
        return NULL;
    }

    Image *ori_d = NULL;
    Image *cmp_d = NULL;

    //downsampling factor as suggested by authors
    if(bDownsampling) {
        float f = MAX(1.0f, lround(MIN(ori->widthf, ori->heightf) / 256.0f));

        #ifdef PIC_DEBUG
            printf("\nDownsampling factor: %f\n", f);
        #endif

        if(f > 1.0f) {
            ori_d = FilterDownSampler2D::execute(ori, NULL, f);
            cmp_d = FilterDownSampler2D::execute(cmp, NULL, f);

            ori = ori_d;
            cmp = cmp_d;
        }
    }

    Image *L_ori = FilterLuminance::execute(ori, NULL);
    Image *L_cmp = FilterLuminance::execute(cmp, NULL);

    if(dynamic_range < 0.0f) {
        float min_val, max_val;
        L_ori->getMaxVal(NULL, &max_val);
        L_ori->getMinVal(NULL, &min_val);

        if(min_val <= 0.0f) {
            IntCoord coord;
            IndexedArray::findSimple(L_ori->data, L_ori->size(), IndexedArray::bFuncNotNeg, coord);
            min_val = IndexedArray::min(L_ori->data, coord);

            if(min_val <= 0.0f) {
                min_val = 1.0f / 255.0f;
            }
        }

        dynamic_range = max_val / min_val;
    }

    float C0 = K0 * dynamic_range;
    C0 = C0 * C0;

    float C1 = K1 * dynamic_range;
    C1 = C1 * C1;

    Image *img_mu1 = FilterGaussian2D::execute(L_ori, NULL, sigma_window);
    Image *img_mu2 = FilterGaussian2D::execute(L_cmp, NULL, sigma_window);

    Image img_ori_cmp = (*L_ori) * (*L_cmp);

    (*L_ori) *= (*L_ori);
    (*L_cmp) *= (*L_cmp);

    Image *img_sigma1_sq = FilterGaussian2D::execute(L_ori, NULL, sigma_window);
    Image *img_sigma2_sq = FilterGaussian2D::execute(L_cmp, NULL, sigma_window);
    Image *img_sigma1_sigma2 = FilterGaussian2D::execute(&img_ori_cmp, NULL, sigma_window);

    if(C0 > 0.0f && C1 > 0.0f) {
        FilterSSIM flt_ssim(C0, C1);

        ImageVec src;
        src.push_back(img_mu1);
        src.push_back(img_mu2);
        src.push_back(img_sigma1_sq);
        src.push_back(img_sigma2_sq);
        src.push_back(img_sigma1_sigma2);

        ssim_map = flt_ssim.ProcessP(src, ssim_map);
    } else {

    }

    ssim_map->getMeanVal(NULL, &ssim_index);

    return ssim_map;
}

} // end namespace pic

#endif /* PIC_METRICS_SSIM_INDEX_HPP */

