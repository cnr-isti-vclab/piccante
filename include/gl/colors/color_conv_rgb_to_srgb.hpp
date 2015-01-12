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

#ifndef PIC_GL_COLORS_COLOR_CONV_RGB_TO_SRGB_HPP
#define PIC_GL_COLORS_COLOR_CONV_RGB_TO_SRGB_HPP

#include "gl/colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvGLRGBtosRGB class
 */
class ColorConvGLRGBtosRGB: public ColorConvGL
{
protected:
    float a, a_plus_1, gamma, gamma_inv;

public:

    /**
     * @brief ColorConvGLRGBtosRGB
     */
    ColorConvGLRGBtosRGB(bool direct = true) : ColorConvGL(direct)
    {
        gamma = 2.4f;
        gamma_inv = 1.0f / gamma;
        a = 0.055f;
        a_plus_1 = 1.0f + a;
    }

    /**
     * @brief getDirectFunction
     * @return
     */
    std::string getDirectFunction()
    {
        std::string fragment_source = GLW_STRINGFY(
            uniform sampler2D u_tex; \n
            uniform float a; \n
            uniform float a_plus_one; \n
            uniform float gamma_inv; \n
            out     vec4 f_color; \n
            \n
            float tosRGB(float x) {
                if(x > 0.0031308) {
                    return a_plus_one * pow(x, gamma_inv) - a;
                } else {
                    return x * 12.92;
                }
            }

            void main(void) {
                ivec2 coords = ivec2(gl_FragCoord.xy); \n
                vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
                f_color = vec4(tosRGB(color.x), tosRGB(color.y), tosRGB(color.z), 1.0); \n
                \n
            }
                              );

        return fragment_source;
    }

    /**
     * @brief getInverseFunction
     * @return
     */
    std::string getInverseFunction()
    {
        std::string fragment_source = GLW_STRINGFY(
            uniform sampler2D u_tex; \n
            uniform float a; \n
            uniform float a_plus_one; \n
            uniform float gamma; \n
            out     vec4 f_color; \n
            \n
            float toRGB(float x) {
                if(x > 0.04045) {
                    return pow((x + a) / a_plus_one, gamma);
                } else {
                    return x / 12.92;
                }
            }

            void main(void) {
                \n
                ivec2 coords = ivec2(gl_FragCoord.xy); \n
                vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
                f_color = vec4(toRGB(color.x), toRGB(color.y), toRGB(color.z), 1.0); \n
            \n
            }
                );

        return fragment_source;
    }

    /**
     * @brief setUniforms
     */
    void setUniforms()
    {
        if(direct) {
            glw::bind_program(programs[0]);
            programs[0].uniform("a", a);
            programs[0].uniform("a_plus_one", a_plus_1);
            programs[0].uniform("gamma_inv", gamma_inv);
        } else {
            glw::bind_program(programs[1]);
            programs[1].uniform("a", a);
            programs[1].uniform("a_plus_one", a_plus_1);
            programs[1].uniform("gamma", gamma);
        }
        glw::bind_program(0);
    }

};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_RGB_TO_SRGB_HPP */

