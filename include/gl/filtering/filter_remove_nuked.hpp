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

#ifndef PIC_GL_FILTERING_FILTER_REMOVE_NUKED_HPP
#define PIC_GL_FILTERING_FILTER_REMOVE_NUKED_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLRemoveNuked: public FilterGL
{
protected:
    float threshold;

    void InitShaders();
    void FragmentShader();

public:
    //Basic constructors
    FilterGLRemoveNuked(float threshold);

    void Update(float threshold);

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);
};

//Constructor
FilterGLRemoveNuked::FilterGLRemoveNuked(float threshold): FilterGL()
{
    this->threshold = threshold;

    FragmentShader();
    InitShaders();
}

void FilterGLRemoveNuked::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D	u_tex;
                          uniform float		threshold;
                          out     vec4		f_color;

    void main(void) {

        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);

        float ref = texelFetch(u_tex, coordsFrag, 0).x;
        float tmp;

        int count = 0;

        vec3 colorNuked = vec3(0.0);

        for(int i = -2; i < 3; i++) {
            for(int j = -2; j < 3; j++) {
                //Coordinates
                ivec2 coords = ivec2(i, j);
                //Texture fetch
                tmp = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0).x;
                float tmp2 = abs(tmp - ref);

                if(tmp2 > threshold) {
                    count = count + 1;
                    colorNuked = vec3(tmp);
                }
            }\n
        }\n

        vec3 color = vec3(ref);
        \n

        if(count > 12)\n
            color = colorNuked;

        \n
        f_color = vec4(color, 1.0);
        \n
    }\n
                      );
}

//Generating shaders
void FilterGLRemoveNuked::InitShaders()
{
    std::string prefix;
    prefix += glw::version("150");
//	prefix += glw::ext_require("GL_EXT_gpu_shader4");
    filteringProgram.setup(prefix, vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    glw::bind_program(0);
    Update(-1.0f);
}

void FilterGLRemoveNuked::Update(float threshold)
{

    if(threshold > 0.0f) {
        this->threshold = threshold;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("threshold",  this->threshold);
    glw::bind_program(0);
}

//Processing
ImageRAWGL *FilterGLRemoveNuked::Process(ImageRAWGLVec imgIn,
        ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(1, w, h, imgIn[0]->channels, IMG_GPU, GL_TEXTURE_2D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, 1, false, imgOut->getTexture());

    ImageRAWGL *edge, *base;

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

#endif /* PIC_GL_FILTERING_FILTER_REMOVE_NUKED_HPP */

