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

#ifndef PIC_COLORS_COLOR_CONV_XYZ_TO_HDRLAB_HPP
#define PIC_COLORS_COLOR_CONV_XYZ_TO_HDRLAB_HPP

#include "colors/color_conv.hpp"
#include <math.h>

namespace pic {

class ColorConvXYZtoHDRLAB: public ColorConv
{
protected:

    float		Yabs, Ys, two_e, epsilon;
    float		whitePoint[3];

public:

    ColorConvXYZtoHDRLAB()
    {
        whitePoint[0] = 1.0f;
        whitePoint[1] = 1.0f;
        whitePoint[2] = 1.0f;

        Ys = 0.5f;
        Yabs = 1.0f;

        two_e = powf(2.0f, computeEpsilon(Ys, Yabs));
    }

    ColorConvXYZtoHDRLAB(float Yabs, float *whitePoint)
    {
        this->Yabs = Yabs;
        this->whitePoint[0] = whitePoint[0];
        this->whitePoint[1] = whitePoint[1];
        this->whitePoint[2] = whitePoint[2];

        Ys = 0.5f;

        epsilon = computeEpsilon(Ys, Yabs);
        two_e = powf(2.0f, epsilon);
    }

    //from XYZ to HDR-CIELAB
    void direct(float *colIn, float *colOut)
    {
        //L_hdr
        colOut[0]  = f(colIn[1] / whitePoint[1]);

        //a_hdr
        colOut[1] = 5.0f * (f(colIn[0] / whitePoint[0]) - f(colIn[1] / whitePoint[1]));

        //b_hdr
        colOut[2] = 2.0f * (f(colIn[1] / whitePoint[1]) - f(colIn[2] / whitePoint[2]));
    }

    //from HDR-CIELAB to XYZ
    void inverse(float *colIn, float *colOut)
    {
        colOut[1] = whitePoint[1] * f_inv( colIn[0] );
        colOut[0] = whitePoint[0] * f_inv( colIn[0] + colIn[1]/5.0f );
        colOut[2] = whitePoint[2] * f_inv( colIn[0] - colIn[2]/2.0f );
    }

    float *WhitePointD65(float *whitePoint)
    {
        if(whitePoint == NULL) {
            whitePoint = new float[3];
        }

        whitePoint[0] = 95.047f;
        whitePoint[1] = 100.0f;
        whitePoint[2] = 108.883f;

        return whitePoint;
    }

    float f(float omega)
    {
        float omega_e = powf(omega, epsilon);
        return (247.0f * omega_e) / (omega_e + two_e) + 0.02f;
    }

    float f_inv(float x)
    {
        float omega_e = ( (x - 0.02f) * two_e ) / (247.0f + 0.02f - x);
        return powf(omega_e, 1.0f / epsilon);
    }

    static float computeEpsilon(float Ys, float Yabs)
    {
        if(Yabs <= 0.0f) {
            Yabs = 1.0f;
        }

        if(Ys < 0.0f || Ys > 1.0f) {
            Ys = 0.5f;
        }

        float sf = 1.25f - 0.25f * (Ys / 0.184f);

        float lf = log(318.0f) / log(Yabs);

        return 0.58f / (sf * lf);
    }
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_SPACE_HDRLAB_HPP */

