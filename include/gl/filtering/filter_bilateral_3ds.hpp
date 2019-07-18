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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_3DS_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_3DS_HPP

#include "../../base.hpp"
#include "../../util/vec.hpp"
#include "../../util/std_util.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLBilateral3DS class
 */
class FilterGLBilateral3DS: public FilterGL
{
protected:
    float sigma_s, sigma_r, sigma_t;
    float sigmas2, sigmar2, sigmat2;
    int	  frame, kernelSizeTime;
    MRSamplersGL<3> *ms;

    //Random numbers tile
    ImageGL *imageRand;

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
     * @brief FilterGLBilateral3DS
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    FilterGLBilateral3DS(float sigma_s, float sigma_r, float sigma_t);

    ~FilterGLBilateral3DS();

    /**
     * @brief update
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    void update(float sigma_s, float sigma_r, float sigma_t);

    /**
     * @brief setUniform
     */
    void setUniform();

    /**
     * @brief setFrame
     * @param frame
     */
    void setFrame(int frame)
    {
        this->frame = frame;
    }

    /**
     * @brief nextFrame
     */
    void nextFrame()
    {
        frame++;
    }

    /**
     * @brief getFrame
     * @return
     */
    int  getFrame()
    {
        return frame;
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

PIC_INLINE FilterGLBilateral3DS::FilterGLBilateral3DS(float sigma_s, float sigma_r,
        float sigma_t): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;
    this->sigma_t = sigma_t;

    int nRand = 32;
    imageRand = new ImageGL(1, 256, 256, 1, IMG_CPU, GL_TEXTURE_2D);
    imageRand->setRand(1);
    imageRand->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);
    *imageRand *= float(nRand - 1);

    //Precomputation of the Gaussian Kernel
    int kernelSizeSpace = PrecomputedGaussian::getKernelSize(sigma_s);
    kernelSizeTime  = PrecomputedGaussian::getKernelSize(sigma_t);

    int kernelSize = MAX(kernelSizeSpace, kernelSizeTime);
    int halfKernelSize = kernelSize >> 1;
    int halfKernelSizeTime = kernelSizeTime >> 1;

    frame = halfKernelSizeTime;

    //Poisson samples
    Vec3i window = Vec3i(halfKernelSize, halfKernelSize, halfKernelSizeTime);
    ms = new MRSamplersGL<3>(ST_BRIDSON, window, 2 * kernelSize, 1, nRand);
    ms->generateTexture();

#ifdef PIC_DEBUG
    printf("Window Space: %d Window Time: %d\n", halfKernelSize,
           halfKernelSizeTime);
    printf("Nsamples: %d\n", ms->nSamples / 3);
#endif

    FragmentShader();
    initShaders();
}

PIC_INLINE FilterGLBilateral3DS::~FilterGLBilateral3DS()
{
    release();
}

PIC_INLINE void FilterGLBilateral3DS::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
                          uniform sampler2DArray  u_tex;
//		uniform sampler3D		u_tex;
                          uniform isampler2D		u_poisson;
                          uniform sampler2D		u_rand;
                          uniform int			TOKEN_BANANA;
                          uniform int			frame;
                          uniform float			sigmas2;
                          uniform float			sigmar2;
                          uniform float			sigmat2;
                          out     vec4			f_color;

    void main(void) {
        vec3 tmpCol;
        vec3  color = vec3(0.0, 0.0, 0.0);
        ivec3 texSize = textureSize(u_tex, 0);
        ivec3 coordsFrag = ivec3(gl_FragCoord.xy, frame % texSize.z);

        vec3 colRef = texelFetch(u_tex, coordsFrag, 0).xyz;
        float weight = 0.0;

        for(int i = 0; i < TOKEN_BANANA; i++) {
            //Coordinates
            float shifter = texelFetch(u_rand, coordsFrag.xy % 128, 0).x;
            ivec4 coords = texelFetch(u_poisson, ivec2(i, shifter), 0).xyzw;
            //Texture fetch
            ivec3 tmpCoords;
            tmpCoords.xy = coords.xy + coordsFrag.xy;
            tmpCoords.z = (frame + coords.z) % texSize.z;

            tmpCol = texelFetch(u_tex, tmpCoords, 0).xyz;
            vec3 tmpCol2 = tmpCol - colRef;
            float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
            float tmp = exp(-dstR / sigmar2 - float(coords.w) / sigmas2 - float(
                                coords.z * coords.z) / sigmat2);
            color.xyz += tmpCol * tmp;
            weight += tmp;
        }

        f_color = vec4(weight > 0.0 ? (color / weight) : vec3(1.0), 1.0);
    }
                      );
}

PIC_INLINE void FilterGLBilateral3DS::initShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLBilateral3DS");

    sigmas2 = 2.0f * sigma_s * sigma_s;
    sigmat2 = 2.0f * sigma_t * sigma_t;
    sigmar2 = 2.0f * sigma_r * sigma_r;
    setUniform();
}

PIC_INLINE void FilterGLBilateral3DS::update(float sigma_s, float sigma_r, float sigma_t)
{

    bool flag = false;

    if(sigma_s > 0.0f) {
        flag = (this->sigma_s == sigma_s);
        this->sigma_s = sigma_s;
    }

    if(sigma_r > 0.0f) {
        flag = flag || (this->sigma_r == sigma_r);
        this->sigma_r = sigma_r;
    }

    if(sigma_t > 0.0f) {
        flag = flag || (this->sigma_t == sigma_t);
        this->sigma_t = sigma_t;
    }

    if(!flag) {
    }

    int kernelSize = PrecomputedGaussian::getKernelSize(this->sigma_s);
    int halfKernelSize = kernelSize >> 1;

    Vec3i window = Vec3i(halfKernelSize, halfKernelSize, halfKernelSize);
    ms->updateGL(window, halfKernelSize);

    //shader update
    sigmas2 = 2.0f * this->sigma_s * this->sigma_s;
    sigmat2 = 2.0f * this->sigma_t *this->sigma_t;
    sigmar2 = 2.0f * this->sigma_r * this->sigma_r;

    setUniform();
}

PIC_INLINE void FilterGLBilateral3DS::setUniform()
{
    technique.bind();
    technique.setUniform1i("u_tex",      0);
    technique.setUniform1i("u_poisson",  1);
    technique.setUniform1i("u_rand",	 2);

    technique.setUniform1f("sigmas2",  sigmas2);
    technique.setUniform1f("sigmat2",  sigmat2);
    technique.setUniform1f("sigmar2",  sigmar2);
    technique.setUniform1i("TOKEN_BANANA",  ms->nSamples / 3);
    technique.setUniform1i("frame",  frame);

    technique.unbind();
}

PIC_INLINE ImageGL *FilterGLBilateral3DS::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(1, w, h, imgIn[0]->channels, IMG_GPU, imgIn[0]->getTarget());
    }

    if(fbo == NULL) {
        fbo = new Fbo();
        fbo->create(w, h, 1, false, imgOut->getTexture());
    }

    ImageGL *base = imgIn[0];

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    technique.bind();

    //Textures
    glActiveTexture(GL_TEXTURE2);
    imageRand->bindTexture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ms->getTexture());

    glActiveTexture(GL_TEXTURE0);
    base->bindTexture();

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    technique.unbind();

    //Textures
    glActiveTexture(GL_TEXTURE2);
    imageRand->unBindTexture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    base->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_3DS_HPP */

