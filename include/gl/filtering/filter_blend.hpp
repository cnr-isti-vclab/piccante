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

#ifndef PIC_GL_FILTERING_FILTER_BLEND_HPP
#define PIC_GL_FILTERING_FILTER_BLEND_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLBlend class
 */
class FilterGLBlend: public FilterGL
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
     * @brief FilterGLBlend
     */
    FilterGLBlend();
};

FilterGLBlend::FilterGLBlend(): FilterGL()
{
    //protected values are assigned/computed
    FragmentShader();
    InitShaders();
}

void FilterGLBlend::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex0; \n
                          uniform sampler2D u_tex1; \n
                          uniform sampler2D u_texMask; \n
                          out vec4      f_color;	\n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);\n
        vec4  color0 = texelFetch(u_tex0, coords, 0);\n
        vec4  color1 = texelFetch(u_tex1, coords, 0);\n
        float weight = texelFetch(u_texMask, coords, 0).x;\n
        f_color = mix(color0, color1, weight);
    }\n
                      );
}

void FilterGLBlend::InitShaders()
{
    FragmentShader();

    std::string prefix;

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLBlend log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex0", 0);
    filteringProgram.uniform("u_tex1", 1);
    filteringProgram.uniform("u_texMask", 2);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BLEND_HPP */

