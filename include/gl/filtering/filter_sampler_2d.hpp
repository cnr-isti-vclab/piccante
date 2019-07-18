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

#ifndef PIC_GL_FILTERING_FILTER_SAMPLER_2D_HPP
#define PIC_GL_FILTERING_FILTER_SAMPLER_2D_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLSampler2D class
 */
class FilterGLSampler2D: public FilterGL
{
protected:
    float scale;

    /**
     * @brief initShaders
     */
    void initShaders()
    {
        fragment_source = MAKE_STRING
                          (
                              uniform sampler2D u_tex; \n
                              uniform float   scale; \n
                              out     vec4    f_color; \n

        void main(void) { \n
            vec2 coords = gl_FragCoord.xy / vec2(scale);
            vec3  color = texelFetch(u_tex, ivec2(coords), 0).xyz; \n
            f_color = vec4(color.xyz, 1.0); \n
        }
                          );

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLSampler2D");
    }

public:
    /**
     * @brief FilterGLSampler2D
     * @param scale
     */
    FilterGLSampler2D(float scale) : FilterGL()
    {
        initShaders();
        update(scale);
    }

    ~FilterGLSampler2D()
    {
        release();
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
        if(imgIn.empty()) {
            width = -1;
            height = -2;
            channels = -2;
            frames = -2;
        }

        if(imgIn.size() == 1) {
            width = int(imgIn[0]->widthf * scale);
            height = int(imgIn[0]->heightf * scale);
        } else {
            width = imgIn[1]->width;
            height = imgIn[1]->height;
        }

        channels = imgIn[0]->channels;
        frames = imgIn[0]->frames;
    }

    /**
     * @brief update
     * @param scale
     */
    void update(float scale)
    {
        if(scale > 0.0f) {
            this->scale = scale;
        }

        if(technique.isValid()) {
            technique.bind();
            technique.setUniform1f("scale", scale);
            technique.unbind();
        }
    }        

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param scale
     * @return
     */
    static ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut, float scale)
    {
        imgIn->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);

        FilterGLSampler2D filter(scale);

        imgOut = filter.Process(SingleGL(imgIn), imgOut);

        return imgOut;
    }
};


} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SAMPLER_2D_HPP */

