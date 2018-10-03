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

#ifndef PIC_ALGORITHMS_GROW_CUT_HPP
#define PIC_ALGORITHMS_GROW_CUT_HPP

#include "../base.hpp"

#include "../image.hpp"
#include "../filtering/filter_laplacian.hpp"
#include "../filtering/filter_max.hpp"
#include "../filtering/filter_grow_cut.hpp"

namespace pic {

/**
 * @brief computeGrowCut
 * @param img
 * @param seeds
 * @param state_cur
 * @return
 */
PIC_INLINE Image *computeGrowCut(Image *img, Image *seeds, Image *state_cur = NULL)
{
    if(img == NULL || seeds == NULL) {
        return NULL;
    }

    if(state_cur == NULL) {
        state_cur = new Image(img->width, img->height, 2);
    }

    Image *state_next = state_cur->allocateSimilarOne();

    //compute max
    Image *img_max = FilterMax::execute(img, NULL, 5);

    for(int i = 0; i < state_cur->nPixels(); i++) {
        //init state_cur
        int j  = i * state_cur->channels;
        int j2 = i * seeds->channels;
        state_cur->data[j] = seeds->data[j2];
        state_cur->data[j + 1] = seeds->data[j2] > 0.0f ? 1.0f : 0.0f;

        //fix max
        j = i * img_max->channels;
        float C = img_max->data[j] * img_max->data[j];
        for(int c = 1; c < img_max->channels; c++) {
            float tmp = img_max->data[j + c];
            C += tmp * tmp;
        }
        img_max->data[j] = C;
    }

    //iterative filtering...
    int iterations = int(sqrtf(img->widthf * img->widthf + img->heightf * img->heightf));
    if((iterations % 2) == 1) {
        iterations++;
    }

    FilterGrowCut flt;
    ImageVec input = Triple(state_cur, img, img_max);
    Image *output = state_next;

    for(int i = 0; i < iterations; i++) {

        output = flt.ProcessP(input, output);

        Image *tmp = input[0];
        input[0] = output;
        output = tmp;
    }

    delete output;
    delete img_max;

    return input[0];
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_GROW_CUT_HPP */

