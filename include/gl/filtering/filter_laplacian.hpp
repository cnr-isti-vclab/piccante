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

#ifndef PIC_GL_FILTERING_FILTER_LAPLACIAN_HPP
#define PIC_GL_FILTERING_FILTER_LAPLACIAN_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLLaplacian class
 */
class FilterGLLaplacian: public FilterGL
{
protected:
    /**
     * @brief InitShaders
     */
    void InitShaders();

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

public:

    /**
     * @brief FilterGLLaplacian
     */
    FilterGLLaplacian();
};

FilterGLLaplacian::FilterGLLaplacian(): FilterGL()
{
    //protected values are assigned/computed
    FragmentShader();
    InitShaders();
}

void FilterGLLaplacian::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex; \n
                          out vec4      f_color;	\n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);\n
        vec3  color = -4.0 * texelFetch(u_tex, coords, 0).xyz;\n
        color += texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz;\n
        color += texelFetch(u_tex, coords - ivec2(1, 0), 0).xyz;\n
        color += texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz;\n
        color += texelFetch(u_tex, coords - ivec2(0, 1), 0).xyz;\n
        f_color = vec4(color, 1.0); //Magnitude
    }\n
                      );
}

void FilterGLLaplacian::InitShaders()
{
    FragmentShader();

    std::string prefix;

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLLaplacian log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_LAPLACIAN_HPP */

