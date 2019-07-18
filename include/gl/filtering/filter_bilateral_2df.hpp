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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_2DF_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_2DF_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLBilateral2DF class provides
 * an HW accelerated bilateral filter implementation without
 * approximations.
 */
class FilterGLBilateral2DF: public FilterGL
{
protected:
    float sigma_s, sigma_r;

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
     * @brief FilterGLBilateral2DF
     */
    FilterGLBilateral2DF();

    /**
     * @brief FilterGLBilateral2DF
     * @param sigma_s
     * @param sigma_r
     */
    FilterGLBilateral2DF(float sigma_s, float sigma_r);

    ~FilterGLBilateral2DF();

    /**
     * @brief update
     * @param sigma_s
     * @param sigma_r
     */
    void update(float sigma_s, float sigma_r);

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *setupAux(ImageGLVec imgIn, ImageGL *imgOut)
    {
        imgOut = allocateOutputMemory(imgIn, imgOut, false);

        if(imgIn.size() == 1) {
            param.clear();
            param.push_back(imgIn[0]);
        }

        return imgOut;
    }
};

PIC_INLINE FilterGLBilateral2DF::FilterGLBilateral2DF(): FilterGL()
{
}

PIC_INLINE FilterGLBilateral2DF::~FilterGLBilateral2DF()
{
    release();
}

PIC_INLINE FilterGLBilateral2DF::FilterGLBilateral2DF(float sigma_s,
        float sigma_r): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    FragmentShader();
    initShaders();
}

PIC_INLINE void FilterGLBilateral2DF::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
                          uniform sampler2D u_tex;
                          uniform float     sigmas2;
                          uniform float     sigmar2;
                          uniform int       halfKernelSize;
                          out     vec4      f_color;

    void main(void) {
        vec3  color = vec3(0.0, 0.0, 0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec3 tmpCol;

        vec3 colRef = texelFetch(u_tex, coordsFrag, 0).xyz;

        float weight = 0.0;

        for(int i = -halfKernelSize; i <= halfKernelSize; i++) {
            for(int j = -halfKernelSize; j <= halfKernelSize; j++) {
                //Coordinates
                ivec2 coords = ivec2(i, j);
                //Texture fetch
                tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0).xyz;
                vec3 tmpCol2 = tmpCol - colRef;
                float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
                float tmp = exp(-dstR / sigmar2 - float(coords.x * coords.x + coords.y *
                                                        coords.y) / sigmas2);
                color.xyz += tmpCol * tmp;
                weight += tmp;
            }
        }

        color = weight > 0.0 ? color / weight : colRef;
        f_color = vec4(color.xyz, 1.0);
    }
                      );
}

PIC_INLINE void FilterGLBilateral2DF::initShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLBilateral2DF");

    update(-1.0f, -1.0f);
}

PIC_INLINE void FilterGLBilateral2DF::update(float sigma_s, float sigma_r)
{
    if(sigma_s > 0.0f) {
        this->sigma_s = sigma_s;
    }

    if(sigma_r > 0.0f) {
        this->sigma_r = sigma_r;
    }

    float sigmas2 = 2.0f * this->sigma_s * this->sigma_s;
    float sigmar2 = 2.0f * this->sigma_r * this->sigma_r;

    //Precomputation of the Gaussian Kernel
    int halfKernelSize = PrecomputedGaussian::getKernelSize(this->sigma_s) >> 1;

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1f("sigmas2", sigmas2);
    technique.setUniform1f("sigmar2", sigmar2);
    technique.setUniform1i("halfKernelSize", halfKernelSize);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DF_HPP */

