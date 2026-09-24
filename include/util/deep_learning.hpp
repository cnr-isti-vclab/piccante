/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2026
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_UTIL_DEEP_LEARNING
#define PIC_UTIL_DEEP_LEARNING

#include "../base.hpp"

#include "../util/math.hpp"

namespace pic {

template <typename T>
PIC_INLINE T ReLU(float x)
{
    return MAX(x, 0.0);
}

template <typename T>
PIC_INLINE float Sigmoid(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

} // end namespace pic

#endif //PIC_UTIL_DEEP_LEARNING
