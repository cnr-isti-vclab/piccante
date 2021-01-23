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

#include "../../colors/color_conv_rgb_to_lms.hpp"
#include "../../colors/color_conv_lms_to_ipt.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../gl/filtering/filter_luminance.hpp"
#include "../../filtering/filter_color_correction_pouli.hpp"
namespace pic {

/**
 * @brief The FilterGLColorCorrectionPouli class
 */
class FilterGLColorCorrectionPouli: public FilterGL
{
protected:
    bool bGammaCorrection;

    float mtx_RGBtoLMS[9], mtx_RGBtoLMS_inv[9];
    float mtx_LMStoIPT[9], mtx_LMStoIPT_inv[9];

    /**
     * @brief initShaders
     */
    void initShaders()
    {
        FragmentShader();
        technique.initStandard("330", vertex_source, fragment_source, "FilterGLColorCorrectionPouli");
    }

    /**
     * @brief FragmentShader
     */
    void FragmentShader()
    {
        fragment_source = MAKE_STRING
                          (
        uniform sampler2D u_hdr;\n
        uniform sampler2D u_tmo;\n
        uniform float mHDR;\n
        uniform float mTMO;\n
        uniform mat3 mRGBtoLMS;\n
        uniform mat3 mLMStoIPT;\n
        uniform mat3 mRGBtoLMS_inv;\n
        uniform mat3 mLMStoIPT_inv;\n

        out     vec4  f_color;\n

        vec3 ccRGBtoICh(vec3 col, vec3 g)
        {
            vec3 LMS = mRGBtoLMS * col;

            vec3 s = sign(LMS);
            vec3 LMSp = pow(abs(LMS), g) * s;

            vec3 IPT = mLMStoIPT * LMSp;

            vec3 ICh;
            ICh.x = IPT.x;
            ICh.y = sqrt(dot(IPT.yz, IPT.yz));
            ICh.z = atan(IPT.y, IPT.z);
            return ICh;
        }

        vec3 ccIChtoRGB(vec3 col, vec3 g)
        {
            vec3 IPT;
            IPT.x = col.x;
            IPT.y = col.y * sin(col.z);
            IPT.z = col.y * cos(col.z);

            vec3 LMSp = mLMStoIPT_inv * IPT;

            vec3 s = sign(LMSp);
            vec3 LMS = pow(abs(LMSp), g) * s;

            vec3 rgb = mRGBtoLMS_inv * LMS;
            return rgb;
        }

        float saturation(float C, float I)
        {
            return C / sqrt(C * C + I * I);
        }

        void main(void) {
            \n
            ivec2 coords   = ivec2(gl_FragCoord.xy);\n
            vec3 cHDR    = texelFetch(u_hdr, coords, 0).xyz / mHDR;\n
            vec3 cTMO    = texelFetch(u_tmo, coords, 0).xyz / mTMO;\n
            vec3 ICh_hdr =  ccRGBtoICh(cHDR, vec3(0.43));\n
            vec3 ICh_tmo =  ccRGBtoICh(cTMO, vec3(0.43));\n

            float I_tmo = ICh_tmo.x;\n
            ICh_hdr.xy += vec2(1e-5);\n
            ICh_tmo.xy += vec2(1e-5);\n

            float C_tmo_p = (ICh_tmo.y * ICh_hdr.x) / ICh_tmo.x;\n
            float s1 = saturation(ICh_hdr.y, ICh_hdr.x);\n
            float s2 = saturation(C_tmo_p, ICh_tmo.x);\n
            ICh_tmo.y = I_tmo;\n
            ICh_tmo.y = (C_tmo_p * s1) / s2;\n
            ICh_tmo.z = ICh_hdr.z;\n
            vec3 col = ccIChtoRGB(ICh_tmo, vec3(2.3256));\n
            col = clamp(col * mTMO, vec3(0.0), vec3(1.0));\n
            f_color = vec4(col, 1.0);\n
        }\n
                          );

    }

public:
    /**
     * @brief FilterGLColorCorrectionPouli
     */
    FilterGLColorCorrectionPouli() : FilterGL()
    {
        ColorConvRGBtoLMS RGBtoLMS;
        ColorConvLMStoIPT LMStoIPT;

        memcpy(mtx_RGBtoLMS, RGBtoLMS.getMatrix(), 9 * sizeof(float));
        memcpy(mtx_RGBtoLMS_inv, RGBtoLMS.getMatrixInverse(), 9 * sizeof(float));
        memcpy(mtx_LMStoIPT, LMStoIPT.getMatrix(), 9 * sizeof(float));
        memcpy(mtx_LMStoIPT_inv, LMStoIPT.getMatrixInverse(), 9 * sizeof(float));

        initShaders();

        update(1.0f, 1.0f);
    }

    ~FilterGLColorCorrectionPouli()
    {
        release();
    }

    /**
     * @brief update
     * @param Ld_Max
     * @param b
     * @param LMax
     * @param Lwa
     */
    void update(float mHDR, float mTMO)
    {
        technique.bind();
        technique.setUniform1i("u_hdr", 0);
        technique.setUniform1i("u_tmo", 1);

        technique.setUniform1f("mHDR", mHDR);
        technique.setUniform1f("mTMO", mTMO);

        technique.setUniform3x3("mRGBtoLMS", mtx_RGBtoLMS, true);
        technique.setUniform3x3("mLMStoIPT", mtx_LMStoIPT, true);
        technique.setUniform3x3("mRGBtoLMS_inv", mtx_RGBtoLMS_inv, true);
        technique.setUniform3x3("mLMStoIPT_inv", mtx_LMStoIPT_inv, true);

        technique.unbind();
    }

    static ImageGL *execute(FilterGLColorCorrectionPouli *flt,
                            ImageGL *imgHDR, ImageGL *imgTMO, ImageGL *imgOut)
    {
        if(imgHDR == NULL || imgTMO == NULL) {
            return imgOut;
        }

        if(imgHDR->channels != 3 || imgTMO->channels != 3) {
            return imgOut;
        }

        float mHDR[3], mTMO[3];

        imgHDR->getMaxVal(mHDR);
        imgTMO->getMaxVal(mTMO);

        int ind;
        float maxHDR = Arrayf::getMax(mHDR, 3, ind);
        float maxTMO = Arrayf::getMax(mTMO, 3, ind);
        printf("%f %f", maxHDR, maxTMO);

        if(maxHDR > 0.0f && maxTMO > 0.0f) {
            printf("AAA");
            flt->update(maxHDR, maxTMO);
            imgOut = flt->Process(DoubleGL(imgHDR, imgTMO), imgOut);
        }
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_COLOR_CORRECTION_POULI_HPP */

