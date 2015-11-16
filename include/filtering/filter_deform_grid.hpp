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

#include "filtering/filter.hpp"
#include "image_samplers/image_sampler_bsplines.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"
#include "util/vec.hpp"

namespace pic {

/**
 * @brief The FilterDeformGrid class
 */
class FilterDeformGrid: public Filter
{
protected:
//    ImageSamplerBilinear    isb;
    ImageSamplerBSplines   isb;
    Image *grid_rest, *grid_move, grid_diff;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        float *data  = src[0]->data;

        float dx = 1.0f / grid_diff.widthf;
        float dy = 1.0f / grid_diff.heightf;

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_dst = (*dst)(i, j);

                float x = float(i) / dst->widthf;
                float y = float(j) / dst->heightf;

                float vDiff[2];

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
    }

    static Image* getUniformGrid(int sampleX, int sampleY)
    {
        if(sampleX < 1) {
            sampleX = 5;
        }

        if(sampleY < 1) {
            sampleY = 5;
        }

        Image *ret = new Image(1, sampleX, sampleY, 2);

        float tmp_x = 1.0f / float(sampleX - 1);
        float tmp_y = 1.0f / float(sampleY - 1);

        for(int y = 0; y < sampleY; y++) {
            int ind = y * sampleX;

            float y_f = float(y) * tmp_y;

            for(int x = 0; x < sampleX; x++) {
                int ind1 = ind + x;
                float *ret_val = (*ret)(x, y);

                ret_val[0]= float(x) * tmp_x;
                ret_val[1]= y_f;
            }
        }

        return ret;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LUMINANCE_HPP */

