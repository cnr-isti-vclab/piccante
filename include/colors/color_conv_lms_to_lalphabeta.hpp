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
        float c1 = 1.0f / sqrtf(3.0f);
        float c2 = 1.0f / sqrtf(6.0f);
        float c3 = 1.0f / sqrtf(2.0f);
        float c4 = -c2 * 2.0f;
        
        float mtxLMStoLalphabeta[] = { c1, c1, c1,
                                       c2, c2, c4,
                                       c3, -c3, 0.0f};

        float mtxLalphabetatoLMS[] = { c1, c2,  c3,
                                       c1, c2, -c3,
                                       c1, c4, 0.0f};

        memcpy(mtx, mtxLMStoLalphabeta, 9 * sizeof(float));
        memcpy(mtx_inv, mtxLalphabetatoLMS, 9 * sizeof(float));
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_XYZ_HPP */
