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

#ifndef PIC_ALGORITHMS_WHITE_BALANCE_HPP
#define PIC_ALGORITHMS_WHITE_BALANCE_HPP

#include "../base.hpp"

#include "../filtering/filter_white_balance.hpp"

namespace pic {

/**
 * @brief applyWhiteBalance
 * @param img
 * @param white_color
 * @return
 */
PIC_INLINE Image* applyWhiteBalance(Image *img, float *white_color)
{
    if(img == NULL || white_color == NULL) {
        return NULL;
    }

    FilterWhiteBalance flt_wb(white_color, img->channels, true);
    Image *img_wb = flt_wb.ProcessP(Single(img), NULL);

    return img_wb;
}

/**
 * @brief applyWhiteBalance
 * @param img
 * @param x
 * @param y
 * @param bRobust
 * @return
 */
PIC_INLINE Image* applyWhiteBalance(Image *img, int x, int y, bool bRobust = false)
{
    if(img == NULL) {
        return NULL;
    }

    float *white_color;

    int patchSize = 5;

    if(!bRobust) {
        white_color = (*img)(x, y);
    } else {
        BBox patch(x - patchSize, x + patchSize, y - patchSize, y + patchSize);
        white_color = img->getMeanVal(&patch, NULL);
    }

    Image *out = applyWhiteBalance(img, white_color);

    if(bRobust) {
        delete[] white_color;
    }

    return out;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_WHITE_BALANCE_HPP */

