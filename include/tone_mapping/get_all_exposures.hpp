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

#ifndef PIC_GET_ALL_EXPOSURES_HPP
#define PIC_GET_ALL_EXPOSURES_HPP

#include "image_raw.hpp"
#include "histogram.hpp"
#include "filtering/filter_luminance.hpp"

namespace pic {

//This function converts an HDR image into a stack of LDR images
ImageRAWVec getAllExposures(ImageRAW *imgIn)
{
    ImageRAWVec ret;

    if(imgIn == NULL) {
        return ret;
    }

    if(! imgIn->isValid()) {
        return ret;
    }
        
    ImageRAW *lum = FilterLuminance::Execute(imgIn, NULL);
    Histogram m(lum, VS_LOG_2, 1024);

    std::vector< float > exposures = m.ExposureCovering();

    FilterSimpleTMO flt(1.0f, 0.0f);

    ImageRAWVec input = Single(imgIn);

    for(unsigned int i=0; i<exposures.size(); i++) {
        flt.Update(2.2f, exposures[i]);
        ImageRAW *expo = flt.ProcessP(input, NULL);

        ret.push_back(expo);
    }

    delete lum;

    return ret;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP */

