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

#ifndef PIC_ALGORITHMS_ESTIMATE_NOISE
#define PIC_ALGORITHMS_ESTIMATE_NOISE

#include "../base.hpp"

#include "../filtering/filter_conv_2d.hpp"



namespace pic {

/**
 * @brief estimateNoise
 * @param img
 * @param noise
 * @return
 */
float *estimateNoise(Image *img, float *noise = NULL)
{
    float * data = new float[9];

    data[0] = 1.0f;
    data[1] = -2.0f;
    data[2] = 1.0f;

    data[3] = -2.0f;
    data[4] = 4.0f;
    data[5] = -2.0f;

    data[6] = 1.0f;
    data[7] = -2.0f;
    data[8] = 1.0f;

    Image conv(1, 3, 3, 1, data);

    FilterConv2D flt;

    auto *tmp = flt.Process(Double(img, &conv), NULL);

    tmp->applyFunction(square);

    BBox box = tmp->getFullBox();

    box.x0++;
    box.x1--;
    box.y0++;
    box.y1--;

    noise = tmp->getMeanVal(&box, noise);

    Arrayf::div(noise, tmp->channels, 36.0f);

    delete tmp;
    delete data;

    return noise;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_ESTIMATE_NOISE */

