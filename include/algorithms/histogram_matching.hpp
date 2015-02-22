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

#ifndef PIC_ALGORITHMS_HISTOGRAM_MATCHING_HPP
#define PIC_ALGORITHMS_HISTOGRAM_MATCHING_HPP

#include "image.hpp"
#include "histogram.hpp"

namespace pic {

/**
 * @brief histogramMatching
 * @param img_source is the input image
 * @param img_target is the image with target colors
 * @param out is
 * @return It returns out.
 */
Image *histogramMatching(Image* img_source, Image* img_target, Image* out = NULL)
{
    if(img_source == NULL || img_target == NULL) {
        return out;
    }

    if(img_source->channels != img_target->channels) {
        return out;
    }

    int channels = img_source->channels;

    Histogram *h_source = new Histogram[channels];
    Histogram *h_target = new Histogram[channels];

    for(int i=0;i <channels; i++) {
        h_source[i].Calculate(img_source, VS_LIN, 256, i);
        h_target[i].Calculate(img_target, VS_LIN, 256, i);
    }

    return out;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_HISTOGRAM_MATCHING_HPP */

