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

#ifndef PIC_GL_COLORS_COLOR_CONV_XYZ_TO_LMS_HPP
#define PIC_GL_COLORS_COLOR_CONV_XYZ_TO_LMS_HPP

#include "../../colors/color_conv_xyz_to_lms.hpp"

#include "../../gl/colors/color_conv_linear.hpp"

namespace pic {

/**
 * @brief The ColorConvGLXYZtoLMS class
 */
class ColorConvGLXYZtoLMS: public ColorConvGLLinear
{
public:

    /**
     * @brief ColorConvGLXYZtoLMS
     */
    ColorConvGLXYZtoLMS(bool direct = true) : ColorConvGLLinear(direct)
    {
        memcpy(mtx, mtxXYZtoLMS, 9 * sizeof(float));
        memcpy(mtx_inv, mtxLMStoXYZ, 9 * sizeof(float));
    }
};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_XYZ_TO_LMS_HPP */

