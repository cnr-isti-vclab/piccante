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

#ifndef PIC_FILTERING_FILTER_MAX_HPP
#define PIC_FILTERING_FILTER_MAX_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterMax class
 */
class FilterMax: public Filter
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
        
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {

                float *dst_data = (*dst)(i, j);
                float *src_data = (*src[0])(i, j);

                for(int k = 0; k < channels; k++) {
                    dst_data[k] = src_data[k];
                }

                for(int k = -halfSize; k <= halfSize; k++) {
                    for(int l = -halfSize; l <= halfSize; l++) {

                        src_data = (*src[0])(i + l, j + k);

                        for(int ch = 0; ch < channels; ch++) {
                            dst_data[ch] = dst_data[ch] > src_data[ch] ?
                                           dst_data[ch] : src_data[ch];
                        }
                    }
                }

            }
        }
    }

public:

    /**
     * @brief FilterMax
     * @param size
     */
    FilterMax(int size) : Filter()
    {
        update(size);
    }

    ~FilterMax()
    {
        release();
    }

    /**
     * @brief update
     * @param size
     */
    void update(int size)
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
        FilterMax filter(size);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MAX_HPP */

