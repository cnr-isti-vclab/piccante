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

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLConv2D class
 */
class FilterGLConv2D: FilterGL
{
protected:

    GLenum target;
    ImageGL *weights;

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

public:

    /**
     * @brief FilterGLConv2D
     */
    FilterGLConv2D();

    /**
     * @brief FilterGLConv2D
     * @param weights
     * @param direction
     * @param target
     */
    FilterGLConv2D(ImageGL *weights, GLenum target);

    /**
     * @brief InitShaders
     */
    void InitShaders();

    /**
     * @brief SetUniform
     * @return
     */
    void SetUniform()
    {
        int kernelSize = 0;
        int halfKernelSize = 0;

        if(weights != NULL) {
            kernelSize = weights->width;
            halfKernelSize = kernelSize >> 1;
        }

        technique.bind();
        technique.setUniform("u_tex", 0);
        technique.setUniform("u_weights", 1);
        technique.setUniform("halfKernelSize", halfKernelSize);
        technique.setUniform("kernelSize", kernelSize);

        if(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY) {
           // technique.setUniform("slice", slice);
        }

        technique.unbind();
    }
};

FilterGLConv2D::FilterGLConv2D(ImageGL *weights, GLenum target): FilterGL()
{
    this->weights = weights;
    this->target = target;

    FragmentShader();
    InitShaders();
}

void FilterGLConv2D::InitShaders()
{
    technique.initStandard("330", vertex_source, fragment_source, "FilterGL1D");

    SetUniform();
}

void FilterGLConv2D::FragmentShader()
{
    std::string fragment_source_2D = MAKE_STRING
                                     (
                                         uniform sampler2D u_tex;
                                         uniform sampler2D u_weights;
                                         uniform int       halfKernelSize;
                                         uniform int       kernelSize;
                                         out     vec4      f_color;

    void main(void) {
        vec4  color = vec4(0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        ivec2 shift = ivec2(halfKernelSize, halfKernelSize);
        vec4 tmpCol;
        float weight = 0.0;

        for(int i = 0; i < kernelSize; i++) {
            for(int j = 0; j < kernelSize; j++) {
                //Texture fetch
                tmpCol = texelFetch(u_tex, coordsFrag.xy + ivec2(j, i) - shift, 0);

                //Weight
                float tmp = texelFetch(u_weights, ivec2(j, i), 0).x;
                color  += tmpCol * tmp;
                weight += tmp;
            }
        }

        f_color = weight > 0.0 ? vec4(color / weight) : color;
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

