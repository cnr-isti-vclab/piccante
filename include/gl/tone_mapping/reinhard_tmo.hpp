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
#include "gl/filtering/filter_drago_tmo.hpp"

namespace pic {


class ReinhardTMOGL
{
protected:
    FilterGLLuminance  *flt_lum;
    FilterGLSigmoidTMO *flt_tmo;

    FilterGL           *filter;

    ImageGL            *img_lum;

public:
    /**
     * @brief ReinhardTMOGL
     */
    ReinhardTMOGL()
    {
        flt_lum = new FilterGLLuminance();
        flt_tmo = new FilterGLSigmoidTMO();

        img_lum = NULL;
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
    ImageGL *ProcessLocal(ImageGL *imgIn, float alpha = 0.18f, FilterGL *filter = NULL, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        float Lwa;
        img_lum->getMeanVal(&Lwa);

        ImageGL *img_lum_adapt = filter->Process(SingleGL(img_lum), NULL);

        flt_tmo->Update(alpha / Lwa);
        imgOut = flt_tmo->Process(DoubleGL(imgIn, img_lum_adapt), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP */

