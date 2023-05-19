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

#include "../metrics/pu08_data.hpp"

namespace pic {

/**
 * @brief PUEncode encodes luminance values in a perceptually uniform space.
 * @param L is a luminance value in cd/m^2; it works for values
 * in the range [10^-6, 10^10] cd/m^2
 * @return it returns a perceptually uniform value
 */
PIC_INLINE float PU08Encode(float L)
{
    return Arrayf::interp(PU08_x, PU08_y, 256, log10f(L + 1e-7f));
}

/**
 * @brief PUDecode decodes perceptually uniform values into luminance values.
 * @param p is a perceptually uniform luminance value
 * @return it returns a luminance value in the range [10^-6, 10^10] cd/m^2
 */
PIC_INLINE float PU08Decode(float p)
{
    return powf(10.0f, Arrayf::interp(PU08_y, PU08_x, 256, p));
}

} // end namespace pic

#endif /* PIC_METRICS_PU_ENCODE_HPP */

