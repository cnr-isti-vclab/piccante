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

#include "../base.hpp"

#include "../image.hpp"
#include "../histogram.hpp"

namespace pic {

/**
 * @brief matchHistograms
 * @param img_source is the input image
 * @param img_target is the image with target colors
 * @param out is
 * @return It returns out.
 */
PIC_INLINE Image *matchHistograms(Image* img_source, Image* img_target, int nBin = 256, Image* out = NULL)
{
    if(img_source == NULL || img_target == NULL) {
        return out;
    }

    if(img_source->channels != img_target->channels) {
        return out;
    }

    if(nBin < 1) {
        nBin = 256;
    }

    int channels = img_source->channels;

    Histogram *h_source = new Histogram[channels];
    Histogram *h_target = new Histogram[channels];

    std::vector<int *> lut;

    for(int i=0; i<channels; i++) {
        h_source[i].calculate(img_source, VS_LIN, nBin, i);
        h_target[i].calculate(img_target, VS_LIN, nBin, i);

        h_source[i].cumulativef(true);
        h_target[i].cumulativef(true);

        float *c_source = h_source[i].getCumulativef();
        float *c_target = h_target[i].getCumulativef();

        int *tmp_lut = new int[nBin];

        for(int j=0; j<nBin; j++) {
            float x = c_source[j];
            float *ptr = std::upper_bound(c_target, c_target + nBin, x);
            tmp_lut[j] = MAX((int)(ptr - c_target), 0);
        }

        lut.push_back(tmp_lut);
    }

    if(out == NULL) {
        out = img_source->clone();
    }

    for(int i=0; i<out->size(); i+=channels) {

        for(int j=0; j<channels; j++) {
            int ind_source = h_source[j].project(img_source->data[i + j]);

            int ind_target = lut[j][ind_source];

            out->data[i + j] = h_target[j].unproject(ind_target);
        }
    }

    return out;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_HISTOGRAM_MATCHING_HPP */

