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

#include "gl/filtering/filter.hpp"

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
     * @brief InitShaders
     */
    void InitShaders();

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

    /**
     * @brief Update
     * @param sigma_s
     * @param sigma_r
     */
    void Update(float sigma_s, float sigma_r);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static ImageGL *Execute(std::string nameIn,
                               std::string nameOut,
                               float sigma_s, float sigma_r)
    {
        //Load the image
        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        //Filtering
        FilterGLBilateral2DF filter(sigma_s, sigma_r);

        long t0 = timeGetTime();
        ImageGL *imgRet = filter.Process(SingleGL(&imgIn), NULL);
        long t1 = timeGetTime();
        printf("Full Bilateral Filter time: %ld\n", t1 - t0);

        imgRet->loadToMemory();
        imgRet->Write(nameOut);
        return imgRet;
    }
};

FilterGLBilateral2DF::FilterGLBilateral2DF(): FilterGL()
{
}

FilterGLBilateral2DF::FilterGLBilateral2DF(float sigma_s,
        float sigma_r): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    FragmentShader();
    InitShaders();
}

void FilterGLBilateral2DF::FragmentShader()
{
    fragment_source = GLW_STRINGFY
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

void FilterGLBilateral2DF::InitShaders()
{
    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update(-1.0f, -1.0f);
}

void FilterGLBilateral2DF::Update(float sigma_s, float sigma_r)
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
    int halfKernelSize = PrecomputedGaussian::KernelSize(this->sigma_s) >> 1;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("sigmas2", sigmas2);
    filteringProgram.uniform("sigmar2", sigmar2);
    filteringProgram.uniform("halfKernelSize", halfKernelSize);
    glw::bind_program(0);
}

ImageGL *FilterGLBilateral2DF::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(1, w, h, imgIn[0]->channels, IMG_GPU, GL_TEXTURE_2D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, 1, false, imgOut->getTexture());

    ImageGL *edge, *base;

    if(imgIn.size() == 2) {
        //Joint/Cross Bilateral Filtering
        base = imgIn[0];
        edge = imgIn[1];
    } else {
        base = imgIn[0];
        edge = imgIn[0];
    }

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, base->getTexture());

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DF_HPP */

