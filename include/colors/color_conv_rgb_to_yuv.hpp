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

#ifndef PIC_COLORS_COLOR_CONV_RGB_TO_YUV_HPP
#define PIC_COLORS_COLOR_CONV_RGB_TO_YUV_HPP

#include "../util/matrix_3_x_3.hpp"
#include "../colors/color_conv.hpp"

namespace pic {

const float mtxRGBtoYUV[] = {	0.2126f,  0.7152f, 0.722f,
                               -0.09991f,-0.33609f, 0.436f,
                                0.615f, -0.55861f, -0.05639f
                            };

const float mtxYUVtoRGB[] = {	1.0f, 0.0f, 1.13983f,
                                1.0f, -0.39465f, -0.58060f,
                                1.0f, 2.03211f, 0.0f
                            };

/**
 * @brief The ColorConvRGBtoXYZ class
 */
class ColorConvRGBtoYUV: public ColorConv
{
public:

    /**
     * @brief ColorConvRGBtoYUV
     */
    ColorConvRGBtoYUV() : ColorConv()
    {
        memcpy(mtx, mtxRGBtoYUV, 9 * sizeof(float));
        memcpy(mtx_inv, mtxYUVtoRGB, 9 * sizeof(float));
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_XYZ_HPP */

