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

#ifndef PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP
#define PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP

#include "filtering/filter_luminance.hpp"
#include "filtering/filter_laplacian.hpp"
#include "algorithms/pyramid.hpp"

namespace pic {

inline float computeSaturation(float *data, int channels = 3)
{
    if(channels == 1) {
        return 1.0f;
    }

    float tmp, var, tmpMu;

    tmpMu = 0.0f;

    for(int i = 0; i < channels; i++) {
        tmpMu += data[i];
    }

    tmpMu /= float(channels);

    var = 0.0f;

    for(int i = 0; i < channels; i++) {
        tmp = data[i] - tmpMu;
        var += tmp * tmp;
    }

    var = sqrtf(var / float(channels));

    return var;
}

ImageRAW *ExposureFusion(ImageRAWVec imgIn, ImageRAW *imgOut, float wC = 1.0f,
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

    ImageRAW *L       = new ImageRAW(1, width, height, 1);
    ImageRAW *weights = new ImageRAW(n, width, height, 1);
    ImageRAW *acc     = new ImageRAW(1, width, height, 1);

    acc->SetZero();

    ImageRAWVec weights_list;

    for(int j = 0; j < n; j++) {
        #ifdef PIC_DEBUG
            printf("Processing image %d\n", j);
        #endif
        ImageRAW *curWeight = new ImageRAW(1, width, height, 1,
                                           &weights->data[size * j]);
        weights_list.push_back(curWeight);

        fltL.ProcessP(Single(imgIn[j]), L);

        fltLap.ProcessP(Single(L), curWeight);

        float mu = 0.5f;
        float sigma = 0.2f;
        float sigma2 = 2.0f * sigma * sigma;

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
            float weight =	powf(pCon, wC) *
                            powf(pWE,  wE) *
                            powf(pSat, wS);

            curWeight->data[ind] = weight;
        }

        acc->Add(curWeight);
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
    Pyramid *pW = new Pyramid(width, height, 1, false, 0);

    Pyramid *pI   = new Pyramid(width, height, channels, true, 0);
    Pyramid *pOut = new Pyramid(width, height, channels, true, 0);
    
    for(int j = 0; j < n; j++) {
        //normalization
        weights_list[j]->Div(acc);

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

