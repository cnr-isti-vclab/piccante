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
#include "algorithms/pyramid.hpp"

namespace pic {

/**
 * @brief ExposureFusion
 * @param imgIn
 * @param imgOut
 * @param wC
 * @param wE
 * @param wS
 * @return
 */
Image *ExposureFusion(ImageVec imgIn, Image *imgOut, float wC = 1.0f,
                         float wE = 1.0f, float wS = 1.0f)
{
    int n = imgIn.size();

    if(n < 2) {
        return NULL;
    }

    //Compute weights
    int channels = imgIn[0]->channels;
    int width = imgIn[0]->width;
    int height = imgIn[0]->height;
    int size = width * height;

    FilterLuminance fltL;
    FilterLaplacian fltLap;

    Image *L       = new Image(1, width, height, 1);
    Image *weights = new Image(n, width, height, 1);
    Image *acc     = new Image(1, width, height, 1);

    acc->SetZero();

    ImageVec weights_list;

    float mu = 0.5f;
    float sigma = 0.2f;
    float sigma2 = 2.0f * sigma * sigma;

    for(int j = 0; j < n; j++) {
        #ifdef PIC_DEBUG
            printf("Processing image %d\n", j);
        #endif

        Image *curWeight = new Image(1, width, height, 1,
                                           &weights->data[size * j]);

        weights_list.push_back(curWeight);

        fltL.ProcessP(Single(imgIn[j]), L);

        fltLap.ProcessP(Single(L), curWeight);

        float *data = imgIn[j]->data;

        for(int ind = 0; ind < size; ind++) {
            int i = ind * channels;

            //Contrast
            float pCon = fabsf(curWeight->data[ind]);

            //Saturation
            float pSat = computeSaturation(&data[i], channels);

            //Well-exposedness
            float tmpL = L->data[ind] - mu;
            float pWE = expf(-(tmpL * tmpL) / sigma2);

            //Final weights
            curWeight->data[ind] =  powf(pCon, wC) *
                                    powf(pWE,  wE) *
                                    powf(pSat, wS);
        }

        *acc += *curWeight;
    }

    for(int i=0; i<acc->size(); i++) {
        if(acc->data[i] <= 0.0f) {
            acc->data[i] = 1.0f;
        }
    }

    //Accumulation Pyramid
    #ifdef PIC_DEBUG
        printf("Blending...");
    #endif

    Pyramid *pW   = new Pyramid(width, height, 1, false, 0);
    Pyramid *pI   = new Pyramid(width, height, channels, true, 0);
    Pyramid *pOut = new Pyramid(width, height, channels, true, 0);
    
    for(int j = 0; j < n; j++) {
        //normalization
        *weights_list[j] /= *acc;

        pW->Update(weights_list[j]);

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
        if(imgOut->data[i])
        imgOut->data[i] = imgOut->data[i] > 0.0f ? imgOut->data[i] : 0.0f;
    }

    //free the memory
    delete pW;
    delete pOut;
    delete pI;
    delete acc;
    delete weights;
    delete L;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP */

