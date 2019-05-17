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

#ifndef PIC_GL_FILTERING_FILTER_CONV_2D_HPP
#define PIC_GL_FILTERING_FILTER_CONV_2D_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLConv2D class
 */
class FilterGLConv2D: public FilterGL
{
protected:

    GLenum target;

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

public:

    /**
     * @brief FilterGLConv2D
     * @param target
     */
    FilterGLConv2D(GLenum target);

    /**
     * @brief initShaders
     */
    void initShaders();

    /**
     * @brief setUniform
     * @return
     */
    void setUniform()
    {
        technique.bind();
        technique.setUniform1i("u_tex", 0);
        technique.setUniform1i("u_weights", 1);

        if(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY) {
           // technique.setUniform("slice", slice);
        }

        technique.unbind();
    }
};

PIC_INLINE FilterGLConv2D::FilterGLConv2D(GLenum target): FilterGL()
{
    this->target = target;

    FragmentShader();
    initShaders();
}

PIC_INLINE void FilterGLConv2D::initShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGLConv2D");

    setUniform();
}

PIC_INLINE void FilterGLConv2D::FragmentShader()
{
    std::string fragment_source_2D = MAKE_STRING
                                     (
                                         uniform sampler2D u_tex;
                                         uniform sampler2D u_weights;
                                         out     vec4      f_color;

    void main(void) {
        ivec2 kernelSize = textureSize(u_weights, 0);
        ivec2 halfKernelSize = kernelSize >> 1;

        ivec2 shift = ivec2(halfKernelSize.x, halfKernelSize.y);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy) - shift;

        vec4  color = vec4(0.0);

        for(int i = 0; i < kernelSize.y; i++) {

            for(int j = 0; j < kernelSize.x; j++) {
                //do a texture fetch
                vec4 tmpCol = texelFetch(u_tex, coordsFrag.xy + ivec2(j, i), 0);

                //weight
                color += tmpCol * texelFetch(u_weights, ivec2(j, i), 0);
            }
        }

        f_color = color;
    }
                                     );

    switch(target) {
    case GL_TEXTURE_2D: {
        fragment_source = fragment_source_2D;
    }
    break;

    case GL_TEXTURE_3D: {
        //fragment_source = fragment_source_3D;
    }
    break;
    }
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_CONV_2D_HPP */

