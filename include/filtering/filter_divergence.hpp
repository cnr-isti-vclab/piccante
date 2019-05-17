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

#ifndef PIC_FILTERING_FILTER_DIVERGENCE_HPP
#define PIC_FILTERING_FILTER_DIVERGENCE_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterDivergence class
 */
class FilterDivergence: public Filter
{
protected:

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    /**
     * @brief FilterDivergence
     */
    FilterDivergence()
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
        FilterDivergence filter;
        return filter.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE void FilterDivergence::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int width = dst->width;
    int height = dst->height;

    float *data = src[0]->data;
    float gradX, gradY;

    int channels = src[0]->channels;
    int c, ci, cj, ci1, cj1, tmpc, ind;

    for(int j = box->y0; j < box->y1; j++) {
        ind = j * width;

        for(int i = box->x0; i < box->x1; i++) {
            c = (ind + i) * channels;
            //Positions
            ci  = CLAMP(i + 1, width);
            cj  = CLAMP(j + 1, height);
            ci1 = CLAMP(i - 1, width);
            cj1 = CLAMP(j - 1, height);

            //Grad X
            tmpc  = (ind + ci) * channels;
            gradX = data[tmpc];

            tmpc  = (ind + ci1) * channels;
            gradX -= data[tmpc];

            //Grad Y
            tmpc  = (cj * width + i) * channels;
            gradY = data[tmpc];

            tmpc  = (cj1 * width + i) * channels;
            gradY -= data[tmpc];

            //Divergence
            dst->data[c] = (gradX + gradY) * 0.5f;
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DIVERGENCE_HPP */

