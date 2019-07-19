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
#include "../../util/std_util.hpp"

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

    std::vector<FilterGL*> filters;

    bool bAllocate, bStatisticsRecompute;
    float min_log_base, max_log_base, target_contrast;

    float sigma_s, sigma_r;

    /**
     * @brief allocateFilters
     */
    void allocateFilters()
    {
        bAllocate = true;

        flt_lum = new FilterGLLuminance();
        flt_log10 = new FilterGLOp("log(I0) * " + fromNumberToString(1.0f / logf(10.0f)), true, NULL, NULL);
        flt_durand = new FilterGLDurandTMO();
        flt_bil = new FilterGLBilateral2DS(sigma_s, sigma_r);

        filters.push_back(flt_lum);
        filters.push_back(flt_log10);
        filters.push_back(flt_durand);
        filters.push_back(flt_bil);
    }

public:

    /**
     * @brief DurandTMOGL
     */
    DurandTMOGL(float target_contrast = 5.0f, bool bStatisticsRecompute = true)
    {
        bAllocate = false;

        this->sigma_r = 0.4f;

        update(target_contrast);

        img_lum = NULL;
        img_lum_base = NULL;

        min_log_base = -1e10f;
        max_log_base = -1e10f;

        this->bStatisticsRecompute = bStatisticsRecompute;
    }

    ~DurandTMOGL()
    {       
        stdVectorClear<FilterGL>(filters);

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
     * @brief update
     * @param target_contrast
     */
    void update(float target_contrast)
    {
        this->target_contrast = target_contrast > 0.0f ? target_contrast : 5.0f;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(!bAllocate) {
            allocateFilters();
        }

        this->sigma_s = MAX(imgIn->widthf, imgIn->heightf) * 0.02f;
        flt_bil->update(sigma_s, sigma_r, BF_CLASSIC);

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        img_lum = flt_log10->Process(SingleGL(img_lum), img_lum);

        img_lum_base = flt_bil->Process(SingleGL(img_lum), img_lum_base);

         if(bStatisticsRecompute || (min_log_base < -1e6f) || (max_log_base < -1e6f)) {
            img_lum_base->getMaxVal(&max_log_base);
            img_lum_base->getMinVal(&min_log_base);
         }

        float compression_factor = log10fPlusEpsilon(target_contrast) / (max_log_base - min_log_base);
        float log_absoulte = compression_factor * max_log_base;

        flt_durand->update(compression_factor, log_absoulte);

        return flt_durand->Process(TripleGL(imgIn, img_lum, img_lum_base), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_DURAND_TMO_HPP */

