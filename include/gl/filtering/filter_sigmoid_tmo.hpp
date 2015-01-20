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

#ifndef PIC_GL_FILTERING_FILTER_SIGMOID_TMO_HPP
#define PIC_GL_FILTERING_FILTER_SIGMOID_TMO_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The FilterGLSigmoidTMO class
 */
class FilterGLSigmoidTMO: public FilterGL
{
protected:
    float alpha, epsilon;
    bool  bGammaCorrection, bLocal;

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
     * @brief FilterGLSigmoidTMO
     */
    FilterGLSigmoidTMO();

    /**
     * @brief FilterGLSigmoidTMO
     * @param alpha
     * @param bLocal
     * @param bGammaCorrection
     */
    FilterGLSigmoidTMO(float alpha, bool bLocal, bool bGammaCorrection);

    /**
     * @brief Update
     * @param alpha
     */
    void Update(float alpha);
};

FilterGLSigmoidTMO::FilterGLSigmoidTMO(): FilterGL()
{
    alpha = 0.15f;
    bGammaCorrection = false;
    bLocal = false;
    epsilon = 1.0f;

    InitShaders();
}

FilterGLSigmoidTMO::FilterGLSigmoidTMO(float alpha, bool bLocal,
                                       bool bGammaCorrection): FilterGL()
{
    this->alpha = alpha;
    this->bLocal = bLocal;
    this->bGammaCorrection = bGammaCorrection;
    epsilon = 1.0f;

    InitShaders();
}

void FilterGLSigmoidTMO::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D	u_tex; \n
                          uniform sampler2D	u_tex_adapt; \n
                          uniform float		alpha; \n
                          uniform float		epsilon; \n
                          out     vec4		f_color; \n
                          const	vec3		LUM_XYZ =	vec3(0.213, 0.715, 0.072); \n

    void main(void) {
        \n
        ivec2  coords = ivec2(gl_FragCoord.xy);
        \n
        vec3   color  = texelFetch(u_tex, coords, 0).xyz;
        \n
        __LOCAL__1__ \n
        float Lw =	dot(color,		LUM_XYZ);
        \n
        __LOCAL__2__ \n
        color = (color.xyz * Ld) / Lw;
        \n
        __GAMMA__CORRECTION__ \n
        f_color = vec4(color, 1.0);
        \n
    }\n
                      );

    size_t processing_found1 = fragment_source.find("__LOCAL__1__");

    if(bLocal) {
        fragment_source.replace(processing_found1, 12,
                                " float Lwa = texelFetch(u_tex_adapt,	coords,0).x; ");

        size_t processing_found2 = fragment_source.find("__LOCAL__2__");
        fragment_source.replace(processing_found2, 12,
                                " float Ld  = (Lw * alpha)/(Lwa * alpha + epsilon); ");
    } else {
        fragment_source.replace(processing_found1, 12, " ");

        size_t processing_found2 = fragment_source.find("__LOCAL__2__");
        fragment_source.replace(processing_found2, 12,
                                " float Lscale = Lw * alpha;\n float Ld = Lscale / (Lscale + epsilon); ");
    }

    fragment_source = GammaCorrection(fragment_source, bGammaCorrection);
}

void FilterGLSigmoidTMO::InitShaders()
{
    FragmentShader();

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLSigmoidTMO log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position",		0);
    filteringProgram.fragment_target("f_color",			0);
    filteringProgram.relink();

    Update(alpha);
}

void FilterGLSigmoidTMO::Update(float alpha)
{
    if(alpha > 0.0f) {
        this->alpha = alpha;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("u_tex_adapt", 1);
    filteringProgram.uniform("alpha", this->alpha);
    filteringProgram.uniform("epsilon", epsilon);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SIGMOID_TMO_HPP */

