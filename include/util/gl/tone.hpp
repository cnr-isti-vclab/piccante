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

#ifndef PIC_UTIL_GL_TONE_HPP
#define PIC_UTIL_GL_TONE_HPP

#include <string>

#include "../../base.hpp"

namespace pic {

/**
 * @brief GLSL_SIMPLE_EXPOSURE_GAMMA applies a simple gamma correctiona and
 * exposure in a shader.
 * @return It returns a string which represents a part of a shader.
 */
PIC_INLINE std::string GLSL_SIMPLE_EXPOSURE_GAMMA()
{
    std::string ret;

    ret = MAKE_STRING(
    vec3 SimpleTMO(vec3 col, float exposure, float gamma) {
        pow(color.xyz * exposure, vec3(gamma))
    }
          );

    return ret;
}

/**
 * @brief GLSL_DRAGO_TMO returns Drago et al.'s tone mapping operator.
 * @return It returns a string; a building block for a shader.
 */
PIC_INLINE std::string GLSL_DRAGO_TMO()
{
    std::string ret;

    ret = MAKE_STRING(
              const vec3 LUM_XYZ =   vec3(0.213, 0.715,  0.072);
              //maxL: maximum luminance
              //c1: log(Drago_b)/log(0.5)
              //c2: (Drago_Ld_Max/100)/(log10(1+LMax))
    vec3 DragoTMO(vec3 col, float maxL, float c1, float c2) {
        float Lw = dot(LUM_XYZ, col);
        float Ld = c2 * log(1 + L) / log(2.0 + 8.0 * pow((L / maxL), c2));
        return (col.xyz * Ld) / Lw;
    }
          );

    return ret;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_TONE_HPP */

