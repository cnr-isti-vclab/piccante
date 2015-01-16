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

#ifndef PIC_GL_FILTERING_FILTER_DOWN_PP_HPP
#define PIC_GL_FILTERING_FILTER_DOWN_PP_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLDownPP class
 */
class FilterGLDownPP: public FilterGL
{
protected:

    float threshold, *value;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLDownPP
     * @param scale
     */
    FilterGLDownPP(float *value, float threshold);

    /**
     * @brief Update
     * @param value
     * @param threshold
     */
    void Update(float *value, float threshold);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

FilterGLDownPP::FilterGLDownPP(float *value, float threshold): FilterGL()
{
    value = NULL;

    InitShaders();
    Update(value, threshold);
}

void FilterGLDownPP::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex; \n
                          uniform vec4    value; \n
                          uniform float   threshold; \n
                          out     vec4    f_color; \n

    void main(void) { \n
        ivec2 coords = ivec2(gl_FragCoord.xy) * ivec2(2); \n
        vec3 color[4]; \n
        color[0] = texelFetch(u_tex, coords              , 0).xyz; \n
        color[1] = texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz; \n
        color[2] = texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz; \n
        color[3] = texelFetch(u_tex, coords + ivec2(1, 1), 0).xyz; \n

        int counter = 0;
        vec3 ret = vec3(0.0);

        for(int i=0; i<4; i++) {
            if(distance(color[i], value.xyz) > threshold) {
                ret += color[i];
                counter++;
            }
        }

        if(counter > 0) {
            ret /= vec3(counter);
        } else {
            ret = value.xyz;
        }

        f_color = vec4(ret.xyz, 1.0); \n
    }
                      );

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLDownPP log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    glw::bind_program(0);
}

/**
 * @brief Update
 * @param value
 * @param threshold
 */
void FilterGLDownPP::Update(float *value, float threshold)
{
    if(value == NULL) {
        this->value = new float[4];

        for(int i = 0; i < 4; i++) {
            this->value[i] = 0.0f;
        }
    } else {
        this->value = value;
    }

    if(threshold > 0.0f) {
        this->threshold = threshold;
    } else {
        this->threshold = 1e-4f;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("threshold", this->threshold);
    filteringProgram.uniform4("value", this->value);
    glw::bind_program(0);
}

ImageGL *FilterGLDownPP::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return NULL;
    }

    if(imgIn.size() != 1) {
        return imgOut;
    }

    int w = imgIn[0]->width  >> 1;
    int h = imgIn[0]->height >> 1;
    int f = imgIn[0]->frames;

    if(imgOut == NULL) {
        imgOut = new ImageGL(f, w, h, imgIn[0]->channels, IMG_GPU, imgIn[0]->getTarget());
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

#endif /* PIC_GL_FILTERING_FILTER_DOWN_PP_HPP */
