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
#include "tone_mapping/input_estimates.hpp"

namespace pic {

/**
 * @brief LischinskiTMO
 * @param imgIn
 * @param imgOut
 * @param alpha
 * @return
 */
Image *LischinskiTMO(Image *imgIn, Image *imgOut = NULL, float alpha = -1.0f,
        float whitePoint = -1.0f)
{
    if(imgIn == NULL) {
        return NULL;
    }

    if(imgIn->channels != 3) {
        return NULL;
    }

    //extract luminance
    FilterLuminance fltLum;
    Image *lum = fltLum.ProcessP(Single(imgIn), NULL);

    float maxL = lum->getMaxVal()[0];
    float minL = lum->getMinVal()[0];
    float Lav = lum->getLogMeanVal()[0];
    float maxL_log = log2fPlusEpsilon(maxL);
    float minL_log = log2fPlusEpsilon(minL);

    if(alpha <= 0.0f) {
        alpha = EstimateAlpha(maxL, minL, Lav);
    }

    if(whitePoint <= 0.0f) {
        whitePoint = EstimateWhitePoint(maxL, minL);
    }

    float whitePoint_sq = whitePoint * whitePoint;

    int Z = int(ceilf(maxL_log - minL_log));

    if(Z <= 0) {
        return NULL;
    } else {
        if(imgOut == NULL) {
            imgOut = imgIn->clone();
        }
    }

    //Choose the representative Rz for each zone
    std::vector<float> *zones = new std::vector<float>[Z];
    float *fstop = new float[Z];
    float *Rz = new float[Z];

    for(int i = 0; i < Z; i++) {
        Rz[i] = 0.0f;
        fstop[i] = 0.0f;
    }

    for(int i = 0; i < lum->height; i++) {
        for(int j = 0; j < lum->width; j++) {
            float L = (*lum)(j,i)[0];
            float L_log = log2fPlusEpsilon(L);

            int zone = CLAMP(int(ceilf(L_log - minL_log)), Z);

            zones[zone].push_back(L);
        }
    }

    for(int i = 0; i < Z; i++) {
        unsigned int n = zones[i].size();
        if(n > 0) {
            std::sort(zones[i].begin(), zones[i].end());
            Rz[i] = zones[i][n / 2];
            if(Rz[i] > 0.0f) {
                //photographic operator
                float Rz2 = Rz[i] * alpha / Lav;
                float f = (Rz2 * (1 + Rz2 / whitePoint_sq) ) / (1.0f + Rz2);
                fstop[i] = log2fPlusEpsilon(f / Rz[i]);
            }
        }
    }

    //creating the fstop maps
    lum->applyFunction(log2fPlusEpsilon);

    Image *fstopMap = lum->allocateSimilarOne();

    for(int i = 0; i < lum->height; i++) {
        for(int j = 0; j < lum->width; j++) {
            float L_log = (*lum)(j,i)[0];

            int zone = CLAMP(int(ceilf(L_log - minL_log)), Z);

            (*fstopMap)(j,i)[0] = fstop[zone];

        }
    }

    //Lischinski minimization
    Image *tmp = lum->allocateSimilarOne();
    *tmp = 0.007f;
    Image *fstopMap_min = LischinskiMinimization(lum, fstopMap, tmp);

    for(int i = 0; i < imgOut->height; i++) {
        for(int j = 0; j < imgOut->width; j++) {
            float *val = (*imgOut)(j, i);

            float exposure = powf(2.0f, (*fstopMap_min)(j, i)[0]);

            for(int i = 0; i < imgOut->channels; i++) {
                val[i] *= exposure;
            }
        }
    }

    delete[] zones;
    delete[] Rz;
    delete[] fstop;
    delete fstopMap;
    delete fstopMap_min;

    return imgOut;
}

} // end namespace pic

#endif

#endif /* PIC_TONE_MAPPING_LISCHINSKI_TMO_HPP */

