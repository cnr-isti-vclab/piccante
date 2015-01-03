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

#ifndef PIC_FILTERING_FILTER_MIN_HPP
#define PIC_FILTERING_FILTER_MIN_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterMin class
 */
class FilterMin: public Filter
{
protected:
    int halfSize;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int width = dst->width;
        int channels = dst->channels;

        float *data = src[0]->data;

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                int c = (j * width + i) * channels;

                for(int ch = 0; ch < channels; ch++) {
                    float minVal = FLT_MAX;

                    for(int k = -halfSize; k <= halfSize; k++) {
                        int ci = CLAMP((i + k), dst->width);

                        for(int l = -halfSize; l <= halfSize; l++) {
                            int cj = CLAMP((j + l), dst->height);
                            float tmp = data[(cj * width + ci) * channels + ch];
                            minVal = minVal > tmp ? tmp : minVal;
                        }
                    }

                    dst->data[c + ch] = minVal;
                }
            }
        }
    }

public:

    /**
     * @brief FilterMin
     * @param size
     */
    FilterMin(int size)
    {
        this->halfSize = checkHalfSize(size);
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, int size)
    {
        FilterMed filter(size);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param size
     * @return
     */
    static Image *Execute(std::string nameIn, std::string nameOut, int size)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, size);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MIN_HPP */

