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

#ifndef PIC_GL_FILTERING_FILTER_HSL_REPLACE_HPP
#define PIC_GL_FILTERING_FILTER_HSL_REPLACE_HPP

#include "gl/colors/color_conv_rgb_to_hsl.hpp"
#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLHSLReplace class
 */
class FilterGLHSLReplace: public FilterGL
{
protected:
    float delta_hue;
    float delta_saturation;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLHSLReplace
     * @param delta_hue
     * @param delta_saturation
     */
    FilterGLHSLReplace(float delta_hue, float delta_saturation);

    /**
     * @brief setDeltaHue
     * @param delta_hue
     */
    void setDeltaHue(float delta_hue)
    {
        this->delta_hue = delta_hue;
    }
};

//Basic constructor
FilterGLHSLReplace::FilterGLHSLReplace(float delta_hue,
                                       float delta_saturation): FilterGL()
{
    this->delta_hue = delta_hue;
    this->delta_saturation = delta_saturation;
    InitShaders();
}

void FilterGLHSLReplace::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform float	  delta_hue; \n
                          uniform float	  delta_saturation; \n
                          uniform sampler2D u_tex; \n
                          uniform sampler2D u_change; \n
                          out     vec4      f_color; \n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec3  color = texelFetch(u_tex, coords, 0).xyz;
        \n
        float weight = texelFetch(u_change, coords, 0).x;
        weight = min(max(weight, 0.0), 1.0);

        if(weight > 0.0) {
            color = RGB2HSL(color);
            \n
            color.x += delta_hue * weight;
            \n
            color.z += max(delta_saturation * weight, 0.0);
            \n
            color = HSL2RGB(color);
            \n
        }

        f_color = vec4(color.xyz, 1.0);
        \n
    }
                      );

    //Final fragment source
    std::string final_fragment_source;
    final_fragment_source  = ColorConvGLRGBtoHSL::getDirect();
    final_fragment_source += ColorConvGLRGBtoHSL::getInverse();
    final_fragment_source += fragment_source;

    filteringProgram.setup(glw::version("330"), vertex_source, final_fragment_source);
#ifdef PIC_DEBUG
    printf("[FilterGLHSLReplace log]\n%s\n", filteringProgram.log().c_str());
#endif
    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("u_change",   1);
    filteringProgram.uniform("delta_hue",  delta_hue);
    filteringProgram.uniform("delta_saturation",  delta_saturation);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_HSL_REPLACE_HPP */

