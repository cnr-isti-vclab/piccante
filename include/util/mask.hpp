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

#ifndef PIC_UTIL_MASK_HPP
#define PIC_UTIL_MASK_HPP

#include "base.hpp"
#include "util/math.hpp"

namespace pic {

/**
 * @brief MaskRemoveIsolatedPixels removes isolated pixels.
 * @param dataIn
 * @param dataOut
 * @param width
 * @param height
 * @return
 */
PIC_INLINE bool *MaskRemoveIsolatedPixels(bool *dataIn, bool *dataOut,
        int width, int height)
{
    if(dataIn == NULL) {
        return dataOut;
    }

    if(dataOut == NULL) {
        dataOut = new bool[width * height];
    }

    #pragma omp parallel for

    for(int i = 0; i < height; i++) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            int c = ind + j;

            int counter = 0;

            for(int k = -1; k <= 1; k++) {
                int ci = CLAMP((i + k), height) * width;

                for(int l = -1; l <= 1; l++) {
                    if((l != 0) && (k != 0)) {
                        int cj = CLAMP((j + l), width);

                        if(dataIn[ci + cj]) {
                            counter++;
                        }
                    }
                }
            }

            dataOut[c] = counter > 0;
        }
    }

    return dataOut;
}

/**
 * @brief MaskErode erodes a mask.
 * @param dataIn
 * @param dataOut
 * @param width
 * @param height
 * @param kernelSize
 * @return
 */
PIC_INLINE bool *MaskErode(bool *dataIn, bool *dataOut, int width, int height,
                           int kernelSize = 3)
{
    if(dataIn == NULL) {
        return dataOut;
    }

    if(dataOut == NULL) {
        dataOut = new bool[width * height];
    }

    int halfKernelSize = kernelSize >> 1;

    #pragma omp parallel for

    for(int i = 0; i < height; i++) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            int c = ind + j;

            bool out = false;

            for(int k = -halfKernelSize; k <= halfKernelSize; k++) {
                int ci = CLAMP((i + k), height) * width;

                for(int l = -halfKernelSize; l <= halfKernelSize; l++) {
                    int cj = CLAMP((j + l), width);
                    out = out || (!dataIn[ci + cj]);
                }
            }

            dataOut[c] = !out;
        }
    }

    return dataOut;
}

/**
 * @brief MaskDilate dilates a mask.
 * @param dataIn
 * @param dataOut
 * @param width
 * @param height
 * @param kernelSize
 * @return
 */
PIC_INLINE bool *MaskDilate(bool *dataIn, bool *dataOut, int width, int height,
                            int kernelSize = 3)
{
    if(dataIn == NULL) {
        return dataOut;
    }

    if(dataOut == NULL) {
        dataOut = new bool[width * height];
    }

    int halfKernelSize = kernelSize >> 1;

    #pragma omp parallel for

    for(int i = 0; i < height; i++) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            int c = ind + j;

            bool out = false;

            for(int k = -halfKernelSize; k <= halfKernelSize; k++) {
                int ci = CLAMP((i + k), height) * width;

                for(int l = -halfKernelSize; l <= halfKernelSize; l++) {
                    int cj = CLAMP((j + l), width);
                    out = out || (dataIn[ci + cj]);
                }
            }

            dataOut[c] = out;
        }
    }

    return dataOut;
}

/**
 * @brief MaskEmpty checks if a mask is empty.
 * @param dataIn
 * @param width
 * @param height
 * @return
 */
PIC_INLINE bool MaskEmpty(bool *dataIn, int width, int height)
{
    if(dataIn == NULL) {
        return true;
    }

    for(int i = 0; i < height; i++) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            if(dataIn[ind + j]) {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief MaskNegative negates a mask.
 * @param dataIn
 * @param width
 * @param height
 */
PIC_INLINE void MaskNegative(bool *dataIn, int width, int height)
{
    if(dataIn == NULL) {
        return;
    }

    int n = (height * width);

    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        dataIn[i] = !dataIn[i];
    }
}

/**
 * @brief MaskSetValue assigns a constant value to the mask.
 * @param buffer
 * @param n
 * @param value
 * @return
 */
PIC_INLINE bool *MaskSetValue(bool *buffer, int n, bool value)
{
    if(buffer == NULL) {
        buffer = new bool[n];
    }

    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        buffer[i] = value;
    }

    return buffer;
}

/**
 * @brief MaskClone clones a mask.
 * @param dataIn
 * @param dataOut
 * @param width
 * @param height
 * @return
 */
PIC_INLINE bool *MaskClone(bool *dataIn, bool *dataOut, int width, int height)
{
    if(dataIn == NULL) {
        return dataOut;
    }

    if(dataOut == NULL) {
        dataOut = new bool[width * height];
    }

    memcpy(dataOut, dataIn, width * height * sizeof(bool));

    return dataOut;
}

} // end namespace pic

#endif /* PIC_UTIL_MASK_HPP */
