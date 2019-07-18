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

#ifndef PIC_GL_FILTERING_FILTER_DRAGO_TMO_HPP
#define PIC_GL_FILTERING_FILTER_DRAGO_TMO_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../gl/filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The FilterGLDragoTMO class
 */
class FilterGLDragoTMO: public FilterGL
{
protected:
    float b, Ld_Max, LMax, LMax_scaled, Lwa, Lwa_scaled;
    float constant1, constant2;
    bool bGammaCorrection;

    /**
     * @brief computeConstants
     */
    void computeConstants();

    /**
     * @brief initShaders
     */
    void initShaders();

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

public:
    /**
     * @brief FilterGLDragoTMO
     */
    FilterGLDragoTMO();

    ~FilterGLDragoTMO();

    /**
     * @brief update
     * @param Ld_Max
     * @param b
     * @param LMax
     * @param Lwa
     */
    void update(float Ld_Max, float b, float LMax, float Lwa);
};

PIC_INLINE FilterGLDragoTMO::FilterGLDragoTMO(): FilterGL()
{
    Ld_Max	=  100.0f;
    b		=  0.95f;
    LMax	=  1e6f;
    Lwa		= -1.0f;

    bGammaCorrection = false;

    FragmentShader();
    initShaders();
}

PIC_INLINE FilterGLDragoTMO::~FilterGLDragoTMO()
{
    release();
}


PIC_INLINE void FilterGLDragoTMO::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
    uniform sampler2D u_tex;\n
    uniform float constant1;\n
    uniform float constant2;\n
    uniform float LMax;\n
    uniform float Lwa;\n
    out     vec4  f_color;\n

    void main(void) {
        \n
        ivec2 coords   = ivec2(gl_FragCoord.xy);\n
        vec3  color    = texelFetch(u_tex, coords, 0).xyz;\n
        float L        = dot(vec3(0.213, 0.715, 0.072), color);
        float L_scaled = L / Lwa;\n
        float tmp      = pow((L_scaled / LMax), constant1);\n
        float Ld       = constant2 * log(1.0 + L_scaled) / log(2.0 + 8.0 * tmp);\n
        color		   = (color * Ld) / L;\n
        __GAMMA__CORRECTION__ \n
        f_color        = vec4(color, 1.0);\n
        \n
    }\n
                      );

    fragment_source = gammaCorrection(fragment_source, bGammaCorrection);
}

PIC_INLINE void FilterGLDragoTMO::initShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLDragoTMO");
    update(Ld_Max, b, LMax, Lwa);
}

PIC_INLINE void FilterGLDragoTMO::update(float Ld_Max, float b, float LMax, float Lwa)
{
    this->Ld_Max = Ld_Max > 0.0f ? Ld_Max : 100.0f;
    this->b = b > 0.0f ? b : 0.95f;
    this->LMax = LMax > 0.0f ? LMax : 1e6f;
    this->Lwa = Lwa > 0.0f ? Lwa : 1.0f;

    Lwa_scaled  = Lwa / powf(1.0f + b - 0.85f, 5.0f);
    LMax_scaled = LMax / Lwa_scaled;
    constant1   = logf(b) / logf(0.5f);
    constant2   = (Ld_Max / 100.0f) / (log10(1 + LMax_scaled));

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1f("constant1", constant1);
    technique.setUniform1f("constant2", constant2);
    technique.setUniform1f("LMax", LMax_scaled);
    technique.setUniform1f("Lwa", Lwa_scaled);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_DRAGO_TMO_HPP */

