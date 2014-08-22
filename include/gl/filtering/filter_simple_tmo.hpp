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

#ifndef PIC_GL_FILTERING_FILTER_SIMPLE_TMO_HPP
#define PIC_GL_FILTERING_FILTER_SIMPLE_TMO_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLSimpleTMO class
 */
class FilterGLSimpleTMO: public FilterGL
{
protected:
    float fstop, gamma;

    void InitShaders();

    void FragmentShader();

public:
    /**
     * @brief FilterGLSimpleTMO
     */
    FilterGLSimpleTMO();

    /**
     * @brief FilterGLSimpleTMO
     * @param fstop
     * @param gamma
     */
    FilterGLSimpleTMO(float fstop, float gamma);

    /**
     * @brief Update
     * @param fstop
     * @param gamma
     */
    void Update(float fstop, float gamma);

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param gamma
     * @param fstop
     * @return
     */
    ImageRAWGL *Execute(std::string nameIn, std::string nameOut, float gamma,
                        float fstop)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLSimpleTMO filter(gamma, fstop);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageRAWGL *imgOut = new ImageRAWGL(1, imgIn.width, imgIn.height, 4,
                                            IMG_GPU_CPU, GL_TEXTURE_2D);

        int n = 100;

        for(int i = 0; i < n; i++) {
            filter.Process(SingleGL(&imgIn), imgOut);
        }

        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Full Bilateral Filter on GPU time: %f ms\n",
               double(timeVal) / (double(n) * 1000000.0));

        imgOut->readFromFBO(filter.getFbo());
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLSimpleTMO::FilterGLSimpleTMO(): FilterGL()
{
    gamma = 2.2f;
    fstop = 0.0f;

    FragmentShader();
    InitShaders();
}

FilterGLSimpleTMO::FilterGLSimpleTMO(float fstop, float gamma): FilterGL()
{
    //protected values are assigned/computed
    if(gamma <= 0.0f) {
        gamma = 2.2f;
    }

    this->gamma = gamma;
    this->fstop = fstop;

    FragmentShader();
    InitShaders();
}

void FilterGLSimpleTMO::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex; \n
                          uniform float	  tn_gamma; \n
                          uniform float	  tn_exposure; \n
                          out     vec4      f_color;	\n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec3  color = texelFetch(u_tex, coords, 0).xyz;
        \n
        f_color = vec4(pow(color * tn_exposure, vec3(tn_gamma)), 1.0);
        \n
    }\n
                      );
}

void FilterGLSimpleTMO::InitShaders()
{
    float invGamma = 1.0f / gamma;
    float exposure = powf(2.0f, fstop);

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLSimpleTMO log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();

    filteringProgram.uniform("tn_gamma",   invGamma);
    filteringProgram.uniform("tn_exposure", exposure);
    filteringProgram.uniform("u_tex",      0);
    glw::bind_program(0);
}

void FilterGLSimpleTMO::Update(float fstop, float gamma)
{
    this->gamma = gamma;
    this->fstop = fstop;

    float invGamma = 1.0f / gamma;
    float exposure = powf(2.0f, fstop);

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("tn_gamma",		invGamma);
    filteringProgram.uniform("tn_exposure",	exposure);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SIMPLE_TMO_HPP */

