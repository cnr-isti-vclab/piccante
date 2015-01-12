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

#ifndef PIC_GL_FILTERING_FILTER_COLOR_CONV_HPP
#define PIC_GL_FILTERING_FILTER_COLOR_CONV_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLColorConv class
 */
class FilterGLColorConv: public FilterGL
{
protected:

    ColorConvGL *color_conv;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLColorConv
     */
    FilterGLColorConv(ColorConvGL *color_conv, bool direct);

    void setTransform(bool direct);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);

};

FilterGLColorConv::FilterGLColorConv(ColorConvGL *color_conv, bool direct = true): FilterGL()
{
    this->color_conv = color_conv;

    InitShaders();
    setTransform(direct);
}

void FilterGLColorConv::InitShaders()
{
    color_conv->generatePrograms(vertex_source);
}

void FilterGLColorConv::setTransform(bool direct)
{
    color_conv->setTransform(direct);
    color_conv->setUniforms();
}

ImageGL *FilterGLColorConv::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.empty()) {
        return imgOut;
    }

    if(imgIn[0] == NULL) {
        return imgOut;
    }

    if(imgIn[0]->channels != 3) {
        #ifdef PIC_DEBUG
        printf("FilterGLColorConv::Process - it has to be a three color channles image.\n");
        #endif
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(1, w, h, 3, IMG_GPU, GL_TEXTURE_2D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, 1, false, imgOut->getTexture());

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    color_conv->bindProgram();

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->bindTexture();

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    color_conv->unbindProgram();

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_COLOR_CONV_HPP */

