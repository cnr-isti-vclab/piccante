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

#ifndef PIC_COLORS_COLOR_CONV_IPT_TO_ICH_HPP
#define PIC_COLORS_COLOR_CONV_IPT_TO_ICH_HPP

#include "../colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvIPTtoICH class
 */
class ColorConvIPTtoICH: public ColorConv
{
protected:
    float epsilon;

public:

    /**
     * @brief ColorConvIPTtoICH
     */
    ColorConvIPTtoICH()
    {
        linear = false;
        epsilon = 1.0f;
    }

    /**
     * @brief direct from XYZ to CIE LUV
     * @param colIn
     * @param colOut
     */
    void direct(float *colIn, float *colOut)
    {
        colOut[0] = colIn[0];

        colOut[1] = sqrtf(colIn[1] * colIn[1] + colIn[2] * colIn[2]);
        colOut[2] = atan2f(colIn[1], colIn[2]);
    }

    /**
     * @brief inverse from CIE LUV to XYZ
     * @param colIn
     * @param colOut
     */
    void inverse(float *colIn, float *colOut)
    {
        colOut[0] = colIn[0];

        colOut[1] = colIn[1] * sinf(colIn[2]);
        colOut[2] = colIn[1] * cosf(colIn[2]);
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_CONV_IPT_TO_ICH_HPP */

