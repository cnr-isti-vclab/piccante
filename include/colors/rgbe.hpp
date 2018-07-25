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

#ifndef PIC_COLORS_RGBE_HPP
#define PIC_COLORS_RGBE_HPP

/**
*
*	Note:
*	- colFloat has to be an array of 3 floats
*	- colRGBE has to be an array of 4 unsigned char
*
**/

#include "../base.hpp"

namespace pic {

/**
 * @brief fromFloatToRGBE
 * @param colFloat
 * @param colRGBE
 */
PIC_INLINE void fromFloatToRGBE(float *colFloat, unsigned char *colRGBE)
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

/**
 * @brief fromSingleFloatToRGBE
 * @param colFloat
 * @param colRGBE
 */
PIC_INLINE void fromSingleFloatToRGBE(float *colFloat, unsigned char *colRGBE)
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

/**
 * @brief fromRGBEToFloat
 * @param colRGBE
 * @param colFloat
 */
PIC_INLINE void fromRGBEToFloat(unsigned char *colRGBE, float *colFloat)
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

