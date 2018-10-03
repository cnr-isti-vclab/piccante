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
    FilterGLSigmoidTMO *flt_tmo_global, *flt_tmo_local;

    FilterGL           *filter;
    FilterGLOp         *simple_sigmoid, *simple_sigmoid_inv;
    ImageGL            *img_lum, *img_lum_adapt;

    float              Lwa, alpha, phi;
    bool               bStatisticsRecompute;

    FilterGLReinhardSinglePass *fTMO;

    /**
     * @brief allocateFilters
     */
    void allocateFilters()
    {
        flt_lum = new FilterGLLuminance();
        flt_tmo_global = new FilterGLSigmoidTMO(0.18f, false, false);
        flt_tmo_local = new FilterGLSigmoidTMO(0.18f, true, false);

        simple_sigmoid     = new FilterGLOp("I0 / (I0 + 1.0)", true, NULL, NULL);
        simple_sigmoid_inv = new FilterGLOp("I0 / (1.0 - I0)", true, NULL, NULL);
    }

    /**
     * @brief executeGlobal
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *executeGlobal(ImageGL *imgIn, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(flt_lum == NULL) {
            allocateFilters();
        }

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
        if(imgIn == NULL) {
            return imgOut;
        }

        if(flt_lum == NULL) {
            allocateFilters();
        }

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

    void setNULL()
    {
        flt_lum = NULL;
        flt_tmo_global = NULL;
        flt_tmo_local = NULL;

        simple_sigmoid = NULL;
        simple_sigmoid_inv = NULL;

        img_lum = NULL;
        img_lum_adapt = NULL;

        filter = NULL;

        Lwa = -1.0f;

        fTMO = NULL;
    }

public:

    /**
     * @brief ReinhardTMOGL
     */
    ReinhardTMOGL(float alpha = 0.15f, float phi = 8.0f, bool bStatisticsRecompute = true)
    {
        update(alpha, phi);

        setNULL();

        this->bStatisticsRecompute = bStatisticsRecompute;
    }

    ~ReinhardTMOGL()
    {
        if(flt_lum != NULL) {
            delete flt_lum;
            flt_lum = NULL;
        }

        if(img_lum != NULL) {
            delete img_lum;
            img_lum = NULL;
        }

        if(img_lum_adapt != NULL) {
            delete img_lum_adapt;
            img_lum_adapt = NULL;
        }

        if(filter != NULL) {
            delete filter;
            filter = NULL;
        }

        if(simple_sigmoid != NULL) {
            delete simple_sigmoid;
            simple_sigmoid = NULL;
        }
    }

    /**
     * @brief update
     * @param alpha
     * @param phi
     */
    void update(float alpha, float phi)
    {
        this->alpha = alpha > 0.0f ? alpha : 0.15f;
        this->phi = phi > 0.0f ? phi : 8.0f;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param bGlobal
     * @return
     */
    ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut = NULL, bool bGlobal = true)
    {
        if(bGlobal) {
            return executeGlobal(imgIn, imgOut);
        } else {
            return executeLocal(imgIn, imgOut);
        }
    }


};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP */

