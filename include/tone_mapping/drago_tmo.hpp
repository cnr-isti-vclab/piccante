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

#ifndef PIC_TONE_MAPPING_DRAGO_TMO_HPP
#define PIC_TONE_MAPPING_DRAGO_TMO_HPP

#include "filtering/filter.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_drago_tmo.hpp"

namespace pic {

/**
 * @brief DragoTMO tone maps an image using Drago et al. 2003 tone mapping
 * operator.
 * @param imgIn
 * @param Ld_Max
 * @param b
 * @param imgOut
 * @return
 */
Image *DragoTMO(Image *imgIn, float Ld_Max = 100.0f, float b = 0.95f, Image *imgOut = NULL)
{
    //Computing luminance and its statistics
    FilterLuminance filterLum;
    Image *imgLum = filterLum.ProcessP(Single(imgIn), NULL);

    float Lw_Max = imgLum->getMaxVal()[0];
    float Lw_a = imgLum->getMaxVal()[0];

    //tone mapping
    FilterDragoTMO filterDrago(Ld_Max, b, Lw_Max, Lw_a);
    imgOut = filterDrago.ProcessP(Double(imgIn, imgLum), imgOut);

    delete imgLum;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_DRAGO_TMO_HPP */

