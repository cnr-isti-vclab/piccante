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

#ifndef PIC_FILTERING_FILTER_ZERO_CROSSING_HPP
#define PIC_FILTERING_FILTER_ZERO_CROSSING_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterZeroCrossing class
 */
class FilterZeroCrossing: public Filter
{
protected:

    /**
     * @brief f
     * @param data
     */
    void f(FilterFData *data)
    {

        float value;

        float *data_src  = (*data->src[0])(data->x, data->y);
        float *data_src0 = (*data->src[0])(data->x, data->y + 1);
        float *data_src1 = (*data->src[0])(data->x + 1, data->y);

        for(int k = 0; k < data->src[0]->channels; k++) {
            value = (data_src[k] == 0.0f) ? 1.0f : 0.0f;
            value = (data_src[k] > 0.0f && data_src0[k] < 0.0f) ? 1.0f : value;
            value = (data_src[k] < 0.0f && data_src0[k] > 0.0f) ? 1.0f : value;
            value = (data_src[k] > 0.0f && data_src1[k] < 0.0f) ? 1.0f : value;
            value = (data_src[k] < 0.0f && data_src1[k] > 0.0f) ? 1.0f : value;
            data->out[k] = value;
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
        Image *src_p = src[0];

        int channels = src_p->channels;
        float value;

        for(int i = box->y0; i < box->y1; i++) {

            for(int j = box->x0; j < box->x1; j++) {

                float *data_src = (*src_p)(j, i);
                float *data_dst = (*dst)(j, i);

                float *data_src0 = (*src_p)(j, i + 1);
                float *data_src1 = (*src_p)(j + 1, i);

                for(int k = 0; k < channels; k++) {
                    value = (data_src[k] == 0.0f) ? 1.0f : 0.0f;
                    value = (data_src[k] > 0.0f && data_src0[k] < 0.0f) ? 1.0f : value;
                    value = (data_src[k] < 0.0f && data_src0[k] > 0.0f) ? 1.0f : value;
                    value = (data_src[k] > 0.0f && data_src1[k] < 0.0f) ? 1.0f : value;
                    value = (data_src[k] < 0.0f && data_src1[k] > 0.0f) ? 1.0f : value;
                    data_dst[k] = value;
                }
            }
        }
    }
    */

public:

    /**
     * @brief FilterZeroCrossing
     * @param type
     */
    FilterZeroCrossing() : Filter()
    {
    }

    ~FilterZeroCrossing()
    {
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FilterZeroCrossing flt;
        return flt.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ZERO_CROSSING_HPP */

