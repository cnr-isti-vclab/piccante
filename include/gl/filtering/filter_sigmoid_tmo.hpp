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

#ifndef PIC_GL_FILTERING_FILTER_SIGMOID_TMO_HPP
#define PIC_GL_FILTERING_FILTER_SIGMOID_TMO_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_luminance.hpp"

namespace pic {

class FilterGLSigmoidTMO: public FilterGL
{
protected:
    float alpha, epsilon;
    bool  bGammaCorrection, bLocal;

    void InitShaders();
    void FragmentShader();

public:
    //Basic constructors
    FilterGLSigmoidTMO();
    FilterGLSigmoidTMO(float alpha, bool bLocal, bool bGammaCorrection);

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    void Update(float alpha);

    static ImageRAWGL *ExecuteReinhardLocal(std::string nameIn, std::string nameOut,
                                            FilterGL *filter)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false);

        FilterGLLuminance lum;

        ImageRAWGL *L = lum.Process(SingleGL(&imgIn), NULL);
        L->loadToMemory();
        L->Write("lum.pfm");

        GLuint testTQ1 = glBeginTimeQuery();
        ImageRAWGL *adapt = filter->Process(SingleGL(L), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Bilateral 2DG Filter on GPU time: %f ms\n",
               double(timeVal) / 1000000.0);

        adapt->loadToMemory();
        adapt->Write("adapt.pfm");

        float Lav = imgIn.getLogMeanVal()[0];

        FilterGLSigmoidTMO *tmo = new FilterGLSigmoidTMO(0.18f / Lav, true, true);

        testTQ1 = glBeginTimeQuery();
        ImageRAWGL *imgOut = tmo->Process(DoubleGL(&imgIn, adapt), NULL);
        timeVal = glEndTimeQuery(testTQ1);
        printf("Sigmoid Local Filter on GPU time: %f ms\n",
               double(timeVal) / 1000000.0);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    };

    static ImageRAWGL *ExecuteReinhardGlobal(std::string nameIn,
            std::string nameOut)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false);

        float Lav = imgIn.getLogMeanVal()[0];

        FilterGLSigmoidTMO *filter = new FilterGLSigmoidTMO(0.18f / Lav, false, true);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageRAWGL *imgOut = filter->Process(SingleGL(&imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Sigmoid Filter on GPU time: %f ms\n", double(timeVal) / 1000000.0);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Basic constructor
FilterGLSigmoidTMO::FilterGLSigmoidTMO(): FilterGL()
{
    alpha = 0.15f;
    bGammaCorrection = false;
    bLocal = false;
    epsilon = 1.0f;

    InitShaders();
}

FilterGLSigmoidTMO::FilterGLSigmoidTMO(float alpha, bool bLocal,
                                       bool bGammaCorrection): FilterGL()
{
    this->alpha = alpha;
    this->bLocal = bLocal;
    this->bGammaCorrection = bGammaCorrection;
    epsilon = 1.0f;

    InitShaders();
}

void FilterGLSigmoidTMO::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D	u_tex; \n
                          uniform sampler2D	u_tex_adapt; \n
                          uniform float		alpha; \n
                          uniform float		epsilon; \n
                          out     vec4		f_color; \n
                          const	vec3		LUM_XYZ =	vec3(0.213, 0.715, 0.072); \n

    void main(void) {
        \n
        ivec2  coords = ivec2(gl_FragCoord.xy);
        \n
        vec3   color  = texelFetch(u_tex, coords, 0).xyz;
        \n
        __LOCAL__1__ \n
        float Lw =	dot(color,		LUM_XYZ);
        \n
        __LOCAL__2__ \n
        color = (color.xyz * Ld) / Lw;
        \n
        __GAMMA__CORRECTION__ \n
        f_color = vec4(color, 1.0);
        \n
    }\n
                      );

    size_t processing_found1 = fragment_source.find("__LOCAL__1__");

    if(bLocal) {
        fragment_source.replace(processing_found1, 12,
                                " float Lwa = texelFetch(u_tex_adapt,	coords,0).x; ");

        size_t processing_found2 = fragment_source.find("__LOCAL__2__");
        fragment_source.replace(processing_found2, 12,
                                " float Ld  = (Lw*alpha)/(Lwa*alpha+epsilon); ");
    } else {
        fragment_source.replace(processing_found1, 12, " ");

        size_t processing_found2 = fragment_source.find("__LOCAL__2__");
        fragment_source.replace(processing_found2, 12,
                                " float Lscale = Lw*alpha;\n float Ld = Lscale/(Lscale+epsilon); ");
    }

    fragment_source = GammaCorrection(fragment_source, bGammaCorrection);
}

void FilterGLSigmoidTMO::InitShaders()
{
    FragmentShader();

    std::string prefix;

    prefix += glw::version("330");
//	prefix += glw::ext_require("GL_EXT_gpu_shader4");
    filteringProgram.setup(prefix, vertex_source, fragment_source);
#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif
    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position",		0);
    filteringProgram.fragment_target("f_color",			0);
    filteringProgram.relink();

    Update(alpha);
}

void FilterGLSigmoidTMO::Update(float alpha)
{
    if(alpha > 0.0f) {
        this->alpha = alpha;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",				0);
    filteringProgram.uniform("u_tex_adapt",		1);
    filteringProgram.uniform("alpha",	  this->alpha);
    filteringProgram.uniform("epsilon",	  epsilon);
    glw::bind_program(0);
}

//Processing
ImageRAWGL *FilterGLSigmoidTMO::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(1, w, h, imgIn[0]->channels, IMG_GPU);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, 1, false, imgOut->getTexture());

    ImageRAWGL *ori, *adapt;

    if(imgIn.size() == 2) {
        ori   = imgIn[0];
        adapt = imgIn[1];
    } else {
        ori   = imgIn[0];
        adapt = imgIn[0];
    }

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE1);
    adapt->bindTexture();

    glActiveTexture(GL_TEXTURE0);
    ori->bindTexture();

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    ori->unBindTexture();

    glActiveTexture(GL_TEXTURE1);
    adapt->unBindTexture();
    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SIGMOID_TMO_HPP */

