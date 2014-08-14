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

#ifndef PIC_GL_FILTERING_FILTER_CONV_1D_HPP
#define PIC_GL_FILTERING_FILTER_CONV_1D_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLConv1D class
 */
class FilterGLConv1D: public FilterGL
{
protected:
    int			dirs[3];
    int			slice;
    ImageRAWGL	*weights;

    void InitShaders();
    void FragmentShader();

public:

    /**
     * @brief FilterGLConv1D
     */
    FilterGLConv1D();

    /**
     * @brief FilterGLConv1D
     * @param weights
     * @param direction
     * @param target
     */
    FilterGLConv1D(ImageRAWGL *weights, int direction, GLenum target);

    /**
     * @brief ChangePass
     * @param pass
     * @param tPass
     */
    void ChangePass(int pass, int tPass);

    /**
     * @brief SetUniform
     */
    void SetUniform();

    /**
     * @brief setSlice
     * @param slice
     */
    void setSlice(int slice)
    {
        this->slice = slice;
        SetUniform();
    }

    /**
     * @brief setSlice2
     * @param slice
     */
    void setSlice2(int slice)
    {
        this->slice = slice;
        filteringProgram.uniform("slice", slice);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);
};

/**
 * @brief FilterGLConv1D
 */
FilterGLConv1D::FilterGLConv1D() : FilterGL()
{
    weights = NULL;
}

FilterGLConv1D::FilterGLConv1D(ImageRAWGL *weights, int direction,
                                       GLenum target): FilterGL()
{
    //protected values are assigned/computed
    this->target = target;

    this->weights = weights;

    slice = 0;

    dirs[0] = dirs[1] = dirs[2] = 0;

    switch(target) {
    case GL_TEXTURE_2D:
        dirs[direction % 2] = 1;
        break;

    case GL_TEXTURE_3D:
        dirs[direction % 3] = 1;
        break;
    }

    FragmentShader();
    InitShaders();
}

void FilterGLConv1D::FragmentShader()
{
    std::string fragment_source_2D = GLW_STRINGFY
                                     (
                                         uniform sampler2D	u_tex;
                                         uniform sampler2D	u_weights;
                                         uniform int        iX;
                                         uniform int        iY;
                                         uniform int        halfKernelSize;
                                         uniform int        kernelSize;
                                         out     vec4		f_color;

    void main(void) {
        vec4  color = vec4(0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec4 tmpCol;
        float weight = 0.0;

        for(int i = 0; i < kernelSize; i++) {
            //Coordinates
            int j = i - halfKernelSize;
            ivec2 coords = ivec2(j * iX, j * iY);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0);
            //Weight
            float tmp = texelFetch(u_weights, ivec2(i, 0), 0).x;
            color  += tmpCol * tmp;
            weight += tmp;
        }

        f_color = vec4(color / weight);
    }
                                     );

    std::string fragment_source_3D = GLW_STRINGFY
                                     (
                                         uniform sampler3D  u_tex;
                                         uniform sampler3D  u_weights;
                                         uniform int        slice;
                                         uniform int        iX;
                                         uniform int        iY;
                                         uniform int        iZ;
                                         uniform int        halfKernelSize;
                                         uniform int        kernelSize;
                                         out     vec4       f_color;

    void main(void) {
        vec4  color = vec4(0.0);
        ivec3 coordsFrag = ivec3(ivec2(gl_FragCoord.xy), slice);
        vec4 tmpCol;
        float weight = 0.0;

        for(int i = 0; i < kernelSize; i++) {
            //Coordinates
            int j = i - halfKernelSize;
            ivec3 coords = coordsFrag.xyz + ivec3(j * iX, j * iY, j * iZ);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coords.xyz, 0);
            //Weight
            float tmp = texelFetch(u_weights, ivec2(i, 0), 0).x;
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

void FilterGLConv1D::ChangePass(int pass, int tPass)
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

    SetUniform();
}

void FilterGLConv1D::SetUniform()
{
    int kernelSize = 0;
    int halfKernelSize = 0;

    if(weights != NULL) {
        kernelSize = weights->width;
        halfKernelSize = kernelSize >> 1;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("u_weights", 1);
    filteringProgram.uniform("halfKernelSize", halfKernelSize);
    filteringProgram.uniform("iX", dirs[0]);
    filteringProgram.uniform("iY", dirs[1]);
    filteringProgram.uniform("kernelSize", kernelSize);

    if(target == GL_TEXTURE_3D) {
        filteringProgram.uniform("iZ", dirs[2]);
        filteringProgram.uniform("slice", slice);
    }

    glw::bind_program(0);
}

void FilterGLConv1D::InitShaders()
{
    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();

    SetUniform();
}

ImageRAWGL *FilterGLConv1D::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
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

#endif /* PIC_GL_FILTERING_FILTER_CONV_1D_HPP */

