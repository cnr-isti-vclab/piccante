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

#ifndef PIC_COLORS_RGBE_HPP
#define PIC_COLORS_RGBE_HPP

/**
*
*	Note:
*	- colFloat has to be an array of 3 floats
*	- colRGBE has to be an array of 4 unsigned char
*
**/

#include "base.hpp"

namespace pic {

PIC_INLINE void Float2RGBE(float *colFloat, unsigned char *colRGBE)
{
    float v;
    int e;

    v = *colFloat;

    if(v < * (colFloat + 1)) {
        v = *(colFloat + 1);
    }

    if(v < * (colFloat + 2)) {
        v = *(colFloat + 2);
    }

    if(v < 1e-32f) { //is it too small?
        *(colRGBE) = 0;
        *(colRGBE + 1) = 0;
        *(colRGBE + 2) = 0;
        *(colRGBE + 3) = 0;
        return;
    }

    v = frexp(v, &e) * 256.0f / v;

    *(colRGBE) = int((*(colFloat)) * v);
    *(colRGBE + 1) = int((*(colFloat + 1)) * v);
    *(colRGBE + 2) = int((*(colFloat + 2)) * v);
    *(colRGBE + 3) = (e + 128);
}

PIC_INLINE void SingleFloat2RGBE(float *colFloat, unsigned char *colRGBE)
{
    float v;
    int e;

    v = *colFloat;

    if(v < 1e-32f) { //is it too small?
        *(colRGBE) = 0;
        *(colRGBE + 1) = 0;
        *(colRGBE + 2) = 0;
        *(colRGBE + 3) = 0;
        return;
    }

    v = frexp(v, &e) * 256.0f / v;

    *(colRGBE) = int((*(colFloat)) * v);
    *(colRGBE + 1) = *colRGBE;
    *(colRGBE + 2) = *colRGBE;
    *(colRGBE + 3) = (e + 128);
}

PIC_INLINE void RGBE2Float(unsigned char *colRGBE, float *colFloat)
{

    if((*(colRGBE) == 0) && (*(colRGBE + 1) == 0) &&
       (*(colRGBE + 2) == 0)) { //if it is small
        *(colFloat) = 0;
        *(colFloat + 1) = 0;
        *(colFloat + 2) = 0;
        return;
    }

    int E;
    float f;

    E = *(colRGBE + 3) - 128 - 8;
    f = ldexpf(1.0f, E);

    *(colFloat) = (float(*(colRGBE)) + 0.5f) * f;
    *(colFloat + 1) = (float(*(colRGBE + 1)) + 0.5f) * f;
    *(colFloat + 2) = (float(*(colRGBE + 2)) + 0.5f) * f;
}

} // end namespace pic

#endif /* PIC_COLORS_RGBE_HPP */

