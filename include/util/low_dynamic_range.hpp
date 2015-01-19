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

#include "base.hpp"

namespace pic {

/**
 * @brief The LDR_type enum
 */
enum LDR_type { LT_NOR, LT_NOR_GAMMA, LT_NONE};

/**
 * @brief CheckNormalized checks if data is in [0,1].
 * @param data
 * @param size
 * @param delta
 * @return
 */
PIC_INLINE bool CheckNormalized(const float *data, int size, float delta = 1e-6f)
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
 * @brief ConvertLDR2HDR converts a buffer of unsigned char into float.
 * @param dataIn
 * @param dataOut
 * @param size
 * @param type
 * @param gamma
 * @return
 */
PIC_INLINE float *ConvertLDR2HDR(unsigned char *dataIn, float *dataOut,
                                 int size, LDR_type type, float gamma = 2.2f)
{
    if(dataIn == NULL) {
        return NULL;
    }

    if(dataOut == NULL) {
        dataOut = new float[size];
    }

    switch(type) {
    //Simple cast
    case LT_NONE: {
        #pragma omp parallel for

        for(int i = 0; i < size; i++) {
            dataOut[i] = float(dataIn[i]);
        }
    }
    break;

    //Normalization in [0,1]
    case LT_NOR: {
        float inv_255 = 1.0f / 255.0f;
        #pragma omp parallel for

        for(int i = 0; i < size; i++) {
            dataOut[i] = float(dataIn[i]) * inv_255;
        }
    }
    break;

    //Normalization in [0,1] + GAMMA removal
    case LT_NOR_GAMMA: {
        float inv_255 = 1.0f / 255.0f;
        #pragma omp parallel for

        for(int i = 0; i < size; i++) {
            dataOut[i] = powf(float(dataIn[i]) * inv_255, gamma);
        }
    }
    break;
    }

    return dataOut;
}

/**
 * @brief ConvertHDR2LDR converts a buffer of unsigned char into float.
 * @param dataIn
 * @param dataOut
 * @param size
 * @param type
 * @param gamma
 * @return
 */
PIC_INLINE unsigned char *ConvertHDR2LDR(const float *dataIn, unsigned char *dataOut,
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
    //Simple cast
    case LT_NONE: {
        #pragma omp parallel for

        for(int i = 0; i < size; i++) {
            dataOut[i] = CLAMPi(int(lround(dataIn[i])), 0, 255);
        }
    }
    break;

    //Converting into 8-bit
    case LT_NOR: {
        #pragma omp parallel for

        for(int i = 0; i < size; i++) {
            dataOut[i] = CLAMPi(int(lround(dataIn[i] * 255.0f)), 0, 255);
        }
    }
    break;

    //Normalization in [0,1] + GAMMA removal
    case LT_NOR_GAMMA: {
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
