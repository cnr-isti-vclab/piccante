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

#ifndef PIC_GL_FILTERING_FILTER_COLOR_CORRECTION_POULI_HPP
#define PIC_GL_FILTERING_FILTER_COLOR_CORRECTION_POULI_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../gl/filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The FilterGLColorCorrectionPouli class
 */
class FilterGLColorCorrectionPouli: public FilterGL
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
     * @brief FilterGLColorCorrectionPouli
     */
    FilterGLColorCorrectionPouli();

    ~FilterGLColorCorrectionPouli();

    /**
     * @brief update
     * @param Ld_Max
     * @param b
     * @param LMax
     * @param Lwa
     */
    void update(float Ld_Max, float b, float LMax, float Lwa);
};

PIC_INLINE FilterGLColorCorrectionPouli::FilterGLColorCorrectionPouli(): FilterGL()
{
    Ld_Max	=  100.0f;
    b		=  0.95f;
    LMax	=  1e6f;
    Lwa		= -1.0f;

    bGammaCorrection = false;

    FragmentShader();
    initShaders();
}

PIC_INLINE FilterGLColorCorrectionPouli::~FilterGLColorCorrectionPouli()
{
    release();
}


PIC_INLINE void FilterGLColorCorrectionPouli::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
    uniform sampler2D u_hdr;\n
    uniform sampler2D u_tmo;\n
    uniform mat3 mRGBtoLMS;\n
    uniform mat3 mLMStoIPT;\n

    out     vec4  f_color;\n

    vec3 colorConv(float3 col, float g) {
        vec3 LMS = mRGBtoLMS * col;
        vec3 s = sign(LMS);
        vec3 LMSp = pow(LMS * s, g) * s;
        vec3 IPT = mLMStoIPT * LMSp;
        vec3 ICh;
        ICh.x = IPT.x  + 1e-5;
        ICh.y = sqrt(dot(IPT.yz, IPT.yz)) + 1e-5;
        ICh.z = atan2(IPT.y, IPT.z);
        return ICh;
    }

    vec3 colorConvInv(float3 col, float g) {
        vec3 LMS = mRGBtoLMS * col;
        vec3 s = sign(LMS);
        vec3 LMSp = pow(LMS * s, g) * s;
        vec3 IPT = mLMStoIPT * LMSp;
        vec3 ICh;
        ICh.x = IPT.x  + 1e-5;
        ICh.y = sqrt(dot(IPT.yz, IPT.yz)) + 1e-5;
        ICh.z = atan2(IPT.y, IPT.z);
        return ICh;
    }

    float saturation(vec3 col)
    {
        return col.y / sqrt(dot(col.xy, col.xy));
    }

    void main(void) {
        \n
        ivec2 coords   = ivec2(gl_FragCoord.xy);\n
        vec3  cHDR    = texelFetch(u_hdr, coords, 0).xyz;\n
        vec3 ICh_hdr = colorConv(cHDR, 0.43);
        vec3 ICh_tmo = colorConv(cTMO, 0.43);
        float C_tmo_prime = ICh_tmo.y * ICh_hdr.x / ICh_tmo.x;
        float r1 = saturation(ICh_hdr.y, ICh_hdr.x);
        float r2 = saturation(C_tmo_prime, ICh_tmo.x);
        ICh_tmo.y = (C_tmo_prime * r1) / r2;
        ICh_tmo.z = ICh_hdr.z;
        vec3 color = colorConvInv(ICh_tmo, 1.0/0.43);
        f_color = vec4(color, 1.0);\n
        \n
    }\n
                      );

    fragment_source = gammaCorrection(fragment_source, bGammaCorrection);
}

PIC_INLINE void FilterGLColorCorrectionPouli::initShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLColorCorrectionPouli");
    update(Ld_Max, b, LMax, Lwa);
}

PIC_INLINE void FilterGLColorCorrectionPouli::update(float Ld_Max, float b, float LMax, float Lwa)
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
    technique.setUniform1i("u_hdr", 0);
    technique.setUniform1i("u_tmo", 0);
    technique.setUniform1f("constant1", constant1);
    technique.setUniform1f("constant2", constant2);
    technique.setUniform1f("LMax", LMax_scaled);
    technique.setUniform1f("Lwa", Lwa_scaled);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_COLOR_CORRECTION_POULI_HPP */

