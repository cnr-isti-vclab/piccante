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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_2DAS_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_2DAS_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_sampling_map.hpp"
#include "gl/point_samplers/sampler_random_m.hpp"

namespace pic {

class FilterGLBilateral2DAS: public FilterGL
{
protected:
    float sigma_s, sigma_r;
    MRSamplersGL<2> *ms;

    //Random numbers tile
    ImageGL *imageRand;

    //Sampling map
    FilterGLSamplingMap *fGLsm;

    ImageGL			*imgTmp;

    void InitShaders();
    void FragmentShader();

public:
    //Basic constructors
    FilterGLBilateral2DAS();
    ~FilterGLBilateral2DAS();
    //Init constructors
    FilterGLBilateral2DAS(float sigma_s, float sigma_r);

    //Change parameters
    void Update(float sigma_s, float sigma_r);

    //Processing
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);

    //Execute
    //Execute
    static ImageGL *Execute(ImageGL *imgIn, float sigma_s, float sigma_r)
    {
        FilterGLBilateral2DAS *filter = new FilterGLBilateral2DAS(sigma_s, sigma_r);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageGL *imgOut = filter->Process(SingleGL(imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);

        printf("Bilateral 2DS Filter on GPU time: %f ms\n",
               double(timeVal) / 1000000.0);

        delete filter;
        return imgOut;
    }

    static ImageGL *Execute(std::string nameFile, std::string nameOut,
                               float sigma_s, float sigma_r, int testing)
    {
        ImageGL imgIn(nameFile);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLBilateral2DAS filter(sigma_s, sigma_r);

        ImageGL *imgRet = new ImageGL(1, imgIn.width, imgIn.height, 3, IMG_GPU, GL_TEXTURE_2D);
        GLuint testTQ1;

        if(testing > 1) {
            filter.Process(SingleGL(&imgIn), imgRet);

            testTQ1 = glBeginTimeQuery();

            for(int i = 0; i < testing; i++) {
                filter.Process(SingleGL(&imgIn), imgRet);
            }
        } else {
            testTQ1 = glBeginTimeQuery();
            filter.Process(SingleGL(&imgIn), imgRet);
        }

     //   GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
   //     double ms = double(timeVal) / (double(testing) * 1000000.0);
     //   printf("Adaptive Stochastic Bilateral Filter on GPU time: %f ms\n", ms);

        /*
        std::string nameTime = FileLister::FileNumber(GenBilString("AS", sigma_s,
                               sigma_r), "txt");

        FILE *file = fopen(nameTime.c_str(), "w");

        if(file != NULL) {
            fprintf(file, "%f", ms);
            fclose(file);
        }*/

        //Read from the GPU
        imgRet->loadToMemory();
        imgRet->Write(nameOut);
        return imgRet;
    }
};

//Basic constructor
FilterGLBilateral2DAS::FilterGLBilateral2DAS(): FilterGL()
{
    ms = NULL;
    imageRand = NULL;
    fGLsm = NULL;
    imgTmp = NULL;
}

FilterGLBilateral2DAS::~FilterGLBilateral2DAS()
{
    delete ms;
    delete imageRand;
    delete fGLsm;
    delete imgTmp;
}

//Init constructors
FilterGLBilateral2DAS::FilterGLBilateral2DAS(float sigma_s,
        float sigma_r): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    fGLsm = new FilterGLSamplingMap(sigma_s);
    imgTmp = NULL;

    int nRand = 32;

    Image tmp_image_rand(1, 128, 128, 1);
    tmp_image_rand.SetRand();
    tmp_image_rand *= float(nRand - 1);

    imageRand = new ImageGL(&tmp_image_rand, true);
    imageRand->generateTextureGL(GL_TEXTURE_2D, GL_INT);

    //Precomputation of the Gaussian Kernel
    int kernelSize = PrecomputedGaussian::KernelSize(sigma_s);
    int halfKernelSize = kernelSize >> 1;

    //Poisson samples
#ifdef PIC_DEBUG
    printf("Window: %d\n", halfKernelSize);
#endif

    ms = new MRSamplersGL<2>(ST_BRIDSON, halfKernelSize, halfKernelSize, 3, nRand);

    ms->generateTexture();
    ms->generateLevelsRTexture();

    FragmentShader();
    InitShaders();
}

