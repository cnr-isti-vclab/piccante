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

#ifndef PIC_FILTERING_FILTER_NSWE_HPP
#define PIC_FILTERING_FILTER_NSWE_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterNSWE class
 */
class FilterNSWE: public Filter
{
protected:
    /**
     * @brief f
     * @param data
     */
    void f(FilterFData *data)
    {
        float *img_data  = (*data->src[0])(data->x    , data->y);
        float *img_dataN = (*data->src[0])(data->x + 1, data->y);
        float *img_dataS = (*data->src[0])(data->x - 1, data->y);
        float *img_dataW = (*data->src[0])(data->x    , data->y - 1);
        float *img_dataE = (*data->src[0])(data->x    , data->y + 1);

        for(int k = 0; k < data->src[0]->channels; k++) {
            int tmp = k << 2;
            data->out[tmp    ] = img_dataN[k] - img_data[k];
            data->out[tmp + 1] = img_dataS[k] - img_data[k];
            data->out[tmp + 2] = img_dataW[k] - img_data[k];
            data->out[tmp + 3] = img_dataE[k] - img_data[k];
        }
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
        Image *img = src[0];
        int channels = img->channels;

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *dst_data   = (*dst)(i  , j);

                float *img_data  = (*img)(i    , j);
                float *img_dataN = (*img)(i + 1, j);
                float *img_dataS = (*img)(i - 1, j);
                float *img_dataW = (*img)(i    , j - 1);
                float *img_dataE = (*img)(i    , j + 1);

                for(int k = 0; k < channels; k++) {
                    int tmp = k * 4;
                    dst_data[tmp    ] = img_dataN[k] - img_data[k];
                    dst_data[tmp + 1] = img_dataS[k] - img_data[k];
                    dst_data[tmp + 2] = img_dataW[k] - img_data[k];
                    dst_data[tmp + 3] = img_dataE[k] - img_data[k];
                }
            }
        }
    }
    */

public:
    /**
     * @brief FilterNSWE
     */
    FilterNSWE() : Filter()
    {

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
        width    = imgIn[0]->width;
        height   = imgIn[0]->height;
        channels = imgIn[0]->channels << 2;
        frames   = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FilterNSWE filter;
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NSWE_HPP */

