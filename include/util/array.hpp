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
protected:
    bool bShallow;

public:
    T *data;
    int nData;

    /**
     * @brief Array
     */
    Array()
    {
        bShallow = false;
        data = NULL;
        nData = -1;
    }

    /**
     * @brief Array
     * @param n
     */
    Array(int n)
    {
        bShallow = false;
        data = NULL;
        allocate(n);
    }

    /**
     * @brief Array
     * @param data
     * @param nData
     * @param bShallow
     */
    Array(T *data, int nData, bool bShallow)
    {
        this->nData = nData;

        if(bShallow) {
            this->data = data;
            this->bShallow = bShallow;
        } else {
            this->data = new T[nData];
            memcpy(this->data, data, sizeof(T) * nData);
        }
    }

    ~Array()
    {
        release();
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

        if((data != NULL) && (!bShallow)) {
            delete[] data;
        }

        data = new T[n];
        this->nData = n;
        bShallow = false;
    }

    /**
     * @brief release
     */
    void release()
    {
        if(nData > 0 && data != NULL && !bShallow) {
            delete[] data;
            data = NULL;
            nData = -1;
        }
    }

    /**
     * @brief clone
     * @return
     */
    Array<T> *clone()
    {
        Array<T> *out = new Array<T>(nData);
        memcpy(this->data, data, sizeof(T) * nData);
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
     * @param data0
     * @param data1
     * @param n
     * @return
     */
    static T distanceSq(T *data0, T *data1, int n)
    {
        T tmp = data0[0] - data1[0];
        T distSq = tmp * tmp;

        for(int k = 1; k < n; k++) {
            tmp = data0[k] - data1[k];
            distSq += tmp * tmp;
        }

        return distSq;
    }

    /**
     * @brief zeros
     * @param n
     * @return
     */
    static T* zeros(int n)
    {
        T *ret = new T[n];
        assign(T(0), ret, n);

        return ret;
    }

    /**
     * @brief norm_sq
     * @param data
     * @param n
     * @return
     */
    static T norm_sq(float *data, int n)
    {
        T n_sq = data[0] * data[0];

        for(int k = 1; k < n; k++) {
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
    static T norm(float *data, int n)
    {
        return sqrtf(Array<float>::norm(data, n));
    }

    /**
     * @brief normalize
     * @param data
     * @param n
     * @param norm_sq
     * @return
     */
    static float normalize(float *data, int n, float norm_sq = -1.0f)
    {
        if(norm_sq < 0.0f) {
            norm_sq = Array<float>::norm_sq(data, n);
        }

        if(norm_sq > 0.0f) {
            norm_sq = sqrtf(norm_sq);

            for(int k = 0; k < n; k++) {
                data[k] /= norm_sq;
            }
        }

        return norm_sq;
    }

    static void clamp(T *data, int n, T lower_bound, T upper_bound)
    {
        for(int i = 0; i < n; i++) {
            data[i] = data[i] >= lower_bound ? data[i] : lower_bound;
            data[i] = data[i] <= upper_bound ? data[i] : upper_bound;
        }
    }

    /**
     * @brief dot
     * @param data0
     * @param data1
     * @param n
     * @return
     */
    static T dot(T *data0, T *data1, int n)
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
    static void mul(T *data, int size, T scale)
    {
        if(data == NULL || size < 1) {
            return;
        }

        for(int i = 0; i < size; i++) {
            data[i] *= scale;
        }
    }

    /**
     * @brief mul
     * @param data
     * @param size
     * @param ret
     * @return
     */
    static void mul(T *data, int size, T *ret)
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
    static T* add(T *data, int size, T *ret)
    {
        if(data == NULL || ret == NULL || size < 1) {
            return ret;
        }

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
    static void div(T *data, int size, T value)
    {
        for(int i = 0; i < size; i++) {
            data[i] /= value;
        }
    }

    /**
     * @brief getMean
     * @param data
     * @param size
     * @param ind
     * @return
     */
    static T getMean(T *data, int size)
    {
        if(data == NULL || size < 1) {
            return T(0);
        }

        T ret = data[0];

        for(int i = 1; i < size; i++) {
            ret += data[i];
        }

        ret /= T(size);

        return ret;
    }

    /**
     * @brief getVariance
     * @param data
     * @param size
     * @return
     */
    static T getVariance(T *data, int size)
    {
        if(data == NULL || size < 2) {
            return T(-1);
        }

        T mu = getMean(data, size);

        T tmp = data[0] - mu;
        T ret = tmp * tmp;

        for(int i = 1; i < size; i++) {
            tmp = data[i] - mu;
            ret += tmp * tmp;
        }

        size--;
        return ret / T(size);
    }

    /**
     * @brief sum
     * @param data
     * @param size
     * @return
     */
    static T sum(T *data, int size)
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
    static T *cumsum(T *vec, int size, T *ret)
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
    static T* assign (T* data, int size, T* ret)
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
    static T* assign (T data, T* ret, int size)
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
    static T* apply(T *data,  int size, T *ret, T(*func)(T))
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
    static T getMax(T *data, int size, int &ind)
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
    static T getMin(T *data, int size, int &ind)
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
     * @brief interp linearly interpolates x and y data
     * @param x
     * @param y
     * @param size the size of x and y
     * @param xval
     * @return
     */
    static T interp(T *x, T *y, int size, T xval)
    {
        int sm1 = size - 1;
        if((xval >= x[0]) && (xval <= x[sm1])) {
            int offset2;
            T *ptr = std::lower_bound(&x[0], &x[sm1], xval);
            int offset = MAX(0, (int)(ptr - x - 1));

            if(offset == sm1) {
                offset2 = offset;
                offset  = offset2 - 1;
            } else {
                offset2 = MIN(size - 1, offset + 1);
            }

            T t = (xval - x[offset]) / (x[offset2] - x[offset]);

            return y[offset] * (T(1) - t) + t * y[offset2];
        } else {
            if(xval > x[sm1]) {
                int sm2 = size - 2;
                T t = (xval - x[sm2]) / (x[sm1] - x[sm2]);
                return t * (y[sm1] - y[sm2])  + y[sm2];
            } else {
                T t = (xval - x[0]) / (x[1] - x[0]);
                return t * (y[1] - y[0])  + y[0];
            }
        }

    }

    /**
     * @brief binSearchLeft
     * @param data
     * @param key
     * @param low
     * @param high
     * @return
     */
    static int binSearchLeft(T *data, T key, int low, int high)
    {
        if( (high < low) ||
            (key > data[high - 1]) ||
            (key < data[low]) ) {

            #ifdef PIC_DEBUG
                printf("\nbinSearchLeft: bounds error!\n");
            #endif
            return -1;
        }

        int mid;
        while(low < high) {
            mid = (low + high) / 2;

            if(data[mid] < key) {
                low = mid + 1;
            } else {
                high = mid;
            }

        }

        if (low > 0) {
            low--;
        }

        return low;
    }
};

/**
 * @brief Arrayf
 */
typedef	Array<float> Arrayf;

/**
 * @brief Arrayi
 */
typedef	Array<int> Arrayi;

/**
 * @brief Arrayui
 */
typedef	Array<unsigned int> Arrayui;

} // end namespace pic

#endif /* PIC_UTIL_ARRAY_HPP */

