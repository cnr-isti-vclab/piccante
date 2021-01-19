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



const float mtxLMStoIPT[] = { 0.4f,     0.4f,     0.2f,
                              4.455f,  -4.851f,   0.396f,
                              0.8056f,  0.3572f, -1.1628f
                            };

const float mtxIPTtoLMS[] = {  1.0f,  0.0976f,  0.2052f,
                               1.0f, -0.1139f,  0.1332f,
                               1.0f,  0.0326f, -0.6769f
                             };

/**
 * @brief The ColorConvXYZtoLMS class
 */
class ColorConvLMStoIPT: public ColorConv
{
protected:
    /**
     * @brief LMSNonLinearityFunction
     * @param x
     * @return
     */
    float nonLinearityFunction(float x, float g)
    {
        if(x >= 0.0f) {
            return powf(x, g);
        } else {
            return -powf(-x, g);
        }

    }
public:

    /**
     * @brief ColorConvLMStoIPT
     */
    ColorConvLMStoIPT() : ColorConv()
    {
        memcpy(mtx, mtxLMStoIPT, 9 * sizeof(float));
        memcpy(mtx_inv, mtxIPTtoLMS, 9 * sizeof(float));
    }

    /**
    * @brief direct converts from a color space to another one.
    * \param colIn a pointer to the input color to be converted.
    * \param colOut a pointer to the output color.
    */
    void direct(float *colIn, float *colOut)
    {
        float tmp[3];
        for(int i = 0; i < 3; i++) {
            tmp[i] = nonLinearityFunction(colIn[i], 0.43f);
        }

        apply(mtx, tmp, colOut);
    }

    /**
    * @brief inverse is the inverse of direct.
    * \param colIn a pointer to the input color to be converted.
    * \param colOut a pointer to the output color.
    */
    void inverse(float *colIn, float *colOut)
    {
        float tmp[3];
        apply(mtx_inv, colIn, tmp);

        for(int i = 0; i < 3; i++) {
            colOut[i] = nonLinearityFunction(tmp[i], 1.0f / 0.43f);
        }
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_CONV_LMS_TO_IPT_HPP */
