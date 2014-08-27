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

#ifndef PIC_GL_FILTERING_FILTER_SAMPLER_2D_HPP
#define PIC_GL_FILTERING_FILTER_SAMPLER_2D_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLSampler2D class
 */
class FilterGLSampler2D: public FilterGL
{
protected:
    float scale;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLSampler2D
     * @param scale
     */
    FilterGLSampler2D(float scale);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param scale
     * @return
     */
    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut, float scale)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLSampler2D filter(scale);
        ImageRAWGL *imgOut = new ImageRAWGL(    imgIn.frames,
                                                int(imgIn.widthf  * scale),
                                                int(imgIn.heightf * scale),
                                                imgIn.channels, IMG_GPU_CPU, GL_TEXTURE_2D);

        GLuint testTQ1 = glBeginTimeQuery();
        filter.Process(SingleGL(&imgIn), imgOut);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);

        printf("DownSampling Filter on GPU time: %f ms\n",
               double(timeVal) / 100000000.0);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);

        return imgOut;
    }
};

FilterGLSampler2D::FilterGLSampler2D(float scale): FilterGL()
{
    this->scale = scale;
    InitShaders();
}

void FilterGLSampler2D::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex; \n
                          uniform float   scale; \n
                          out     vec4    f_color; \n

    void main(void) { \n
        ivec2 coords = ivec2(gl_FragCoord.xy / scale); \n
        vec3  color = texelFetch(u_tex, coords, 0).xyz; \n
        f_color = vec4(color.xyz, 1.0); \n
    }
                      );

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLSampler2D log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("scale", scale);
    glw::bind_program(0);
}

ImageRAWGL *FilterGLSampler2D::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return NULL;
    }

    if(imgIn.size() != 1) {
        return imgOut;
    }

    int w = int(imgIn[0]->widthf  * scale);
    int h = int(imgIn[0]->heightf * scale);
    int f = imgIn[0]->frames;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(f, w, h, 4, IMG_GPU, imgIn[0]->getTarget());
    }

    //Fbo
    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, f, false, imgOut->getTexture());

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imgIn[0]->getTexture());

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

#endif /* PIC_GL_FILTERING_FILTER_SAMPLER_2D_HPP */

