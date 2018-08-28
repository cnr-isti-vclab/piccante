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

#ifndef PIC_FILTERING_FILTER_MED_VEC_HPP
#define PIC_FILTERING_FILTER_MED_VEC_HPP

#include "../filtering/filter.hpp"
#include "../util/array.hpp"

namespace pic {

/**
 * @brief The FilterMedVec class
 */
class FilterMedVec: public Filter
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
        float *dists = new float[areaKernel];

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {

                int c = 0;
                for(int k = -halfSize; k <= halfSize; k++) {
                    for(int l = -halfSize; l <= halfSize; l++) {

                        float *color = (*in)(i + l, j + k);

                        for(int ch = 0; ch < in->channels; ch++) {
                            values[c * in->channels + ch] = color[ch];
                        }

                        c++;
                    }
                }

                //compute distances
                for(int k = 0; k < areaKernel; k++) {

                    int index_k = k * in->channels;

                    dists[k] = 0.0f;
                    for(int l = 0; l < areaKernel; k++) {
                        int index_l = l * in->channels;
                        float d_sq = Array<float>::distanceSq(&values[index_k], &values[index_l], in->channels);
                        dists[k] += sqrtf(d_sq);
                    }
                }

                //compute the median
                int best = 0;
                int bestCount = -1;
                for(int k = 0; k < areaKernel; k++) {
                    int count = 0;

                    for(int l = 0; l < areaKernel; k++) {
                        if(dists[k] <= dists[l]) {
                            count++;
                        }
                    }

                    if(count > bestCount) {
                        best = k;
                        bestCount = count;
                    }
                }

                float *out = (*dst) (i, j);

                for(int ch = 0; ch < in->channels; ch++) {
                    out[ch] = values[best * in->channels + ch];
                }
            }
        }

        delete[] values;
        delete[] dists;
    }

public:
    /**
     * @brief FilterMedVec
     * @param size
     */
    FilterMedVec(int size)
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
        this->areaKernel = (halfSize * 2 + 1) * (halfSize * 2 + 1);
        this->midValue = areaKernel >> 1;
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
        FilterMedVec filter(size);
        return filter.ProcessP(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MED_VEC_HPP */

