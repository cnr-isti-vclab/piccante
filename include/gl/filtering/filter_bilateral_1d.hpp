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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_1D_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_1D_HPP

#include "gl/filtering/filter_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLBilateral1D class
 */
class FilterGLBilateral1D: public FilterGL1D
{
protected:
    float	sigma_s, sigma_r;

    void InitShaders();
    void FragmentShader();

public:

    /**
     * @brief FilterGLBilateral1D
     * @param sigma_s
     * @param sigma_r
     * @param direction
     * @param target
     */
    FilterGLBilateral1D(float sigma_s, float sigma_r, int direction,
                        GLenum target);

    /**
     * @brief SetUniformAux
     */
    void SetUniformAux();

    /**
     * @brief Update
     * @param sigma_s
     * @param sigma_r
     */
    void Update(float sigma_s, float sigma_r);

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static ImageGL *Execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r)
    {
        GLuint testTQ = glBeginTimeQuery();
        glEndTimeQuery(testTQ);

        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLBilateral1D filter(sigma_s, sigma_r, true, GL_TEXTURE_2D);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageGL *imgOut = filter.Process(SingleGL(&imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gaussian 1D Filter on GPU time: %g ms\n",
               double(timeVal) / 100000000.0);

        imgOut->readFromFBO(filter.getFbo());
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLBilateral1D::FilterGLBilateral1D(float sigma_s, float sigma_r,
        int direction, GLenum target): FilterGL1D(direction, target)
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    FragmentShader();
    InitShaders();
}

void FilterGLBilateral1D::FragmentShader()
{
    std::string fragment_source_2D = GLW_STRINGFY
                                     (
                                         uniform sampler2D	u_tex;
                                         uniform float		sigma_s2;
                                         uniform float		sigma_r2;
                                         uniform int		iX;
                                         uniform int		iY;
                                         uniform int        halfKernelSize;
                                         out     vec4		f_color;

    void main(void) {
        vec3  color = vec3(0.0);
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec3 tmpCol;
        float weight = 0.0;
        vec3 colRef = texelFetch(u_tex, coordsFrag.xy, 0).xyz;

        for(int i = -halfKernelSize; i <= halfKernelSize; i++) {
            //Coordinates
            ivec2 coords = ivec2(i * iX, i * iY);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0).xyz;
            vec3 tmpCol2 = tmpCol - colRef;
            float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
            float tmp = exp(-dstR / sigma_r2 - float(coords.x * coords.x + coords.y *
                            coords.y) / sigma_s2);
            color.xyz += tmpCol.xyz * tmp;
            weight += tmp;
        }

        color = weight > 0.0 ? color / weight : colRef;
        f_color = vec4(color.xyz, 1.0);
    }
                                     );

    std::string fragment_source_3D = GLW_STRINGFY
                                     (
                                         uniform sampler2DArray	u_tex;
                                         uniform float		    sigma_s2;
                                         uniform float		    sigma_r2;
                                         uniform int		    slice;
                                         uniform int		    iX;
                                         uniform int		    iY;
                                         uniform int		    iZ;
                                         out     vec4		    f_color;

    void main(void) {
        vec3  color = vec3(0.0);
        ivec3 coordsFrag = ivec3(ivec2(gl_FragCoord.xy), slice);
        vec3 tmpCol;
        float weight = 0.0;
        vec3 colRef = texelFetch(u_tex, coordsFrag, 0).xyz;

        for(int i = -halfKernelSize; i <= halfKernelSize; i++) {
            //Coordinates
            ivec3 coords = coordsFrag.xyz + ivec3(i * iX, i * iY, i * iZ);
            //Texture fetch
            tmpCol = texelFetch(u_tex, coordsFrag + coords, 0).xyz;
            vec3 tmpCol2 = tmpCol - colRef;
            float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
            float tmp = exp(-dstR / sigma_r2 - float(coords.x * coords.x + coords.y *
                            coords.y) / sigma_s2);
            color.xyz += tmpCol.xyz * tmp;
            weight += tmp;
        }

        color = weight > 0.0 ? color / weight : colRef;
        f_color = vec4(color.xyz, 1.0);
    }
                                     );

    switch(target) {
    case GL_TEXTURE_2D: {
        fragment_source = fragment_source_2D;
    }
    break;

    case GL_TEXTURE_2D_ARRAY: {
        fragment_source = fragment_source_3D;
    }
    break;

    case GL_TEXTURE_3D: {
        fragment_source = fragment_source_3D;
    }
    break;
    }
}

void FilterGLBilateral1D::InitShaders()
{
    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLBilateral1D log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();

    Update(sigma_s, sigma_r);
}

void FilterGLBilateral1D::SetUniformAux()
{
    float sigma_s2 = 2.0f * sigma_s * sigma_s;
    float sigma_r2 = 2.0f * sigma_r * sigma_r;

    //Precomputation of the Gaussian Kernel
    int halfKernelSize = PrecomputedGaussian::KernelSize(sigma_s) >> 1;

    filteringProgram.uniform("sigma_s2",	sigma_s2);
    filteringProgram.uniform("sigma_r2",	sigma_r2);
    filteringProgram.uniform("halfKernelSize", halfKernelSize);
}

void FilterGLBilateral1D::Update(float sigma_s, float sigma_r)
{
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    SetUniform();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_1D_HPP */

