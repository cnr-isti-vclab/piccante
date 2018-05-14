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

#include "../../gl/colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvGLRGBtosRGB class
 */
class ColorConvGLRGBtosRGB: public ColorConvGL
{
public:

    /**
     * @brief ColorConvGLRGBtosRGB
     */
    ColorConvGLRGBtosRGB(bool direct = true) : ColorConvGL(direct)
    {
    }

    // a = 0.055
    // gamma = 2.4

    /**
     * @brief getDirectFunction
     * @return
     */
    std::string getDirectFunction()
    {
        std::string fragment_source = MAKE_STRING(
            uniform sampler2D u_tex; \n
            out     vec4 f_color; \n
            \n
            float fromRGBtosRGB(float x) {
                if(x > 0.0031308) {
                    return 1.055 * pow(x, 1.0 / 2.4) - 0.055;
                } else {
                    return x * 12.92;
                }
            }

            void main(void) {
                ivec2 coords = ivec2(gl_FragCoord.xy); \n
                vec3 colIn = texelFetch(u_tex, coords, 0).xyz; \n
                vec3 colOut = vec3(fromRGBtosRGB(colIn.x),
                                   fromRGBtosRGB(colIn.y),
                                   fromRGBtosRGB(colIn.z));
                f_color = vec4(colOut, 1.0); \n
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
        std::string fragment_source = MAKE_STRING(
            uniform sampler2D u_tex; \n
            out     vec4 f_color; \n
            \n
            float fromsRGBtoRGB(float x) {
                if(x > 0.04045) {
                    return pow((x + 0.055) / (1.055), 2.4);
                } else {
                    return x / 12.92;
                }
            }

            void main(void) {
                \n
                ivec2 coords = ivec2(gl_FragCoord.xy); \n
                vec3 colIn = texelFetch(u_tex, coords, 0).xyz; \n
                vec3 colOut = vec3(fromsRGBtoRGB(colIn.x),
                                   fromsRGBtoRGB(colIn.y),
                                   fromsRGBtoRGB(colIn.z)); \n
                f_color = vec4(colOut, 1.0); \n
            \n
            }
                );

        return fragment_source;
    }

    /**
     * @brief getDirectFunctionAux
     * @return
     */
    std::string getDirectFunctionAux()
    {
        return "";
    }

    /**
     * @brief getInverseFunctionAux
     * @return
     */
    std::string getInverseFunctionAux()
    {
        return "";
    }

    /**
     * @brief getDirectUniforms
     * @return
     */
    std::string getDirectUniforms()
    {
        return "";
    }

    /**
     * @brief getInverseUniforms
     * @return
     */
    std::string getInverseUniforms()
    {
        return "";
    }

};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_RGB_TO_SRGB_HPP */

