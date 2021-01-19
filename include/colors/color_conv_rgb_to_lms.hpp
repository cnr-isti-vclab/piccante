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

#ifndef PIC_COLORS_COLOR_CONV_RGB_TO_LMS_HPP
#define PIC_COLORS_COLOR_CONV_RGB_TO_LMS_HPP

#include "../util/matrix_3_x_3.hpp"
#include "../colors/color_conv.hpp"
#include "../colors/color_conv_rgb_to_xyz.hpp"
#include "../colors/color_conv_xyz_to_lms.hpp"

namespace pic {

/**
 * @brief The ColorConvRGBtoLMS class
 */
class ColorConvRGBtoLMS: public ColorConv
{
public:

    /**
     * @brief ColorConvRGBtoLMS
     */
    ColorConvRGBtoLMS() : ColorConv()
    {
        Matrix3x3 A, B, C;
        A.set(mtxRGBtoXYZ);
        B.set(mtxXYZtoLMS);
        //RGB --> LMS
        C = B.mul(A);

        memcpy(mtx, C.data, 9 * sizeof(float));
        computeInverse();
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_CONV_RGB_TO_LMS_HPP */