void FilterGLBilateral2DAS::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D	u_tex;
                          uniform isampler2D	u_poisson;
                          uniform sampler2D	u_sample;
                          uniform isampler2D	u_rand;
                          uniform isampler2D	u_levelsR;
                          uniform float		sigmas2;
                          uniform float		sigmar2;
                          uniform	int			levelsR_Size;
                          out     vec4		f_color;

                          //Calculate the number of samples
    int CalculateSamples(int shifter, ivec2 tSize) {
        //Fetch to the sampling map
        float levelVal = 2.0 * texture(u_sample, gl_FragCoord.xy / tSize.xy).x;
        levelVal = clamp(1.0f - levelVal, 0.0, 1.0) * float(levelsR_Size);

        int levelInt = int(floor(levelVal));

        int nSamples = texelFetch(u_levelsR, ivec2(levelInt, shifter), 0).x;

        if(levelInt < (levelsR_Size - 1)) {
            float tmp = (levelVal - float(levelInt));

            if(tmp > 0.0) {
                int nSamples1 = texelFetch(u_levelsR, ivec2(levelInt + 1, shifter), 0).x;

                nSamples += int(float(nSamples1 - nSamples) * tmp);
            }
        }

        return nSamples / 2;
    }

    void main(void) {
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);

        //Coordinates
        int shifter = texelFetch(u_rand, coordsFrag.xy % 128, 0).x;

        //Calculating the number of samples
        ivec2 tSize =  textureSize(u_tex, 0);

        int nSamples = CalculateSamples(shifter, tSize);

        //Filtering
        vec3 tmpCol;
        vec3 colRef = texelFetch(u_tex, coordsFrag, 0).xyz;
        vec3  color = vec3(0.0);
        float weight = 0.0;

        for(int i = 0; i < nSamples; i++) {
            ivec4 coords = texelFetch(u_poisson, ivec2(i, shifter), 0);

            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0).xyz;
            vec3 tmpCol2 = tmpCol - colRef;
            float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
            float tmp = exp(-dstR / sigmar2 - float(coords.z) / sigmas2);
            color += tmpCol * tmp;
            weight += tmp;
        }

        color = weight > 0.0 ? color / weight : colRef;
        f_color = vec4(color, 1.0);
    }
                      );
}

void FilterGLBilateral2DAS::InitShaders()
{
#ifdef PIC_DEBUG
    printf("Number of samples: %d\n", ms->nSamples / 2);
#endif

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
    glw::bind_program(0);

    Update(-1.0f, -1.0f);
}

//Change parameters
void FilterGLBilateral2DAS::Update(float sigma_s, float sigma_r)
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

    if(!flag) {
    }

    //shader update
    int kernelSize = PrecomputedGaussian::KernelSize(this->sigma_s);
    int halfKernelSize = kernelSize >> 1;

    ms->updateGL(halfKernelSize, halfKernelSize);

    //shader update
    float sigmas2 = 2.0f * this->sigma_s * this->sigma_s;
    float sigmar2 = 2.0f * this->sigma_r * this->sigma_r;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("u_poisson",  1);
    filteringProgram.uniform("u_rand",	 2);
    filteringProgram.uniform("u_sample",	 3);
    filteringProgram.uniform("u_levelsR",	 4);
    filteringProgram.uniform("sigmas2",  sigmas2);
    filteringProgram.uniform("sigmar2",  sigmar2);
    filteringProgram.uniform("levelsR_Size", ms->nLevels);
    glw::bind_program(0);
}

//Processing
ImageGL *FilterGLBilateral2DAS::Process(ImageGLVec imgIn,
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
        fbo->create(w, h, 1, false, imgOut->getTexture());
    }

    if(imgTmp == NULL) {
        float scale = fGLsm->getScale();
        imgTmp = new ImageGL(    1, 
                                    int(imgIn[0]->widthf  * scale),
                                    int(imgIn[0]->heightf * scale),
                                    1, IMG_GPU, GL_TEXTURE_2D);
    }

    ImageGL *edge, *base;

    if(imgIn.size() == 2) {
        //Joint/Cross Bilateral Filtering
        base = imgIn[0];
        edge = imgIn[1];
    } else {
        base = imgIn[0];
        edge = imgIn[0];
    }

    //Calculating the sampling map
    fGLsm->Process(imgIn, imgTmp);

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ms->getLevelsRTexture());

    glActiveTexture(GL_TEXTURE3);
    imgTmp->bindTexture();

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
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE3);
    imgTmp->unBindTexture();

    glActiveTexture(GL_TEXTURE2);
    imageRand->unBindTexture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    base->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DAS_HPP */

