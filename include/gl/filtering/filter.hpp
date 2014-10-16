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

#ifndef PIC_GL_FILTERING_FILTER_HPP
#define PIC_GL_FILTERING_FILTER_HPP

#include "gl/image_vec.hpp"
#include "util/gl/quad.hpp"

#include "externals/glw/program.hpp"

namespace pic {

/**
 * @brief The FilterGL class
 */
class FilterGL
{
protected:
    //FBO
    Fbo *fbo;

    //Quad
    QuadGL *quad;

    //Shaders
    glw::program filteringProgram;
    GLenum target;

public:

    std::vector<FilterGL *> filters;

    std::string vertex_source, geometry_source, fragment_source;

    /**
     * @brief FilterGL
     */
    FilterGL()
    {
        fbo = NULL;

        quad = NULL;

        quad = new QuadGL(false);

        target = GL_TEXTURE_2D;

        //getting a vertex program for screen aligned quad
        vertex_source = QuadGL::getVertexProgramV3();
    }

    ~FilterGL()
    {
        if(quad != NULL) {
            delete quad;
        }
    }

    /**
     * @brief setFbo
     * @param fbo
     */
    void setFbo(Fbo *fbo)
    {
        this->fbo = fbo;
    }

    /**
     * @brief getFbo
     * @return
     */
    virtual Fbo  *getFbo()
    {
        return fbo;
    }

    /**
     * @brief setTarget
     * @param target
     */
    void setTarget(GLenum target)
    {
        this->target = target;
    }

    /**
     * @brief ChangePass
     * @param pass
     * @param tPass
     */
    virtual void ChangePass(int pass, int tPass)
    {
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief GammaCorrection
     * @param fragment_source
     * @param bGammaCorrection
     * @return
     */
    static std::string GammaCorrection(std::string fragment_source,
                                       bool bGammaCorrection)
    {
        size_t processing_found = fragment_source.find("__GAMMA__CORRECTION__");

        if(processing_found != std::string::npos) {
            if(bGammaCorrection) {
                fragment_source.replace(processing_found, 21,
                                        " color = pow(color,vec3(1.0/2.2)); ");
            } else {
                fragment_source.replace(processing_found, 21, " ");
            }
        }

        return fragment_source;
    }
};

ImageGL *FilterGL::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(imgIn[0]->frames, w, h, imgIn[0]->channels, IMG_GPU, imgIn[0]->getTarget());
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
    imgIn[0]->bindTexture();

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    imgIn[0]->unBindTexture();

    return imgOut;
}


} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_HPP */

