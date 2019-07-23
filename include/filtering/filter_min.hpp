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

#include "../filtering/filter.hpp"

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
        int channels = dst->channels;
        float *minVal = new float[channels];

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {

                float *dst_data = (*dst)(i, j);
                float *src_data = (*src[0])(i, j);

                for(int k = 0; k < channels; k++) {
                    minVal[k] = src_data[k];
                }

                for(int k = -halfSize; k <= halfSize; k++) {
                    for(int l = -halfSize; l <= halfSize; l++) {

                        src_data = (*src[0])(i + l, j + k);

                        for(int ch = 0; ch < channels; ch++) {
                            minVal[ch] = minVal[ch] < src_data[ch] ?
                                         minVal[ch] : src_data[ch];
                        }
                    }
                }

                for(int k = 0; k < channels; k++) {
                    dst_data[k] = minVal[k];
                }
            }
        }

        delete[] minVal;
    }

public:

    /**
     * @brief FilterMin
     * @param size
     */
    FilterMin(int size) : Filter()
    {
        this->halfSize = checkHalfSize(size);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int size)
    {
        FilterMin filter(size);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MIN_HPP */

