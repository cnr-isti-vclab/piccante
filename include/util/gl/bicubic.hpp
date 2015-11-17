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

#ifndef PIC_UTIL_GL_BICUBIC_HPP
#define PIC_UTIL_GL_BICUBIC_HPP

namespace pic {

/**
 * @brief GLSL_BICUBIC returns bicubic sample.
 * @return It returns a string; a building block for a shader.
 */
std::string GLSL_BICUBIC()
{
    std::string ret;

    ret = GLW_STRINGFY(
                float Rx(float x)
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

                vec4 textureBicubic(, vec2 coord)
                {
                    float Lw = dot(LUM_XYZ, col);
                    float Ld = c2 * log(1 + L) / log(2.0 + 8.0 * pow((L / maxL), c2));
                    return (col.xyz * Ld) / Lw;
                }
    );

    return ret;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_BICUBIC_HPP */

