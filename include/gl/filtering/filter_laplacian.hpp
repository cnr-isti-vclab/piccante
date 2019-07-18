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

#ifndef PIC_GL_FILTERING_FILTER_LAPLACIAN_HPP
#define PIC_GL_FILTERING_FILTER_LAPLACIAN_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLLaplacian class
 */
class FilterGLLaplacian: public FilterGL
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
            \n
            ivec2 coords = ivec2(gl_FragCoord.xy);\n
            vec3  color = -4.0 * texelFetch(u_tex, coords, 0).xyz;\n
            color += texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz;\n
            color += texelFetch(u_tex, coords - ivec2(1, 0), 0).xyz;\n
            color += texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz;\n
            color += texelFetch(u_tex, coords - ivec2(0, 1), 0).xyz;\n
            f_color = vec4(color, 1.0);
        }\n
                          );

        //
        //
        //

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLLaplacian");
    }

public:

    /**
     * @brief FilterGLLaplacian
     */
    FilterGLLaplacian() : FilterGL()
    {
        //protected values are assigned/computed
        initShaders();
    }

    ~FilterGLLaplacian()
    {
        release();
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_LAPLACIAN_HPP */

