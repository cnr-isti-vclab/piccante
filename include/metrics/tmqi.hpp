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

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_ssim.hpp"

namespace pic {

class TMQI
{
public:
    float a, alpha, beta;

    FilterLuminance flt_lum;
    FilterGaussian2D flt_gauss2D;
    FilterSSIM flt_ssim;

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

