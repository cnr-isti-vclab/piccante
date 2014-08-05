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

#ifndef PIC_TONE_MAPPING_DRAGO_TMO_HPP
#define PIC_TONE_MAPPING_DRAGO_TMO_HPP

#include "filtering/filter.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_drago_tmo.hpp"

namespace pic {

ImageRAW *DragoTMO(ImageRAW *imgIn, float Ld_Max = 100.0f, float b = 0.95f, ImageRAW *imgOut = NULL)
{
    //Computing luminance and its statistics
    FilterLuminance filterLum;
    ImageRAW *imgLum = filterLum.ProcessP(Single(imgIn), NULL);

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

