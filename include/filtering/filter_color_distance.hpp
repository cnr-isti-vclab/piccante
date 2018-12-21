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

#ifndef PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP
#define PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP

#include "../filtering/filter.hpp"
#include "../util/array.hpp"

namespace pic {

/**
 * @brief The FilterColorDistance class
 */
class FilterColorDistance: public Filter
{
protected:
    float *color, sigma, sigma_sq_2;

    /**
     * @brief f
     * @param data
     */
    void f(FilterFData *data)
    {
        float *in = (*data->src[0])(data->x, data->y);

        float sum = Arrayf::distanceSq(in, color, data->dst->channels);

        data->out[0] = expf(- sum / sigma_sq_2);
    }

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    /*
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int width = dst->width;
        int channels = src[0]->channels;
        float *data = src[0]->data;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int c1 = (c + i);
                int c2 = c1 * channels;

                float sum = 0.0f;

                for(int k = 0; k < channels; k++) {
                    float tmp = data[c2 + k] - color[k];
                    sum += tmp * tmp;
                }

                dst->data[c1] = expf(-sum / sigma_sq_2);
            }
        }
    }
    */

public:

    /**
     * @brief FilterColorDistance
     * @param color
     * @param sigma
     */
    FilterColorDistance(float *color, float sigma) : Filter()
    {
        update(color, sigma);
    }

    /**
     * @brief update
     * @param color
     * @param sigma
     */
    void update(float *color, float sigma)
    {
        if(color != NULL) {
            this->color = color;
        }

        sigma = sigma > 0.0f ? sigma : 1.0f;
        this->sigma = sigma;
        sigma_sq_2 = sigma * sigma * 2.0f;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width = imgIn[0]->width;
        height = imgIn[0]->height;
        channels = 1;
        frames = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param color
     * @param sigma
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float *color,
                             float sigma)
    {
        FilterColorDistance fltColDst(color, sigma);
        return fltColDst.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP */

