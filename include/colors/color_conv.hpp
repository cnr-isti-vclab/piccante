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

#include "../util/matrix_3_x_3.hpp"

namespace pic {

/**
 * @brief The ColorConv class
 */
class ColorConv
{
protected:
    bool linear;

    float mtx[9], mtx_inv[9];

    /**
     * @brief computeInverse
     */
    void computeInverse()
    {
        Matrix3x3 tmp;
        tmp.set(mtx);

        Matrix3x3 tmp_inv;
        tmp.inverse(&tmp_inv);

        memcpy(mtx_inv, tmp_inv.data, 9 * sizeof(float));
    }

public:

    /**
     * @brief ColorConv
     */
    ColorConv()
    {
        linear = true;
    }

    /**
    * @brief direct converts from a color space to another one.
    * \param colIn a pointer to the input color to be converted.
    * \param colOut a pointer to the output color.
    */
    virtual void direct(float *colIn, float *colOut)
    {
        apply(mtx, colIn, colOut);
    }

    /**
    * @brief inverse is the inverse of direct.
    * \param colIn a pointer to the input color to be converted.
    * \param colOut a pointer to the output color.
    */
    virtual void inverse(float *colIn, float *colOut)
    {
        apply(mtx_inv, colIn, colOut);
    }

    /**
     * @brief transform
     * @param colIn
     * @param colOut
     * @param bDirection
     */
    void transform(float *colIn, float *colOut, bool bDirection) {
        if(bDirection) {
            direct(colIn, colOut);
        } else {
            inverse(colIn, colOut);
        }
    }

    /**
     * @brief apply
     * @param mtx
     * @param colIn
     * @param colOut
     */
    static void apply(const float *mtx, float *colIn, float *colOut)
    {
        //working copy
        float tmp[3];
        tmp[0] = colIn[0];
        tmp[1] = colIn[1];
        tmp[2] = colIn[2];

        //conversion
        colOut[0] = tmp[0] * mtx[0] + tmp[1] * mtx[1] + tmp[2] * mtx[2];
        colOut[1] = tmp[0] * mtx[3] + tmp[1] * mtx[4] + tmp[2] * mtx[5];
        colOut[2] = tmp[0] * mtx[6] + tmp[1] * mtx[7] + tmp[2] * mtx[8];
    }

    /**
     * @brief apply_s a safe apply
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

    /**
     * @brief getMatrix
     * @return
     */
    float *getMatrix()
    {
        return mtx;
    }

    /**
     * @brief getMatrixInverse
     * @return
     */
    float *getMatrixInverse()
    {
        return mtx_inv;
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_CONV_HPP */

