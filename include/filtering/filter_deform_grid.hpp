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

#ifndef PIC_FILTERING_FILTER_DEFORM_GRID_HPP
#define PIC_FILTERING_FILTER_DEFORM_GRID_HPP

#include "../util/vec.hpp"

#include "../util/std_util.hpp"

#include "../filtering/filter.hpp"
#include "../image_samplers/image_sampler_bicubic.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"
#include "../image_samplers/image_sampler_nearest.hpp"

namespace pic {

/**
 * @brief The FilterDeformGrid class
 */
class FilterDeformGrid: public Filter
{
protected:
    ImageSamplerBicubic isb;
    ImageSamplerNearest isb_lin;
    Image *grid_rest, *grid_move, grid_diff;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        float vDiff[3];
        for(int j = box->y0; j < box->y1; j++) {
            float y = float(j) / dst->height1f;

            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_dst = (*dst)(i, j);

                float x = float(i) / dst->width1f;

                isb.SampleImage(&grid_diff, x, y, vDiff);

                isb.SampleImage(src[0], x + vDiff[0], y + vDiff[1] , tmp_dst);
            }
        }
    }

public:

    /**
     * @brief FilterDeformGrid
     * @param type
     */
    FilterDeformGrid(Image *grid_move)
    {
        this->grid_rest = getUniformGrid(grid_move->width, grid_move->height);
        this->grid_move = grid_move;

        grid_diff = *grid_rest - *grid_move;
    }

    ~FilterDeformGrid()
    {
        delete_s(grid_rest);
    }

    /**
     * @brief getUniformGrid
     * @param sampleX
     * @param sampleY
     * @return
     */
    static Image* getUniformGrid(int sampleX, int sampleY)
    {
        if(sampleX < 1) {
            sampleX = 5;
        }

        if(sampleY < 1) {
            sampleY = 5;
        }

        //the grid has sampleX \times sampleY squares, so it has to have
        //some one extra control point for each direction

        Image *ret = new Image(1, sampleX, sampleY, 3);

        float tmp_x = 1.0f / float(sampleX - 1);
        float tmp_y = 1.0f / float(sampleY - 1);

        for(int y = 0; y < sampleY; y++) {
            float y_f = float(y) * tmp_y;

            for(int x = 0; x < sampleX; x++) {
                float *ret_val = (*ret)(x, y);

                ret_val[0]= float(x) * tmp_x;
                ret_val[1]= y_f;
            }
        }

        return ret;
    }

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

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LUMINANCE_HPP */

