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

#include "gl/filtering/filter.hpp"

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
     * @brief InitShaders
     */
    void InitShaders();

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

    /**
     * @brief Update
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    void Update(float sigma_s, float sigma_r, float sigma_t);

    /**
     * @brief UpdateUniform
     */
    void UpdateUniform();

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

FilterGLBilateral3DS::FilterGLBilateral3DS(float sigma_s, float sigma_r,
        float sigma_t): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;
    this->sigma_t = sigma_t;

    int nRand = 32;
    imageRand = new ImageGL(1, 256, 256, 1, IMG_CPU, GL_TEXTURE_2D);
    imageRand->SetRand();
    imageRand->generateTextureGL(false, GL_TEXTURE_2D);
    *imageRand *= float(nRand - 1);

    //Precomputation of the Gaussian Kernel
    int kernelSizeSpace = PrecomputedGaussian::KernelSize(sigma_s);
    kernelSizeTime  = PrecomputedGaussian::KernelSize(sigma_t);

    int kernelSize = MAX(kernelSizeSpace, kernelSizeTime);
    int halfKernelSize = kernelSize >> 1;
    int halfKernelSizeTime = kernelSizeTime >> 1;

    frame = halfKernelSizeTime;

    //Poisson samples
    Vec<3, int> window = Vec<3, int>(halfKernelSize, halfKernelSize,
                                     halfKernelSizeTime);
    ms = new MRSamplersGL<3>(ST_BRIDSON, window, 2 * kernelSize, 1, nRand);
    ms->generateTexture();

#ifdef PIC_DEBUG
    printf("Window Space: %d Window Time: %d\n", halfKernelSize,
           halfKernelSizeTime);
    printf("Nsamples: %d\n", ms->nSamples / 3);
#endif

    FragmentShader();
    InitShaders();
}

void FilterGLBilateral3DS::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2DArray  u_tex;
//		uniform sampler3D		u_tex;
                          uniform isampler2D		u_poisson;
                          uniform sampler2D		u_rand;
                          uniform int				TOKEN_BANANA;
                          uniform int				frame;
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

void FilterGLBilateral3DS::InitShaders()
{
    //printf("Number of samples: %d\n",ms->nSamples);

    std::string prefix;
    prefix += glw::version("150");
    prefix += glw::ext_require("GL_EXT_gpu_shader4");
    filteringProgram.setup(prefix, vertex_source, fragment_source);
#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();

    sigmas2 = 2.0f * sigma_s * sigma_s;
    sigmat2 = 2.0f * sigma_t * sigma_t;
    sigmar2 = 2.0f * sigma_r * sigma_r;
    UpdateUniform();
}

void FilterGLBilateral3DS::Update(float sigma_s, float sigma_r, float sigma_t)
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

    int kernelSize = PrecomputedGaussian::KernelSize(this->sigma_s);
    int halfKernelSize = kernelSize >> 1;

    ms->updateGL(halfKernelSize, halfKernelSize);

    //shader update
    sigmas2 = 2.0f * this->sigma_s * this->sigma_s;
    sigmat2 = 2.0f * this->sigma_t *this->sigma_t;
    sigmar2 = 2.0f * this->sigma_r * this->sigma_r;
    UpdateUniform();
}

void FilterGLBilateral3DS::UpdateUniform()
{
    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("u_poisson",  1);
    filteringProgram.uniform("u_rand",	 2);

    filteringProgram.uniform("sigmas2",  sigmas2);
    filteringProgram.uniform("sigmat2",  sigmat2);
    filteringProgram.uniform("sigmar2",  sigmar2);
    filteringProgram.uniform("TOKEN_BANANA",  ms->nSamples / 3);
    filteringProgram.uniform("frame",  frame);

    glw::bind_program(0);
}

ImageGL *FilterGLBilateral3DS::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(w, h, 1, imgIn[0]->channels, IMG_GPU, imgIn[0]->getTarget());
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
    glw::bind_program(filteringProgram);

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
    glw::bind_program(0);

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

