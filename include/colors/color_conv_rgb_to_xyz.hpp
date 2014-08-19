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

#ifndef PIC_COLORS_COLOR_CONV_RGB_TO_XYZ_HPP
#define PIC_COLORS_COLOR_CONV_RGB_TO_XYZ_HPP

#include "colors/color_conv.hpp"

namespace pic {

const float mtxRGBtoXYZ[] = {	0.4124f, 0.3576f, 0.1805f,
                                0.2126f, 0.7152f, 0.0722f,
                                0.0193f, 0.1192f, 0.9505f
                            };

const float mtxXYZtoRGB[] = {	3.2406f,   -1.5372f,   -0.4986f,
                                -0.9689f,    1.8758f,    0.0415f,
                                0.0557f,   -0.2040f,    1.0570f
                            };

/**
 * @brief The ColorConvRGBtoXYZ class
 */
class ColorConvRGBtoXYZ: public ColorConv
{
public:

    /**
     * @brief ColorConvRGBtoXYZ
     */
    ColorConvRGBtoXYZ()
    {
    }

    /**
     * @brief direct
     * @param colIn
     * @param colOut
     */
    void direct(float *colIn, float *colOut)
    {
        apply(mtxRGBtoXYZ, colIn, colOut);
    }

    /**
     * @brief inverse
     * @param colIn
     * @param colOut
     */
    void inverse(float *colIn, float *colOut)
    {
        apply(mtxXYZtoRGB, colIn, colOut);
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_XYZ_HPP */

