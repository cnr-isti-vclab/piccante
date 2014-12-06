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
#include "gl/image_statistics.hpp"

namespace pic {

/**
 * @brief DragoTMOGL tone maps an image using Drago et al. 2003 tone mapping
 * operator.
 * @param imgIn
 * @param Ld_Max
 * @param b
 * @param imgOut
 * @param imgStat
 * @return
 */
ImageGL *DragoTMOGL(ImageGL *imgIn, float Ld_Max = 100.0f, float b = 0.95f,
                     ImageGL *imgOut = NULL, ImageStatisticsGL *imgStat = NULL)
{
    if(imgIn == NULL) {
        return imgOut;
    }

    if(imgStat == NULL) {
        imgStat = new ImageStatisticsGL();
    }

    ImageGL *img_lum = FilterGLLuminance::Execute(imgIn, NULL);

    float LMax, Lwa;
    imgStat->getMaxVal(img_lum, &LMax);
    imgStat->getMeanVal(img_lum, &Lwa);

    printf("%f %f\n", LMax, Lwa);

    FilterGLDragoTMO filter(Ld_Max, b, LMax, Lwa, false);

    imgOut = filter.Process(DoubleGL(imgIn, img_lum), imgOut);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP */

