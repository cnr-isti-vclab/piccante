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

#ifndef PIC_COLORS_COLOR_CONV_RGB_TO_SRGB_HPP
#define PIC_COLORS_COLOR_CONV_RGB_TO_SRGB_HPP

#include "../colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvRGBtosRGB class
 */
class ColorConvRGBtosRGB: public ColorConv
{
protected:

    float a, a_plus_1, gamma, gamma_inv;

public:

    /**
     * @brief ColorConvRGBtosRGB
     */
    ColorConvRGBtosRGB()
    {
        linear = false;

        gamma = 2.4f;
        gamma_inv = 1.0f / gamma;
        a = 0.055f;
        a_plus_1 = 1.0f + a;
    }

    /**
     * @brief direct
     * @param colIn
     * @param colOut
     */
    void direct(float *colIn, float *colOut)
    {
        for(int i = 0; i < 3; i++) {
            if(colIn[i] > 0.0031308f) {
                colOut[i] = a_plus_1 * powf(colIn[i], gamma_inv) - a;
            } else {
                colOut[i] = 12.92f * colIn[i];
            }
        }
    }

    /**
     * @brief inverse
     * @param colIn
     * @param colOut
     */
    void inverse(float *colIn, float *colOut)
    {
        for(int i = 0; i < 3; i++) {
            if(colIn[i] > 0.04045f) {
                colOut[i] = powf((colIn[i] + a) / a_plus_1, gamma);
            } else {
                colOut[i] = colIn[i] / 12.92f;
            }
        }
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_XYZ_HPP */

