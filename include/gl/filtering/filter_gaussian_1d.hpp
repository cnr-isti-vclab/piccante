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

#ifndef PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP
#define PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLGaussian1D: public FilterGL
{
protected:
    float		sigma;
    int			dirs[3];
    int			slice;
    ImageRAWGL	*weights;

    void InitShaders();
    void FragmentShader();

public:

    //Init constructors
    FilterGLGaussian1D(float sigma, int filteringDirection, GLenum target);
    //Update
    void ChangePass(int pass, int tPass);
    void Update(float sigma);
    void setSlice(int slice);

    void setSlice2(int slice)
    {
        this->slice = slice;
        filteringProgram.uniform("slice",	     slice);
    }

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    //Execute
    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut, float sigma)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false);

        FilterGLGaussian1D filter(sigma, true, GL_TEXTURE_2D);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageRAWGL *imgOut = filter.Process(SingleGL(&imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gaussian 1D Filter on GPU time: %g ms\n",
               double(timeVal) / 100000000.0);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Constructor
FilterGLGaussian1D::FilterGLGaussian1D(float sigma, int filteringDirection,
                                       GLenum target): FilterGL()
{
    //protected values are assigned/computed
    this->sigma = sigma;
    this->target = target;

    weights = NULL;

    slice = 0;

    dirs[0] = dirs[1] = dirs[2] = 0;

    switch(target) {
    case GL_TEXTURE_2D:
        dirs[filteringDirection % 2] = 1;
        break;

    case GL_TEXTURE_3D:
        dirs[filteringDirection % 3] = 1;
        break;
    }

    FragmentShader();
    InitShaders();
}

void FilterGLGaussian1D::FragmentShader()
{
    std::string fragment_source_2D = GLW_STRINGFY
                                     (
                                         uniform sampler2D	u_tex;
                                         uniform sampler2D	u_weights;
                                         uniform	int         iX;
                                         uniform int         iY;
                                         uniform int         nSamples;
                                         out     vec4		f_color;

    void main(void) {
        vec4  color = vec4(0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec4 tmpCol;
        float weight = 0.0;

        for(int i = 0; i <= nSamples; i++) {
            //Coordinates
            ivec2 coords = ivec2(i * iX, i * iY);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0);
            //Weight
            float tmp = texelFetch(u_weights, ivec2(i, 0), 0).x; //exp(-float(i*i)/sigma2);
            color  += tmpCol * tmp;
            weight += tmp;
        }

        f_color = vec4(color / weight);
    }
                                     );

    std::string fragment_source_3D = GLW_STRINGFY
                                     (
                                         uniform sampler3D	u_tex;
                                         uniform sampler3D	u_weights;
                                         uniform int			slice;
                                         uniform	int			iX;
                                         uniform int			iY;
                                         uniform int			iZ;
                                         uniform int			nSamples;
                                         out     vec4		f_color;

    void main(void) {
        vec4  color = vec4(0.0);
        ivec3 coordsFrag = ivec3(ivec2(gl_FragCoord.xy), slice);
        vec4 tmpCol;
        float weight = 0.0;

        for(int i = 0; i <= nSamples; i++) {
            //Coordinates
            ivec3 coords = coordsFrag.xyz + ivec3(i * iX, i * iY, i * iZ);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coords.xyz, 0);
            //Weight
            float tmp = texelFetch(u_weights, ivec2(i, 0), 0).x; //exp(-float(i*i)/sigma2);
            color += tmpCol * tmp;
            weight += tmp;
        }

        f_color = vec4(color / weight);
    }
                                     );

    switch(target) {
    case GL_TEXTURE_2D: {
        fragment_source = fragment_source_2D;
    }
    break;

    case GL_TEXTURE_3D: {
        fragment_source = fragment_source_3D;
    }
    break;
    }
}

//Change data for this pass
void FilterGLGaussian1D::ChangePass(int pass, int tPass)
{

    if(target == GL_TEXTURE_2D) {
        dirs[  pass % 2 ] = 1;
        dirs[(pass + 1) % 2 ] = 0;
    }

    if(target == GL_TEXTURE_3D) {
        dirs[  pass % 3 ] = 1;
        dirs[(pass + 1) % 3 ] = 0;
        dirs[(pass + 2) % 3 ] = 0;
    }

#ifdef PIC_DEBUG
    printf("%d %d %d\n", dirs[0], dirs[1], dirs[2]);
#endif

    Update(sigma);
}

//Update
void FilterGLGaussian1D::Update(float sigma)
{

    bool bChanges = false;

    if((this->sigma != sigma) && sigma > 0.0f) {
        this->sigma = sigma;
        bChanges = true;
    }

    //Precomputation of the Gaussian Kernel
    int halfKernelSize = PrecomputedGaussian::KernelSize(sigma) >> 1;
    int nSamples = halfKernelSize * 2 + 1;
    float sigma2 = 2.0 * sigma * sigma;

    if(bChanges || weights == NULL) {
        weights = new ImageRAWGL(1, nSamples, 1, 1, IMG_CPU);

        for(int i = -halfKernelSize; i <= halfKernelSize; i++) {
            weights->data[i + halfKernelSize] = expf(-float(i * i) / sigma2);
        }

        weights->generateTextureGL(false);
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",			 0);
    filteringProgram.uniform("u_weights",		 1);
    filteringProgram.uniform("iX",	   dirs[0]);
    filteringProgram.uniform("iY",	   dirs[1]);
    filteringProgram.uniform("nSamples", nSamples);

    if(target == GL_TEXTURE_3D) {
        filteringProgram.uniform("iZ",	   dirs[2]);
        filteringProgram.uniform("slice",	     slice);
    }

    glw::bind_program(0);
}

void FilterGLGaussian1D::setSlice(int slice)
{
    this->slice = slice;
    Update(sigma);
}

//Generating shaders
void FilterGLGaussian1D::InitShaders()
{
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

    Update(sigma);
}

//Processing
ImageRAWGL *FilterGLGaussian1D::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL || imgIn.size() > 1) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;
    int f = imgIn[0]->frames;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(f, w, h, 4, IMG_GPU);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, f, false, imgOut->getTexture());

    ImageRAWGL *base = imgIn[0];

    //Textures
    glActiveTexture(GL_TEXTURE0);
    base->bindTexture();

    glActiveTexture(GL_TEXTURE1);
    weights->bindTexture();

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Rendering
    fbo->bind2();

    //Rendering aligned quad
    for(int z = 0; z < f; z++) {
        setSlice2(z);
        fbo->attachColorBuffer2(0, target, z);

        quad->Render();
    }

    //Fbo
    fbo->unbind2();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE1);
    weights->unBindTexture();

    glActiveTexture(GL_TEXTURE0);
    base->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP */

