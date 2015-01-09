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

#include "filtering/filter.hpp"

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
        if(src.size() != 2) {
            return;
        }

        Image *img  = src[0];
        Image *conv = src[1];

        int channels = dst->channels;

        int c_width	 = conv->width;
        int c_height = conv->height;

        int c_width_h  = (c_width >> 1);
        int c_height_h = (c_height >> 1);

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *dst_data = (*dst)(i, j);

                for(int c = 0; c < channels; c++) {
                    dst_data[c] = 0.0f;
                }

                for(int k = -c_height_h; k <= c_height_h; k++) {
                    for(int l = -c_width_h; l <= c_width_h; l++) {

                        float *img_data  = (*img)(i + l, j + k);
                        float kernel_val = (*conv)(l + c_width_h, k + c_height_h)[0];

                        //Accumulation
                        for(int c = 0; c < channels; c++) {
                            dst_data[c] += img_data[c] * kernel_val;
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
    FilterConv2D()
    {

    }

    /**
     * @brief Execute
     * @param img
     * @param conv
     * @param imgOut
     * @return
     */
    static Image *Execute(Image *img, Image *conv, Image *imgOut)
    {
        FilterConv2D flt;
        return flt.ProcessP(Double(img, conv), imgOut);
    }

    /**
     * @brief Execute
     * @param nameImg
     * @param nameConv
     * @param nameOut
     */
    static void Execute(std::string nameImg, std::string nameConv,
                        std::string nameOut)
    {
        Image img(nameImg, LT_NOR_GAMMA);
        Image conv(nameConv, LT_NOR_GAMMA);

        float *sumVal = conv.getSumVal(NULL, NULL);
        conv /= sumVal[0];

        Image *imgOut = Execute(&img, &conv, NULL);

        imgOut->Write(nameOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_2D_HPP */

