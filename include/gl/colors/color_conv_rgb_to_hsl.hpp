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

#ifndef PIC_GL_COLORS_COLOR_CONV_RGB_TO_HSL_HPP
#define PIC_GL_COLORS_COLOR_CONV_RGB_TO_HSL_HPP

#include "../../gl/colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvGLRGBtoHSL class
 */
class ColorConvGLRGBtoHSL: public ColorConvGL
{
public:

    /**
     * @brief ColorConvGLRGBtoHSL
     */
    ColorConvGLRGBtoHSL(bool direct = true) : ColorConvGL(direct)
    {
    }

    /**
     * @brief getDirect
     * @return
     */
    static std::string getDirect()
    {
        std::string fun = MAKE_STRING(
                    const vec3 LUM_XYZ =   vec3(0.213,  0.715,   0.072);
                    const vec3 ALPHA_VAL = vec3(0.0,    0.866,  -0.866);
                    const vec3 BETA_VAL =  vec3(1.0,   -0.5,    -0.5);

                    vec3 RGB2HSL(vec3 col) {
                        vec3 ret;

                        //Intensity
                        ret.y = dot(col, LUM_XYZ);

                        //alpha beta
                        float alpha = dot(col, BETA_VAL);
                        float beta =  dot(col, ALPHA_VAL);

                        //Hue
                        if(alpha == 0.0 && beta == 0.0) {
                            ret.x = 0.0;
                            ret.z = 0.0;
                        } else {
                            ret.x = atan(alpha, beta); //atan2(x,y) == atan(y,x)
                            ret.z = sqrt(alpha * alpha + beta * beta);
                        }

                        return ret;
                    }
         );
        return fun;
    }

    /**
     * @brief getInverse
     * @return
     */
    static std::string getInverse()
    {
        std::string fun = MAKE_STRING(
                    const vec3 R_VAL =	vec3(0.9990,    -0.3709,    0.7862);
                    const vec3 G_VAL =	vec3(0.9990,     0.2065,   -0.2138);
                    const vec3 B_VAL =	vec3(0.9990,    -0.9482,   -0.2138);
                    out     vec4 f_color; \n
                    \n

                  vec3 HSL2RGB(vec3 col) {
                      vec3 ret;
                      vec3 tmp;
                      tmp.x = col.y;				//Luminance
                      tmp.y = col.z * cos(col.x);	//Alpha
                      tmp.z = col.z * sin(col.x);	//Beta

                      ret.x = dot(tmp, R_VAL);
                      ret.y = dot(tmp, G_VAL);
                      ret.z = dot(tmp, B_VAL);

                      return ret;
                  }
                    );
        return fun;
    }

    /**
     * @brief getDirectFunction
     * @return
     */
    std::string getDirectFunction()
    {
        std::string fragment_source = getDirect() + MAKE_STRING(
            uniform sampler2D u_tex; \n                
            out vec4 f_color; \n

            void main(void) {
                ivec2 coords = ivec2(gl_FragCoord.xy); \n
                vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
                f_color = vec4(RGB2HSL(color), 1.0); \n
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
        std::string fragment_source = getInverse() + MAKE_STRING(
            uniform sampler2D u_tex; \n
            out     vec4 f_color; \n
            void main(void) {\n
                ivec2 coords = ivec2(gl_FragCoord.xy); \n
                vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
                f_color = vec4(HSL2RGB(color), 1.0); \n
            \n
            }
        );

        return fragment_source;
    }
};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_RGB_TO_HSL_HPP */

