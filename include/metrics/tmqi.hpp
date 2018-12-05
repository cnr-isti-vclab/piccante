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
    float a, alpha, beta;

    FilterLuminance flt_lum;
    FilterGaussian2D flt_gauss2D;
    FilterSSIM flt_ssim;

    /**
     * @brief TMQI
     */
    TMQI()
    {
        a = 0.8012f;
        alpha = 0.3046f;
        beta = 0.7088f;
    }

    /**
     * @brief statisticalNaturalness
     * @return
     */
    float statisticalNaturalness(Image *img_LDR)
    {
        if(img_LDR == NULL) {
            return FLT_MAX;
        }

        if(img_LDR->channels != 1) {
            return FLT_MAX;
        }

        float u;
        img_LDR->getMeanVal(NULL, &u);

        TileList tl(11, img_LDR->width, img_LDR->height);

        float sig = 0.0f;
        int n = int(tl.size());
        for(int i = 0; i < n; i++) {
            auto j = tl.getNext();

            auto box = tl.getBBox(j);

            float var_i;
            img_LDR->getVarianceVal(NULL, &box, &var_i);

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
     * @brief execute
     * @param ori
     * @param cmp
     */
    Image *execute(Image *ori, Image *img_LDR)
    {
        return NULL;
    }

};

} // end namespace pic

#endif /* PIC_METRICS_TMQI_HPP */

