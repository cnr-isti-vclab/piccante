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

#ifndef PIC_GL_FILTERING_FILTER_GRADIENT_HPP
#define PIC_GL_FILTERING_FILTER_GRADIENT_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLGradient class
 */
class FilterGLGradient: public FilterGL
{
protected:
    /**
     * @brief InitShaders
     */
    void InitShaders();

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

public:

    /**
     * @brief FilterGLGradient
     */
    FilterGLGradient();

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @return
     */
    static ImageGL *Execute(std::string nameIn, std::string nameOut)
    {
        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLGradient filter;

        GLuint testTQ1 = glBeginTimeQuery();
        ImageGL *imgOut = filter.Process(SingleGL(&imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gradient Filter on GPU time: %g ms\n", double(timeVal) / 100000000.0);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);

        return imgOut;
    }
};

FilterGLGradient::FilterGLGradient(): FilterGL()
{
    //protected values are assigned/computed
    FragmentShader();
    InitShaders();
}

void FilterGLGradient::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
                          uniform sampler2D u_tex; \n
                          out vec4      f_color;	\n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy); \n
        vec3  c0 = texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz; \n
        vec3  c1 = texelFetch(u_tex, coords - ivec2(1, 0), 0).xyz; \n
        vec3  c2 = texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz; \n
        vec3  c3 = texelFetch(u_tex, coords - ivec2(0, 1), 0).xyz; \n
        vec3 Gx = c1 - c0; \n
        vec3 Gy = c2 - c3; \n
        f_color = vec4(sqrt(Gx.xyz * Gx.xyz + Gy.xyz * Gy.xyz), 1.0); //Magnitude
    }\n
                      );
}

void FilterGLGradient::InitShaders()
{
    FragmentShader();

    technique.initStandard("330", vertex_source, fragment_source, "FilterGLGradient");

    technique.bind();
    technique.setUniform("u_tex",      0);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GRADIENT_HPP */

