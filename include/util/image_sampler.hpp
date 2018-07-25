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

#ifndef PIC_UTIL_IMAGE_SAMPLER_HPP
#define PIC_UTIL_IMAGE_SAMPLER_HPP

#include "../base.hpp"

#include "../util/math.hpp"

namespace pic {

/*
			a--x----b
			|  |	|
			y--?----y
			|  |	|
			c--x----d
*/

/**
 * @brief Bilinear calculates 2D bilinear interpolation at the point (x,y).
 * @param a is the NW pixel value.
 * @param b is the NE pixel value.
 * @param c is the SW pixel value.
 * @param d is the SE pixel value.
 * @param x is the horizontal coordinate.
 * @param y is the vertical coordinate.
 * @return the evaluation of the B-spline.
 */
template<class Scalar> inline Scalar Bilinear(Scalar a, Scalar b, Scalar c, Scalar d, float x, float y)
{
    Scalar px0 = a + y * (c - a);
    Scalar px1 = b + y * (d - b);
    return px0 + x * (px1 - px0);
}

/**
 * @brief invBilinear
 * @param A
 * @param dx
 * @param dy
 * @param out
 */
inline void invBilinear(float A, float dx, float dy, float *out)
{
    dx = CLAMPi(dx, 0.0f, 1.0f);
    dy = CLAMPi(dy, 0.0f, 1.0f);

    out[0] = A * dx;
    out[1] = A * (1.0f - dx);

    float i_dy = 1.0f - dy;
    out[2] = out[0] * i_dy;
    out[3] = out[1] * i_dy;

    out[0] = out[0] * dy;
    out[1] = out[0] * dy;
}

/**
 * @brief Rx evaluates B-spline (cubic).
 * @param x is the curve parameter in [0, 1].
 * @return the evaluation of the B-spline.
 */
inline float Rx(float x)
{
    float px_1 = MAX(x - 1.0f, 0.0f);
    float px   = MAX(x,        0.0f);
    float px1  = MAX(x + 1.0f, 0.0f);
    float px2  = MAX(x + 2.0f, 0.0f);

    return (         px2  * px2  * px2
            - 4.0f * px1  * px1  * px1 +
              6.0f * px   * px   * px
            - 4.0f * px_1 * px_1 * px_1
           ) / 6.0f;
}

/**
 * @brief MitchellNetravali
 * @param x
 * @param B
 * @param C
 * @return
 */
inline float MitchellNetravali(float x, float B, float C)
{
    float y = fabsf(x);
    if(y < 1.0f) {
        float y_sq = y * y;
        float t_3 =  12.0f - 9.0f * B - 6.0f * C;
        float t_2 = -18.0f + 12.0f * B + 6.0f * C;
        float c   = 6.0f - 2.0f * B;
        return (t_3 * y_sq * y + t_2 * y_sq + c) / 6.0f;
    } else {
        if(y < 2.0f) {
            float y_sq = y * y;
            float t_3 = -B - 6.0f * C;
            float t_2 = 6.0f * B + 30.0f * C;
            float t_1 = -12.0f * B - 48.0f * C;
            float c   = 8.0f * B + 24.0f * C;
            return (t_3 * y_sq * y + t_2 * y_sq + t_1 * y + c) / 6.0f;
        } else {
            return 0.0f;
        }
    }
}

/**
 * @brief Bicubic
 * @param x
 * @return
 */
inline float Bicubic(float x)
{
    float y = fabsf(x);
    if(y < 1.0f) {
        float y_sq = y * y;
        return (3.0f * y_sq * y -6.0f * y_sq + 4.0f) / 6.0f;
    } else {
        if(y < 2.0f) {
            float y_sq = y * y;
            return (-1.0f * y_sq * y + 6.0f * y_sq - 12.0f * y + 8.0f) / 6.0f;
        } else {
            return 0.0f;
        }
    }
}

/**
 * @brief CatmullRom
 * @param x
 * @return
 */
inline float CatmullRom(float x)
{
    float y = fabsf(x);
    if(y < 1.0f) {
        float y_sq = y * y;
        return (9.0f * y_sq * y - 15.0f * y_sq + 6.0f) / 6.0f;
    } else {
        if(y < 2.0f) {
            float y_sq = y * y;
            return (-3.0f * y_sq * y + 15.0f * y_sq -24.0f * y + 12.0f) / 6.0f;
        } else {
            return 0.0f;
        }
    }
}

/**
 * @brief Lanczos
 * @param x
 * @param a
 * @return
 */
inline float Lanczos(float x, float a)
{
    float y = fabsf(x);

    if(y > 0.0f && y < a) {
        float t = C_PI * x;
        float d = C_PI_2 * x * x;

        return (a * sinf(t) * sinf(t / a)) / d;
    } else {
        if(y > 0.0f) {
            return 0.0f;
        } else {
            return 1.0f;
        }
    }
}

} // end namespace pic

#endif /* PIC_UTIL_IMAGE_SAMPLER_HPP */

