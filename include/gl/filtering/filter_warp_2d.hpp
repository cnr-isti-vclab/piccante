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

#ifndef PIC_GL_FILTERING_GL_FILTER_WARP_2D_HPP
#define PIC_GL_FILTERING_GL_FILTER_WARP_2D_HPP

#include "util/matrix_3_x_3.hpp"
#include "filtering/filter_warp_2d.hpp"
#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLWarp2D class
 */
class FilterGLWarp2D: public FilterGL
{
protected:

    int bmin[2], bmax[2];

    Matrix3x3 h, h_inv;
    bool bSameSize, bCentroid;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:

    /**
     * @brief FilterGLWarp2D
     * @param h
     * @param bSameSize
     * @param bCentroid
     */
    FilterGLWarp2D(Matrix3x3 h, bool bSameSize, bool bCentroid);

    /**
     * @brief Update
     * @param h
     * @param bSameSize
     * @param bCentroid
     */
    void Update(Matrix3x3 h, bool bSameSize, bool bCentroid);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

FilterGLWarp2D::FilterGLWarp2D(Matrix3x3 h, bool bSameSize = false, bool bCentroid = false) : FilterGL()
{
    InitShaders();

    Update(h, bSameSize, bCentroid);
}

void FilterGLWarp2D::Update(Matrix3x3 h, bool bSameSize = false, bool bCentroid = false)
{
    this->bSameSize = bSameSize;
    this->bCentroid = bCentroid;

    this->h = h;
    h.Inverse(&h_inv);

    glw::bind_program(filteringProgram);
    filteringProgram.uniform3x3("h_inv", h_inv.data, true);
    glw::bind_program(0);
}

void FilterGLWarp2D::InitShaders()
{
    //fragment program
    fragment_source = GLW_STRINGFY
                      (
    uniform sampler2D u_tex; \n
    uniform mat3 h_inv; \n
    uniform vec2 mid; \n
    uniform vec2 inv_tSize; \n
    out  vec4 f_color; \n
    \n
    void main(void) {
        vec2 coords   = gl_FragCoord.xy - mid;\n
        vec3 point_proj = h_inv * vec3(coords, 1.0);
        point_proj /= point_proj.z;
        point_proj.xy += mid;
        point_proj.xy *= inv_tSize;
        vec3 color = vec3(0.0);
        if(point_proj.x >= 0.0 && point_proj.x <= 1.0 &&
           point_proj.y >= 0.0 && point_proj.y <= 1.0) {
            color = texture(u_tex, point_proj.xy).xyz;\n
        } \n
        f_color = vec4(color, 1.0); \n
    }
    );

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLWarp2D log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex", 0);
    glw::bind_program(0);
}

ImageGL *FilterGLWarp2D::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.empty()) {
        return imgOut;
    }

    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int width = imgIn[0]->width;
    int height = imgIn[0]->height;
    int channels = imgIn[0]->channels;

    if(imgOut == NULL) {
        if(!bSameSize) {
            FilterWarp2D::ComputingBoundingBox(h, imgIn[0]->widthf, imgIn[0]->heightf, bmin, bmax, bCentroid);
            imgOut = new ImageGL(1, bmax[0] - bmin[0], bmax[1] - bmin[1], channels, IMG_GPU, GL_TEXTURE_2D);
        } else {
            bmin[0] = 0;
            bmin[1] = 0;

            bmax[0] = imgIn[0]->width;
            bmax[1] = imgIn[0]->height;

            imgOut = new ImageGL(1, width, height, channels, IMG_GPU, GL_TEXTURE_2D);
        }
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(width, height, 1, false, imgOut->getTexture());

    //Rendering
    fbo->bind();

    glViewport(0, 0, (GLsizei)imgOut->width, (GLsizei)imgOut->height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Shaders
    glw::bind_program(filteringProgram);

    float mid[2];

    if(bCentroid) {
        mid[0] = imgIn[0]->widthf  * 0.5f;
        mid[1] = imgIn[0]->heightf * 0.5f;
    } else {
        mid[0] = 0.0f;
        mid[1] = 0.0f;
    }

    filteringProgram.uniform("mid", mid[0], mid[1]);
    filteringProgram.uniform("inv_tSize", 1.0f / imgIn[0]->widthf, 1.0f / imgIn[0]->heightf);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->bindTexture();

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_WARP_2D_HPP */

