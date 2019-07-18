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

#ifndef PIC_GL_FILTERING_FILTER_UP_PP_HPP
#define PIC_GL_FILTERING_FILTER_UP_PP_HPP

#include "../../base.hpp"

#include "../../util/array.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLUpPP class
 */
class FilterGLUpPP: public FilterGL
{
protected:

    float threshold, *value;

    /**
     * @brief initShaders
     */
    void initShaders()
    {
        fragment_source = MAKE_STRING
                          (
                              uniform sampler2D u_tex0; \n
                              uniform sampler2D u_tex1; \n
                              uniform vec4    value; \n
                              uniform float   threshold; \n
                              out     vec4    f_color; \n

        void main(void) { \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec3 color = texelFetch(u_tex1, coords, 0).xyz; \n

            vec3 ret;

            if(distance(color, value.xyz) < threshold) { \n
                ret = texelFetch(u_tex0, coords / ivec2(2), 0).xyz; \n
            } else { \n
                ret = color.xyz; \n
            }\n

            f_color = vec4(ret.xyz, 1.0); \n
        }
                          );

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLUpPP");

    }

public:
    /**
     * @brief FilterGLUpPP
     * @param scale
     */
    FilterGLUpPP(float *value, float threshold) : FilterGL()
    {
        initShaders();
        update(value, threshold);
    }

    ~FilterGLUpPP()
    {
        release();
    }

    /**
     * @brief update
     * @param value
     * @param threshold
     */
    void update(float *value, float threshold)
    {
        this->value = value;

        if(value == NULL) {
            printf("Error in FilterGLUpPP\n");
        }

        this->threshold = (threshold > 0.0f) ? threshold : 1e-4f;

        technique.bind();
        technique.setUniform1i("u_tex0", 0);
        technique.setUniform1i("u_tex1", 1);
        technique.setUniform1f("threshold", this->threshold);
        technique.setUniform4fv("value", this->value);
        technique.unbind();
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
        if(imgIn.size() == 1) {
            width = imgIn[0]->width << 1;
            height = imgIn[0]->height << 1;
        } else {
            width = imgIn[1]->width;
            height = imgIn[1]->height;
        }

        channels = imgIn[0]->channels;
        frames = imgIn[0]->frames;
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_UP_PP_HPP */
