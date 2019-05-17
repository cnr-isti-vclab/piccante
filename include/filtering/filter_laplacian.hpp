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

#ifndef PIC_FILTERING_FILTER_LAPLACIAN_HPP
#define PIC_FILTERING_FILTER_LAPLACIAN_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterLaplacian class
 */
class FilterLaplacian: public Filter
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
        int channels = src[0]->channels;

        Image *in = src[0];

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *cur = (*in)(i, j);
                float *out = (*dst)(i, j);

                //neighbors
                float *N = (*in)(i    , j + 1);
                float *S = (*in)(i    , j - 1);
                float *E = (*in)(i + 1, j);
                float *W = (*in)(i - 1, j);

                for(int k = 0; k < channels; k++) {
                    out[k] = (-4.0f * cur[k]) + N[k] + S[k] + E[k] + W[k];
                }
            }
        }
    }

public:
    /**
     * @brief FilterLaplacian
     */
    FilterLaplacian() : Filter()
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
        FilterLaplacian filter;
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LAPLACIAN_HPP */

