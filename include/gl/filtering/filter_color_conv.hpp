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

#include "../../base.hpp"
#include "../../util/std_util.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLColorConv class
 */
class FilterGLColorConv: public FilterGL
{
protected:

    ColorConvGL *color_conv;

    /**
     * @brief initShaders
     */
    void initShaders()
    {
        color_conv->generatePrograms(vertex_source);
    }

public:
    /**
     * @brief FilterGLColorConv
     */
    FilterGLColorConv(ColorConvGL *color_conv, bool direct) : FilterGL()
    {
        this->color_conv = color_conv;

        initShaders();
        setup(direct);
    }

    ~FilterGLColorConv()
    {
        release();
    }

    void releaseAux()
    {
        delete_s(color_conv);
    }

    /**
     * @brief setup
     * @param direct
     */
    void setup(bool direct)
    {
        color_conv->setTransform(direct);
        color_conv->setUniforms();
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageGLVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width    = imgIn[0]->width;
        height   = imgIn[0]->height;
        channels = 3;
        frames   = imgIn[0]->frames;
    }

    void bindTechnique()
    {
        color_conv->bindProgram();
    }

    void unbindTechnique()
    {
        color_conv->unbindProgram();
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_COLOR_CONV_HPP */

