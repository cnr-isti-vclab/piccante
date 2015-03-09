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

#ifndef PIC_GL_FILTERING_FILTER_DURAND_TMO_HPP
#define PIC_GL_FILTERING_FILTER_DURAND_TMO_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The FilterGLDurandTMO class
 */
class FilterGLDurandTMO: public FilterGL
{
protected:
    float compression_factor, log_absolute;
    bool bGammaCorrection;

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
     * @brief FilterGLDurandTMO
     */
    FilterGLDurandTMO();

    /**
     * @brief FilterGLDurandTMO
     * @param compression_factor
     * @param log_absolute
     * @param bGammaCorrection
     */
    FilterGLDurandTMO(float compression_factor, float log_absolute,
                     bool bGammaCorrection);

    /**
     * @brief Update
     * @param compression_factor
     * @param log_absolute
     */
    void Update(float compression_factor, float log_absolute);
};

FilterGLDurandTMO::FilterGLDurandTMO() : FilterGL()
{
    this->compression_factor = log10fPlusEpsilon(5.0f);
    this->log_absolute = this->compression_factor * 6.0f;

    this->bGammaCorrection = false;

    FragmentShader();
    InitShaders();
}

FilterGLDurandTMO::FilterGLDurandTMO(float compression_factor, float log_absolute,
                                   bool bGammaCorrection = false): FilterGL()
{
    this->compression_factor = compression_factor;
    this->log_absolute = log_absolute;

    this->bGammaCorrection = bGammaCorrection;

    FragmentShader();
    InitShaders();
}

void FilterGLDurandTMO::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
    uniform sampler2D u_tex;        \n
    uniform sampler2D u_lum_log;    \n
    uniform sampler2D u_base;       \n
    uniform float	  compression_factor;\n
    uniform float	  log_absolute;\n
    out     vec4      f_color;	\n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);\n
        vec3  color  = texelFetch(u_tex, coords, 0).xyz;\n
        float L      = dot(vec3(0.213, 0.715, 0.072), color);
        float L_log  = texelFetch(u_lum_log, coords, 0).x;\n
        float base   = texelFetch(u_base, coords, 0).x;\n
        float detail = L_log - base;\n
        float L_comp = (base * compression_factor + detail) - (compression_factor + log_absolute);\n
        L_comp = pow(10.0, L_comp);\n
        color = (color * L_comp / L);\n
        __GAMMA__CORRECTION__ \n
        f_color        = vec4(color, 1.0);\n
        \n
    }\n
                      );

    fragment_source = GammaCorrection(fragment_source, bGammaCorrection);
}

void FilterGLDurandTMO::InitShaders()
{
    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLDurandTMO log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();

    Update(compression_factor, log_absolute);
}

void FilterGLDurandTMO::Update(float compression_factor, float log_absolute)
{
    this->compression_factor = compression_factor;
    this->log_absolute = log_absolute;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("u_lum_log", 1);
    filteringProgram.uniform("u_base", 2);
    filteringProgram.uniform("compression_factor", compression_factor);
    filteringProgram.uniform("log_absolute", log_absolute);
    glw::bind_program(0);   
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_DURAND_TMO_HPP */

