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
    T *data;
    int nData;

    /**
     * @brief Array
     */
    Array()
    {
    }

    /**
     * @brief allocate
     * @param n
     */
    void allocate(int n)
    {
        if(n < 1) {
            return;
        }

        data = new T[n];
        this->nData = n;
    }

    /**
     * @brief release
     */
    void release()
    {
        if(nData > 0 && data != NULL) {
            delete[] data;
            data = NULL;
            nData = -1;
        }
    }


    /**
     * @brief genValue
     * @param value
     * @param n
     * @param ret
     * @return
     */
    static T* genValue(T value, int n, T *ret)
    {
        if(n < 1) {
            return ret;
        }

        if(ret == NULL) {
            ret = new T[n];
        }

        for(int i = 0; i < n; i++) {
            ret[i] = value;
        }

        return ret;
    }

    /**
     * @brief genRange
     * @param minVal
     * @param step
     * @param maxVal
     * @param ret
     */
    static T *genRange(T minVal, T step, T maxVal, T *ret, int &n)
    {
        n = int((maxVal - minVal) / step) + 1;

        if(ret == NULL) {
            ret = new T[n];
        }

        ret[0] = minVal;
        for(int i = 1; i < n; i++) {
            ret[i] = ret[i - 1] + step;
        }

        return ret;
    }

    /**
     * @brief linspace
     * @param minVal
     * @param maxVal
     * @param n
     * @param ret
     * @return
     */
    static T *linspace(T minVal, T maxVal, int n, T *ret)
    {
        T step = (maxVal - minVal) / (n - 1);
        int tmp = n;

        return genRange(minVal, step, maxVal, ret, tmp);
    }

    /**
     * @brief distanceSq
     * @param a0
     * @param a1
     * @param n
     * @return
     */
    static inline T distanceSq(T *a0, T *a1, int n)
    {
        T distSq = T(0);

        for(int k = 0; k < n; k++) {
            T tmp = a0[k] - a1[k];
            distSq += tmp * tmp;
        }

        return distSq;
    }

    /**
     * @brief norm_sq
     * @param data
     * @param n
     * @return
     */
    static inline T norm_sq(float *data, int n)
    {
        T n_sq = T(0);

        for(int k = 0; k < n; k++) {
            T tmp = data[k];
            n_sq += tmp * tmp;
        }

        return n_sq;
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
     * @brief dot
     * @param data0
     * @param data1
     * @param n
     * @return
     */
    static inline T dot(T *data0, T *data1, int n)
    {
        T out = data0[0] * data1[0];

        for(int k = 1; k < n; k++) {
            out += data0[k] * data1[k];
        }

        return out;
    }

    /**
     * @brief mul
     * @param vec
     * @param size
     * @param scale
     * @return
     */
    static inline void mul(T *vec, int size, T scale)
    {
        if(vec == NULL || size < 1) {
            return;
        }

        for(int i = 0; i < size; i++) {
            vec[i] *= scale;
        }
    }

    /**
     * @brief mul
     * @param data
     * @param size
     * @param ret
     * @return
     */
    static inline void mul(T *data, int size, T *ret)
    {
        if(data == NULL || size < 1 || ret == NULL) {
            return;
        }

        for(int i = 0; i < size; i++) {
            ret[i] *= data[i];
        }
    }

    /**
     * @brief add
     * @param data
     * @param size
     * @param ret
     */
    static inline T* add(T *data, int size, T *ret)
    {
        for(int i = 0; i < size; i++) {
            ret[i] += data[i];
        }

        return ret;
    }

    /**
     * @brief div
     * @param data
     * @param size
     * @param value
     */
    static inline void div(T *data, int size, T value)
    {
        for(int i = 0; i < size; i++) {
            data[i] /= value;
        }
    }

    /**
     * @brief sum
     * @param data
     * @param size
     * @return
     */
    static inline T sum(T *data, int size)
    {
        if(data == NULL || size < 1) {
            return T(0);
        }

        T ret = data[0];

        for(int i = 1; i < size; i++) {
            ret += data[i];
        }

        return ret;
    }

    /**
     * @brief cumsum
     * @param vec
     * @param size
     * @param ret
     * @return
     */
    static inline T *cumsum(T *vec, int size, T *ret)
    {
        if(vec == NULL || size < 1) {
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
     * @brief assign
     * @param data
     * @param size
     * @param ret
     * @return
     */
    static inline T* assign (T* data, int size, T* ret)
    {
        memcpy(ret, data, sizeof(T) * size);
        return ret;
    }

    /**
     * @brief assign
     * @param data
     * @param ret
     * @param size
     * @return
     */
    static inline T* assign (T data, T* ret, int size)
    {
        for(int i = 0; i < size; i++) {
            ret[i] = data;
        }
        return ret;
    }

    /**
     * @brief apply
     * @param data
     * @param size
     * @param ret
     * @return
     */
    static inline T* apply(T *data,  int size, T *ret, T(*func)(T))
    {
        if(data == NULL || size < 1) {
            return ret;
        }

        if(ret == NULL) {
            ret = new T[size];
        }

        for(int i = 1; i < size; i++) {
            ret[i] = func(data[i]);
        }

        return ret;
    }

    /**
     * @brief getMax
     * @param data
     * @param size
     * @param ind
     * @return
     */
    static inline T getMax(T *data, int size, int &ind)
    {
        if(data == NULL || size < 1) {
            return T(size + 1);
        }

        T ret = data[0];
        ind = 0;

        for(int i = 1; i < size; i++) {
            if(ret < data[i]) {
                ret = data[i];
                ind = i;
            }
        }

        return ret;
    }

    /**
     * @brief getMin
     * @param data
     * @param size
     * @param ind
     * @return
     */
    static inline T getMin(T *data, int size, int &ind)
    {
        if(data == NULL || size < 1) {
            return T(size + 1);
        }

        T ret = data[0];
        ind = 0;

        for(int i = 1; i < size; i++) {
            if(ret > data[i]) {
                ret = data[i];
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

