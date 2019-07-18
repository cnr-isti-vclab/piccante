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

#ifndef PIC_GL_FILTERING_FILTER_FROM_STROKE_TO_MASK_HPP
#define PIC_GL_FILTERING_FILTER_FROM_STROKE_TO_MASK_HPP

#include "../../base.hpp"
#include "../../filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLFromStrokeToMask class
 */
class FilterGLFromStrokeToMask: public FilterGL
{
protected:

    /**
     * @brief initShaders
     */
    void initShaders();

public:
    /**
     * @brief FilterGLFromStrokeToMask
     */
    FilterGLFromStrokeToMask();

    /**
     * @brief FilterGLFromStrokeToMask
     */
    FilterGLFromStrokeToMask(LUMINANCE_TYPE type);

    ~FilterGLFromStrokeToMask();

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
        FilterGLFromStrokeToMask filter(LT_CIE_LUMINANCE);
        imgOut = filter.Process(SingleGL(imgIn), imgOut);
        return imgOut;
    }
};

PIC_INLINE FilterGLFromStrokeToMask::FilterGLFromStrokeToMask(): FilterGL()
{
    this->type = LT_CIE_LUMINANCE;

    initShaders();
}

PIC_INLINE FilterGLFromStrokeToMask::FilterGLFromStrokeToMask(LUMINANCE_TYPE type): FilterGL()
{
    this->type = type;

    initShaders();
}

PIC_INLINE FilterGLFromStrokeToMask::~FilterGLFromStrokeToMask()
{
    release();
}

PIC_INLINE void FilterGLFromStrokeToMask::initShaders()
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

    technique.initStandard("330", vertex_source, fragment_source, "FilterGLFromStrokeToMask");

    update(type);
}

PIC_INLINE void FilterGLFromStrokeToMask::update()
{
    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_FROM_STROKE_TO_MASK_HPP */

