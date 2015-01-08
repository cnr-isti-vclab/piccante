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

#ifndef PIC_TONE_MAPPING_HISTOGRAM_TMO_HPP
#define PIC_TONE_MAPPING_HISTOGRAM_TMO_HPP

#include <vector>
#include "image.hpp"
#include "filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief HistogramTMO
 * @param imgOut
 * @param imgIn
 * @return
 */
inline Image *HistogramTMO(Image *imgOut, Image *imgIn)
{
    if(imgOut == NULL) {
        imgOut = imgIn->Clone();
    }

    Image *lum    = FilterLuminance::Execute(imgIn, NULL, LT_CIE_LUMINANCE);	//Luminance
    Image *lumOld = lum->Clone();
    lum->sort();

    int size = lum->width * lum->height * lum->frames;

    float table[257];

    for(int i = 1; i <= 256; i++) {
        table[i] = lum->getMedVal(float(i) / 256.0f);
    }

    table[0] = lum->getMinVal()[0];

    std::vector<float> v(table, table + 257);

    for(int i = 0; i < size; i++) {
        std::vector<float>::iterator low = std::lower_bound(v.begin(), v.end(),
                                           lum->data[i]);
        lum->data[i] = powf(float(low - v.begin()) / 256.0f, 2.2f);
    }

    *imgOut /= *lumOld;
    *imgOut *= *lum;

    imgOut->removeSpecials();

    delete lum;
    delete lumOld;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_HISTOGRAM_TMO_HPP */

