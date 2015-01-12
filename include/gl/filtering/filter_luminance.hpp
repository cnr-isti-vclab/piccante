/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_GL_FILTERING_FILTER_LUMINANCE_HPP
#define PIC_GL_FILTERING_FILTER_LUMINANCE_HPP

#include "filtering/filter_luminance.hpp"
#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLLuminance class
 */
class FilterGLLuminance: public FilterGL
{
protected:

    LUMINANCE_TYPE type;
    float weights[3];

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLLuminance
     */
    FilterGLLuminance();

    /**
     * @brief FilterGLLuminance
     */
    FilterGLLuminance(LUMINANCE_TYPE type);

    /**
     * @brief Update
     * @param type
     */
    void Update(LUMINANCE_TYPE type);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static ImageGL *Execute(ImageGL *imgIn, ImageGL *imgOut)
    {
        FilterGLLuminance filter(LT_CIE_LUMINANCE);
        imgOut = filter.Process(SingleGL(imgIn), imgOut);
        return imgOut;
    }

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

        ImageGL *imgOut = Execute(&imgIn, NULL);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLLuminance::FilterGLLuminance(): FilterGL()
{
    this->type = LT_CIE_LUMINANCE;

    InitShaders();
}

FilterGLLuminance::FilterGLLuminance(LUMINANCE_TYPE type): FilterGL()
{
    this->type = type;

    InitShaders();
}

void FilterGLLuminance::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
    uniform sampler2D u_tex; \n
    uniform vec3 weights; \n
    out     vec4 f_color; \n
    \n
    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy); \n
        vec3 color = texelFetch(u_tex, coords, 0).xyz; \n
        float L = dot(color, weights); \n
        f_color = vec4(L, L, L, 1.0); \n
        \n
    }
                      );

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLLuminance log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update(type);
}

void FilterGLLuminance::Update(LUMINANCE_TYPE type)
{
    this->type = type;

    switch(type)
    {
    case LT_WARD_LUMINANCE:
        {
            weights[0] = 54.0f  / 256.0f;
            weights[1] = 183.0f / 256.0f;
            weights[2] = 19.0f  / 256.0f;
        }
        break;

    case LT_CIE_LUMINANCE:
        {
            weights[0] = 0.213f;
            weights[1] = 0.715f;
            weights[2] = 0.072f;
        }
        break;

    case LT_MEAN:
        {
            float inv_3 = 1.0f / 3.0f;
            weights[0] = inv_3;
            weights[1] = inv_3;
            weights[2] = inv_3;
        }
        break;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("weights", weights[0], weights[1], weights[2]);
    glw::bind_program(0);
}

ImageGL *FilterGLLuminance::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.empty()) {
        return imgOut;
    }

    if(imgIn[0] == NULL) {
        return imgOut;
    }

    if(imgIn[0]->channels != 3) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(1, w, h, 1, IMG_GPU, GL_TEXTURE_2D);
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

#endif /* PIC_GL_FILTERING_FILTER_LUMINANCE_HPP */

