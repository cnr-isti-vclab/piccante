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

