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
     * @param dataOut
     * @param dataIn
     * @param width
     * @param height
     * @return
     */
    static bool *removeIsolatedPixels(bool *dataOut, bool *dataIn,
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
     * @param dataOut
     * @param dataIn
     * @param width
     * @param height
     * @param kernelSize
     * @return
     */
    static bool *erode(bool *dataOut, bool *dataIn, int width, int height,
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
     * @param dataOut
     * @param dataIn
     * @param width
     * @param height
     * @param kernelSize
     * @return
     */
    static bool *dilate(bool *dataOut, bool *dataIn, int width, int height,
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
     * @param dataOut
     * @param dataIn
     * @param width
     * @param height
     */
    static bool* thinning(bool *dataOut, bool *dataIn, int width, int height)
    {
        if(dataIn == NULL) {
            return dataIn;
        }

        dataOut = clone(dataOut, dataIn, width * height, 1);

        bool P[9];

        //first-pass
        std::vector< int > list;
        for(int i = 1; i < (height - 1); i++) {
            int tmp = i * width;
            for(int j = 1; j < (width - 1); j++) {
                int index = tmp + j;

                if(!dataOut[index]) {
                    continue;
                }

                P[0] = dataOut[index];
                P[1] = dataOut[index + 1];
                P[2] = dataOut[index - width + 1];
                P[3] = dataOut[index - width];
                P[4] = dataOut[index - width - 1];
                P[5] = dataOut[index - 1];
                P[6] = dataOut[index + width - 1];
                P[7] = dataOut[index + width];
                P[8] = dataOut[index + width + 1];

                int X_h = 0;
                int n1 = 0;
                int n2 = 0;
                for(int k = 1; k <= 4; k++) {
                    int k_2 = k << 1;
                    bool x_2km1 = P[k_2 - 1];
                    bool x_2k   = P[k_2    ];
                    bool x_2kp1 = P[k_2 + 1];

                    if( !x_2km1 && (x_2k || x_2kp1) ) {
                        X_h++;
                    }

                    if(x_2km1 || x_2k) {
                        n1++;
                    }

                    if(x_2k || x_2kp1) {
                        n2++;
                    }
                }

                int min_12 = MIN(n1, n2);

                bool G1 = (X_h == 1);
                bool G2 = (min_12 > 1) && (min_12 < 4);
                bool G3 = P[1] && ( P[2] || P[3] || !P[8]);

                if(G1 && G2 && G3) {
                    list.push_back(index);
                }

            }
        }

        for(unsigned int i = 0; i < list.size(); i++) {
            dataOut[list[i]] = false;
        }

        list.clear();

        for(int i = 1; i < (height - 1); i++) {
            int tmp = i * width;
            for(int j = 1; j < (width - 1); j++) {
                int index = tmp + j;

                if(!dataOut[index]) {
                    continue;
                }

                P[0] = dataOut[index];
                P[1] = dataOut[index + 1];
                P[2] = dataOut[index - width + 1];
                P[3] = dataOut[index - width];
                P[4] = dataOut[index - width - 1];
                P[5] = dataOut[index - 1];
                P[6] = dataOut[index + width - 1];
                P[7] = dataOut[index + width];
                P[8] = dataOut[index + width + 1];

                int X_h = 0;
                int n1 = 0;
                int n2 = 0;
                for(int k = 1; k <= 4; k++) {
                    int k_2 = k << 1;
                    bool x_2km1 = P[k_2 - 1];
                    bool x_2k   = P[k_2    ];
                    bool x_2kp1 = P[k_2 + 1];

                    if( !x_2km1 && (x_2k || x_2kp1) ) {
                        X_h++;
                    }

                    if(x_2km1 || x_2k) {
                        n1++;
                    }

                    if(x_2k || x_2kp1) {
                        n2++;
                    }
                }

                int min_12 = MIN(n1, n2);

                bool G1 = (X_h == 1);
                bool G2 = (min_12 > 1) && (min_12 < 4);
                bool G3 =  P[5] && (P[6] || P[7] || !P[4]);

                if(G1 && G2 && G3) {
                    list.push_back(index);
                }

            }
        }

        for(unsigned int i = 0; i < list.size(); i++) {
            dataOut[list[i]] = false;
        }

        return dataOut;
    }
};

} // end namespace pic

#endif /* PIC_UTIL_MASK_HPP */
