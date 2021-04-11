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

#ifndef PIC_UTIL_BUFFER_HPP
#define PIC_UTIL_BUFFER_HPP

#include <string.h>

#include "../base.hpp"
#include "../util/math.hpp"
#include "../util/array.hpp"

namespace pic {

template<class T>
class Buffer
{
public:
    Buffer()
    {

    }

    /**
     * @brief assign assigns value to buffer
     * @param buffer is the output buffer
     * @param n is the number of elements of the buffer
     * @param value is the value to be assigned to all values of buffer
     * @return it returns the pointer to buffer
     */
    static T *assign(T *buffer, int n, T value)
    {
        if(buffer == NULL) {
            if(n > 0) {
                buffer = new T[n];
            } else {
                return buffer;
            }
        }

        #pragma omp parallel for
        for(int i = 0; i < n; i++) {
            buffer[i] = value;
        }

        return buffer;
    }

    /**
     * @brief assign assigns bufferIn to bufferOut
     * @param bufferOut is the output buffer
     * @param bufferIn is the input buffer
     * @param n is the number of elements of bufferIn and bufferOut
     * (they have to be of same length)
     * @return it returns the pointer to buffer
     */
    static T *assign(T *bufferOut, T *bufferIn, int n)
    {
        memcpy(bufferOut, bufferIn, n * sizeof(T));
        return bufferOut;
    }

    /**
     * @brief add peforms addition
     * @param buffer is the output buffer
     * @param n is the number of elements of the buffer
     * @param value is the value to be added to all values of buffer
     * @return it returns the pointer to buffer
     */
    static T *add(T *buffer, int n, T value)
    {
        #pragma omp parallel for

        for(int i = 0; i < n; i++) {
            buffer[i] += value;
        }

        return buffer;
    }

    /**
     * @brief add
     * @param bufferOut
     * @param bufferIn0
     * @param bufferIn1
     * @param n
     * @return
     */
    static T *add(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
    {
        #pragma omp parallel for
        for(int i = 0; i < n; i++) {
            bufferOut[i] = bufferIn0[i] + bufferIn1[i];
        }

        return bufferOut;
    }

    static T *add(T *bufferOut, T *bufferIn, int n)
    {
        #pragma omp parallel for

        for(int i = 0; i < n; i++) {
            bufferOut[i] += bufferIn[i];
        }

        return bufferOut;
    }


    static T *addS(T *bufferOut, T *bufferIn, int n, int channels)
    {
        #pragma omp parallel for
        for(int ind = 0; ind < n; ind++) {
            int i = ind * channels;

            float val = bufferIn[ind];

            for(int j = 0; j < channels; j++) {
                bufferOut[i + j] += val;
            }
        }

        return bufferOut;
    }

     /**
     * @brief mul multiplies a constant value
     * @param buffer
     * @param n
     * @param value
     * @return
     */
    static T *mul(T *buffer, int n, T value)
    {
        #pragma omp parallel for
        for(int i = 0; i < n; i++) {
            buffer[i] *= value;
        }

        return buffer;
    }


    /**
     * @brief mul
     * @param bufferOut
     * @param bufferIn0
     * @param bufferIn1
     * @param n
     * @return
     */
    static T *mul(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
    {
        #pragma omp parallel for

        for(int i = 0; i < n; i++) {
            bufferOut[i] = bufferIn0[i] * bufferIn1[i];
        }

        return bufferOut;
    }

    /**
     * @brief BufferMul
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @return
     */
    static T *mul(T *bufferOut, T *bufferIn, int n)
    {
        #pragma omp parallel for

        for(int i = 0; i < n; i++) {
            bufferOut[i] *= bufferIn[i];
        }

        return bufferOut;
    }

    /**
     * @brief mulS
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @param channels
     * @return
     */
    static T *mulS(T *bufferOut, T *bufferIn, int n, int channels)
    {
        #pragma omp parallel for
        for(int ind = 0; ind < n; ind++) {
            int i = ind * channels;

            float val = bufferIn[ind];

            for(int j = 0; j < channels; j++) {
                bufferOut[i + j] *= val;
            }
        }

        return bufferOut;
    }

    /**
     * @brief sub
     * @param buffer
     * @param n
     * @param value
     * @return
     */
    static T *sub(T *buffer, int n, T value)
    {
        #pragma omp parallel for
        for(int i = 0; i < n; i++) {
            buffer[i] -= value;
        }

        return buffer;
    }

    /**
     * @brief sub
     * @param bufferOut
     * @param bufferIn0
     * @param bufferIn1
     * @param n
     * @return
     */
    static T *sub(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
    {
        #pragma omp parallel for
        for(int i = 0; i < n; i++) {
            bufferOut[i] = bufferIn0[i] - bufferIn1[i];
        }

        return bufferOut;
    }

    /**
     * @brief sub
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @return
     */
    static T *sub(T *bufferOut, T *bufferIn, int n)
    {
        #pragma omp parallel for
        for(int i = 0; i < n; i++) {
            bufferOut[i] -= bufferIn[i];
        }

        return bufferOut;
    }

    /**
     * @brief subS
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @param channels
     * @return
     */
    static T *subS(T *bufferOut, T *bufferIn, int n, int channels)
    {
        #pragma omp parallel for
        for(int ind = 0; ind < n; ind++) {
            int i = ind * channels;

            float val = bufferIn[ind];

            for(int j = 0; j < channels; j++) {
                bufferOut[i + j] -= val;
            }
        }

        return bufferOut;
    }

    /**
     * @brief div divides by a constant value
     * @param buffer
     * @param n
     * @param value
     * @return
     */
    static T *div(T *buffer, int n, T value)
    {
        #pragma omp parallel for
        for(int i = 0; i < n; i++) {
            buffer[i] /= value;
        }

        return buffer;
    }

    /**
     * @brief div
     * @param bufferOut
     * @param bufferIn0
     * @param bufferIn1
     * @param n
     * @return
     */
    static T *div(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
    {
        #pragma omp parallel for

        for(int i = 0; i < n; i++) {
            bufferOut[i] = bufferIn0[i] / bufferIn1[i];
        }

        return bufferOut;
    }

    /**
     * @brief div
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @return
     */
    static T *div(T *bufferOut, T *bufferIn, int n)
    {
        #pragma omp parallel for

        for(int i = 0; i < n; i++) {
            bufferOut[i] /= bufferIn[i];
        }

        return bufferOut;
    }

    /**
     * @brief divS
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @param channels
     * @return
     */
    static T *divS(T *bufferOut, T *bufferIn, int n, int channels)
    {
        #pragma omp parallel for
        for(int ind = 0; ind < n; ind++) {
            int i = ind * channels;

            float val = bufferIn[ind];

            for(int j = 0; j < channels; j++) {
                bufferOut[i + j] /= val;
            }
        }

        return bufferOut;
    }

    /**
     * @brief flipH flips a buffer horizontally
     * @param buffer
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    static void flipH(T *buffer, int width, int height, int channels,
                            int frames)
    {
        int steps = width >> 1;

    //	for(int l=0;l<frames;l++)
        #pragma omp parallel for

        for(int i = 0; i < height; i++) {
            int ind = i * width;

            for(int j = 0; j < steps; j++) {
                int i0 = (ind + j) * channels;
                int i1 = (ind + width - j - 1) * channels;

                for(int k = 0; k < channels; k++) { //swap
                    T tmp        = buffer[i0 + k];
                    buffer[i0 + k] = buffer[i1 + k];
                    buffer[i1 + k] = tmp;
                }
            }
        }
    }

    /**
     * @brief flipV flips an image vertically
     * @param buffer
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    static void flipV(T *buffer, int width, int height, int channels,
                                int frames)
    {
        int steps = height >> 1;

    //	for(int l=0;l<frames;l++)
        #pragma omp parallel for

        for(int i = 0; i < steps; i++) {
            int ind0 = i * width;
            int ind1 = (height - i - 1) * width;

            for(int j = 0; j < width; j++) {
                int i0 = (ind0 + j) * channels;
                int i1 = (ind1 + j) * channels;

                for(int k = 0; k < channels; k++) { //swap
                    T tmp          = buffer[i0 + k];
                    buffer[i0 + k] = buffer[i1 + k];
                    buffer[i1 + k] = tmp;
                }
            }
        }
    }


    /**
     * @brief rotate90CW rotates an image 90 CW
     * @param buffer
     * @param width
     * @param height
     * @param channels
     */
    static void rotate90CW(T *buffer, int &width, int &height, int channels)
    {
        if(buffer==NULL) {
            return;
        }

        if(width == height) { //in place rotation
          //  #pragma omp parallel for
            int n = width;
            for(int i = 0; i < n/2; i++) {
                int i_n = n - i  - 1 ;

                for(int j = i; j < (n - i - 1); j++) {
                    int j_n = n - j  - 1 ;

                    int i0 = (i   * n + j  ) * channels;
                    int i1 = (j_n * n + i  ) * channels;
                    int i2 = (i_n * n + j_n) * channels;
                    int i3 = (j   * n + i_n) * channels;


                    for(int k = 0; k < channels; k++) { //swap
                        T tmp          = buffer[i0 + k];
                        buffer[i0 + k] = buffer[i1 + k];
                        buffer[i1 + k] = buffer[i2 + k];
                        buffer[i2 + k] = buffer[i3 + k];
                        buffer[i3 + k] = tmp;
                    }
                }
            }
        } else {
            T *tmpBuffer = new T[width * height * channels];
            memcpy(tmpBuffer, buffer, sizeof(T) * width * height * channels);

            #pragma omp parallel for
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    int i0 = (i * width + j) * channels;
                    int i1 = (j * height + height - i - 1) * channels;

                    for(int k = 0; k < channels; k++) {
                        buffer[i1 + k] = tmpBuffer[i0 + k];
                    }
                }
            }

            delete[] tmpBuffer;

            int tmp = width;
            width   = height;
            height  = tmp;
        }
    }

    /**
     * @brief rotate90CCW rotates an image 90 CCW
     * @param buffer
     * @param width
     * @param height
     * @param channels
     */
    static void rotate90CCW(T *buffer, int &width, int &height, int channels)
    {
        if(buffer==NULL) {
            return;
        }

        if(width == height) { //in place rotation
            #pragma omp parallel for
            for(int i = 0; i < (height - 2); i++) {

                for(int j = (i + 1); j < (width - 1); j++) {

                    int i0 = (i * width + j) * channels;
                    int i1 = (j * width + i) * channels;

                    for(int k = 0; k < channels; k++) { //swap
                        T tmp          = buffer[i0 + k];
                        buffer[i0 + k] = buffer[i1 + k];
                        buffer[i1 + k] = tmp;
                    }
                }
            }
        } else {
            T *tmpBuffer = new T[width * height * channels];
            memcpy(tmpBuffer, buffer, sizeof(T) * width * height * channels);

            #pragma omp parallel for
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    int i0 = (i * width  + j) * channels;
                    int i1 = ((width-j-1) * height + i) * channels;

                    for(int k = 0; k < channels; k++) {
                        buffer[i1 + k] = tmpBuffer[i0 + k];
                    }
                }
            }

            delete[] tmpBuffer;

            int tmp = width;
            width   = height;
            height  = tmp;
        }
    }

    /**
     * @brief shift
     * @param bufferOut
     * @param bufferIn
     * @param dx
     * @param dy
     * @param bReplicate
     * @param width
     * @param height
     * @param channels
     * @param frames
     * @return
     */
    static T *shift(T *bufferOut, T *bufferIn, int dx, int dy, bool bReplicate,
                    int width, int height, int channels, int frames)
    {
        if(bufferOut == NULL) {
            int n = width * height * channels * frames;
            bufferOut = new T[n];
            assign(bufferOut, n, T(0));
        }

        T zero = T(0);
        int wS, wE, hS, hE;
        int wfS, wfE, hfS, hfE;
        if(dx > 0) {
            wS = 0;
            wE = width - dx;

            wfS = width - dx;
            wfE = width;
        } else {
            wS = -dx;
            wE = width;

            wfS = 0;
            wfE = -dx;
        }

        if(dy > 0) {
            hS = 0;
            hE = height - dy;

            hfS = height - dy;
            hfE = height;
        } else {
            hS = -dy;
            hE = height;

            hfS = 0;
            hfE = -dy;
        }

        #pragma omp parallel for
        for(int iOut = hS; iOut < hE; iOut++) {
            int strideOut = iOut * width;
            int iIn = iOut + dy;
            int strideIn = iIn * width;
            for(int jOut = wS; jOut < wE; jOut++) {
                int jIn = jOut + dx;

                int indOut = (strideOut + jOut) * channels;
                int indIn = (strideIn + jIn ) * channels;

                Array<T>::assign(&bufferIn[indIn], channels,
                                 &bufferOut[indOut]);
            }
        }


        //block 1
        int strideIn = dy > 0 ? (height - 1) * width : 0;
        for(int iOut = hfS; iOut < hfE; iOut++) {
            int strideOut = iOut * width;

            for(int jOut = wS; jOut < wE; jOut++) {
                int jIn = jOut + dx;

                int indOut = (strideOut + jOut) * channels;
                int indIn = (strideIn + jIn ) * channels;

                if(bReplicate) {
                    Array<T>::assign(&bufferIn[indIn], channels,
                                     &bufferOut[indOut]);
                } else {
                    Array<T>::assign(zero, &bufferOut[indOut], channels);
                }
            }
        }

        //block 2
        int jIn = dx > 0 ? width - 1 : 0;
        for(int iOut = hS; iOut < hE; iOut++) {
            int strideOut = iOut * width;
            int iIn = iOut + dy;
            int strideIn = iIn * width;

            for(int jOut = wfS; jOut < wfE; jOut++) {

                int indOut = (strideOut + jOut) * channels;
                int indIn = (strideIn + jIn ) * channels;

                if(bReplicate) {
                    Array<T>::assign(&bufferIn[indIn], channels,
                                     &bufferOut[indOut]);
                } else {
                    Array<T>::assign(zero, &bufferOut[indOut], channels);
                }
            }
        }

        //block3
        jIn = dx > 0 ? width - 1 : 0;
        strideIn = dy > 0 ? (height - 1) * width : 0;
        int indIn = (strideIn + jIn ) * channels;

        for(int iOut = hfS; iOut < hfE; iOut++) {
            int strideOut = iOut * width;

            for(int jOut = wfS; jOut < wfE; jOut++) {

                int indOut = (strideOut + jOut) * channels;

                if(bReplicate) {
                    Array<T>::assign(&bufferIn[indIn], channels,
                                     &bufferOut[indOut]);
                } else {
                    Array<T>::assign(zero, &bufferOut[indOut], channels);
                }
            }
        }
        return bufferOut;
    }

