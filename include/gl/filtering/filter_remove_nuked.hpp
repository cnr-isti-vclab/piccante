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

#ifndef PIC_GL_FILTERING_FILTER_REMOVE_NUKED_HPP
#define PIC_GL_FILTERING_FILTER_REMOVE_NUKED_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLRemoveNuked class
 */
class FilterGLRemoveNuked: public FilterGL
{
protected:
    float threshold;

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
     * @brief FilterGLRemoveNuked
     * @param threshold
     */
    FilterGLRemoveNuked(float threshold);

    /**
     * @brief Update
     * @param threshold
     */
    void Update(float threshold);
};

FilterGLRemoveNuked::FilterGLRemoveNuked(float threshold): FilterGL()
{
    this->threshold = threshold;

    FragmentShader();
    InitShaders();
}

void FilterGLRemoveNuked::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D	u_tex;
                          uniform float		threshold;
                          out     vec4		f_color;

    void main(void) {

        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);

        float ref = texelFetch(u_tex, coordsFrag, 0).x;
        float tmp;

        int count = 0;

        vec3 colorNuked = vec3(0.0);

        for(int i = -2; i < 3; i++) {
            for(int j = -2; j < 3; j++) {
                //Coordinates
                ivec2 coords = ivec2(i, j);
                //Texture fetch
                tmp = texelFetch(u_tex, coordsFrag.xy + coords.xy, 0).x;
                float tmp2 = abs(tmp - ref);

                if(tmp2 > threshold) {
                    count = count + 1;
                    colorNuked = vec3(tmp);
                }
            }\n
        }\n

        vec3 color = vec3(ref);
        \n

        if(count > 12)\n
            color = colorNuked;

        \n
        f_color = vec4(color, 1.0);
        \n
    }\n
                      );
}

void FilterGLRemoveNuked::InitShaders()
{
    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLRemoveNuked log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update(-1.0f);
}

void FilterGLRemoveNuked::Update(float threshold)
{
    if(threshold > 0.0f) {
        this->threshold = threshold;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("threshold",  this->threshold);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_REMOVE_NUKED_HPP */

