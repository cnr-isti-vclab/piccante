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

#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_sigmoid_tmo.hpp"
#include "gl/filtering/filter_bilateral_2ds.hpp"
#include "gl/filtering/filter_op.hpp"

namespace pic {


class ReinhardTMOGL
{
protected:
    FilterGLLuminance  *flt_lum;
    FilterGLSigmoidTMO *flt_tmo;

    FilterGL           *filter;
    FilterGLOp         *simple_sigmoid;
    ImageGL            *img_lum, *img_lum_adapt;

public:
    /**
     * @brief ReinhardTMOGL
     */
    ReinhardTMOGL()
    {
        flt_lum = new FilterGLLuminance();
        flt_tmo = new FilterGLSigmoidTMO();

        simple_sigmoid = new FilterGLOp("I0 / (I0 + 1.0)", true, NULL, NULL);

        img_lum = NULL;
        img_lum_adapt = NULL;

        filter = NULL;
    }

    ~ReinhardTMOGL()
    {
        if(flt_lum != NULL) {
            delete flt_lum;
            flt_lum = NULL;
        }

        if(flt_tmo != NULL) {
            delete flt_tmo;
            flt_tmo = NULL;
        }

        if(img_lum != NULL) {
            delete img_lum;
            img_lum = NULL;
        }

        if(filter != NULL) {
            delete filter;
            filter = NULL;
        }
    }

    /**
     * @brief Process
     * @param imgIn
     * @param Ld_Max
     * @param bias
     * @param imgOut
     * @return
     */
    ImageGL *ProcessGlobal(ImageGL *imgIn, float alpha = 0.18f, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        float Lwa;
        img_lum->getMeanVal(&Lwa);


        flt_tmo->Update(alpha / Lwa);
        imgOut = flt_tmo->Process(DoubleGL(imgIn, img_lum), imgOut);

        return imgOut;
    }

    /**
     * @brief ProcessLocal
     * @param imgIn
     * @param alpha
     * @param flt_local
     * @param imgOut
     * @return
     */
    ImageGL *ProcessLocal(ImageGL *imgIn, float alpha = 0.18f, float phi = 8.0f, FilterGL *filter = NULL, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        bool bDomainChange = false;

        if(filter == NULL) {
            if(this->filter == NULL) {

                float epsilon = 0.05f;
                float s_max = 8.0f;
                float sigma_s = 0.56f * powf(1.6f, s_max);

                float sigma_r = (powf(2.0f, phi) * alpha / (s_max * s_max)) * epsilon;

                this->filter = new FilterGLBilateral2DS(sigma_s, sigma_r);
            }

            bDomainChange = true;
            filter = this->filter;
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        float Lwa;
        img_lum->getMeanVal(&Lwa);

        if(bDomainChange) {
            img_lum_adapt = simple_sigmoid->Process(SingleGL(img_lum), img_lum_adapt);
            img_lum = filter->Process(SingleGL(img_lum_adapt), img_lum);

            ImageGL *tmp = img_lum;
            img_lum = img_lum_adapt;
            img_lum_adapt = tmp;

        } else {
            img_lum_adapt = filter->Process(SingleGL(img_lum), img_lum_adapt);
        }


        flt_tmo->Update(alpha / Lwa);
        imgOut = flt_tmo->Process(DoubleGL(imgIn, img_lum_adapt), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP */

