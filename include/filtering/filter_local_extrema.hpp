/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_FILTERING_FILTER_LOCAL_EXTREMA_HPP
#define PIC_FILTERING_FILTER_LOCAL_EXTREMA_HPP

#include "filtering/filter.hpp"

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
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
    {
        ImageRAW *img  = src[0];

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

                        //Accumulation
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

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new ImageRAW(1, imgIn[0]->width, imgIn[0]->height, 1);
        } else {
            if((imgIn[0]->width  != imgOut->width)  ||
               (imgIn[0]->height != imgOut->height) ||
               (imgOut->channels != 1)) {
                imgOut = new ImageRAW(1, imgIn[0]->width, imgIn[0]->height, 1);
            }
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterLocalExtrema
     */
    FilterLocalExtrema(int kernelSize = 3)
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
     * @brief Execute
     * @param img
     * @param conv
     * @param imgOut
     * @return
     */
    static ImageRAW *Execute(ImageRAW *img, ImageRAW *imgOut, int kernelSize = 3)
    {
        FilterLocalExtrema flt(kernelSize);
        return flt.ProcessP(Single(img), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LOCAL_EXTREMA_HPP */

