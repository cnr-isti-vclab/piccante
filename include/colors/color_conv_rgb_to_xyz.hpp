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

#ifndef PIC_COLORS_COLOR_CONV_RGB_TO_XYZ_HPP
#define PIC_COLORS_COLOR_CONV_RGB_TO_XYZ_HPP

#include "../colors/color_conv.hpp"

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
    ColorConvRGBtoXYZ() : ColorConv()
    {
        memcpy(mtx, mtxRGBtoXYZ, 9 * sizeof(float));
        memcpy(mtx_inv, mtxXYZtoRGB, 9 * sizeof(float));
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_XYZ_HPP */

