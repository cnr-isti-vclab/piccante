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

#ifndef PIC_FILTERING_FILTER_LOCAL_EXTREMA_HPP
#define PIC_FILTERING_FILTER_LOCAL_EXTREMA_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterLocalExtrema class
 */
class FilterLocalExtrema: public Filter
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

        int channels = dst->channels;

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *dst_data = (*dst)(i, j);

                float *img_data  = (*img)(i, j);

                float val = 0.0f;

                for(int c = 0; c < channels; c++) {
                    val += img_data[c];
                }

                int counter_higher = 0;
                int counter_lower = 0;
                for(int k = -halfKernelSize; k <= halfKernelSize; k++) {
                    for(int l = -halfKernelSize; l <= halfKernelSize; l++) {
                        if(l == k) {
                            continue;
                        }

                        float *img_data_lk  = (*img)(i + l, j + k);

                        //accumulation
                        float val_lk = 0.0f;
                        for(int c = 0; c < channels; c++) {
                            val_lk += img_data_lk[c];
                        }

                        if(val_lk >= val) {
                            counter_higher++;
                        }

                        if(val_lk <= val) {
                            counter_lower++;
                        }
                    }
                }

                if(counter_higher < kernelSize) {
                    dst_data[0] = 1.0f;
                } else {
                    if(counter_lower < kernelSize) {
                        dst_data[0] = -1.0f;
                    } else {
                        dst_data[0] = 0.0f;
                    }
                }
            }
        }
    }

    int kernelSize, halfKernelSize;

public:

    /**
     * @brief FilterLocalExtrema
     */
    FilterLocalExtrema(int kernelSize = 3) : Filter()
    {
        if(kernelSize < 2) {
            kernelSize = 3;
        }

        if((kernelSize % 2) == 0) {
            kernelSize++;
        }

        this->kernelSize = kernelSize;
        this->halfKernelSize = kernelSize >> 1;
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
        width       = imgIn[0]->width;
        height      = imgIn[0]->height;
        channels    = 1;
        frames      = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param img
     * @param conv
     * @param imgOut
     * @return
     */
    static Image *execute(Image *img, Image *imgOut, int kernelSize = 3)
    {
        FilterLocalExtrema flt(kernelSize);
        return flt.Process(Single(img), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LOCAL_EXTREMA_HPP */

