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

#ifndef PIC_GL_FILTERING_FILTER_GRADIENT_HPP
#define PIC_GL_FILTERING_FILTER_GRADIENT_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLGradient class
 */
class FilterGLGradient: public FilterGL
{
protected:
    /**
     * @brief initShaders
     */
    void initShaders()
    {
        fragment_source = MAKE_STRING
                          (
                              uniform sampler2D u_tex; \n
                              out vec4      f_color;	\n

        void main(void) {
            \n d
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3  c0 = texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz; \n
            vec3  c1 = texelFetch(u_tex, coords - ivec2(1, 0), 0).xyz; \n
            vec3  c2 = texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz; \n
            vec3  c3 = texelFetch(u_tex, coords - ivec2(0, 1), 0).xyz; \n
            vec3 Gx = c1 - c0; \n
            vec3 Gy = c2 - c3; \n
            f_color = vec4(sqrt(Gx.xyz * Gx.xyz + Gy.xyz * Gy.xyz), 1.0);\n d d
        }\n
                          );

        //
        //
        //

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLGradient");

        technique.bind();
        technique.setUniform1i("u_tex", 0);
        technique.unbind();
    }

public:

    /**
     * @brief FilterGLGradient
     */
    FilterGLGradient() : FilterGL()
    {
        //protected values are assigned/computed
        initShaders();
    }

    ~FilterGLGradient()
    {
        release();
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GRADIENT_HPP */

