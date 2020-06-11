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

#ifndef PIC_COLORS_COLOR_CONV_XYZ_TO_CIELUV_HPP
#define PIC_COLORS_COLOR_CONV_XYZ_TO_CIELUV_HPP

#include "../colors/color_conv.hpp"

namespace pic {

class ColorConvXYZtoCIELUV: public ColorConv
{
protected:

    float white_point[3];

public:

    ColorConvXYZtoCIELUV()
    {
        linear = false;
        white_point[0] = 1.0f;
        white_point[1] = 1.0f;
        white_point[2] = 1.0f;
    }

    //from XYZ to CIE LUV
    void direct(float *colIn, float *colOut)
    {

    }

    //from CIE LUV to XYZ
    void inverse(float *colIn, float *colOut)
    {

    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_CIELUV_HPP */

