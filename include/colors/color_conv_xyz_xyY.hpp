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

#ifndef PIC_COLORS_COLOR_CONV_XYZ_TO_xyY_HPP
#define PIC_COLORS_COLOR_CONV_XYZ_TO_xyY_HPP

#include "../colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvXYZtoxyY class
 */
class ColorConvXYZtoxyY: public ColorConv
{
public:

    ColorConvXYZtoxyY()
    {
        linear = false;
    }

    /**
     * @brief direct
     * @param colIn
     * @param colOut
     */
    void direct(float *colIn, float *colOut)
    {
        float XYZ = colIn[0] + colIn[1] + colIn[2];

        if(XYZ > 0.0f) {
            colOut[0] = colIn[0] / XYZ;
            colOut[1] = colIn[1] / XYZ;
            colOut[2] = colIn[2];
        } else {
            colOut[0] = -1.0f;
            colOut[1] = -1.0f;
            colOut[2] = -1.0f;
        }
    }

    /**
     * @brief inverse
     * @param colIn
     * @param colOut
     */
    void inverse(float *colIn, float *colOut)
    {
        if(colIn[0] != 0.0f) {
            float ratio = colIn[2] / colIn[1];
            float z = CLAMPi(1.0f - colIn[0] - colIn[1], 0.0f, 1.0f);

            colOut[0] = colIn[0] * ratio;
            colOut[1] = colIn[2];
            colOut[2] = z * ratio;
        } else {
            colOut[0] = -1.0f;
            colOut[1] = -1.0f;
            colOut[2] = -1.0f;
        }
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_CIELAB_HPP */

