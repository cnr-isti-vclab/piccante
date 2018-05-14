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

#include "../../util/string.hpp"
#include "../../util/math.hpp"

#include "../../gl/filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter_bilateral_2ds.hpp"
#include "../../gl/filtering/filter_op.hpp"
#include "../../gl/filtering/filter_durand_tmo.hpp"

namespace pic {

/**
 * @brief The DurandTMOGL class
 */
class DurandTMOGL
{
protected:
    FilterGLLuminance       *flt_lum;
    FilterGLBilateral2DS    *flt_bil;
    FilterGLOp              *flt_log10;
    FilterGLDurandTMO       *flt_durand;
    ImageGL                 *img_lum, *img_lum_base;

    bool                    bStatisticsRecompute;
    float                   min_log_base, max_log_base;

    /**
     * @brief AllocateFilters
     */
    void AllocateFilters()
    {
        flt_lum = new FilterGLLuminance();
        flt_log10 = new FilterGLOp("log(I0) * " + fromNumberToString(1.0f / logf(10.0f)), true, NULL, NULL);
        flt_durand = new FilterGLDurandTMO();
    }

public:
    /**
     * @brief DurandTMOGL
     */
    DurandTMOGL(bool bStatisticsRecompute = true)
    {
        flt_lum = NULL;
        flt_log10 = NULL;

        img_lum = NULL;
        img_lum_base = NULL;

        flt_bil = NULL;
        flt_durand = NULL;

        min_log_base = -1e10f;
        max_log_base = -1e10f;

        this->bStatisticsRecompute = bStatisticsRecompute;
    }

    ~DurandTMOGL()
    {       
        if(flt_lum != NULL) {
            delete flt_lum;
            flt_lum = NULL;
        }

        if(flt_log10 != NULL) {
            delete flt_log10;
            flt_log10 = NULL;
        }

        if(flt_bil != NULL) {
            delete flt_bil;
            flt_bil = NULL;
        }

        if(flt_durand != NULL) {
            delete flt_durand;
            flt_durand = NULL;
        }

        if(img_lum != NULL) {
            delete img_lum;
            img_lum = NULL;
        }

        if(img_lum_base != NULL) {
            delete img_lum_base;
            img_lum_base = NULL;
        }
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

        if(flt_bil == NULL) {
            float sigma_s = MAX(imgIn->widthf, imgIn->heightf) * 0.02f;
            float sigma_r = 0.4f;
            flt_bil = new FilterGLBilateral2DS(sigma_s, sigma_r);
            AllocateFilters();
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        img_lum = flt_log10->Process(SingleGL(img_lum), img_lum);

        img_lum_base = flt_bil->Process(SingleGL(img_lum), img_lum_base);

         if(bStatisticsRecompute || (min_log_base < -1e6f) || (max_log_base < -1e6f)) {
            img_lum_base->getMaxVal(&max_log_base);
            img_lum_base->getMinVal(&min_log_base);
         }

        float compression_factor = log10fPlusEpsilon(target_contrast) / (max_log_base - min_log_base);
        float log_absoulte = compression_factor * max_log_base;

        flt_durand->Update(compression_factor, log_absoulte);

        return flt_durand->Process(TripleGL(imgIn, img_lum, img_lum_base), imgOut);
    }


};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_DURAND_TMO_HPP */

