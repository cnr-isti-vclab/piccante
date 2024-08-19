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

#ifndef PIC_METRICS_PU_ENCODE_HPP
#define PIC_METRICS_PU_ENCODE_HPP

#include <math.h>

#include "../base.hpp"
#include "../image.hpp"
#include "../util/array.hpp"

//#include "../metrics/pu08_data.hpp"

namespace pic {

/**
 * @brief PUEncode encodes luminance values in a perceptually uniform space.
 * @param L is a luminance value in cd/m^2; it works 
 * @return it returns a perceptually uniform value
 */
    PIC_INLINE float PU21Encode(float L)
    {
        L = Clamp(L, 0.005f, 10000.0f);

        float p[] = { 0.353487901, 0.3734658629, 8.277049286e-05, 0.9062562627, 0.09150303166, 0.9099517204, 596.3148142 };

        float L3 = powf(L, p[3]);
        float t1 = (p[0] + p[1] * L3);
        float t2 = (1.0f + p[2] * L3);
        float out = p[6] * (powf(t1 / t2, p[4]) - p[5]);
        return MAX(out, 0.0f);
    }

/**
 * @brief PUDecode decodes perceptually uniform values into luminance values.
 * @param p is a perceptually uniform luminance value
 * @return it returns a luminance value in the range [10^-5, 10^10] cd/m^2
 */
PIC_INLINE float PU21Decode(float p)
{
    return 1.0;
}

} // end namespace pic

#endif /* PIC_METRICS_PU_ENCODE_HPP */

