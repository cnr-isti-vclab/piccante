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

#ifndef PIC_ALGORITHMS_WEIGHT_FUNCTION_HPP
#define PIC_ALGORITHMS_WEIGHT_FUNCTION_HPP

#include "../base.hpp"

namespace pic {

/**
 * @brief The CRF_WEIGHT enum
 */
enum CRF_WEIGHT {CW_ALL, CW_IDENTITY, CW_REVERSE, CW_HAT, CW_DEB97, CW_DEB97p01, CW_ROBERTSON};

/**
 * @brief weightFunction computes weight functions for x in [0,1].
 * @param x is an input value in [0, 1].
 * @param type is the type of the function.
 * @return It returns a weight for x.
 */
PIC_INLINE float weightFunction(float x, CRF_WEIGHT type)
{
    switch(type) {

    case CW_ROBERTSON: {
        // w(x) = exp(-4*(x*255 - 127.5)^2/(127.5)^2) = exp(-16.0 * (x - 0.5)^2)
        // (according to the paper it should be scaled and shifted s.t. w(0) = w(255) = 0 and w(127.5) = 1)
        static const double shift    = exp(-4);
        static const double scaleDiv = (1.0 - shift);
        const double t = x - 0.5;
        return float((exp(-16.0 * (t * t) ) - shift) / scaleDiv);
    }
    break;

    case CW_HAT: {
        float val = (2.0f * x - 1.0f);
        float val_squared = val * val;
        float val_quartic = val_squared * val_squared;
        return (1.0f - val_quartic * val_quartic * val_quartic);
    }
    break;
            
    case CW_IDENTITY:
    {
        return x;
    }
    break;

    case CW_REVERSE:
    {
        return 1.0 - x;
    }
    break;
            
    case CW_DEB97: {
        static const float Zmin = 0.0f;
        static const float Zmax = 1.0f;
        static const float tr = (Zmin + Zmax) / 2.0f;

        if(x <= tr) {
            return x - Zmin;
        } else {
            return Zmax - x;
        }
    }
    break;

    case CW_DEB97p01: {
        static const float Zmin = 0.01f;
        static const float Zmax = 0.99f;
        float tr = (Zmin + Zmax) / 2.0f;

        if(x <= tr) {
            return CLAMPi(x - Zmin, 0.0f, 1.0f);
        } else {
            return CLAMPi(Zmax - x, 0.0f, 1.0f);
        }
    }
    break;

    default: {
        return 1.0f;
    }
    break;
    }

    return 1.0f;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_WEIGHT_FUNCTION_HPP */

