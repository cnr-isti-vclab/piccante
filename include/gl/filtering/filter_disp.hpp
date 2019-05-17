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

#ifndef PIC_GL_FILTERING_FILTER_DISP_HPP
#define PIC_GL_FILTERING_FILTER_DISP_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

#define DEBUG_GL

/**
 * @brief The FilterGLDisp class
 */
class FilterGLDisp: public FilterGL
{
protected:

    float sigma;
    float sigma_s;
    float sigma_r;

    /**
     * @brief initShaders
     */
    void initShaders();

public:

    /**
     * @brief FilterGLDisp
     */
    FilterGLDisp();

    /**
     * @brief update
     * @param sigma
     * @param sigma_s
     * @param sigma_r
     * @param bUse
     * @param bLeft
     */
    void update(float sigma, float sigma_s, float sigma_r, bool bUse, bool bLeft);

    /**
     * @brief execute
     * @param nameLeft
     * @param nameRight
     * @param nameDisp
     * @param nameOut
     * @return
     */
    static ImageGL *execute(std::string nameLeft,
                               std::string nameRight,
                               std::string nameDisp,
                               std::string nameOut)
    {
        ImageGL imgL(nameLeft);
        ImageGL imgR(nameRight);
        ImageGL imgD(nameDisp);

        imgL.generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);
        imgR.generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);
        imgD.generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);

        FilterGLDisp filter;

        ImageGL *imgOut = filter.Process(TripleGL(&imgL, &imgR, &imgD), NULL);
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLDisp::FilterGLDisp(): FilterGL()
{
    sigma = 2.0f;
    sigma_s = 2.0f;
    sigma_r = 0.05f;
    initShaders();
}

void FilterGLDisp::initShaders()
{
    fragment_source = MAKE_STRING
                      (
                          uniform sampler2D u_texL; \n
                          uniform sampler2D u_texR; \n
                          uniform sampler2D u_texD; \n
                          uniform int halfKernelSize; \n
                          uniform float	sigma; \n
                          uniform float	sigma_s2; \n
                          uniform float	sigma_r2; \n
                          uniform float	bUse; \n
                          uniform float bLeft; \n
                          out     vec4 f_color; \n

    vec4 fetchDispCol(ivec2 coords) {
        float shiftf = texelFetch(u_texD, coords, 0).x;

        if(shiftf > 1e-3) {
            coords.x += int(shiftf);
            vec3  col_R = texelFetch(u_texR, coords, 0).xyz;
            return vec4(col_R, shiftf);
        } else {
            return vec4(0.0);
        }
    }

    /*	(x --> disp)
    	(y --> mask)
    	(z --> score)	*/

    void main(void) {
        \n
        f_color = vec4(0.0);
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec2 delta;
        vec3 acc = vec3(0.0);
        float tot = 0.0;
        vec3 refDisp = texelFetch(u_texD, coords, 0).xyz;
        vec3 refCol = texelFetch(u_texL, coords, 0).xyz;

        for(int i = -halfKernelSize; i <= halfKernelSize; i++) {
            delta.y = float(i);

            for(int j = -halfKernelSize; j <= halfKernelSize; j++) {
                delta.x = float(j);

                //Color fetch
                ivec2 tmpCoords = coords + ivec2(j, i);
                vec3  tmpCol = texelFetch(u_texL, tmpCoords, 0).xyz;

                //Disparity fetch
                vec3  tmpDisp = texelFetch(u_texD, tmpCoords, 0).xyz;
                tmpCoords.x += int(bLeft * tmpDisp.x);
                vec3  tmpCol2 = texelFetch(u_texR, tmpCoords, 0).xyz;

                //Spatial weight
                float ws = exp(-dot(delta, delta) / sigma_s2);

                //Disparity weight
                float deltaDisp = tmpDisp.x - refDisp.x;
                float wd = exp(-deltaDisp * deltaDisp / sigma_r2);

                //Other pixels: color similarity
                vec3 diffCol = tmpCol - refCol;
                float wc = exp(-dot(diffCol, diffCol) / sigma);

                if(bUse < 0.5f) {
                    wc = 1.0f;
                }

                //((tmpDisp.z+1e-6)*sigma/tmpDisp.y))*bUse;

                //Weights
                float w = wd * ws;
                tot += w * wc;
                acc += tmpCol * w * wc;
                /*					acc += (tmpCol+tmpCol2*wc)*w;
                					tot += (1.0+wc)*w;*/
            }
        }

        acc /= tot;
        f_color = vec4(acc, 1.0);
    }
                      );

    technique.initStandard("330", vertex_source, fragment_source, "FilterGLDisp");

    float sigma_s2 = 2.0f * sigma_s * sigma_s;
    float sigma_r2 = 2.0f * sigma_r * sigma_r;
    int halfKernelSize = PrecomputedGaussian::getKernelSize(sigma_s) >> 1;

    technique.bind();
    technique.setUniform1f("sigma_s2", sigma_s2);
    technique.setUniform1f("sigma_r2", sigma_r2);
    technique.setUniform1f("sigma", sigma * sigma * 2.0f);
    technique.setUniform1i("halfKernelSize", halfKernelSize);
    technique.setUniform1f("bUse", 1.0f);
    technique.setUniform1f("bLeft", -1.0f);

    technique.setUniform1i("u_texL", 0);
    technique.setUniform1i("u_texR", 1);
    technique.setUniform1i("u_texD", 2);
    technique.unbind();
}

void FilterGLDisp::update(float sigma, float sigma_s, float sigma_r, bool bUse,
                          bool bLeft)
{
    this->sigma = sigma;
    this->sigma_r = sigma_r;
    this->sigma_s = sigma_s;

    int halfKernelSize = PrecomputedGaussian::getKernelSize(sigma_s) >> 1;

    float sigma_s2 = 2.0f * sigma_s * sigma_s;
    float sigma_r2 = 2.0f * sigma_r * sigma_r;

    technique.bind();
    technique.setUniform1i("u_texL",      0);
    technique.setUniform1i("u_texR",      1);
    technique.setUniform1i("u_texD",      2);
    technique.setUniform1f("sigma_s2",	sigma_s2);
    technique.setUniform1f("sigma_r2",	sigma_r2);

    if(bUse) {
        technique.setUniform1f("bUse", 1.0f);
    } else {
        technique.setUniform1f("bUse", 0.0f);
    }

    if(bLeft) {
        technique.setUniform1f("bLeft", 1.0f);
    } else {
        technique.setUniform1f("bLeft", -1.0f);
    }

    technique.setUniform1f("sigma", sigma * sigma * 2.0f);
    technique.setUniform1i("halfKernelSize", halfKernelSize);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_DISP_HPP */

