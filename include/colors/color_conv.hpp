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

#ifndef PIC_COLORS_COLOR_CONV_HPP
#define PIC_COLORS_COLOR_CONV_HPP

namespace pic {

/**
 * @brief The ColorConv class
 */
class ColorConv
{
public:

    /**
     * @brief ColorConv
     */
    ColorConv()
    {
    }

    /** This method converts from a color space to another one.
    * \param colIn a pointer to the input color to be converted.
    * \param colOut a pointer to the output color.
    */
    virtual void direct(float *colIn, float *colOut) {}

    /** This method is the inverse of direct.
    * \param colIn a pointer to the input color to be converted.
    * \param colOut a pointer to the output color.
    */
    virtual void inverse(float *colIn, float *colOut) {}

    /**
     * @brief apply
     * @param mtx
     * @param colIn
     * @param colOut
     */
    static void apply(const float *mtx, float *colIn, float *colOut)
    {
        //Working copy
        float tmp[3];
        tmp[0] = colIn[0];
        tmp[1] = colIn[1];
        tmp[2] = colIn[2];

        //Conversion
        colOut[0] = tmp[0] * mtx[0] + tmp[1] * mtx[1] + tmp[2] * mtx[2];
        colOut[1] = tmp[0] * mtx[3] + tmp[1] * mtx[4] + tmp[2] * mtx[5];
        colOut[2] = tmp[0] * mtx[6] + tmp[1] * mtx[7] + tmp[2] * mtx[8];
    }

    /**
     * @brief apply_s
     * @param mtx
     * @param colIn
     * @param colOut
     */
    static void apply_s(const float *mtx, float *colIn, float *colOut)
    {
        if(mtx == NULL || colIn == NULL || colOut == NULL) {
            printf("Error in ColorSpaceLinear::ConvertLinearSpace_s");
            return;
        }

        apply(mtx, colIn, colOut);
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_CONV_HPP */

