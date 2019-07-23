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

#ifndef PIC_FILTERING_FILTER_MED_HPP
#define PIC_FILTERING_FILTER_MED_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterMed class
 */
class FilterMed: public Filter
{
protected:
    int halfSize, areaKernel, midValue;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        Image *in = src[0];
        float *values = new float[areaKernel * in->channels];

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {

                int c = 0;
                for(int k = -halfSize; k <= halfSize; k++) {
                    for(int l = -halfSize; l <= halfSize; l++) {

                        float *color = (*in)(i + l, j + k);

                        for(int ch = 0; ch < in->channels; ch++) {
                            values[areaKernel * ch + c] = color[ch];
                        }

                        c++;
                    }
                }

                float *out = (*dst) (i, j);

                for(int ch = 0; ch < in->channels; ch++) {
                    float *tmp_v_ch = &values[areaKernel * ch];
                    std::sort(tmp_v_ch, tmp_v_ch + areaKernel);

                    out[ch] = tmp_v_ch[midValue];
                }
            }
        }

        delete[] values;
    }

public:
    /**
     * @brief FilterMed
     * @param size
     */
    FilterMed(int size) : Filter()
    {
        update(size);
    }

    /**
     * @brief update
     * @param size
     */
    void update(int size)
    {
        this->halfSize = checkHalfSize(size);
        size = (halfSize << 1) + 1;
        this->areaKernel = size * size;
        this->midValue = areaKernel >> 1;
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
        FilterMed filter(size);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MED_HPP */

