/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_GL_FILTERING_FILTER_CONV_1D_HPP
#define PIC_GL_FILTERING_FILTER_CONV_1D_HPP

#include "gl/filtering/filter_1d.hpp"

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
    FilterGLConv1D(ImageRAWGL *weights, int direction, GLenum target);

    /**
     * @brief SetUniformAux
     * @return
     */
    void SetUniformAux()
    {
        int kernelSize = 0;
        int halfKernelSize = 0;

        if(weights != NULL) {
            kernelSize = weights->width;
            halfKernelSize = kernelSize >> 1;
        }
        filteringProgram.uniform("halfKernelSize", halfKernelSize);
        filteringProgram.uniform("kernelSize", kernelSize);

        filteringProgram.uniform("u_weights", 1);
    }
};

FilterGLConv1D::FilterGLConv1D(ImageRAWGL *weights, int direction,
                                       GLenum target): FilterGL1D(direction, target)
{
    this->weights = weights;

    FragmentShader();
    InitShaders();
}

void FilterGLConv1D::FragmentShader()
{
    std::string fragment_source_2D = GLW_STRINGFY
                                     (
                                         uniform sampler2D	u_tex;
                                         uniform sampler2D	u_weights;
                                         uniform int        iX;
                                         uniform int        iY;
                                         uniform int        halfKernelSize;
                                         uniform int        kernelSize;
                                         out     vec4		f_color;

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

    std::string fragment_source_3D = GLW_STRINGFY
                                     (
                                         uniform sampler3D  u_tex;
                                         uniform sampler3D  u_weights;
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

