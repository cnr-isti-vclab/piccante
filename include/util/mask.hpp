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

#include "../base.hpp"
#include "../util/math.hpp"
#include "../util/buffer.hpp"

namespace pic {

class Mask: public Buffer<bool>
{
public:
    /**
     * @brief removeIsolatedPixels removes isolated pixels.
     * @param dataIn
     * @param dataOut
     * @param width
     * @param height
     * @return
     */
    static bool *removeIsolatedPixels(bool *dataIn, bool *dataOut,
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
     * @brief erode erodes a mask.
     * @param dataIn
     * @param dataOut
     * @param width
     * @param height
     * @param kernelSize
     * @return
     */
    static bool *erode(bool *dataIn, bool *dataOut, int width, int height,
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
    static bool *dilate(bool *dataIn, bool *dataOut, int width, int height,
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
    static bool empty(bool *dataIn, int width, int height)
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
     * @brief thinning thins a mask.
     * @param dataIn
     * @param width
     * @param height
     */
    static bool* thinning(bool *dataIn, int width, int height)
    {
        if(dataIn == NULL) {
            return dataIn;
        }

        bool *tmp  = clone(NULL, dataIn, width * height, 1);

        int n = (height * width);

        int P[9];

        //first-pass
        for(int i = 1; i < (height - 1); i++) {
            int tmp = i * width;
            for(int j = 1; j < (width - 1); j++) {
                int index = tmp + j;

                if(!dataIn[index]) {
                    continue;
                }

                P[0] = index;
                P[1] = index - width;
                P[2] = index - width + 1;
                P[3] = index + 1;
                P[4] = index + width + 1;
                P[5] = index + width;
                P[6] = index + width - 1;
                P[7] = index - 1;
                P[8] = index - width - 1;

                int N = 0;
                for(int k = 1; k < 9; i++) {
                    N = dataIn[P[k]] ? N + 1 : N;
                }

            }
        }

//        return ;
    }
};

} // end namespace pic

#endif /* PIC_UTIL_MASK_HPP */
