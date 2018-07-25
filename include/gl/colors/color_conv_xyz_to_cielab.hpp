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

#ifndef PIC_GL_COLORS_COLOR_CONV_XYZ_TO_CIE_LAB_HPP
#define PIC_GL_COLORS_COLOR_CONV_XYZ_TO_CIE_LAB_HPP

#include "../../gl/colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvGLXYZtoCIELAB class
 */
class ColorConvGLXYZtoCIELAB: public ColorConvGL
{
protected:
    float		white_point[3];

public:

    /**
     * @brief ColorConvGLXYZtoCIELAB
     */
    ColorConvGLXYZtoCIELAB(bool direct = true) : ColorConvGL(direct)
    {
        white_point[0] = 1.0f;
        white_point[1] = 1.0f;
        white_point[2] = 1.0f;
    }

    /**
     * @brief getDirectFunction
     * @return
     */
    std::string getDirectFunction()
    {
        std::string fragment_source = MAKE_STRING
                          (
        uniform sampler2D u_tex; \n
        uniform vec3 white_point; \n
        out     vec4 f_color; \n
        \n

        const float C_SIX_OVER_TWENTY_NINE_CUBIC = 0.00885645167903563081717167575546;
        const float C_FOUR_OVER_TWENTY_NINE = 0.13793103448275862068965517241379;
        const float C_CIELAB_C1 = 7.787037037037037037037037037037;

        float f(float t) {
            if(t > C_SIX_OVER_TWENTY_NINE_CUBIC) {
                return pow(t, 1.0 / 3.0);
            } else {
                return C_CIELAB_C1 * t +
                       C_FOUR_OVER_TWENTY_NINE;
            }
        }

        void main(void) {
            \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3 colIn = texelFetch(u_tex, coords, 0).xyz; \n

            vec3 colOut;
            float fY_Yn = f(colIn[1] / white_point[1]);

            colOut[0] = 116.0 * fY_Yn - 16.0;
            colOut[1] = 500.0 * (f(colIn[0] / white_point[0]) - fY_Yn);
            colOut[2] = 200.0 * (fY_Yn - f(colIn[2] / white_point[2]));

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
        std::string fragment_source = MAKE_STRING
                          (
        uniform sampler2D u_tex; \n
        uniform vec3 white_point; \n
        out     vec4 f_color; \n
        const float C_CIELAB_C1_INV = 0.12841854934601664684898929845422; \n
        const float C_FOUR_OVER_TWENTY_NINE = 0.13793103448275862068965517241379; \n
        const float C_SIX_OVER_TWENTY_NINE = 0.20689655172413793103448275862069; \n
        \n
        \n
        float f_inv(float t) {
            if(t > C_SIX_OVER_TWENTY_NINE ){
                return pow(t, 3.0);
            } else {
                return (t - C_FOUR_OVER_TWENTY_NINE) * C_CIELAB_C1_INV;
            }
        }

        void main(void) {
            \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3 colIn = texelFetch(u_tex, coords, 0).xyz; \n
            vec3 colOut;

             float tmp = (colIn[0] + 16.0f) / 116.0;

             colOut[1] = white_point[1] * f_inv(tmp);
             colOut[0] = white_point[0] * f_inv(tmp + colIn[1] / 500.0);
             colOut[2] = white_point[2] * f_inv(tmp - colIn[2] / 200.0);

            f_color = vec4(colOut, 1.0); \n
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
            techniques[0].bind();
            techniques[0].setUniform3fv("white_point", white_point);
            techniques[0].unbind();
        } else {
            techniques[1].bind();
            techniques[1].setUniform3fv("white_point", white_point);
            techniques[1].unbind();
        }
    }
};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_XYZ_TO_CIE_LAB_HPP */

