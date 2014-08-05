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

#ifndef PIC_TONE_MAPPING_HISTOGRAM_TMO_HPP
#define PIC_TONE_MAPPING_HISTOGRAM_TMO_HPP

#include <vector>
#include "image_raw.hpp"
#include "filtering/filter_luminance.hpp"

namespace pic {

inline ImageRAW *HistogramTMO(ImageRAW *imgOut, ImageRAW *imgIn)
{
    if(imgOut == NULL) {
        imgOut = imgIn->Clone();
    }

    ImageRAW *lum    = FilterLuminance::Execute(imgIn, NULL, LT_CIE_LUMINANCE);	//Luminance
    ImageRAW *lumOld = lum->Clone();
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

    imgOut->changeLum(lumOld, lum);

    delete lum;
    delete lumOld;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_HISTOGRAM_TMO_HPP */

