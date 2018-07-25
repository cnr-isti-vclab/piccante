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

#ifndef PIC_UTIL_GL_BICUBIC_HPP
#define PIC_UTIL_GL_BICUBIC_HPP

#include "../../base.hpp"

#include "../../util/string.hpp"

namespace pic {

/**
 * @brief GLSL_BICUBIC returns bicubic sample.
 * @return It returns a string; a building block for a shader.
 */
PIC_INLINE std::string GLSL_BICUBIC()
{
    std::string ret;

    ret = MAKE_STRING(
                float Bicubic(float x)
                {
                    float y = abs(x);
                    if(y < 1.0) {
                        float y_sq = y * y;
                        return (3.0 * y_sq * y - 6.0 * y_sq + 4.0) / 6.0;
                    } else {
                        if(y < 2.0) {
                            float y_sq = y * y;
                            return (-1.0 * y_sq * y + 6.0 * y_sq - 12.0 * y + 8.0) / 6.0;
                        } else {
                            return 0.0;
                        }
                    }
                }
    );

    return ret;
}

/**
 * @brief GLSL_TEXTURE_BICUBIC
 * @return
 */
PIC_INLINE std::string GLSL_TEXTURE_BICUBIC()
{
    std::string ret;

    ret = MAKE_STRING(

            vec4 textureBicubic(sampler2D u_tex, vec2 coords)
            {
                ivec2 tSize_u = textureSize(u_tex, 0) - ivec2(1, 1);
                vec2 tSize = vec2(tSize_u);
                vec2 coords_uc = vec2(coords * tSize);
                vec2 d = fract(coords_uc);

                ivec2 coords_i = ivec2(floor(coords_uc));
                vec2 r;
                ivec2 e;
                vec4 ret = vec4(0.0);

                for(int j = -1; j < 3; j++) {
                    r.y = Bicubic(float(j) - d.y);
                    e.y = clamp(coords_i.y + j, 0, tSize_u.y);

                    for(int i = -1; i < 3; i++) {
                        r.x = Bicubic(-(float(i) - d.x));
                        e.x = clamp(coords_i.x + i, 0, tSize_u.x);
                        r.x *= r.y;

                        ret += r.x * texelFetch(u_tex, e, 0);
                    }
                }
                return ret;
            }
    );

    return ret;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_BICUBIC_HPP */