    /**
     * @brief transpose transposes a buffer
     * @param bufferOut
     * @param bufferIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     * @return
     */
    static T* transpose(T *bufferOut, T *bufferIn, int width, int height,
                        int channels, int frames)
    {
        if(bufferIn == NULL) {
            return bufferOut;
        }

        if(bufferOut == NULL) {
            bufferOut = new T[width * height * channels * frames];
        }

        for(int i = 0; i < height; i++) {
            int indIn = i * width;

            for(int j = i; j < width; j++) {
                indIn = (indIn + j) * channels;

                int indOut = ((j * height) + i) *  channels;

                for(int k = 0; k < channels; k++) {
                    bufferOut[indOut + k] = bufferIn[indIn + k];
                }
            }
        }
    }

    /**
     * @brief BGRtoRGB swizzles from BGR to RGB a buffer
     * @param buffer
     * @param width
     * @param height
     * @param channels
     * @param frames
     * @return
     */
    static T* BGRtoRGB(T *buffer, int width, int height,
                       int channels, int frames)
    {
        int size = width * height * channels * frames;
        for(int i = 0; i < size; i += channels) {
            T tmp         = buffer[i];
            buffer[i    ] = buffer[i + 2];
            buffer[i + 2] = tmp;
        }

        return buffer;
    }

