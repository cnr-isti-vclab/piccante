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

#ifndef PIC_GL_FILTERING_FILTER_SLICER_HPP
#define PIC_GL_FILTERING_FILTER_SLICER_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLSlicer class
 */
class FilterGLSlicer: public FilterGL
{
protected:

    void InitShaders();
    void FragmentShader();

    float s_S, s_R, mul_E;

public:
    /**
     * @brief FilterGLSlicer
     * @param s_S
     * @param s_R
     */
    FilterGLSlicer(float s_S, float s_R);

    /**
     * @brief Update
     * @param s_S
     * @param s_R
     */
    void Update(float s_S, float s_R);
};

FilterGLSlicer::FilterGLSlicer(float s_S, float s_R): FilterGL()
{
    this->s_S = s_S;
    this->s_R = s_R;

    FragmentShader();
    InitShaders();
}

void FilterGLSlicer::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D	u_tex;
                          uniform sampler3D	u_grid;
                          uniform float		mul_E;
                          uniform float		s_S;

                          out     vec4       f_color;

    void main(void) {
        //Fetch texture color
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec4 colRef = texelFetch(u_tex, coordsFrag, 0);

        //Fetch E
        vec3 tSize3 = vec3(textureSize(u_grid, 0));
        float E = dot(colRef.xyz, vec3(1.0)) * mul_E;
        E /= tSize3.z;

        //Fetch from the grid
        vec2 coord = (gl_FragCoord.xy * s_S) / tSize3.xy;
        vec4 sliced = texture(u_grid, vec3(coord.xy, E));

        vec3 color = sliced.w > 0.0 ? sliced.xyz / sliced.w : vec3(0.0);

        f_color = vec4(color.xyz, 1.0);
    }
                      );
}

void FilterGLSlicer::InitShaders()
{
    filteringProgram.setup(glw::version("400"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLSlicer Shader log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update(s_S, s_R);
}

void FilterGLSlicer::Update(float s_S, float s_R)
{
    this->s_S = s_S;
    this->s_R = s_R;

    mul_E = s_R / 3.0f;

#ifdef PIC_DEBUG
    printf("Rate S: %f Rate R: %f Mul E: %f\n", s_S, s_R, mul_E);
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("u_grid", 1);
    filteringProgram.uniform("s_S", s_S);
    filteringProgram.uniform("mul_E", mul_E);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SLICER_HPP */
