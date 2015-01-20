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

#ifndef PIC_TONE_MAPPING_GET_ALL_EXPOSURES_HPP
#define PIC_TONE_MAPPING_GET_ALL_EXPOSURES_HPP

#include "image.hpp"
#include "histogram.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_simple_tmo.hpp"

namespace pic {

/**
 * @brief getAllExposures converts an HDR image into a stack of exposure values
 * @param imgIn
 * @return
 */
std::vector<float> getAllExposures(Image *imgIn) {
    std::vector<float> exposures;

    if(imgIn == NULL) {
        return exposures;
    }

    if(!imgIn->isValid()) {
        return exposures;
    }

    Image *lum = FilterLuminance::Execute(imgIn, NULL);

    Histogram m(lum, VS_LOG_2, 1024);
    exposures = m.ExposureCovering();

    delete lum;

    return exposures;
}

/**
 * @brief getAllExposuresImages converts an HDR image into a stack of LDR images
 * @param imgIn
 * @return
 */
ImageVec getAllExposuresImages(Image *imgIn)
{
    ImageVec ret;

    std::vector<float> exposures = getAllExposures(imgIn);

    FilterSimpleTMO flt(1.0f, 0.0f);

    ImageVec input = Single(imgIn);

    for(unsigned int i = 0; i < exposures.size(); i++) {
        flt.Update(2.2f, exposures[i]);
        Image *expo = flt.ProcessP(input, NULL);

        ret.push_back(expo);
    }

    return ret;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_GET_ALL_EXPOSURES_HPP */

