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

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLBlend class
 */
class FilterGLBlend: public FilterGL
{
protected:

    /**
     * @brief initShaders
     */
    void initShaders()
    {
        fragment_source = MAKE_STRING
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

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLBlend");

        technique.bind();
        technique.setUniform1i("u_tex0", 0);
        technique.setUniform1i("u_tex1", 1);
        technique.setUniform1i("u_texMask", 2);
        technique.unbind();
    }

public:

    /**
     * @brief FilterGLBlend
     */
    FilterGLBlend() : FilterGL()
    {
        //protected values are assigned/computed
        initShaders();
    }

    ~FilterGLBlend()
    {
        release();
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BLEND_HPP */

