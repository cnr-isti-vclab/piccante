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

#ifndef PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP
#define PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP

#include "colors/saturation.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_laplacian.hpp"
#include "filtering/filter_exposure_fusion_weights.hpp"
#include "algorithms/pyramid.hpp"

namespace pic {

/**
 * @brief ExposureFusion
 * @param imgIn
 * @param wC
 * @param wE
 * @param wS
 * @param imgOut
 * @return
 */
Image *ExposureFusion(ImageVec imgIn, float wC = 1.0f, float wE = 1.0f,
                      float wS = 1.0f, Image *imgOut = NULL)
{
    int n = imgIn.size();

    if(n < 2) {
        return imgOut;
    }

    //Computing weights values
    int channels = imgIn[0]->channels;
    int width = imgIn[0]->width;
    int height = imgIn[0]->height;

    Image *lum     = new Image(1, width, height, 1);
    Image *weights = new Image(1, width, height, 1);
    Image *acc     = new Image(1, width, height, 1);

    acc->SetZero();

    FilterLuminance flt_lum;
    FilterExposureFusionWeights flt_weights(wC, wE, wS);

    for(int j = 0; j < n; j++) {
        #ifdef PIC_DEBUG
            printf("Processing image %d\n", j);
        #endif

        lum = flt_lum.ProcessP(Single(imgIn[j]), lum);

        weights = flt_weights.ProcessP(Double(lum, imgIn[j]), weights);

        *acc += *weights;
    }

    for(int i=0; i<acc->size(); i++) {
        acc->data[i] = acc->data[i] > 0.0f ? acc->data[i] : 1.0f;
    }

    //Accumulation Pyramid
    #ifdef PIC_DEBUG
        printf("Blending...");
    #endif

    Pyramid *pW   = new Pyramid(width, height, 1, false);
    Pyramid *pI   = new Pyramid(width, height, channels, true);
    Pyramid *pOut = new Pyramid(width, height, channels, true);

    pOut->SetValue(0.0f);

    for(int j = 0; j < n; j++) {
        lum = flt_lum.ProcessP(Single(imgIn[j]), lum);
        weights = flt_weights.ProcessP(Double(lum, imgIn[j]), weights);

        //normalization
        *weights /= *acc;

        pW->Update(weights);
        pI->Update(imgIn[j]);

        pI->Mul(pW);

        pOut->Add(pI);
    }

    #ifdef PIC_DEBUG
        printf(" ok\n");
    #endif

    //final result
    imgOut = pOut->Reconstruct(imgOut);

    #pragma omp parallel for
    for(int i = 0; i < imgOut->size(); i++) {
        imgOut->data[i] = MAX(imgOut->data[i], 0.0f);
    }

    //free the memory
    delete pW;
    delete pOut;
    delete pI;

    delete acc;
    delete weights;
    delete lum;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP */

