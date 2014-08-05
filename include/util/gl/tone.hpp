/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_UTIL_GL_TONE_HPP
#define PIC_UTIL_GL_TONE_HPP

namespace pic {

/**GLSL_SIMPLE_EXPOSURE_GAMMA: apply simple gamma correction and exposure*/
std::string GLSL_SIMPLE_EXPOSURE_GAMMA()
{
    std::string ret;

    ret = GLW_STRINGFY(
    vec3 SimpleTMO(vec3 col, float exposure, float gamma) {
        pow(color.xyz * exposure, vec3(gamma))
    }
          );

    return ret;
};

/**GLSL_DRAGO_TMO: Drago et al.'s TMO*/
std::string GLSL_DRAGO_TMO()
{
    std::string ret;

    ret = GLW_STRINGFY(
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
};

} // end namespace pic

#endif /* PIC_UTIL_GL_TONE_HPP */

