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

#ifndef PIC_FILTERING_FILTER_CONV_2D_HPP
#define PIC_FILTERING_FILTER_CONV_2D_HPP

#include "../util/array.hpp"

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterConv2D class
 */
class FilterConv2D: public Filter
{
protected:

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        Image *img  = src[0];
        Image *conv = src[1];

        int channels = dst->channels;

        int c_w_h = (conv->width >> 1);
        int c_h_h = (conv->height >> 1);

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *dst_data = (*dst)(i, j);

                Arrayf::assign(0.0f, dst_data, channels);

                for(int k = -c_h_h; k <= c_h_h; k++) {
                    for(int l = -c_w_h; l <= c_w_h; l++) {

                        float *img_data  = (*img)(i + l, j + k);
                        float *conv_data = (*conv)(l + c_w_h, k + c_h_h);

                        for(int c = 0; c < channels; c++) {
                            int c2 = c % conv->channels;
                            dst_data[c] += img_data[c] * conv_data[c2];
                        }
                    }
                }
            }
        }
    }

public:

    /**
     * @brief FilterConv2D
     */
    FilterConv2D() : Filter()
    {
        minInputImages = 2;
    }

    /**
     * @brief execute
     * @param img
     * @param conv
     * @param imgOut
     * @return
     */
    static Image *execute(Image *img, Image *conv, Image *imgOut)
    {
        FilterConv2D flt;
        return flt.Process(Double(img, conv), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_2D_HPP */

