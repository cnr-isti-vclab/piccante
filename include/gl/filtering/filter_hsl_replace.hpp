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

#ifndef PIC_GL_FILTERING_FILTER_HSL_REPLACE_HPP
#define PIC_GL_FILTERING_FILTER_HSL_REPLACE_HPP

#include "gl/filtering/filter.hpp"
#include "util/gl/colors.hpp"

namespace pic {

class FilterGLHSLReplace: public FilterGL
{
protected:
    float delta_hue;
    float delta_saturation;

    void InitShaders();

public:
    //Basic constructor
    FilterGLHSLReplace(float delta_hue, float delta_saturation);

    void setDeltaHue(float delta_hue)
    {
        this->delta_hue = delta_hue;
    }

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);
};

//Basic constructor
FilterGLHSLReplace::FilterGLHSLReplace(float delta_hue,
                                       float delta_saturation): FilterGL()
{
    this->delta_hue = delta_hue;
    this->delta_saturation = delta_saturation;
    InitShaders();
}

void FilterGLHSLReplace::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform float	  delta_hue; \n
                          uniform float	  delta_saturation; \n
                          uniform sampler2D u_tex; \n
                          uniform sampler2D u_change; \n
                          out     vec4      f_color; \n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec3  color = texelFetch(u_tex, coords, 0).xyz;
        \n
        float weight = texelFetch(u_change, coords, 0).x;
        weight = min(max(weight, 0.0), 1.0);

        if(weight > 0.0) {
            color = RGB2HSL(color);
            \n
            color.x += delta_hue * weight;
            \n
            color.z += max(delta_saturation * weight, 0.0);
            \n
            color = HSL2RGB(color);
            \n
        }

        f_color = vec4(color.xyz, 1.0);
        \n
    }
                      );

    std::string prefix;
    prefix += glw::version("150");
    prefix += glw::ext_require("GL_EXT_gpu_shader4");

    //Final fragment source
    std::string final_fragment_source;
    final_fragment_source  = GLSL_RGB2HSL();
    final_fragment_source += GLSL_HSL2RGB();
    final_fragment_source += fragment_source;

    filteringProgram.setup(prefix, vertex_source, final_fragment_source);
#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif
    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("u_change",   1);
    filteringProgram.uniform("delta_hue",  delta_hue);
    filteringProgram.uniform("delta_saturation",  delta_saturation);
    glw::bind_program(0);
}

//Processing
ImageRAWGL *FilterGLHSLReplace::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return NULL;
    }

    if(imgIn.size() != 2) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    //Fbo
    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOneGL();
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, 1, false, imgOut->getTexture());

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, imgIn[1]->getTexture());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imgIn[0]->getTexture());

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_HSL_REPLACE_HPP */

