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

#ifndef PIC_COLORS_SATURATION_HPP
#define PIC_COLORS_SATURATION_HPP

namespace pic {

#include "../base.hpp"
#include "../util/math.hpp"
#include "../util/array.hpp"

/**
 * @brief computeSaturation
 * @param data
 * @param channels
 * @return
 */
PIC_INLINE float computeSaturation(float *data, int channels = 3)
{
    if(channels > 1) {
        return sqrtf_s(Arrayf::getVariance(data, channels));
    } else {
        return 0.0f;
    }
}

} // end namespace pic

#endif /* PIC_COLORS_SATURATION_HPP */
