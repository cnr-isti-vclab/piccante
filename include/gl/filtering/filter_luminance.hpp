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

#include "../../base.hpp"
#include "../../filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter.hpp"

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
     * @brief initShaders
     */
    void initShaders();

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
     * @brief update
     * @param type
     */
    void update(LUMINANCE_TYPE type);

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut)
    {
        FilterGLLuminance filter(LT_CIE_LUMINANCE);
        imgOut = filter.Process(SingleGL(imgIn), imgOut);
        return imgOut;
    }

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *setupAux(ImageGLVec imgIn, ImageGL *imgOut)
    {
        int w = imgIn[0]->width;
        int h = imgIn[0]->height;
        int f = imgIn[0]->frames;
        int c = 1;

        if(imgOut == NULL) {
            imgOut = new ImageGL(f, w, h, 1, IMG_GPU, imgIn[0]->getTarget());
        } else {
            if((imgOut->width != w) &&
               (imgOut->height != h) &&
               (imgOut->channels != c) &&
               (imgOut->frames != f)) {
                delete imgOut;
                imgOut = new ImageGL(f, w, h, 1, IMG_GPU, imgIn[0]->getTarget());
            }
        }

        return imgOut;
    }
};

PIC_INLINE FilterGLLuminance::FilterGLLuminance(): FilterGL()
{
    this->type = LT_CIE_LUMINANCE;

    initShaders();
}

PIC_INLINE FilterGLLuminance::FilterGLLuminance(LUMINANCE_TYPE type): FilterGL()
{
    this->type = type;

    initShaders();
}

PIC_INLINE void FilterGLLuminance::initShaders()
{
    fragment_source = MAKE_STRING
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

    technique.initStandard("330", vertex_source, fragment_source, "FilterGLLuminance");

    update(type);
}

PIC_INLINE void FilterGLLuminance::update(LUMINANCE_TYPE type)
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
            weights[0] = 0.2126f;
            weights[1] = 0.7152f;
            weights[2] = 0.0722f;
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

    default:
        {

        } break;
    }

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform3f("weights", weights[0], weights[1], weights[2]);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_LUMINANCE_HPP */

