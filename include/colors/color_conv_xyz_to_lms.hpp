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

#ifndef PIC_COLORS_COLOR_CONV_XYZ_TO_LMS_HPP
#define PIC_COLORS_COLOR_CONV_XYZ_TO_LMS_HPP

#include "../colors/color_conv.hpp"

namespace pic {

const float mtxXYZtoLMS[] = {	0.3897f, 0.6890f, -0.0787f,
                               -0.2298f, 1.1834f, 0.0464f,
                                0.0f,    0.0f,    1.0f
                            };

const float mtxLMStoXYZ[] = {   1.9102f, -1.112f,   0.2019f,
                                0.3709f,  0.6291f,  5.1332e-6f,
                                0.0f,     0.0f,     1.0f
                            };

/**
 * @brief The ColorConvXYZtoLMS class
 */
class ColorConvXYZtoLMS: public ColorConv
{
public:

    /**
     * @brief ColorConvXYZtoLMS
     */
    ColorConvXYZtoLMS() : ColorConv()
    {
        memcpy(mtx, mtxXYZtoLMS, 9 * sizeof(float));
        memcpy(mtx_inv, mtxLMStoXYZ, 9 * sizeof(float));
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_XYZ_HPP */
