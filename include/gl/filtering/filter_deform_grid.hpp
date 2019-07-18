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

#include "../../base.hpp"
#include "../../util/std_util.hpp"
#include "../../util/gl/bicubic.hpp"
#include "../../image_samplers/image_sampler_bicubic.hpp"
#include "../../filtering/filter_deform_grid.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLDeformGrid class
 */
class FilterGLDeformGrid: public FilterGL
{
protected:
    ImageSamplerBicubic isb;

    Image *grid_rest, *grid_move, grid_diff;
    ImageGL *grid_diff_gl;

    /**
     * @brief initShaders
     */
    void initShaders();

public:
    /**
     * @brief FilterGLDeformGrid
     */
    FilterGLDeformGrid(Image *grid_move);

    ~FilterGLDeformGrid();

    /**
     * @brief releaseAux
     */
    void releaseAux();

    /**
     * @brief update
     * @param type
     */
    void update(Image *grid_move);

    /**
     * @brief getCoordinatesAfterTransform
     * @param x is normalized in [0,1]
     * @param y is normalized in [0,1]
     * @param xOut
     * @param yOut
     */
    void getCoordinatesAfterTransform(float x, float y, float &xOut, float &yOut)
    {
        float vDiff[3];
        isb.SampleImage(&grid_diff, x, y, vDiff);

        xOut = x + vDiff[0];
        yOut = y + vDiff[1];
    }
};

PIC_INLINE FilterGLDeformGrid::FilterGLDeformGrid(Image *grid_move): FilterGL()
{
    this->grid_rest = FilterDeformGrid::getUniformGrid(grid_move->width, grid_move->height);
    this->grid_move = grid_move;

    grid_diff = *grid_rest - *grid_move;

    grid_diff_gl = new ImageGL(&grid_diff, true);
    grid_diff_gl->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);
    param.push_back(grid_diff_gl);

    initShaders();
}

PIC_INLINE FilterGLDeformGrid::~FilterGLDeformGrid()
{
    release();
}

PIC_INLINE void FilterGLDeformGrid::releaseAux()
{
    grid_rest = delete_s(grid_rest);
    grid_diff_gl = delete_s(grid_diff_gl);
}

PIC_INLINE void FilterGLDeformGrid::initShaders()
{
    fragment_source  = GLSL_BICUBIC();
    fragment_source += GLSL_TEXTURE_BICUBIC();

    fragment_source += MAKE_STRING
                      (
    uniform sampler2D u_tex; \n
    uniform sampler2D u_grid; \n
    out     vec4 f_color; \n
    \n

    void main(void) {
        vec2 tSize = vec2(textureSize(u_tex, 0));
        vec2 coords = gl_FragCoord.xy / tSize.xy; \n

        vec2 shifts = textureBicubic(u_grid, coords.xy).xy;
        f_color = texture(u_tex, coords + shifts.xy);
    }
                      );

    technique.initStandard("330", vertex_source, fragment_source, "FilterGLDeformGrid");

    update(NULL);
}

PIC_INLINE void FilterGLDeformGrid::update(Image *grid_move)
{
    if(grid_move != NULL) {

        this->grid_move = grid_move;

        grid_diff = *grid_rest;
        grid_diff -= *this->grid_move;

        grid_diff_gl->loadFromMemory();
    }

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1i("u_grid", 1);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_DEFORM_GRID_HPP */

