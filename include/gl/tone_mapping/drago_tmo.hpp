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

#ifndef PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP
#define PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP

#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_drago_tmo.hpp"

namespace pic {

/**
 * @brief DragoTMOGL tone maps an image using Drago et al. 2003 tone mapping
 * operator.
 * @param imgIn
 * @param Ld_Max
 * @param b
 * @param imgOut
 * @return
 */
ImageGL *DragoTMOGL(ImageGL *imgIn, float Ld_Max = 100.0f, float b = 0.95f,
                     ImageGL *imgOut = NULL)
{
    if(imgIn == NULL) {
        return imgOut;
    }

    ImageGL *img_lum = FilterGLLuminance::Execute(imgIn, NULL);

    float LMax, Lwa;
    img_lum->getMaxVal(&LMax);
    img_lum->getMeanVal(&Lwa);

    #ifdef PIC_DEBUG
        printf("%f %f\n", LMax, Lwa);
    #endif

    FilterGLDragoTMO filter(Ld_Max, b, LMax, Lwa, false);

    imgOut = filter.Process(DoubleGL(imgIn, img_lum), imgOut);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP */

