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

#ifndef PIC_GL_TONE_MAPPING_DURAND_TMO_HPP
#define PIC_GL_TONE_MAPPING_DURAND_TMO_HPP

#include "util/string.hpp"
#include "util/math.hpp"

#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_bilateral_2ds.hpp"
#include "gl/filtering/filter_op.hpp"
#include "gl/filtering/filter_durand_tmo.hpp"

namespace pic {

/**
 * @brief The DurandTMOGL class
 */
class DurandTMOGL
{
protected:
    FilterGLLuminance       *flt_lum;
    FilterGLBilateral2DS    *filter;
    FilterGLOp              *flt_log10;
    FilterGLDurandTMO       *filter_durand;
    ImageGL                 *img_lum, *img_lum_base;

public:
    /**
     * @brief DurandTMOGL
     */
    DurandTMOGL()
    {
        flt_lum = new FilterGLLuminance();
        flt_log10 = new FilterGLOp("log(I0) * " + NumberToString(1.0f / logf(10.0f)), true, NULL, NULL);

        img_lum = NULL;
        img_lum_base = NULL;

        filter = NULL;
        filter_durand = NULL;
    }

    ~DurandTMOGL()
    {

    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @param target_contrast
     * @return
     */
    ImageGL *Process(ImageGL *imgIn, ImageGL *imgOut = NULL, float target_contrast = 5.0f)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(filter == NULL) {
            float sigma_s = MAX(imgIn->widthf, imgIn->heightf) * 0.02f;
            float sigma_r = 0.4f;
            filter = new FilterGLBilateral2DS(sigma_s, sigma_r);

            filter_durand = new FilterGLDurandTMO();
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        img_lum = flt_log10->Process(SingleGL(img_lum), img_lum);

        img_lum_base = filter->Process(SingleGL(img_lum), img_lum_base);

        float max_log_base, min_log_base;

        img_lum_base->getMaxVal(&max_log_base);
        img_lum_base->getMinVal(&min_log_base);

        float compression_factor = log10fPlusEpsilon(target_contrast) / (max_log_base - min_log_base);
        float log_absoulte = compression_factor * max_log_base;

        filter_durand->Update(compression_factor, log_absoulte);

        return filter_durand->Process(TripleGL(imgIn, img_lum, img_lum_base), imgOut);
    }


};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_DURAND_TMO_HPP */

