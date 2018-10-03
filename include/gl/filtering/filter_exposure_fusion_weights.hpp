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

#ifndef PIC_GL_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS_HPP
#define PIC_GL_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLExposureFusionWeights class
 */
class FilterGLExposureFusionWeights: public FilterGL
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

    float sigma2, mu;
    float wC, wE, wS;

public:

    /**
     * @brief FilterGLExposureFusionWeights
     * @param wC
     * @param wE
     * @param wS
     */
    FilterGLExposureFusionWeights(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f) : FilterGL()
    {
        float sigma = 0.2f;
        mu = 0.5f;
        sigma2 = 2.0f * sigma * sigma;

        this->wC = wC >= 0.0f ? wC : 1.0f;
        this->wE = wE >= 0.0f ? wE : 1.0f;
        this->wS = wS >= 0.0f ? wS : 1.0f;

        //protected values are assigned/computed
        FragmentShader();

        initShaders();
    }
};

void FilterGLExposureFusionWeights::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
                          uniform sampler2D u_tex; \n
                          uniform sampler2D u_tex_lum; \n
                          uniform float wC;   \n
                          uniform float wE;   \n
                          uniform float wS;   \n
                          uniform float sigma2; \n
                          uniform float mu; \n
                          out vec4      f_color;	\n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);\n

        //saturation weight
        vec3 color = texelFetch(u_tex, coords, 0).xyz;\n
        float tmpMu = dot(color.xyz, vec3(1.0 / 3.0));\n
        vec3 tmpVar = color - vec3(tmpMu);\n
        float pSat = sqrt( dot(tmpVar, tmpVar) / 3.0);\n
        pSat = pow(pSat, wS);\n

        //well-exposedness weight
        float L = texelFetch(u_tex_lum, coords, 0).x;\n
        float tmp = (L - mu);\n
        float pExp = exp(-(tmp * tmp) / sigma2);\n
        pExp = pow(pExp, wE);\n

        //contrast weight
        float pCon = -4.0 * L;
        pCon += texelFetch(u_tex_lum, coords + ivec2(1, 0), 0).x;\n
        pCon += texelFetch(u_tex_lum, coords - ivec2(1, 0), 0).x;\n
        pCon += texelFetch(u_tex_lum, coords + ivec2(0, 1), 0).x;\n
        pCon += texelFetch(u_tex_lum, coords - ivec2(0, 1), 0).x;\n
        pCon = abs(pCon);\n
        pCon = pow(pCon, wC);\n

        f_color = vec4(vec3(pCon * pExp * pSat), 1.0);\n
    }\n
                      );
}

void FilterGLExposureFusionWeights::initShaders()
{
    FragmentShader();

    technique.initStandard("330", vertex_source, fragment_source, "FilterGLExposureFusionWeights");

    technique.bind();
    technique.setUniform1i("u_tex_lum", 0);
    technique.setUniform1i("u_tex", 1);
    technique.setUniform1f("wC", wC);
    technique.setUniform1f("wE", wE);
    technique.setUniform1f("wS", wS);
    technique.setUniform1f("mu", mu);
    technique.setUniform1f("sigma2", sigma2);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS_HPP */

