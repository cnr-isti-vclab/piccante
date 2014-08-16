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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_1D_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_1D_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLBilateral1D: public FilterGL
{
protected:
    float	sigma_s, sigma_r;
    int		dirs[3];
    int		slice;

    void InitShaders();
    void FragmentShader();

public:

    //Init constructors
    FilterGLBilateral1D(float sigma_s, float sigma_r, int filteringDirection,
                        GLenum target);
    //Update
    void ChangePass(int pass, int tPass);
    void Update(float sigma_s, float sigma_r);
    void setSlice(int slice);

    void setSlice2(int slice)
    {
        this->slice = slice;
        filteringProgram.uniform("slice",	     slice);
    }

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    //Execute
    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r)
    {
        GLuint testTQ = glBeginTimeQuery();
        glEndTimeQuery(testTQ);

        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLBilateral1D filter(sigma_s, sigma_r, true, GL_TEXTURE_2D);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageRAWGL *imgOut = filter.Process(SingleGL(&imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gaussian 1D Filter on GPU time: %g ms\n",
               double(timeVal) / 100000000.0);

        imgOut->readFromFBO(filter.getFbo());
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Constructor
FilterGLBilateral1D::FilterGLBilateral1D(float sigma_s, float sigma_r,
        int filteringDirection, GLenum target): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;
    this->target = target;

    slice = 0;

    dirs[0] = dirs[1] = dirs[2] = 0;

    switch(target) {
    case GL_TEXTURE_2D:
        dirs[filteringDirection % 2] = 1;
        break;

    case GL_TEXTURE_3D:
        dirs[filteringDirection % 3] = 1;
        break;

    case GL_TEXTURE_2D_ARRAY:
        dirs[filteringDirection % 3] = 1;
        break;
    }

    FragmentShader();
    InitShaders();
}

void FilterGLBilateral1D::FragmentShader()
{
    std::string fragment_source_2D = GLW_STRINGFY
                                     (
                                         uniform sampler2D	u_tex;
                                         uniform float		sigma_s2;
                                         uniform float		sigma_r2;
                                         uniform int		iX;
                                         uniform int		iY;
                                         out     vec4		f_color;

    void main(void) {
        vec3  color = vec3(0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec3 tmpCol;
        float weight = 0.0;
        vec3 colRef = texelFetch(u_tex, coordsFrag.xy, 0).xyz;

        for(int i = -TOKEN_BANANA; i < TOKEN_BANANA; i++) {
            //Coordinates
            ivec2 coords = ivec2(i * iX, i * iY);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0).xyz;
            vec3 tmpCol2 = tmpCol - colRef;
            float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
            float tmp = exp(-dstR / sigma_r2 - float(coords.x * coords.x + coords.y *
                            coords.y) / sigma_s2);
            color.xyz += tmpCol.xyz * tmp;
            weight += tmp;
        }

        color = weight > 0.0 ? color / weight : colRef;
        f_color = vec4(color.xyz, 1.0);
    }
                                     );

    std::string fragment_source_3D = GLW_STRINGFY
                                     (
                                         uniform sampler2DArray	u_tex;
                                         uniform float		    sigma_s2;
                                         uniform float		    sigma_r2;
                                         uniform int		    slice;
                                         uniform int		    iX;
                                         uniform int		    iY;
                                         uniform int		    iZ;
                                         out     vec4		    f_color;

    void main(void) {
        vec3  color = vec3(0.0);
        ivec3 coordsFrag = ivec3(ivec2(gl_FragCoord.xy), slice);
        vec3 tmpCol;
        float weight = 0.0;
        vec3 colRef = texelFetch(u_tex, coordsFrag, 0).xyz;

        for(int i = -TOKEN_BANANA; i < TOKEN_BANANA; i++) {
            //Coordinates
            ivec3 coords = coordsFrag.xyz + ivec3(i * iX, i * iY, i * iZ);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag + coords, 0).xyz;
            vec3 tmpCol2 = tmpCol - colRef;
            float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
            float tmp = exp(-dstR / sigma_r2 - float(coords.x * coords.x + coords.y *
                            coords.y) / sigma_s2);
            color.xyz += tmpCol.xyz * tmp;
            weight += tmp;
        }

        color = weight > 0.0 ? color / weight : colRef;
        f_color = vec4(color.xyz, 1.0);
    }
                                     );

    switch(target) {
    case GL_TEXTURE_2D: {
        fragment_source = fragment_source_2D;
    }
    break;

    case GL_TEXTURE_2D_ARRAY: {
        fragment_source = fragment_source_3D;
    }
    break;

    case GL_TEXTURE_3D: {
        fragment_source = fragment_source_3D;
    }
    break;
    }
}

//Change data for this pass
void FilterGLBilateral1D::ChangePass(int pass, int tPass)
{

    if(target == GL_TEXTURE_2D) {
        dirs[  pass % 2    ] = 1;
        dirs[(pass + 1) % 2 ] = 0;
    }

    if(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY) {
        dirs[  pass % 3    ] = 1;
        dirs[(pass + 1) % 3 ] = 0;
        dirs[(pass + 2) % 3 ] = 0;
    }

#ifdef PIC_DEBUG
    printf("%d %d %d\n", dirs[0], dirs[1], dirs[2]);
#endif

    Update(sigma_s, sigma_r);
}

//Update
void FilterGLBilateral1D::Update(float sigma_s, float sigma_r)
{
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    float sigma_s2 = 2.0 * sigma_s * sigma_s;
    float sigma_r2 = 2.0 * sigma_r * sigma_r;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",		 0);
    filteringProgram.uniform("sigma_s2",	sigma_s2);
    filteringProgram.uniform("sigma_r2",	sigma_r2);
    filteringProgram.uniform("iX",	        dirs[0]);
    filteringProgram.uniform("iY",	        dirs[1]);

    if(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY) {
        filteringProgram.uniform("iZ",	    dirs[2]);
        filteringProgram.uniform("slice",	slice);
    }

    glw::bind_program(0);
}

void FilterGLBilateral1D::setSlice(int slice)
{
    this->slice = slice;
    Update(sigma_s, sigma_r);
}

//Generating shaders
void FilterGLBilateral1D::InitShaders()
{
    //Precomputation of the Gaussian Kernel
    int halfKernelSize = PrecomputedGaussian::KernelSize(sigma_s) >> 1;

    //Poisson samples

#ifdef PIC_DEBUG
    printf("Window: %d\n", halfKernelSize);
#endif

    std::string prefix;
    prefix += glw::version("150");
    prefix += glw::ext_require("GL_EXT_gpu_shader4");
    prefix += glw::define("TOKEN_BANANA", halfKernelSize);
    filteringProgram.setup(prefix, vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();

    Update(sigma_s, sigma_r);
}

//Processing
ImageRAWGL *FilterGLBilateral1D::Process(ImageRAWGLVec imgIn,
        ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL || imgIn.size() > 1) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;
    int f = 1;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(f, w, h, 4, IMG_GPU, imgIn[0]->getTarget());
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, f, false, imgOut->getTexture());

    ImageRAWGL *base = imgIn[0];

    //Textures
    glActiveTexture(GL_TEXTURE0);
    base->bindTexture();

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
    glActiveTexture(GL_TEXTURE0);
    base->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_1D_HPP */

