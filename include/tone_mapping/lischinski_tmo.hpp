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

#ifndef PIC_TONE_MAPPING_LISCHINSKI_TMO_HPP
#define PIC_TONE_MAPPING_LISCHINSKI_TMO_HPP

#ifndef PIC_DISABLE_EIGEN

#include "tone_mapping/lischinski_minimization.hpp"

namespace pic {

/**
 * @brief LischinskiTMO
 * @param imgIn
 * @param imgOut
 * @param alpha
 * @return
 */
Image *LischinskiTMO(Image *imgIn, Image *imgOut = NULL,
                        float alpha = 0.5f)
{
    if(imgIn == NULL) {
        return NULL;
    }

    if(imgIn->channels != 3) {
        return NULL;
    }

    if(alpha <= 0.0f) {
        alpha = 0.5f;
    }

    //extract luminance
    FilterLuminance fltLum;
    Image *lum = fltLum.ProcessP(Single(imgIn), NULL);

    Image *lum_log = lum->Clone();
    lum_log->ApplyFunction(log2f);

    float maxL = lum->getMaxVal()[0];
    float minL = lum->getMinVal()[0];
    float maxL_log = log2f(maxL);
    float minL_log = log2f(minL);
    float Lav = lum->getLogMeanVal()[0];

    int Z = int(ceilf(maxL_log - minL_log));

    if(Z <= 0) {
        return NULL;
    } else {
        if(imgOut == NULL) {
            imgOut = imgIn->Clone();
        }
    }

    float *Rz = new float[Z];
    float *fstop = new float[Z];
    int	  *counter = new int[Z];

    for(int i = 0; i < Z; i++) {
        Rz[i] = 0.0f;
        fstop[i] = 0.0f;
        counter[i] = 0;
    }

    for(int i = 0; i < lum->height; i++) {
        for(int j = 0; j < lum->width; j++) {

            int indx = i * lum->width + j;
            float L_log = lum_log->data[indx];

            int c = CLAMP(int(ceilf(L_log - minL_log)), Z);

            Rz[c] += lum->data[indx];
            counter[c]++;
        }
    }

    for(int i = 0; i < Z; i++) {
        if(counter[i] > 0) {
            //Average
            Rz[i] /= float(counter[i]);

            //photographic operator
            Rz[i] = Rz[i] * alpha / Lav;
            float f = Rz[i] / (Rz[i] + 1.0f);
            float tmp = f / Rz[i];
            fstop[i] = log2f(tmp);
        }
    }

    //creating the fstop maps
    Image *fstopMap = lum->AllocateSimilarOne();

    for(int i = 0; i < lum->height; i++) {
        for(int j = 0; j < lum->width; j++) {

            int indx = i * lum->width + j;
            float L_log = lum_log->data[indx];

            int c = CLAMP(int(ceilf(L_log - minL_log)), Z);

            fstopMap->data[indx] = fstop[c];

        }
    }

    fstopMap->Write("../fstop.pfm");

    //Lischinski minimization
    Image *tmp = lum->AllocateSimilarOne();
    *tmp = 0.007f;
    fstopMap = LischinskiMinimization(lum_log, fstopMap, tmp);

    for(int i = 0; i < lum->height; i++) {
        for(int j = 0; j < lum->width; j++) {
            float *val = (*imgOut)(j, i);
            float *tmp = (*fstopMap)(j, i);
            float tmpValue = powf(2.0f, tmp[0]);

            for(int i = 0; i < 3; i++) {
                val[i] *= tmpValue;
            }
        }
    }

    delete[] Rz;
    delete[] fstop;
    delete[] counter;
    return imgOut;
}

} // end namespace pic

#endif

#endif /* PIC_TONE_MAPPING_LISCHINSKI_TMO_HPP */

