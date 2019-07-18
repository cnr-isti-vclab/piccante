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

#ifndef PIC_GL_FILTERING_FILTER_NON_LINEAR_1D_HPP
#define PIC_GL_FILTERING_FILTER_NON_LINEAR_1D_HPP

#include "../../gl/filtering/filter_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLNonLinear1D class
 */
class FilterGLNonLinear1D: public FilterGL1D
{
protected:
    int kernelSize, halfKernelSize;
    std::string acc_operator;

    /**
     * @brief FragmentShader
     * @param weights
     * @param direction
     * @param target
     */
    void FragmentShader(ImageGL *weights, int direction, GLenum target)
    {
        std::string fragment_source_2D = MAKE_STRING
                                         (
                                             uniform sampler2D	u_tex;
                                             uniform int        iX;
                                             uniform int        iY;
                                             uniform int        halfKernelSize;
                                             uniform int        kernelSize;
                                             out     vec4	f_color;

        void main(void) {
            ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
            vec4 tmpCol;

            //Texture fetch
            ivec2 coords = ivec2(-halfKernelSize * iX, -halfKernelSize * iY);
            vec4 color = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0);

            for(int i = 1; i < kernelSize; i++) {
                //Coordinates
                int j = i - halfKernelSize;
                ivec2 coords = ivec2(j * iX, j * iY);
                //Texture fetch
                tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0);
                color = _ACC_FUNCTION(color, tmpCol);
            }

            f_color = color;
        }
                                         );

        std::string fragment_source_3D = MAKE_STRING
                                         (
                                             uniform sampler3D  u_tex;
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

            for(int i = 0; i < kernelSize; i++) {
                //Coordinates
                int j = i - halfKernelSize;
                ivec3 coords = coordsFrag.xyz + ivec3(j * iX, j * iY, j * iZ);
                //Texture fetch
                tmpCol = texelFetch(u_tex, coords.xyz, 0);
                color = _ACC_FUNCTION(color, tmpCol);
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
            fragment_source = fragment_source_3D;
        }
        break;
        }

        size_t I_found = fragment_source.find("_ACC_FUNCTION");

        if(I_found != std::string::npos) {
                fragment_source.replace(I_found, 13, acc_operator);
        }
    }

public:

    /**
     * @brief FilterGLNonLinear1D
     */
    FilterGLNonLinear1D(int kernelSize, std::string acc_operator, GLenum target) : FilterGL1D(0, target)
    {
        this->acc_operator = acc_operator;

        update(kernelSize);

        FragmentShader(NULL, 0, GL_TEXTURE_2D);
        initShaders();
    }

    ~FilterGLNonLinear1D()
    {
        release();
    }

    /**
     * @brief setUniformAux
     */
    void setUniformAux()
    {
        technique.setUniform1i("halfKernelSize", halfKernelSize);
        technique.setUniform1i("kernelSize", kernelSize);
    }

    /**
     * @brief update
     * @param kernelSize
     */
    void update(int kernelSize)
    {
        kernelSize = (kernelSize > 0) ? kernelSize : 3;

        if((kernelSize % 2) == 0) {
            kernelSize++;
        }

        this->kernelSize = kernelSize;
        this->halfKernelSize = kernelSize >> 1;
    }
};


} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_NON_LINEAR_1D_HPP */

