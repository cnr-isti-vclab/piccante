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

#ifndef PIC_GL_FILTERING_FILTER_SIMPLE_TMO_HPP
#define PIC_GL_FILTERING_FILTER_SIMPLE_TMO_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLSimpleTMO class
 */
class FilterGLSimpleTMO: public FilterGL
{
protected:
    float fstop, gamma;

    void initShaders()
    {
        fragment_source = MAKE_STRING
                          (
                              uniform sampler2D u_tex; \n
                              uniform float	  tn_gamma; \n
                              uniform float	  tn_exposure; \n
                              out     vec4      f_color;	\n

        void main(void) { \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3  color = texelFetch(u_tex, coords, 0).xyz; \n
            color = pow(color * tn_exposure, vec3(tn_gamma));
            f_color = vec4(color, 1.0);
            \n
        }\n
                          );

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLSimpleTMO");
    }

public:
    /**
     * @brief FilterGLSimpleTMO
     */
    FilterGLSimpleTMO(): FilterGL()
    {
        initShaders();
        update(2.2f, 0.0f);
    }

    /**
     * @brief FilterGLSimpleTMO
     * @param fstop
     * @param gamma
     */
    FilterGLSimpleTMO(float gamma, float fstop) : FilterGL()
    {
        initShaders();
        update(gamma, fstop);
    }

    /**
     * @brief update
     * @param fstop
     * @param gamma
     */
    void update(float gamma, float fstop)
    {
        gamma = gamma > 0.0f ? gamma : 2.2f;

        this->gamma = gamma;
        this->fstop = fstop;

        float invGamma = 1.0f / gamma;
        float exposure = powf(2.0f, fstop);

        if(technique.isValid()) {
            technique.bind();
            technique.setUniform1i("u_tex", 0);
            technique.setUniform1f("tn_gamma", invGamma);
            technique.setUniform1f("tn_exposure", exposure);
            technique.unbind();
        }
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SIMPLE_TMO_HPP */

