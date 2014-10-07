/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

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
ImageRAWGL *DragoTMOGL(ImageRAWGL *imgIn, float Ld_Max = 100.0f, float b = 0.95f,
                     ImageRAWGL *imgOut = NULL, ImageStatisticsGL *imgStat = NULL)
{
    if(imgIn == NULL) {
        return imgOut;
    }

    if(imgStat == NULL) {
        imgStat = new ImageStatisticsGL();
    }

    ImageRAWGL *img_lum = FilterGLLuminance::Execute(imgIn, NULL);

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

