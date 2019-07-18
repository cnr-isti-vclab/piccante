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

#ifndef PIC_GL_FILTERING_FILTER_1D_HPP
#define PIC_GL_FILTERING_FILTER_1D_HPP

#include "../../base.hpp"

#include "../../util/std_util.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGL1D class
 */
class FilterGL1D: public FilterGL
{
protected:
    ImageGL *weights;

    int dirs[3], slice;

    /**
     * @brief initShaders
     */
    virtual void initShaders();

    /**
     * @brief FragmentShader
     */
    virtual void FragmentShader()
    {

    }

public:

    /**
     * @brief FilterGL1D
     * @param direction
     * @param target
     */
    FilterGL1D(int direction, GLenum target);

    ~FilterGL1D()
    {
        release();
    }

    /**
     * @brief changePass
     * @param pass
     * @param tPass
     */
    void changePass(int pass, int tPass);

    /**
     * @brief setUniformAux
     */
    virtual void setUniformAux()
    {

    }

    /**
     * @brief setUniform
     */
    void setUniform();

    /**
     * @brief setSlice
     * @param slice
     */
    void setSlice(int slice)
    {
        this->slice = slice;
        setUniform();
    }

    /**
     * @brief setSlice2
     * @param slice
     */
    void setSlice2(int slice)
    {
        this->slice = slice;
        technique.setUniform1i("slice", slice);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

PIC_INLINE FilterGL1D::FilterGL1D(int direction, GLenum target): FilterGL()
{
    weights = NULL;

    //protected values are assigned/computed
    this->target = target;

    slice = 0;

    dirs[0] = dirs[1] = dirs[2] = 0;

    switch(target) {
    case GL_TEXTURE_2D: {
        dirs[direction % 2] = 1;
    } break;

    case GL_TEXTURE_2D_ARRAY: {
        dirs[direction % 3] = 1;
    } break;

    case GL_TEXTURE_3D: {
        dirs[direction % 3] = 1;
    } break;
    }
}

PIC_INLINE void FilterGL1D::changePass(int pass, int tPass)
{

    if(target == GL_TEXTURE_2D) {
        dirs[ pass % 2 ] = 1;
        dirs[(pass + 1) % 2 ] = 0;
    } else {
        if(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY) {
            dirs[ pass % 3 ] = 1;
            dirs[(pass + 1) % 3 ] = 0;
            dirs[(pass + 2) % 3 ] = 0;
        }
    }

    setUniform();
}

PIC_INLINE void FilterGL1D::setUniform()
{
    technique.bind();
    technique.setUniform1i("u_tex", 0);
    setUniformAux();

    technique.setUniform1i("iX", dirs[0]);
    technique.setUniform1i("iY", dirs[1]);

    if(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY) {
        technique.setUniform1i("iZ", dirs[2]);
        technique.setUniform1i("slice", slice);
    }

    technique.unbind();
}

PIC_INLINE void FilterGL1D::initShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLConv1D");

    setUniform();
}

PIC_INLINE ImageGL *FilterGL1D::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn[0] == NULL || imgIn.size() > 1) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;
    int f = imgIn[0]->frames;

    if(imgOut == NULL) {
        imgOut = new ImageGL(f, w, h, imgIn[0]->channels, IMG_GPU, imgIn[0]->getTarget());
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, f, false, imgOut->getTexture());

    ImageGL *base = imgIn[0];

    //bind textures
    glActiveTexture(GL_TEXTURE0);
    base->bindTexture();

    if(weights != NULL) {
        glActiveTexture(GL_TEXTURE1);
        weights->bindTexture();
    }

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //bind shaders
    technique.bind();

    //bind the fbo
    fbo->bindSimple();

    //render an aligned quad
    for(int z = 0; z < f; z++) {
        setSlice2(z);
        fbo->attachColorBuffer2(0, target, z);

        quad->Render();
    }

    //unbind the fbo
    fbo->unbindSimple();

    //unbingd shaders
    technique.unbind();

    //unbind textures
    if(weights != NULL) {
        glActiveTexture(GL_TEXTURE1);
        weights->unBindTexture();
    }

    glActiveTexture(GL_TEXTURE0);
    base->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_1D_HPP */

