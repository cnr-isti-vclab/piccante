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
#include "../util/array.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_ssim.hpp"

namespace pic {

class SSIMIndex
{
public:
    float K0, K1, sigma_window, dynamic_range;
    bool bDownsampling;

    FilterLuminance flt_lum;
    FilterGaussian2D flt_gauss2D;

    SSIMIndex()
    {
        update(0.01f, 0.03f, 1.5f, -1.0f, true);
    }

    /**
     * @brief update
     * @param K0
     * @param K1
     * @param sigma_window
     * @param dynamic_range
     * @param bDownsampling
     */
    void update(float K0 = 0.01f, float  K1 = 0.03f, float  sigma_window = 1.5f, float dynamic_range = -1.0f, bool bDownsampling = true)
    {
        this->K0 = K0;
        this->K1 = K1;
        this->sigma_window = sigma_window;
        this->dynamic_range = dynamic_range;
        this->bDownsampling = bDownsampling;

        flt_gauss2D.update(sigma_window);
    }

    /**
     * @brief getDynamicRange
     * @param img
     * @return
     */
    static float getDynamicRange(Image *img)
    {
        float ret = -1.0f;

        if(img == NULL) {
            return ret;
        }

        if(!img->isValid()) {
            return ret;
        }

        float *min_val_v = img->getMinVal(NULL, NULL);
        float *max_val_v = img->getMaxVal(NULL, NULL);

        int ind;
        float min_val = Arrayf::getMin(min_val_v, img->channels, ind);
        float max_val = Arrayf::getMax(max_val_v, img->channels, ind);

        if(min_val <= 0.0f) {
            IntCoord coord;
            IndexedArray::findSimple(img->data, img->size(), IndexedArray::bFuncNotNeg, coord);
            min_val = IndexedArray::min(img->data, coord);

            if(min_val <= 0.0f) {
                min_val = 1.0f / 255.0f;
            }

            if(max_val > min_val) {
                ret = max_val / min_val;
            } else {
                ret = min_val / max_val;
            }

        }

        if(min_val_v != NULL) {
            delete [] min_val_v;
        }

        if(max_val_v != NULL) {
            delete [] max_val_v;
        }

        return ret;
    }

    /**
     * @brief execute
     * @param ori
     * @param cmp
     */
    Image *execute(Image *ori, Image *cmp, float &ssim_index, Image *ssim_map = NULL)
    {
        ssim_index = -1.0f;

        if(ori == NULL || cmp == NULL) {
            return ssim_map;
        }

        if(!ori->isValid() || !cmp->isValid()) {
            return ssim_map;
        }

        Image *ori_d = NULL;
        Image *cmp_d = NULL;
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

        Image *L_ori = flt_lum.Process(Single(ori), NULL);
        Image *L_cmp = flt_lum.Process(Single(cmp), NULL);

        if(dynamic_range <= 0.0f) {
            dynamic_range = getDynamicRange(L_ori);
        }

        float C0 = K0 * dynamic_range;
        C0 = C0 * C0;

        float C1 = K1 * dynamic_range;
        C1 = C1 * C1;

        Image *img_mu1 = FilterGaussian2D::execute(L_ori, NULL, sigma_window);
        Image *img_mu2 = FilterGaussian2D::execute(L_cmp, NULL, sigma_window);

        Image img_ori_cmp = (*L_ori) * (*L_cmp);

        L_ori->applyFunction(square);
        L_cmp->applyFunction(square);

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

            ssim_map = flt_ssim.Process(src, ssim_map);
        } else {

        }

        ssim_map->getMeanVal(NULL, &ssim_index);

        return ssim_map;
    }

};

} // end namespace pic

#endif /* PIC_METRICS_SSIM_INDEX_HPP */

