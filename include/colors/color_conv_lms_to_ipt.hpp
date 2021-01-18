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

#ifndef PIC_COLORS_COLOR_CONV_LMS_TO_IPT_HPP
#define PIC_COLORS_COLOR_CONV_LMS_TO_IPT_HPP

#include "../colors/color_conv.hpp"

namespace pic {



const float mtxLMStoIPT[] = { 0.4f,     0.4f,    0.2f,
                              4.455f,  -4.851f,  0.396f,
                              0.8056f,  0.3572f, -1.1628f
                            };

const float mtxIPTtoLMS[] = {  0.3897f, 0.6890f, -0.0787f,
                              -0.2298f, 1.1834f, 0.0464f,
                               0.0f,    0.0f,    1.0f
                             };
/**
 * @brief The ColorConvXYZtoLMS class
 */
class ColorConvXYZtoLMS: public ColorConv
{
public:

    /**
     * @brief ColorConvLMStoIPT
     */
    ColorConvLMStoIPT() : ColorConv()
    {
        memcpy(mtx, mtxLMStoIPT, 9 * sizeof(float));
        memcpy(mtx_inv, mtxIPTtoLMS, 9 * sizeof(float));
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_CONV_LMS_TO_IPT_HPP */
