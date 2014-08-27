/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_2DSE_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_2DSE_HPP


#include "gl/filtering/filter.hpp"
#include "util/file_lister.hpp"
#include "gl/point_samplers/sampler_random_m.hpp"

namespace pic {

class FilterGLBilateral2DSE: public FilterGL
{
protected:
    float sigma_s, sigma_p, sigma_n, sigma_a;
    MRSamplersGL<2> *ms;

    //Random numbers tile
    ImageRAWGL *imageRand;

    void InitShaders();
    void FragmentShader();

public:
    /**
     * @brief FilterGLBilateral2DSE
     * @param sigma_s
     * @param sigma_p
     * @param sigma_n
     * @param sigma_a
     */
    FilterGLBilateral2DSE(float sigma_s, float sigma_p, float sigma_n, float sigma_a);

    ~FilterGLBilateral2DSE();

    /**
     * @brief Update
     * @param sigma_s
     * @param sigma_p
     * @param sigma_n
     * @param sigma_a
     */
    void Update(float sigma_s, float sigma_p, float sigma_n, float sigma_a);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    /**
     * @brief main
     * @param argc
     * @param argv
     * @return
     */
    static int main(int argc, char* argv[])
    {
        if(argc < 6) {
            printf("Usage: name_input sigma_s sigma_pos sigma_nor sigma_alb\n");
            return 0;
        }

        Execute(argv[1], float(atof(argv[2])), float(atof(argv[3])), float(atof(argv[4])), float(atof(argv[5])));

        return 0;
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param sigma_s
     * @param sigma_p
     * @param sigma_n
     * @param sigma_a
     * @param testing
     * @return
     */
    static ImageRAWGL *Execute( std::string nameIn, 
                                float sigma_s, float sigma_p, float sigma_n, float sigma_a, int testing = 1)
    {

        std::string name = removeExtension(nameIn);
        std::string ext = getExtension(nameIn);

        std::string nameOut = name + "_flt." + ext; 

        std::string namePos = name +"_pos." + ext;
        std::string nameNor = name +"_nor." + ext;
        std::string nameAlb = name +"_alb." + ext;


        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        ImageRAWGL imgPos(namePos);
        imgPos.generateTextureGL(false, GL_TEXTURE_2D);

        ImageRAWGL imgNor(nameNor);
        imgNor.generateTextureGL(false, GL_TEXTURE_2D);

        ImageRAWGL imgAlb(nameAlb);
        imgAlb.generateTextureGL(false, GL_TEXTURE_2D);

        ImageRAWGLVec vec;
        vec.push_back(&imgIn);
        vec.push_back(&imgPos);
        vec.push_back(&imgNor);
        vec.push_back(&imgAlb);

        FilterGLBilateral2DSE *filter = new FilterGLBilateral2DSE(sigma_s, sigma_p,
                    sigma_n, sigma_a);

        ImageRAWGL *imgOut = new ImageRAWGL(1, imgIn.width, imgIn.height, imgIn.channels,
                                            IMG_GPU_CPU, GL_TEXTURE_2D);

        GLuint testTQ1;

        if(testing > 1) {
            filter->Process(vec, imgOut);

            testTQ1 = glBeginTimeQuery();

            for(int i = 0; i < testing; i++) {
                filter->Process(vec, imgOut);
            }
        } else {
            testTQ1 = glBeginTimeQuery();
            filter->Process(vec, imgOut);
        }

        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        double ms = double(timeVal) / (double(testing) * 1000000.0);
        printf("Cross Bilateral Filter with G-buffer on GPU time: %f ms\n", ms);

        //Read from the GPU
        imgOut->loadToMemory();
        imgOut->Write(nameOut);

        return imgOut;
    }

};

FilterGLBilateral2DSE::FilterGLBilateral2DSE(float sigma_s, float sigma_p, float sigma_n, float sigma_a): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_p = sigma_p;
    this->sigma_n = sigma_n;
    this->sigma_a = sigma_a;

    //Precomputation of the Gaussian Kernel
    int kernelSize = PrecomputedGaussian::KernelSize(sigma_s);//,sigma_r);
    int halfKernelSize = kernelSize >> 1;

    //Random numbers
    int nRand = 32;
    int nSamplers;

    imageRand = new ImageRAWGL(1, 128, 128, 1, IMG_CPU, GL_TEXTURE_2D);
    imageRand->SetRand();
    imageRand->Mul(float(nRand - 1));
    imageRand->generateTexture2DU32GL();
    nSamplers = nRand;

    //Poisson samples
#ifdef PIC_DEBUG
    printf("Window: %d\n", halfKernelSize);
#endif

    ms = new MRSamplersGL<2>(ST_BRIDSON, halfKernelSize, 4*halfKernelSize, 1,
                             nSamplers);
    ms->generateTexture();

