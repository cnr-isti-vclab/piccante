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

#include "../../base.hpp"

#include "../../gl/colors/color_conv_rgb_to_hsl.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLHSLReplace class
 */
class FilterGLHSLReplace: public FilterGL
{
protected:
    float delta_hue, delta_saturation;

    /**
     * @brief initShaders
     */
    void initShaders()
    {
        fragment_source = MAKE_STRING
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

        technique.initStandard("330", vertex_source, final_fragment_source, "FilterGLGradient");

        technique.bind();
        technique.setUniform1i("u_tex",      0);
        technique.setUniform1i("u_change",   1);
        technique.setUniform1f("delta_hue",  delta_hue);
        technique.setUniform1f("delta_saturation",  delta_saturation);
        technique.unbind();
    }

public:
    /**
     * @brief FilterGLHSLReplace
     * @param delta_hue
     * @param delta_saturation
     */
    FilterGLHSLReplace(float delta_hue, float delta_saturation) : FilterGL()
    {
        this->delta_hue = delta_hue;
        this->delta_saturation = delta_saturation;
        initShaders();
    }

    ~FilterGLHSLReplace()
    {
        release();
    }

    /**
     * @brief setDeltaHue
     * @param delta_hue
     */
    void setDeltaHue(float delta_hue)
    {
        this->delta_hue = delta_hue;
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_HSL_REPLACE_HPP */

