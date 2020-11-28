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

#ifndef PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP
#define PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP

#include "../base.hpp"
#include "../image.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_simple_tmo.hpp"
#include "../histogram.hpp"

namespace pic {

/**
 * @brief findBestExposureHistogram computes the best exposure value for an image, img,
 * @param img
 * @return It returns the exposure value in f-stops.
 */
PIC_INLINE float findBestExposureHistogram(Image *img)
{
    if(img == NULL) {
        return 0.0f;
    }

    if(!img->isValid()) {
        return 0.0f;
    }

    Image *lum = NULL;

    if(img->channels == 1) {
        lum = img;
    } else {
        lum = FilterLuminance::execute(img, NULL, LT_CIE_LUMINANCE);
    }

    Histogram hist(lum, VS_LOG_2, 4096, 0);

    std::vector<float> fstops = hist.exposureCovering(8);

    if(img->channels != 1) {
        delete lum;
    }

    return fstops[0];
}

/**
 * @brief findBestExposureMean
 * @param img
 * @return It returns the exposure value in f-stops.
 */
PIC_INLINE float findBestExposureMean(Image *img, bool bMedian = false)
{
    if(img == NULL) {
        return 0.0f;
    }

    if(!img->isValid()) {
        return 0.0f;
    }

    Image *lum = NULL;

    if(img->channels == 1) {
        lum = img;
    } else {
        lum = FilterLuminance::execute(img, NULL, LT_CIE_LUMINANCE);
    }

    float lum_mean;
    if(bMedian) {
        lum->getMeanVal(NULL, &lum_mean);
    } else {
        lum->getMeanVal(NULL, &lum_mean);
    }

    float fstop = -log2f(lum_mean) - 1.0f;

    if(img->channels != 1) {
        delete lum;
    }

    return fstop;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP */
