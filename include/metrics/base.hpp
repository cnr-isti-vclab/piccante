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

#ifndef PIC_METRICS_BASE_HPP
#define PIC_METRICS_BASE_HPP

#include "../util/math.hpp"
#include "../metrics/pu_encode.hpp"

namespace pic {

const double C_SINGULARITY = 1e-6;
const double C_LARGE_DIFFERENCES = 1e6;
const float  C_LARGE_DIFFERENCESf = 1e6f;

enum METRICS_DOMAIN{MD_LIN, MD_LOG10, MD_PU};

/**
 * @brief changeDomain
 * @param x
 * @param type
 * @return
 */
float changeDomain(float x, METRICS_DOMAIN type = MD_LIN)
{
    switch(type){
    case MD_LIN: {
        return x;
    } break;

    case MD_LOG10: {
        return log10f(x);
    } break;

    case MD_PU: {
        return PUEncode(x);
    } break;
    }

    return x;
}

} // end namespace pic

#endif /* PIC_METRICS_BASE_HPP */