    /**
     * @brief BufferFromLayerToIntervaleaved change from RGB RGB RGB... to RRR... GGG... BBB...
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @param channels
     * @return
     */
    static T* BufferFromLayerToIntervaleaved(T *bufferOut, T *bufferIn, int n, int channels)
    {
        #pragma omp parallel for
        for(int i = 0; i < n; i++) {

                for(int k = 0; k < channels; k++) {

                    int iIn  = k * n + i;
                    int iOut = i * channels + k;

                    bufferOut[iOut] = bufferIn[iIn];
                }

        }
    }

    /**
     * @brief clone
     * @param bufferOut
     * @param bufferIn
     * @param n
     * @param channels
     * @return
     */
    static T *clone(T *bufferOut, T *bufferIn, int n, int channels)
    {
        if(bufferIn == NULL) {
            return bufferOut;
        }

        if(bufferOut == NULL) {
            bufferOut = new T[n * channels];
        }

        memcpy(bufferOut, bufferIn, n * channels * sizeof(T));

        return bufferOut;
    }

    /**
     * @brief unique
     * @param buffer
     * @param n
     * @param uniqueValues
     * @return
     */
    static void unique(T *buffer, int n, std::set<T> &uniqueValues)
    {
        for(int i = 0; i < n; i++) {
            uniqueValues.insert(buffer[i]);
        }
    }

    /**
     * @brief copySubBuffer
     * @param bufIn
     * @param bi_width
     * @param bi_height
     * @param bi_channels
     * @param startX
     * @param startY
     * @param bufOut
     * @param bo_width
     * @param bo_height
     * @param bo_channels
     */
    static void copySubBuffer(T *bufIn,
                              int bi_width,
                              int bi_height,
                              int bi_channels,
                              int startX,
                              int startY,
                              T *bufOut,
                              int bo_width,
                              int bo_height,
                              int bo_channels)
    {
        if(bufIn == NULL || bufOut == NULL || bi_channels != bo_channels) {
            return;
        }

        //check bounds
        int sX, sY, eX, eY, dX, dY, shiftX, shiftY;

        //start
        sX = MIN(startX, bo_width);
        sX = MAX(sX, 0);

        sY = MIN(startY, bo_height);
        sY = MAX(startY, 0);

        dX = sX - startX;

        if(dX < 0) {
            shiftX = dX;
        } else {
            shiftX = -sX;
        }

        //end
        eX = MIN(startX + bi_width, bo_width);
        eX = MAX(eX, 0);

        eY = MIN(startY + bi_height, bo_height);
        eY = MAX(eY, 0);

        dY = sY - startY;

        if(dY < 0) {
            shiftY = dY;
        } else {
            shiftY = -sY;
        }

        #pragma omp parallel for
        for(int j = sY; j < eY; j++) {
            int index_bi = (j + shiftY) * bi_width;
            int index_bo = j * bo_width;

            for(int i = sX; i < eX; i++) {
                Array<T>::assign(bufIn + index_bi + i + shiftY,
                                 bi_channels,
                                 bufOut + index_bo + i);
            }
        }
    }
};

} // end namespace pic

#endif /* PIC_UTIL_BUFFER_HPP */

