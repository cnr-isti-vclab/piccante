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

#ifndef PIC_GL_FILTERING_FILTER_ANAGLYPH_HPP
#define PIC_GL_FILTERING_FILTER_ANAGLYPH_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLAnaglyph class
 */
class FilterGLAnaglyph: public FilterGL
{
protected:

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLAnaglyph
     */
    FilterGLAnaglyph();
};

FilterGLAnaglyph::FilterGLAnaglyph(): FilterGL()
{
    InitShaders();
}

void FilterGLAnaglyph::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_texL; \n
                          uniform sampler2D u_texR; \n
                          out     vec4      f_color; \n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec3  colL = texelFetch(u_texL, coords, 0).xyz;
        \n
        vec3  colR = texelFetch(u_texR, coords, 0).xyz;
        \n
        vec3  colA = vec3(colL.x, colR.y, colR.z);
        colA = pow(colA, vec3(0.45));
        f_color = vec4(colA, 1.0);
        \n
    }
                      );


    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_texL",      0);
    filteringProgram.uniform("u_texR",      1);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_ANAGLYPH_HPP */

