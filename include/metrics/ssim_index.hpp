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

#include "image.hpp"
#include "metrics/base.hpp"

#include "filtering/filter_luminance.hpp"
#include "filtering/filter_gaussian_2d.hpp"
#include "filtering/filter_downsampler_2d.hpp"

namespace pic {

/**
 * @brief SSIMIndex
 * @param ori
 * @param cmp
 * @param K0
 * @param K1
 * @param sigma_window
 * @param dynamic_range
 * @param bDownsampling
 * @return
 */
float SSIMIndex(Image *ori, Image *cmp, float K0 = 0.01f, float K1 = 0.03f,
                 float sigma_window = 1.5f, float dynamic_range = -1.0f, bool bDownsampling = false)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    //downsampling factor as suggested by authors
    if(bDownsampling) {
        float f = MAX(1.0f, lround(MIN(ori->widthf, ori->heightf) / 256.0f));

        printf("Downsampling factor: %f\n", f);

        if(f > 1.0f) {
            Image *ori_d = FilterDownSampler2D::Execute(ori, NULL, f);
            Image *cmp_d = FilterDownSampler2D::Execute(cmp, NULL, f);

            double ssim_index_value = SSIMIndex(ori_d, cmp_d, K0, K1, sigma_window, dynamic_range, false);

            delete ori_d;
            delete cmp_d;

            return ssim_index_value;
        }
    }

    Image *L_ori = FilterLuminance::Execute(ori, NULL);
    Image *L_cmp = FilterLuminance::Execute(cmp, NULL);

    if(dynamic_range < 0.0f) {
        float min_val, max_val;
        L_ori->getMaxVal(NULL, &max_val);
        L_ori->getMinVal(NULL, &min_val);

        dynamic_range = max_val / min_val;

        printf("%f %f %f\n", min_val, max_val, dynamic_range);
    }

    float C0 = K0 * dynamic_range;
    C0 = C0 * C0;

    float C1 = K1 * dynamic_range;
    C1 = C1 * C1;


    Image *img_mu1 = FilterGaussian2D::Execute(L_ori, NULL, sigma_window);
    Image *img_mu2 = FilterGaussian2D::Execute(L_cmp, NULL, sigma_window);

    Image img_mu1_mu2 = (*img_mu1) * (*img_mu2);

    Image *img_mu1_sq = img_mu1;
    img_mu1_sq->ApplyFunction(Square);

    Image *img_mu2_sq = img_mu2;
    img_mu2_sq->ApplyFunction(Square);

    Image img_ori_cmp = (*L_ori) * (*L_cmp);

    L_ori->ApplyFunction(Square);
    L_cmp->ApplyFunction(Square);

    Image *img_sigma1_sq = FilterGaussian2D::Execute(L_ori, NULL, sigma_window);
    (*img_sigma1_sq) -= (*img_mu1_sq);

    Image *img_sigma2_sq = FilterGaussian2D::Execute(L_cmp, NULL, sigma_window);
    (*img_sigma2_sq) -= (*img_mu2_sq);

    Image *img_sigma12 = FilterGaussian2D::Execute(&img_ori_cmp, NULL, sigma_window);
    (*img_sigma12) -= img_mu1_mu2;


    Image *ssim_map = NULL;

    if(C0 > 0.0f && C1 > 0.0f) {
        //numerator
       img_mu1_mu2 *= 2.0f;
       img_mu1_mu2 += C0;

       (*img_sigma12) *= 2.0f;
       (*img_sigma12) += C1;

       (*img_sigma12) *= img_mu1_mu2;

       //denominator
       (*img_mu1_sq) += (*img_mu2_sq);
       (*img_mu1_sq) += C0;

       (*img_sigma1_sq) += (*img_sigma2_sq);
       (*img_sigma1_sq) += C1;

        (*img_mu1_sq) *= (*img_sigma1_sq);

        //SSIM-map
        (*img_sigma12) /= (*img_mu1_sq);

        ssim_map = img_sigma12;
    } else {

    }

    float ssim_value;
    ssim_map->getMeanVal(NULL, &ssim_value);
    ssim_map->Write("../ssim_map.pfm");

    delete img_mu1;
    delete img_mu2;
    delete L_ori;
    delete L_cmp;

    return ssim_value;
}

} // end namespace pic

#endif /* PIC_METRICS_SSIM_INDEX_HPP */

