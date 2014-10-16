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

#ifndef PIC_GL_FILTERING_FILTER_THRESHOLDING_HPP
#define PIC_GL_FILTERING_FILTER_THRESHOLDING_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLThresholding class
 */
class FilterGLThresholding: public FilterGL
{
protected:
    void InitShaders();

    void FragmentShader();

public:
    /**
     * @brief FilterGLThresholding
     */
    FilterGLThresholding();

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
     * @return
     */
    static ImageGL *Execute(std::string nameIn, std::string nameOut)
    {
        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLThresholding filter;

        GLuint testTQ1 = glBeginTimeQuery();
        ImageGL *imgOut = filter.Process(SingleGL(&imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gradient Filter on GPU time: %g ms\n", double(timeVal) / 100000000.0);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);

        return imgOut;
    }
};

FilterGLThresholding::FilterGLThresholding(): FilterGL()
{
    //protected values are assigned/computed
    FragmentShader();
    InitShaders();
}

void FilterGLThresholding::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex; \n
                          out     vec4      f_color;	\n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec3  c0 = texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz;
        \n
        vec3  c1 = texelFetch(u_tex, coords - ivec2(1, 0), 0).xyz;
        \n
        vec3  c2 = texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz;
        \n
        vec3  c3 = texelFetch(u_tex, coords - ivec2(0, 1), 0).xyz;
        \n
        vec3 Gx = c1 - c0; //X gradient
        vec3 Gy = c2 - c3; //Y gradient
        f_color = vec4(sqrt(Gx.xyz * Gx.xyz + Gy.xyz * Gy.xyz), 1.0); //Magnitude
    }\n
                      );
}

void FilterGLThresholding::InitShaders()
{
    FragmentShader();

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    glw::bind_program(0);
}

ImageGL *FilterGLThresholding::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(imgIn[0]->frames, w, h, imgIn[0]->channels, IMG_GPU, GL_TEXTURE_2D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, imgIn[0]->frames, false, imgOut->getTexture());

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
    glBindTexture(GL_TEXTURE_2D, 0);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_THRESHOLDING_HPP */

