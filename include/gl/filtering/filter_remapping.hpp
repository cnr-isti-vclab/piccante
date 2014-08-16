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

#ifndef PIC_GL_FILTERING_FILTER_REMAPPING_HPP
#define PIC_GL_FILTERING_FILTER_REMAPPING_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLRemapping: public FilterGL
{
protected:

    void InitShaders();

public:
    //Basic constructor
    FilterGLRemapping();

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);
};

FilterGLRemapping::FilterGLRemapping(): FilterGL()
{
    InitShaders();
}

void FilterGLRemapping::InitShaders()
{
    /*	0 ---> Drago et al. 2003
    	1 ---> Reinhard et al. 2002
    	LumZone     = [-2, -1, 0, 1, 2, 3, 4];
    	TMOForZone =  [ 0,  0, 1, 0, 1, 0, 0];	*/

    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex; \n
                          out     vec4      f_color; \n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        int indx = int(texelFetch(u_tex, coords, 0).x + 2.0);
        \n
        indx = (indx == 2) ? 1 : 0;
        \n
        indx = (indx == 4) ? 1 : indx;
        \n
        f_color = vec4(vec3(float(indx)), 1.0);
        \n
    }
                      );

    std::string prefix;
    prefix += glw::version("330");
    prefix += glw::ext_require("GL_EXT_gpu_shader4");

    filteringProgram.setup(prefix, vertex_source, fragment_source);
#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif
    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",      0);
    glw::bind_program(0);
}

//Processing
ImageRAWGL *FilterGLRemapping::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;
    int channels = 3;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(1, w, h, channels, IMG_GPU, GL_TEXTURE_2D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, channels, false, imgOut->getTexture());

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

#endif /* PIC_GL_FILTERING_FILTER_REMAPPING_HPP */

