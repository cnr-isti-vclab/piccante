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

#ifndef PIC_GL_FILTERING_FILTER_CONV_1D_HPP
#define PIC_GL_FILTERING_FILTER_CONV_1D_HPP

#include "../../base.hpp"
#include "../../gl/filtering/filter_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLConv1D class
 */
class FilterGLConv1D: public FilterGL1D
{
protected:

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

public:

    /**
     * @brief FilterGLConv1D
     */
    FilterGLConv1D();

    /**
     * @brief FilterGLConv1D
     * @param weights
     * @param direction
     * @param target
     */
    FilterGLConv1D(ImageGL *weights, int direction, GLenum target);

    ~FilterGLConv1D();

    /**
     * @brief setUniformAux
     * @return
     */
    void setUniformAux()
    {
        int kernelSize = 0;
        int halfKernelSize = 0;

        if(weights != NULL) {
            kernelSize = weights->width;
            halfKernelSize = kernelSize >> 1;
        }

        technique.setUniform1i("u_weights", 1);
        technique.setUniform1i("halfKernelSize", halfKernelSize);
        technique.setUniform1i("kernelSize", kernelSize);
    }
};

PIC_INLINE FilterGLConv1D::FilterGLConv1D(ImageGL *weights, int direction,
                                       GLenum target): FilterGL1D(direction, target)
{
    this->weights = weights;

    FragmentShader();
    initShaders();
}

PIC_INLINE FilterGLConv1D::~FilterGLConv1D()
{
    release();
}

PIC_INLINE void FilterGLConv1D::FragmentShader()
{
    std::string fragment_source_2D = MAKE_STRING
                                     (
                                         uniform sampler2D u_tex;
                                         uniform sampler2D u_weights;
                                         uniform int       iX;
                                         uniform int       iY;
                                         uniform int       halfKernelSize;
                                         uniform int       kernelSize;
                                         out    vec4       f_color;

    void main(void) {
        vec4  color = vec4(0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec4 tmpCol;
        float weight = 0.0;

        for(int i = 0; i < kernelSize; i++) {
            //Coordinates
            int j = i - halfKernelSize;
            ivec2 coords = ivec2(j * iX, j * iY);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0);
            //Weight
            float tmp = texelFetch(u_weights, ivec2(i, 0), 0).x;
            color  += tmpCol * tmp;
            weight += tmp;
        }

        f_color = vec4(color / weight);
    }
                                     );

    std::string fragment_source_3D = MAKE_STRING
                                     (
                                         uniform sampler3D  u_tex;
                                         uniform sampler2D  u_weights;
                                         uniform int        slice;
                                         uniform int        iX;
                                         uniform int        iY;
                                         uniform int        iZ;
                                         uniform int        halfKernelSize;
                                         uniform int        kernelSize;
                                         out     vec4       f_color;

    void main(void) {
        vec4  color = vec4(0.0);
        ivec3 coordsFrag = ivec3(ivec2(gl_FragCoord.xy), slice);
        vec4 tmpCol;
        float weight = 0.0;

        for(int i = 0; i < kernelSize; i++) {
            //Coordinates
            int j = i - halfKernelSize;
            ivec3 coords = coordsFrag.xyz + ivec3(j * iX, j * iY, j * iZ);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coords.xyz, 0);
            //Weight
            float tmp = texelFetch(u_weights, ivec2(i, 0), 0).x;
            color += tmpCol * tmp;
            weight += tmp;
        }

        f_color = vec4(color / weight);
    }
                                     );

    switch(target) {
    case GL_TEXTURE_2D: {
        fragment_source = fragment_source_2D;
    }
    break;

    case GL_TEXTURE_3D: {
        fragment_source = fragment_source_3D;
    }
    break;
    }
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_CONV_1D_HPP */

