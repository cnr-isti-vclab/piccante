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

#ifndef PIC_GL_FILTERING_FILTER_GROW_CUT_HPP
#define PIC_GL_FILTERING_FILTER_GROW_CUT_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLGrowCut class
 */
class FilterGLGrowCut: public FilterGL
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
        uniform sampler2D u_max; \n
        uniform sampler2D u_state_cur; \n
        const int dx[8] = int[](-1, 0, 1, -1, 1, -1,  0,  1); \n
        const int dy[8] = int[]( 1, 1, 1,  0, 0, -1, -1, -1); \n
        out  vec4 f_color; \n
        \n
        void main(void) { \n
            \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3 col = texelFetch(u_tex, coords, 0).xyz; \n
            vec2 cur = texelFetch(u_state_cur, coords, 0).xy; \n
            vec3 col_max = texelFetch(u_max, coords, 0).xyz; \n
            float C = dot(col_max, col_max); \n
            vec2 next = cur; \n
            \n
            for(int k = 0; k < 8; k++) {\n
                ivec2 coords_k = coords + ivec2(dx[k], dy[k]);\n

                vec2 cur_k = texelFetch(u_state_cur, coords_k, 0).xy; \n
                vec3 col_k = texelFetch(u_tex, coords_k, 0).xyz; \n
                vec3 delta_col = col - col_k;\n
                float g_theta = 1.0 - (dot(delta_col, delta_col) / C);
                g_theta *= cur_k.y;\n
                if(g_theta > cur.y) {\n
                    next.x = cur_k.x;\n
                    next.y = g_theta;\n
                 }\n
            }\n
            f_color = vec4(next.x, next.y, 0.0, 1.0); \n
        }
                          );

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLGrowCut");

        update();
    }

public:

    /**
     * @brief FilterGLGrowCut
     * @param channel
     */
    FilterGLGrowCut() : FilterGL()
    {
        initShaders();
    }

    ~FilterGLGrowCut()
    {
        release();
    }

    /**
     * @brief update
     */
    void update()
    {
        technique.bind();
        technique.setUniform1i("u_state_cur", 0);
        technique.setUniform1i("u_tex", 1);
        technique.setUniform1i("u_max", 2);
        technique.unbind();
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GROW_CUT_HPP */

