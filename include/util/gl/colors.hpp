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

#ifndef PIC_UTIL_GL_COLORS_HPP
#define PIC_UTIL_GL_COLORS_HPP

namespace pic {

/**GLSL_RGB2HSL: converts RGB values into HSL ones*/
std::string GLSL_RGB2HSL()
{
    std::string ret;

    ret = GLW_STRINGFY(
              const vec3 LUM_XYZ =   vec3(0.213, 0.715,  0.072);
              const vec3 ALPHA_VAL = vec3(0.0,   0.866, -0.866);
              const vec3 BETA_VAL =  vec3(1.0,  -0.5,   -0.5);

    vec3 RGB2HSL(vec3 col) {
        vec3 ret;

        //Intensity
        ret.y = dot(col, LUM_XYZ);

        //alpha beta
        float alpha = dot(col, BETA_VAL);
        float beta =  dot(col, ALPHA_VAL);

        //Hue
        if(alpha == 0.0 && beta == 0.0) {
            ret.x = 0.0;
            ret.z = 0.0;
        } else {
            ret.x = atan(alpha, beta); //atan2(x,y) == atan(y,x)
            ret.z = sqrt(alpha * alpha + beta * beta);
        }

        return ret;
    };
          );
    return ret;
}

/**GLSL_HSL2RGB: converts HSL values into RGB ones*/
std::string GLSL_HSL2RGB()
{
    std::string ret;

    ret = GLW_STRINGFY(
              const vec3 R_VAL =	vec3(0.9990,   -0.3709,    0.7862);
              const vec3 G_VAL =	vec3(0.9990,    0.2065,   -0.2138);
              const vec3 B_VAL =	vec3(0.9990,   -0.9482,   -0.2138);

    vec3 HSL2RGB(vec3 col) {
        vec3 ret;
        vec3 tmp;
        tmp.x = col.y;				//Luminance
        tmp.y = col.z * cos(col.x);	//Alpha
        tmp.z = col.z * sin(col.x);	//Beta

        ret.x = dot(tmp, R_VAL);
        ret.y = dot(tmp, G_VAL);
        ret.z = dot(tmp, B_VAL);

        return ret;
    };
          );
    return ret;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_COLORS_HPP */

