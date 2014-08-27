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

#ifndef PIC_GL_FILTERING_FILTER_COLOR_CONV_RGB_TO_SRGB_HPP
#define PIC_GL_FILTERING_FILTER_COLOR_CONV_RGB_TO_SRGB_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_color_conv.hpp"
#include "colors/color_conv_rgb_to_xyz.hpp"

namespace pic {

/**
 * @brief The FilterGLColorConvRGBtosRGB class converts
 * RGB linear values into sRGB color space.
 */
class FilterGLColorConvRGBtosRGB: public FilterGLColorConv
{
protected:

    float a, a_plus_1, gamma, gamma_inv;

    std::string fragment_source_inverse;

    glw::program programs[2];

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLColorConvRGBtosRGB
     */
    FilterGLColorConvRGBtosRGB(bool direct);

    /**
     * @brief Update
     * @param direct
     */
    void Update(bool direct);
};

FilterGLColorConvRGBtosRGB::FilterGLColorConvRGBtosRGB(bool direct): FilterGLColorConv()
{
    this->direct = direct;

    gamma = 2.4f;
    gamma_inv = 1.0f / gamma;
    a = 0.055f;
    a_plus_1 = 1.0f + a;

    InitShaders();
}

void FilterGLColorConvRGBtosRGB::InitShaders()
{
    fragment_source = GLW_STRINGFY(
    uniform sampler2D u_tex; \n
    uniform float a; \n
    uniform float a_plus_one; \n
    uniform float gamma_inv; \n
    out     vec4 f_color; \n
    \n
    float tosRGB(float x) {
        if(x > 0.0031308) {
            return a_plus_one * pow(x, gamma_inv) - a;
        } else {
            return x * 12.92;
        }
    }

    void main(void) {
        ivec2 coords = ivec2(gl_FragCoord.xy); \n
        vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
        f_color = vec4(tosRGB(color.x), tosRGB(color.y), tosRGB(color.z), 1.0); \n
        \n
    }
                      );

    fragment_source_inverse = GLW_STRINGFY(
        uniform sampler2D u_tex; \n
        uniform float a; \n
        uniform float a_plus_one; \n
        uniform float gamma; \n
        out     vec4 f_color; \n
        \n
        float toRGB(float x) {
            if(x > 0.04045) {
                return pow((x + a) / a_plus_one, gamma);
            } else {
                return x / 12.92;
            }
        }

        void main(void) {
            \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
            f_color = vec4(toRGB(color.x), toRGB(color.y), toRGB(color.z), 1.0); \n
        \n
        }
            );

    //direct transform
    programs[0].setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLColorConvRGBtosRGB direct log]\n%s\n", programs[0].log().c_str());
#endif

    glw::bind_program(programs[0]);
    programs[0].attribute_source("a_position", 0);
    programs[0].fragment_target("f_color", 0);
    programs[0].relink();
    programs[0].uniform("u_tex", 0);
    programs[0].uniform("a", a);
    programs[0].uniform("a_plus_one", a_plus_1);
    programs[0].uniform("gamma_inv", gamma_inv);
    glw::bind_program(0);

    //inverse transform
    programs[1].setup(glw::version("330"), vertex_source, fragment_source_inverse);

#ifdef PIC_DEBUG
    printf("[FilterGLColorConvRGBtosRGB inverse log]\n%s\n", programs[1].log().c_str());
#endif

    glw::bind_program(programs[1]);
    programs[1].attribute_source("a_position", 0);
    programs[1].fragment_target("f_color", 0);
    programs[1].relink();
    programs[1].uniform("u_tex", 0);
    programs[1].uniform("a", a);
    programs[1].uniform("a_plus_one", a_plus_1);
    programs[1].uniform("gamma", gamma);
    glw::bind_program(0);

    Update(direct);
}

void FilterGLColorConvRGBtosRGB::Update(bool direct)
{
    this->direct = direct;

    if(direct) {
        filteringProgram = programs[0];
    } else {
        filteringProgram = programs[1];
    }

    glw::bind_program(programs[0]);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_COLOR_CONV_RGB_TO_SRGB_HPP */

