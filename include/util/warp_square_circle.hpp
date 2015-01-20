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

namespace pic {

/**
 * @brief WarpSquareCircle warps from a square to a circle distribution.
 * @param x
 * @param y
 * @param xo
 * @param yo
 */
void WarpSquareCircle(float x, float y, float *xo, float *yo)
{
    float phi, r;

//	float a = 2*O.x - 1;
//	float b = 2*O.y - 1;

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

} // end namespace pic

#endif /* PIC_UTIL_WARP_SQUARE_CIRCLE_HPP */

