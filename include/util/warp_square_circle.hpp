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

#ifndef PIC_UTIL_WARP_SQUARE_CIRCLE_HPP
#define PIC_UTIL_WARP_SQUARE_CIRCLE_HPP

namespace pic {

//Warp from a square to a circle
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

