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

#ifndef PIC_UTIL_ARRAY_HPP
#define PIC_UTIL_ARRAY_HPP

#include <vector>
#include <math.h>

namespace pic {

/**
 * @brief The Array class
 */
template<class T>
class Array
{
public:

    /**
     * @brief Array
     */
    Array()
    {

    }

    /**
     * @brief genRange
     * @param minVal
     * @param step
     * @param maxVal
     * @return
     */
    static std::vector<T> *genRange(T minVal, T step, T maxVal)
    {
        int n = int((maxVal - minVal) / step) + 1;

        std::vector<T> *ret = new std::vector<T>;

        T val;

        for(int i = 0; i < n; i++) {
            val = minVal + T(i) * step;
            ret->push_back(val);
        }

        return ret;
    }

    /**
     * @brief linspace
     * @param minVal
     * @param maxVal
     * @param n
     * @return
     */
    static std::vector<T> *linspace(T minVal, T maxVal, int n)
    {
        T step = (maxVal - minVal) / (n - 1);
        return genRangeArray(minVal, step, maxVal);
    }

    /**
     * @brief norm
     * @param data
     * @param n
     * @return
     */
    static inline float norm(float *data, int n)
    {
        float norm = 0.0f;

        for(int k = 0; k < n; k++) {
            float tmp = data[k];
            norm += tmp * tmp;
        }

        return sqrtf(norm);
    }

    /**
     * @brief distanceSq
     * @param a0
     * @param a1
     * @param n
     * @return
     */
    static inline float distanceSq(float *a0, float *a1, int n)
    {
        float distSq = 0.0f;

        for(int k=0; k<n; k++) {
            float tmp = a1[k] - a0[k];
            distSq += tmp * tmp;
        }

        return distSq;
    }

    /**
     * @brief normalize
     * @param data
     * @param n
     * @param norm
     * @return
     */
    static inline float normalize(float *data, int n, float norm = -1.0f)
    {

        if(norm < 0.0f) {
            norm = Array<float>::norm(data, n);
        }

        if(norm > 0.0f) {
            norm = sqrtf(norm);

            for(int k = 0; k < n; k++) {
                data[k] /= norm;
            }
        }

        return norm;
    }

    /**
     * @brief sum
     * @param vec
     * @param size
     * @return
     */
    static inline T sum(T *vec, int size)
    {
        if(vec == NULL) {
            return T(0);
        }

        if(size < 1) {
            return T(0);
        }

        T ret = vec[0];

        for(int i = 1; i < size; i++) {
            ret += vec[i];
        }

        return ret;
    }

    /**
     * @brief cumsum
     * @param vec
     * @param ret
     * @param size
     * @return
     */
    static inline T *cumsum(T *vec, T *ret, int size)
    {
        if(vec == NULL) {
            return NULL;
        }

        if(size < 1) {
            return NULL;
        }

        if(ret == NULL) {
            ret = new T[size];
        }

        ret[0] = vec[0];

        for(int i = 1; i < size; i++) {
            ret[i] = vec[i] + ret[i - 1];
        }

        return ret;
    }

    /**
     * @brief max
     * @param vec
     * @param size
     * @param ind
     * @return
     */
    static inline T max(T *vec, int size, int &ind)
    {
        if(vec == NULL) {
            return T(size + 1);
        }

        if(size < 1) {
            return T(size + 1);
        }

        T ret = vec[0];
        ind = 0;

        for(int i = 1; i < size; i++) {
            if(ret < vec[i]) {
                ret = vec[i];
                ind = i;
            }
        }

        return ret;
    }

    /**
     * @brief min
     * @param vec
     * @param size
     * @param ind
     * @return
     */
    static inline T min(T *vec, int size, int &ind)
    {
        if(vec == NULL) {
            return T(size + 1);
        }

        if(size < 1) {
            return T(size + 1);
        }

        T ret = vec[0];
        ind = 0;

        for(int i = 1; i < size; i++) {
            if(ret > vec[i]) {
                ret = vec[i];
                ind = i;
            }
        }

        return ret;
    }

    /**
     * @brief interp
     * @param x
     * @param y
     * @param size
     * @param xval
     * @return
     */
    static inline T interp(T *x, T *y, int size, T xval)
    {
        int offset, offset2;
        T *ptr  = std::lower_bound(&x[0], &x[size - 1], xval);
        offset  = MAX(0, (int)(ptr - x - 1));

        if(offset == (size - 1)) {
            offset2 = offset;
            offset  = offset2 - 1;
        } else {
            offset2 = MIN(size - 1, offset + 1);
        }

        T t = (xval - x[offset]) / (x[offset2] - x[offset]);

        return y[offset] * (T(1) - t) + t * y[offset2];
    }
};

} // end namespace pic

#endif /* PIC_UTIL_ARRAY_HPP */

