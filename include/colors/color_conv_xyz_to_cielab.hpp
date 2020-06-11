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

#ifndef PIC_COLORS_COLOR_CONV_XYZ_TO_CIELAB_HPP
#define PIC_COLORS_COLOR_CONV_XYZ_TO_CIELAB_HPP

#include "../colors/color_conv.hpp"

namespace pic {

const float C_SIX_OVER_TWENTY_NINE          = 0.20689655172413793103448275862069f;
const float C_SIX_OVER_TWENTY_NINE_CUBIC    = 0.00885645167903563081717167575546f;
// (29/6)^2 / 3
const float C_CIELAB_C1                     = 7.787037037037037037037037037037f;
// (6/29)^2 * 3
const float C_CIELAB_C1_INV                 = 0.12841854934601664684898929845422f;
const float C_FOUR_OVER_TWENTY_NINE         = 0.13793103448275862068965517241379f;

/**
 * @brief The ColorConvXYZtoCIELAB class
 */
class ColorConvXYZtoCIELAB: public ColorConv
{
protected:

    float white_point[3];

public:

    /**
     * @brief ColorConvXYZtoCIELAB
     */
    ColorConvXYZtoCIELAB()
    {
        linear = false;
        white_point[0] = 1.0f;
        white_point[1] = 1.0f;
        white_point[2] = 1.0f;
    }

    /**
     * @brief direct
     * @param colIn
     * @param colOut
     */
    void direct(float *colIn, float *colOut)
    {
        float fY_Yn = f(colIn[1] / white_point[1]);

        colOut[0] = 116.0f * fY_Yn - 16.0f;
        colOut[1] = 500.0f * (f(colIn[0] / white_point[0]) - fY_Yn);
        colOut[2] = 200.0f * (fY_Yn - f(colIn[2] / white_point[2]));
    }

    /**
     * @brief inverse
     * @param colIn
     * @param colOut
     */
    void inverse(float *colIn, float *colOut)
    {
        float tmp = (colIn[0] + 16.0f) / 116.0f;

        colOut[1] = white_point[1] * f_inv(tmp);
        colOut[0] = white_point[0] * f_inv(tmp + colIn[1] / 500.0f);
        colOut[2] = white_point[2] * f_inv(tmp - colIn[2] / 200.0f);
    }

    /**
     * @brief f
     * @param t
     * @return
     */
    static float f(float t)
    {
        if(t > C_SIX_OVER_TWENTY_NINE_CUBIC) {
            return powf(t, 1.0f / 3.0f);
        } else {
            return C_CIELAB_C1 * t +
                   C_FOUR_OVER_TWENTY_NINE;
        }
    }

    /**
     * @brief f_inv
     * @param t
     * @return
     */
    static float f_inv(float t)
    {
        if(t > C_SIX_OVER_TWENTY_NINE ) {
            return powf(t, 3.0f);
        } else {
            return (t - C_FOUR_OVER_TWENTY_NINE) * C_CIELAB_C1_INV;
        }
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_CIELAB_HPP */

