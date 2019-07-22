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

#ifndef PIC_GL_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP
#define PIC_GL_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP

#include "../../base.hpp"

#include "../../util/std_util.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../gl/filtering/filter_iterative.hpp"

namespace pic {

/**
 * @brief The FilterGLAnisotropicDiffusion class
 */
class FilterGLAnisotropicDiffusion: public FilterGL
{
protected:
    /**
     * @brief initShaders
     */
    void initShaders();

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

    float delta_t, k;
    unsigned int iterations;
    FilterGLIterative *flt;

public:
    /**
     * @brief FilterGLAnisotropicDiffusion
     * @param k
     * @param iterations
     */
    FilterGLAnisotropicDiffusion(float k, unsigned int iterations);

    /**
     * @brief FilterGLAnisotropicDiffusion
     * @param sigma_s
     * @param sigma_r
     */
    FilterGLAnisotropicDiffusion(float sigma_s, float sigma_r);

    ~FilterGLAnisotropicDiffusion();

    void releaseAux()
    {
        delete_s(flt);
    }

    /**
     * @brief update
     * @param k
     */
    void update(float k);

    /**
     * @brief AnisotropicDiffusion
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *AnisotropicDiffusion(ImageGLVec imgIn, ImageGL *imgOut)
    {
        if(flt == NULL) {
            flt = new FilterGLIterative(this, iterations);
        }

        return flt->Process(imgIn, imgOut);
    }
};

PIC_INLINE FilterGLAnisotropicDiffusion::FilterGLAnisotropicDiffusion(float k,
        unsigned int iterations): FilterGL()
{
    if(iterations < 1) {
        iterations = 1;
    }

    flt = NULL;

    this->k = k;
    this->iterations = iterations;

    //protected values are assigned/computed
    FragmentShader();
    initShaders();

    update(k);
}

PIC_INLINE FilterGLAnisotropicDiffusion::FilterGLAnisotropicDiffusion(float sigma_s,
        float sigma_r): FilterGL()
{
    sigma_r = sigma_r <= 0.0f ? 0.11f : sigma_r;

    flt = NULL;

    iterations = int(ceilf(5.0f * sigma_s));

    //protected values are assigned/computed
    FragmentShader();
    initShaders();

    update(sigma_r);

}

PIC_INLINE FilterGLAnisotropicDiffusion::~FilterGLAnisotropicDiffusion()
{
    release();
}

PIC_INLINE void FilterGLAnisotropicDiffusion::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
                          uniform sampler2D u_tex; \n
                          uniform float	  k_sq; \n
                          uniform float	  delta_t; \n
                          out     vec4      f_color; \n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec3 cB = texelFetch(u_tex, coords           , 0).xyz;
        \n
        vec3 c0 = texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz;
        \n
        vec3 c1 = texelFetch(u_tex, coords - ivec2(1, 0), 0).xyz;
        \n
        vec3 c2 = texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz;
        \n
        vec3 c3 = texelFetch(u_tex, coords - ivec2(0, 1), 0).xyz;
        \n
        vec3 gN = c2 - cB;
        \n
        vec3 gS = c3 - cB;
        \n
        vec3 gW = c1 - cB;
        \n
        vec3 gE = c0 - cB;
        \n
        vec4 c = vec4(dot(gN, gN), dot(gS, gS), dot(gW, gW), dot(gE, gE));
        \n
        c = exp(-c / vec4(k_sq));
        \n
        f_color = vec4(cB + delta_t *(c.x * gN + c.y * gS + c.z * gW + c.w * gE), 1.0);
        \n
    }
                      );
}

PIC_INLINE void FilterGLAnisotropicDiffusion::initShaders()
{
    FragmentShader();
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLAnisotropicDiffusion");
}

PIC_INLINE void FilterGLAnisotropicDiffusion::update(float k)
{
    this->k = k > 0.0f ? this->k : 0.11f;
    float k_sq = this->k * this->k;

    this->delta_t = 1.0f / 7.0f;

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1f("k_sq", k_sq);
    technique.setUniform1f("delta_t", delta_t);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP */

