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
    void initShaders();

public:
    /**
     * @brief FilterGLSampler2D
     * @param scale
     */
    FilterGLSampler2D(float scale);

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *setupAux(ImageGLVec imgIn, ImageGL *imgOut)
    {
        int w = int(imgIn[0]->widthf  * scale);
        int h = int(imgIn[0]->heightf * scale);
        int f = imgIn[0]->frames;
        int c = imgIn[0]->channels;

        if(imgOut == NULL) {
            imgOut = new ImageGL(f, w, h, c, IMG_GPU, imgIn[0]->getTarget());
        } else {
            if((imgOut->width != w) &&
               (imgOut->height != h) &&
               (imgOut->channels != c) &&
               (imgOut->frames != f)) {
                delete imgOut;
                imgOut = new ImageGL(f, w, h, imgIn[0]->channels, IMG_GPU, imgIn[0]->getTarget());
            }

            if(!imgIn[0]->isSimilarType(imgOut)) {
                delete imgOut;
                imgOut = new ImageGL(f, w, h, c, IMG_GPU, imgIn[0]->getTarget());
            }
        }

        return imgOut;
    }

    /**
     * @brief update
     * @param scale
     */
    void update(float scale);

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

PIC_INLINE FilterGLSampler2D::FilterGLSampler2D(float scale): FilterGL()
{
    update(scale);

    initShaders();
}

PIC_INLINE void FilterGLSampler2D::update(float scale)
{
    this->scale = scale;

    technique.bind();
    technique.setUniform1f("scale", scale);
    technique.unbind();
}

PIC_INLINE void FilterGLSampler2D::initShaders()
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

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1f("scale", scale);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SAMPLER_2D_HPP */