    FragmentShader();
    InitShaders();
}

FilterGLBilateral2DSE::~FilterGLBilateral2DSE()
{
    delete imageRand;
    delete ms;

    //free shader etc...
}

void FilterGLBilateral2DSE::FragmentShader()
{
    fragment_source = GLW_STRINGFY(
    uniform sampler2D	u_tex;
    uniform sampler2D	u_edge_pos;
    uniform sampler2D	u_edge_nor;
    uniform sampler2D	u_edge_alb;
    uniform isampler2D	u_poisson;
    uniform sampler2D	u_rand;
    uniform int			TOKEN_BANANA;
    uniform float		sigma_s2;
    uniform float		sigma_pos2;
    uniform float		sigma_nor2;
    uniform float		sigma_alb2;
    out     vec4		f_color;

    void main(void) {
        vec3  color = vec3(0.0, 0.0, 0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);

        vec3 colRef      = texelFetch(u_tex, coordsFrag, 0).xyz;
        vec3 edge_posRef = texelFetch(u_edge_pos, coordsFrag, 0).xyz;
        vec3 edge_norRef = texelFetch(u_edge_nor, coordsFrag, 0).xyz;
        vec3 edge_albRef = texelFetch(u_edge_alb, coordsFrag, 0).xyz;

        float weight = 0.0;
        float shifter = texture2D(u_rand, gl_FragCoord.xy, 0).x;

        for(int i = 0; i < TOKEN_BANANA; i++) {
            //Coordinates
            ivec3 coords = texelFetch(u_poisson, ivec2(i, shifter), 0).xyz;
            //pos difference
            vec3 tmpEdge = texelFetch(u_edge_pos, coordsFrag.xy + coords.xy, 0).xyz;
            vec3 tmpEdge2 = tmpEdge - edge_posRef;
            float dstPos = dot(tmpEdge2.xyz, tmpEdge2.xyz);
            //nor difference
            tmpEdge = texelFetch(u_edge_nor, coordsFrag.xy + coords.xy, 0).xyz;
            float dstNor = 1.0 - dot(tmpEdge, edge_norRef);
            //alb difference
            tmpEdge = texelFetch(u_edge_alb, coordsFrag.xy + coords.xy, 0).xyz;
            tmpEdge2 = tmpEdge - edge_albRef;
            float dstAlb = dot(tmpEdge2.xyz, tmpEdge2.xyz);

            float tmp = dstPos / sigma_pos2 + dstAlb / sigma_alb2 + float(coords.z) / sigma_s2 + dstNor / sigma_nor2 ;
            tmp = exp(-tmp);
            //Texture Fetch
            vec3 tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0).xyz;
            color.xyz += tmpCol * tmp;
            weight += tmp;
        }

        f_color = vec4(weight > 0.0 ? color / weight : colRef, 1.0);
    }
                                        );

}

void FilterGLBilateral2DSE::InitShaders()
{
#ifdef PIC_DEBUG
    printf("Number of samples: %d\n", ms->nSamples);
#endif

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update(-1.0f, -1.0f, -1.0f, -1.0f);
}

void FilterGLBilateral2DSE::Update(float sigma_s, float sigma_p, float sigma_n, float sigma_a)
{

    bool flag = false;

    if(sigma_s > 0.0f) {
        flag = (this->sigma_s == sigma_s);
        this->sigma_s = sigma_s;
    }

    if(sigma_p > 0.0f) {
        flag = flag || (this->sigma_p == sigma_p);
        this->sigma_p = sigma_p;
    }

    if(sigma_n > 0.0f) {
        flag = flag || (this->sigma_n == sigma_n);
        this->sigma_n = sigma_n;
    }

    if(sigma_a > 0.0f) {
        flag = flag || (this->sigma_a == sigma_a);
        this->sigma_a = sigma_a;
    }

    int kernelSize = PrecomputedGaussian::KernelSize(this->sigma_s);
    int halfKernelSize = kernelSize >> 1;

    if(flag) {
        ms->updateGL(halfKernelSize, halfKernelSize);
    }
    
    //shader update
    float sigmas2 = 2.0f * this->sigma_s * this->sigma_s;
    float sigmap2 = 2.0f * this->sigma_p * this->sigma_p;
    float sigman2 = 2.0f * this->sigma_n * this->sigma_n;
    float sigmaa2 = 2.0f * this->sigma_a * this->sigma_a;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",       0);
    filteringProgram.uniform("u_poisson",   1);
    filteringProgram.uniform("u_rand",	    2);

    filteringProgram.uniform("u_edge_pos",  3);
    filteringProgram.uniform("u_edge_nor",  4);
    filteringProgram.uniform("u_edge_alb",  5);

    filteringProgram.uniform("kernelSize",      kernelSize);
    filteringProgram.uniform("kernelSizef",     float(kernelSize));
    filteringProgram.uniform("sigma_s2",        sigmas2);
    filteringProgram.uniform("sigma_pos2",	    sigmap2);
    filteringProgram.uniform("sigma_nor2",	    sigman2);
    filteringProgram.uniform("sigma_alb2",	    sigmaa2);
    filteringProgram.uniform("TOKEN_BANANA",    ms->nSamples / 2);
    glw::bind_program(0);
}

//Processing
ImageRAWGL *FilterGLBilateral2DSE::Process(ImageRAWGLVec imgIn,
        ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    //TODO: check if other have height and frames swapped
    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(imgIn[0]->frames, w, h, imgIn[0]->channels, IMG_GPU, GL_TEXTURE_2D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, imgIn[0]->frames, false, imgOut->getTexture());

    ImageRAWGL *base     = imgIn[0];
    ImageRAWGL *edge_pos = imgIn[1];
    ImageRAWGL *edge_nor = imgIn[2];
    ImageRAWGL *edge_alb = imgIn[3];

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, edge_alb->getTexture());

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, edge_nor->getTexture());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, edge_pos->getTexture());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, imageRand->getTexture());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ms->getTexture());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, base->getTexture());

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DS_HPP */

