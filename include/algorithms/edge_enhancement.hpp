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

#ifndef PIC_ALGORITHMS_EDGE_ENHANCEMENT_HPP
#define PIC_ALGORITHMS_EDGE_ENHANCEMENT_HPP

#include "filtering/filter_bilateral_2ds.hpp"

namespace pic {

inline Image *EdgeEnhancement(Image *imgIn, float sigma_s = 4.0f,
                                 float sigma_r = 0.05f)
{
    if(imgIn == NULL) {
        return NULL;
    }

    if(sigma_s <= 0.0f) {
        sigma_s = 4.0f;
    }

    if(sigma_r <= 0.0f) {
        sigma_r = 0.05f;
    }

    Image *imgBase = FilterBilateral2DS::Execute(imgIn, sigma_s, sigma_r);
    Image *detail = imgIn->Clone();

    *detail /= *imgBase;
    imgBase->Assign(imgIn);
    *imgBase *= *detail;
    return imgBase;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_EDGE_ENHANCEMENT_HPP */

