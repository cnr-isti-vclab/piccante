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

#ifndef PIC_GL_FILTERING_FILTER_REMAPPING_HPP
#define PIC_GL_FILTERING_FILTER_REMAPPING_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLRemapping class
 */
class FilterGLRemapping: public FilterGL
{
protected:

    /**
     * @brief initShaders
     */
    void initShaders()
    {
        /*
         *
         * 0 ---> Drago et al. 2003
         * 1 ---> Reinhard et al. 2002
         * LumZone     = [-2, -1, 0, 1, 2, 3, 4];
         * TMOForZone =  [ 0,  0, 1, 0, 1, 0, 0];
        */

        fragment_source = MAKE_STRING
                          (
                              uniform sampler2D u_tex; \n
                              s out     vec4     f_color; \n
        void main(void) { \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            int indx = int(texelFetch(u_tex, coords, 0).x + 2.0); \n
            indx = (indx == 2) ? 1 : 0; \n
            indx = (indx == 4) ? 1 : indx;\n
            f_color = vec4(vec3(float(indx)), 1.0);\n
        }
                          );

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLRemapping");

        technique.bind();
        technique.setUniform1i("u_tex", 0);
        technique.unbind();
    }

public:
    /**
     * @brief FilterGLRemapping
     */
    FilterGLRemapping() : FilterGL()
    {
        initShaders();
    }

    ~FilterGLRemapping()
    {
        release();
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_REMAPPING_HPP */

