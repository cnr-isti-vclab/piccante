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

#ifndef PIC_GL_FILTERING_FILTER_COLOR_CONV_RGB_TO_XYZ_HPP
#define PIC_GL_FILTERING_FILTER_COLOR_CONV_RGB_TO_XYZ_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_color_conv.hpp"
#include "colors/color_conv_rgb_to_xyz.hpp"

namespace pic {

/**
 * @brief The FilterGLColorConvRGBtoXYZ class
 */
class FilterGLColorConvRGBtoXYZ: public FilterGLColorConv
{
protected:

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLColorConvRGBtoXYZ
     */
    FilterGLColorConvRGBtoXYZ();

    /**
     * @brief Update
     * @param direct
     */
    void Update(bool direct);
};

FilterGLColorConvRGBtoXYZ::FilterGLColorConvRGBtoXYZ(): FilterGLColorConv()
{
    InitShaders();
}

void FilterGLColorConvRGBtoXYZ::InitShaders()
{
    fragment_source = GLW_STRINGFY
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

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLColorConvRGBtoXYZ log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update(direct);
}

void FilterGLColorConvRGBtoXYZ::Update(bool direct)
{
    this->direct = direct;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    if(direct) {
        filteringProgram.uniform3x3("mtx", mtxRGBtoXYZ, true);
    } else {
        filteringProgram.uniform3x3("mtx", mtxXYZtoRGB, true);
    }
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_COLOR_CONV_RGB_TO_XYZ_HPP */

