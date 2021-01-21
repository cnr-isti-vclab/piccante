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

#ifndef PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP
#define PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP

#include "../../util/math.hpp"

#include "../../gl/filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter_sigmoid_tmo.hpp"
#include "../../gl/filtering/filter_bilateral_2ds.hpp"
#include "../../gl/filtering/filter_op.hpp"

#include "../../gl/filtering/filter_reinhard_single_pass.hpp"

namespace pic {

/**
 * @brief The ReinhardTMOGL class
 */
class ReinhardTMOGL
{
protected:
    FilterGLLuminance  *flt_lum;
    FilterGLSigmoidTMO *flt_tmo_global;
    std::vector<FilterGL*> filters;

    ImageGL *img_lum, *img_lum_adapt;

    float Lwa, alpha, phi;
    bool bStatisticsRecompute, bGlobal, bAllocate;

    FilterGLReinhardSinglePass *fTMO;

    /**
     * @brief allocateFilters
     */
    void allocateFilters()
    {
        bAllocate = true;
        flt_lum = new FilterGLLuminance();
        flt_tmo_global = new FilterGLSigmoidTMO(alpha, false, false);
    }

    /**
     * @brief executeGlobal
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *executeGlobal(ImageGL *imgIn, ImageGL *imgOut = NULL)
    {
        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        if(bStatisticsRecompute || (Lwa < 0.0f)) {
            img_lum->getLogMeanVal(&Lwa);
        }

        flt_tmo_global->update(alpha / Lwa);
        imgOut = flt_tmo_global->Process(DoubleGL(imgIn, img_lum), imgOut);

        return imgOut;
    }

    /**
     * @brief executeLocal
     * @param imgIn
     * @param imgOut
     * @param alpha
     * @param phi
     * @param filter
     * @return
     */
    ImageGL *executeLocal(ImageGL *imgIn, ImageGL *imgOut = NULL)
    {
        if(fTMO == NULL) {
            fTMO = new FilterGLReinhardSinglePass(alpha, phi);
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        if(bStatisticsRecompute || (Lwa < 0.0f)) {
            img_lum->getLogMeanVal(&Lwa);
            fTMO->update(-1.0f, -1.0f, Lwa);
        }

        imgOut = fTMO->Process(DoubleGL(imgIn, img_lum), imgOut);

        return imgOut;
    }

    /**
     * @brief setNULL
     */
    void setNULL()
    {
        bGlobal = false;
        img_lum = NULL;
        Lwa = -1.0f;
        fTMO = NULL;
    }

public:

    /**
     * @brief ReinhardTMOGL
     */
    ReinhardTMOGL(float alpha = 0.15f, float phi = 8.0f, bool bStatisticsRecompute = true, bool bGlobal = false)
    {
        this->alpha = 0.15f;
        this->phi = 8.0f;

        bAllocate = false;

        update(alpha, phi, bGlobal);

        setNULL();

        this->bStatisticsRecompute = bStatisticsRecompute;
    }

    ~ReinhardTMOGL()
    {
        stdVectorClear<FilterGL>(filters);
    }

    /**
     * @brief update
     * @param alpha
     * @param phi
     * @param bGlobal
     */
    void update(float alpha, float phi, bool bGlobal = true)
    {
        this->bGlobal = bGlobal;
        this->alpha = alpha > 0.0f ? alpha : this->alpha;
        this->phi = phi > 0.0f ? phi : this->phi;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param bGlobal
     * @return
     */
    ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(!imgIn->isValid()) {
            return imgOut;
        }

        if(!bAllocate) {
            allocateFilters();
        }

        if(bGlobal) {
            return executeGlobal(imgIn, imgOut);
        } else {
            return executeLocal(imgIn, imgOut);
        }
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP */

