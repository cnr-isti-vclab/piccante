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

#ifndef PIC_UTIL_LOW_DYNAMIC_RANGE_HPP
#define PIC_UTIL_LOW_DYNAMIC_RANGE_HPP

#include "../base.hpp"

namespace pic {

/**
 * @brief The LDR_type enum
 */
enum LDR_type { LT_NOR, LT_NOR_GAMMA, LT_NONE};

/**
 * @brief checkNormalized checks if data is in [0,1].
 * @param data
 * @param size
 * @param delta
 * @return
 */
PIC_INLINE bool checkNormalized(const float *data, int size, float delta = 1e-6f)
{
    float thr = 1.0f + delta;

    for(int i = 0; i < size; i++) {
        if(data[i] > thr) {
            return false;
        }
    }

    return true;
}

/**
 * @brief convertLDR2HDR converts a buffer of unsigned char into float.
 * @param dataIn
 * @param dataOut
 * @param size
 * @param type
 * @param gamma
 * @return
 */
PIC_INLINE float *convertLDR2HDR(unsigned char *dataIn, float *dataOut,
                                 int size, LDR_type type, float gamma = 2.2f)
{
    if(dataIn == NULL) {
        return NULL;
    }

    if(dataOut == NULL) {
        dataOut = new float[size];
    }

    float LUT[256];
    for(int i = 0; i < 256; i++) {
        float i_f = float(i);

        switch(type) {
        case LT_NONE: {//Simple cast
                LUT[i] = i_f;
        }
        break;

        case LT_NOR: {//Normalization in [0,1]
                LUT[i] = i_f / 255.0f;
        }
        break;

        case LT_NOR_GAMMA: {//Normalization in [0,1] + GAMMA correction removal
            for(int i = 0; i < 256; i++) {
                LUT[i] = powf(i_f / 255.0f, gamma);
            }
        }
        break;
        }
    }

    #pragma omp parallel for
    for(int i = 0; i < size; i++) {
        dataOut[i] = LUT[dataIn[i]];
    }

    return dataOut;
}

/**
 * @brief convertHDR2LDR converts a buffer of float into unsigned char.
 * @param dataIn
 * @param dataOut
 * @param size
 * @param type
 * @param gamma
 * @return
 */
PIC_INLINE unsigned char *convertHDR2LDR(const float *dataIn, unsigned char *dataOut,
        int size, LDR_type type, float gamma = 2.2f)
{
    if(dataIn == NULL) {
        return NULL;
    }

    if(dataOut == NULL) {
        dataOut = new unsigned char[size];
    }

    if(gamma <= 0.0f) {
        gamma = 2.2f;
    }

    float invGamma = 1.0f / gamma;

    switch(type) {

    case LT_NONE: {//simple cast
        #pragma omp parallel for
        for(int i = 0; i < size; i++) {
            dataOut[i] = CLAMPi(int(lround(dataIn[i])), 0, 255);
        }
    }
    break;

    case LT_NOR: {//convert into 8-bit
        #pragma omp parallel for
        for(int i = 0; i < size; i++) {
            dataOut[i] = CLAMPi(int(lround(dataIn[i] * 255.0f)), 0, 255);
        }
    }
    break;

    case LT_NOR_GAMMA: {//convert into 8-bit + GAMMA correction application
        #pragma omp parallel for
        for(int i = 0; i < size; i++) {
            float tmp = powf(dataIn[i], invGamma);
            dataOut[i] = CLAMPi(int(lround(tmp * 255.0f)), 0, 255);
        }
    }
    break;
    }

    return dataOut;
}

} // end namespace pic

#endif //PIC_UTIL_LOW_DYNAMIC_RANGE_HPP
