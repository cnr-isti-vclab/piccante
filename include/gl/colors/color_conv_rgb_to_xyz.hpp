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

#ifndef PIC_GL_COLORS_COLOR_CONV_RGB_TO_XYZ_HPP
#define PIC_GL_COLORS_COLOR_CONV_RGB_TO_XYZ_HPP

#include "colors/color_conv_rgb_to_xyz.hpp"

#include "gl/colors/color_conv.hpp"

namespace pic {

/**
 * @brief The ColorConvGLRGBtoXYZ class
 */
class ColorConvGLRGBtoXYZ: public ColorConvGL
{
public:

    /**
     * @brief ColorConvGLRGBtoXYZ
     */
    ColorConvGLRGBtoXYZ(bool direct = true) : ColorConvGL(direct)
    {
    }

    /**
     * @brief getDirectFunction
     * @return
     */
    std::string getDirectFunction()
    {
        std::string fragment_source = GLW_STRINGFY
                          (
        uniform sampler2D u_tex; \n
        uniform mat3 mtx; \n
        out     vec4 f_color; \n
        \n
        void main(void) {
            \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
            f_color = vec4(mtx * color, 1.0); \n
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
        return getDirectFunction();
    }

    /**
     * @brief setUniforms
     */
    void setUniforms()
    {
        if(direct) {
            glw::bind_program(programs[0]);
            programs[0].uniform3x3("mtx", mtxRGBtoXYZ, true);
        } else {
            glw::bind_program(programs[1]);
            programs[1].uniform3x3("mtx", mtxXYZtoRGB, true);
        }

        glw::bind_program(0);
    }

};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_RGB_TO_XYZ_HPP */

