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

#ifndef PIC_COLORS_COLOR_CONV_XYZ_TO_CIELUV_HPP
#define PIC_COLORS_COLOR_CONV_XYZ_TO_CIELUV_HPP

#include "colors/color_conv.hpp"

namespace pic {

class ColorConvXYZtoCIELUV: public ColorConv
{
protected:

    float		white_point[3];

public:

    ColorConvXYZtoCIELUV()
    {
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

