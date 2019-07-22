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

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../gl/filtering/filter_luminance.hpp"

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
     * @brief initShaders
     */
    void initShaders();

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

    ~FilterGLDurandTMO();

    /**
     * @brief update
     * @param compression_factor
     * @param log_absolute
     */
    void update(float compression_factor, float log_absolute);
};

PIC_INLINE FilterGLDurandTMO::FilterGLDurandTMO() : FilterGL()
{
    this->compression_factor = log10fPlusEpsilon(5.0f);
    this->log_absolute = this->compression_factor * 6.0f;

    this->bGammaCorrection = false;

    FragmentShader();
    initShaders();
}

PIC_INLINE FilterGLDurandTMO::FilterGLDurandTMO(float compression_factor, float log_absolute,
                                   bool bGammaCorrection = false): FilterGL()
{
    this->bGammaCorrection = bGammaCorrection;

    FragmentShader();
    initShaders();

    update(compression_factor, log_absolute);
}

PIC_INLINE FilterGLDurandTMO::~FilterGLDurandTMO()
{
    release();
}

PIC_INLINE void FilterGLDurandTMO::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
    uniform sampler2D u_tex;        \n
    uniform sampler2D u_lum_log;    \n
    uniform sampler2D u_base;       \n
    uniform float     compression_factor;\n
    uniform float     log_absolute;\n
    out     vec4      f_color;	\n

    void main(void) {\n
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);\n
        vec3 color = texelFetch(u_tex, coords, 0).xyz;\n
        float L = dot(vec3(0.213, 0.715, 0.072), color);\n
        float L_log  = texelFetch(u_lum_log, coords, 0).x;\n
        float base = texelFetch(u_base, coords, 0).x;\n
        float detail = L_log - base;\n
        float L_comp = (base * compression_factor + detail) -  log_absolute;\n
        L_comp = pow(10.0, L_comp);\n
        color = (color * L_comp / L);\n
        __GAMMA__CORRECTION__ \n
        f_color = vec4(color, 1.0);\n
        \n
    }\n
                      );

    fragment_source = gammaCorrection(fragment_source, bGammaCorrection);
}

PIC_INLINE void FilterGLDurandTMO::initShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLDurandTMO");
}

PIC_INLINE void FilterGLDurandTMO::update(float compression_factor, float log_absolute)
{
    this->compression_factor = compression_factor;
    this->log_absolute = log_absolute;

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1i("u_lum_log", 1);
    technique.setUniform1i("u_base", 2);
    technique.setUniform1f("compression_factor", compression_factor);
    technique.setUniform1f("log_absolute", log_absolute);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_DURAND_TMO_HPP */

