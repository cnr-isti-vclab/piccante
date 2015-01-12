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

#ifndef PIC_GL_TONE_MAPPING_DRAGO_TMO_HPP
#define PIC_GL_TONE_MAPPING_DRAGO_TMO_HPP

#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_drago_tmo.hpp"

namespace pic {

/**
 * @brief The DragoTMOGL class
 */
class DragoTMOGL
{
protected:
    FilterGLLuminance *flt_lum;
    FilterGLDragoTMO  *flt_tmo;

    ImageGL           *img_lum;

public:
    /**
     * @brief DragoTMOGL
     */
    DragoTMOGL()
    {
        flt_lum = new FilterGLLuminance();
        flt_tmo = new FilterGLDragoTMO();

        img_lum = NULL;
    }

    ~DragoTMOGL()
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
    ImageGL *Process(ImageGL *imgIn, float Ld_Max = 100.0f, float bias = 0.95f,
                     ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        float LMax, Lwa;
        img_lum->getMaxVal(&LMax);
        img_lum->getMeanVal(&Lwa);

        flt_tmo->Update(Ld_Max, bias, LMax, Lwa);
        imgOut = flt_tmo->Process(DoubleGL(imgIn, img_lum), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_DRAGO_TMO_HPP */

