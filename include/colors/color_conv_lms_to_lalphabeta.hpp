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

#ifndef PIC_COLORS_COLOR_CONV_LMS_TO_L_ALPHA_BETA_HPP
#define PIC_COLORS_COLOR_CONV_LMS_TO_L_ALPHA_BETA_HPP

#include "../colors/color_conv.hpp"

namespace pic {

const float mtxLMStoLab[] = {   0.57735f,   0.57735f,   0.57735f,
                                0.408248f,  0.408248f, -0.816497f,
                                0.707107f, -0.707107f,  0.0
                            };

const float mtxLabtoLMS[] = {   0.57735f,   0.408248f,   0.707107f,
                                0.57735f,   0.408248f,  -0.707107f,
                                0.57735f,  -0.816497f,   0.0f
                            };

/**
 * @brief The ColorConvLMStoLAlphaBeta class
 */
class ColorConvLMStoLAlphaBeta: public ColorConv
{
public:

    /**
     * @brief ColorConvLMStoLAlphaBeta
     */
    ColorConvLMStoLAlphaBeta() : ColorConv()
    {
        memcpy(mtx, mtxLMStoLab, 9 * sizeof(float));
        memcpy(mtx_inv, mtxLMStoLab, 9 * sizeof(float));
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_XYZ_HPP */
