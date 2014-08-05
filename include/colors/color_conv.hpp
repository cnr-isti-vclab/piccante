/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_COLORS_COLOR_CONV_HPP
#define PIC_COLORS_COLOR_CONV_HPP

namespace pic {

class ColorConv
{
public:

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

    //secure apply
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

