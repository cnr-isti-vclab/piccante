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

#ifndef PIC_UTIL_WARP_SQUARE_CIRCLE_HPP
#define PIC_UTIL_WARP_SQUARE_CIRCLE_HPP

#include "../base.hpp"

#include "../util/math.hpp"

namespace pic {

/**
 * @brief warpSquareCircle warps from a square to a circle distribution.
 * @param x
 * @param y
 * @param xo
 * @param yo
 */
PIC_INLINE void warpSquareCircle(float x, float y, float *xo, float *yo)
{
    float phi, r;


    if(x * x > y * y) {
        r = x;
        phi = (C_PI / 4.0f) * (y / x);
    } else {
        r = y;
        phi = (C_PI / 4.0f) * (x / y) + (C_PI / 2.0f);
    }

    *xo = r * cos(phi);
    *yo = r * sin(phi);
}

/**
 * @brief warpNormalDistribution warps from uniform distribution to a normal distribution
 * @param u1
 * @param u2
 */
PIC_INLINE float warpNormalDistribution(float u0, float u1)
{
    return sqrtf(MAX(-2.0f * logf(u0), 0.0f)) * cosf(u1);
}

/**
 * @brief warpGaussianDistribution
 * @param u0
 * @param u1
 * @param mu
 * @param sigma
 * @return
 */
PIC_INLINE float warpGaussianDistribution(float u0, float u1, float mu, float sigma)
{
    float x = warpNormalDistribution(u0, u1);
    return (x + mu) * sigma;
}

} // end namespace pic

#endif /* PIC_UTIL_WARP_SQUARE_CIRCLE_HPP */

