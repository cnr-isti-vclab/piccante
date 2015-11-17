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

#ifndef PIC_GL_FILTERING_FILTER_DEFORM_GRID_HPP
#define PIC_GL_FILTERING_FILTER_DEFORM_GRID_HPP

#include "filtering/filter_DEFORM_GRID.hpp"
#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLDeformGrid class
 */
class FilterGLDeformGrid: public FilterGL
{
protected:

    Image *grid_rest, *grid_move, grid_diff;
    ImageGL *grid_diff_gl;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief FilterGLDeformGrid
     */
    FilterGLDeformGrid(Image *grid_move);

    /**
     * @brief Update
     * @param type
     */
    void Update();
};

FilterGLDeformGrid::FilterGLDeformGrid(Image *grid_move): FilterGL()
{
    this->grid_rest = FilterDeformGrid::getUniformGrid(grid_move->width, grid_move->height);
    this->grid_move = grid_move;

    grid_diff = *grid_rest - *grid_move;

    grid_diff_gl = new ImageGL(&grid_diff, true);
    grid_diff_gl->generateTextureGL();
    param.push_back(grid_diff_gl);

    InitShaders();
}

void FilterGLDeformGrid::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
    uniform sampler2D u_tex; \n
    uniform sampler2D u_grid; \n
    out     vec4 f_color; \n
    \n
    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy); \n
        vec2 shift = texelFetch(u_grid, coords, 0).xy; \n
        vec3 color = texelFetch(u_grid, coords + shift, 0).xy; \n
        float L = dot(color, weights); \n
        f_color = vec4(L, L, L, 1.0); \n
        \n
    }
                      );

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[FilterGLDeformGrid log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update();
}

void FilterGLDeformGrid::Update()
{
    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("u_grid", 1);
    glw::bind_program(0);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_DEFORM_GRID_HPP */

