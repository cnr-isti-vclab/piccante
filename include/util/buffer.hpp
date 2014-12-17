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

#include "base.hpp"
#include "util/math.hpp"

namespace pic {

/**BufferAssign assigns a constant value*/
template<class T>
PIC_INLINE T *BufferAssign(T *buffer, int n, T value)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        buffer[i] = value;
    }

    return buffer;
}

template<class T>
PIC_INLINE T *BufferAssign(T *bufferOut, T *bufferIn, int n)
{
    memcpy(bufferOut, bufferIn, n * sizeof(T));
    return bufferOut;
}

/**BufferAdd adds a constant value*/
template<class T>
PIC_INLINE T *BufferAdd(T *buffer, int n, T value)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        buffer[i] += value;
    }

    return buffer;
}

template<class T>
PIC_INLINE T *BufferAdd(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] = bufferIn0[i] + bufferIn1[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferAdd(T *bufferOut, T *bufferIn, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] += bufferIn[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferAddS(T *bufferOut, T *bufferIn, int n, int channels)
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

/**BufferMul multiplies a constant value*/
template<class T>
PIC_INLINE T *BufferMul(T *buffer, int n, T value)
{
    #pragma omp parallel for
    for(int i = 0; i < n; i++) {
        buffer[i] *= value;
    }

    return buffer;
}

template<class T>
PIC_INLINE T *BufferMul(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] = bufferIn0[i] * bufferIn1[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferMul(T *bufferOut, T *bufferIn, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] *= bufferIn[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferMulS(T *bufferOut, T *bufferIn, int n, int channels)
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

/**BufferSub subtracts a constant value*/
template<class T>
PIC_INLINE T *BufferSub(T *buffer, int n, T value)
{
    #pragma omp parallel for
    for(int i = 0; i < n; i++) {
        buffer[i] -= value;
    }

    return buffer;
}

template<class T>
PIC_INLINE T *BufferSub(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] = bufferIn0[i] - bufferIn1[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferSub(T *bufferOut, T *bufferIn, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] -= bufferIn[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferSubS(T *bufferOut, T *bufferIn, int n, int channels)
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

/**BufferDiv divides by a constant value*/
template<class T>
PIC_INLINE T *BufferDiv(T *buffer, int n, T value)
{
    #pragma omp parallel for
    for(int i = 0; i < n; i++) {
        buffer[i] /= value;
    }

    return buffer;
}

template<class T>
PIC_INLINE T *BufferDiv(T *bufferOut, T *bufferIn0, T *bufferIn1, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] = bufferIn0[i] / bufferIn1[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferDiv(T *bufferOut, T *bufferIn, int n)
{
    #pragma omp parallel for

    for(int i = 0; i < n; i++) {
        bufferOut[i] /= bufferIn[i];
    }

    return bufferOut;
}

template<class T>
PIC_INLINE T *BufferDivS(T *bufferOut, T *bufferIn, int n, int channels)
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

/**FlipH: flips a buffer horizontally*/
template<class T>
PIC_INLINE void BufferFlipH(T *buffer, int width, int height, int channels,
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

/**FlipV: flips an image vertically*/
template<class T>
PIC_INLINE void BufferFlipV(T *buffer, int width, int height, int channels,
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

/**BufferRotate90CW: rotates an image 90 CW*/
template<class T>
PIC_INLINE void BufferRotate90CW(T *buffer, int &width, int &height, int channels)
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

/**BufferRotate90CCW: rotates an image 90 CCW*/
template<class T>
PIC_INLINE void BufferRotate90CCW(T *buffer, int &width, int &height, int channels)
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

/**Shift: */
template<class T>
PIC_INLINE T *BufferShift(T *bufferOut, T *bufferIn, int dx, int dy, int width,
                          int height, int channels, int frames)
{
    if(bufferOut == NULL) {
        bufferOut = new T[width * height * channels * frames];
    }

    #pragma omp parallel for

    for(int i = 0; i < height; i++) {
        int tmp1 = i * width;
        int i2 = i + dy;

        if((i2) < 0 || (i2 >= height)) {

            for(int j=0; j<width; j++){
                int ind1 = (tmp1 + j) * channels;
                for(int k = 0; k < channels; k++) {
                    bufferOut[ind1 + k] = T(0);
                }
            }

        } else {
            int tmp2 = i2 * width;

            for(int j = 0; j < width; j++) {
                int j2 = j + dx;
                int ind1 = (tmp1 + j) * channels;

                if((j2) < 0 || (j2 >= width)) {
                    for(int k = 0; k < channels; k++) {
                        bufferOut[ind1 + k] = T(0);
                    }
                } else {

                    int ind2 = (tmp2 + j2) * channels;

                    for(int k = 0; k < channels; k++) {
                        bufferOut[ind1 + k] = bufferIn[ind2 + k];
                    }
                }
            }
        }
    }

    return bufferOut;
}

/**This function transposes a buffer*/
template<class T>
PIC_INLINE T *BufferTranspose(T *bufferOut, T *bufferIn, int width, int height,
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

/**This function swizzle from BGR to RGB a buffer*/
template<class T>
PIC_INLINE T *BufferBGRtoRGB(T *buffer, int width, int height,
                          int channels, int frames)
{
    int size = width * height * channels * frames;
    for(int i=0; i<size; i+=channels) {
        T tmp         = buffer[i];
        buffer[i    ] = buffer[i + 2];
        buffer[i + 2] = tmp;
    }

    return buffer;
}

} // end namespace pic

#endif /* PIC_UTIL_BUFFER_HPP */

