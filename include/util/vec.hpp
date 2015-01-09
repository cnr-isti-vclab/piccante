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

#ifndef PIC_UTIL_VEC_HPP
#define PIC_UTIL_VEC_HPP

#include <random>
#include <assert.h>

#include "util/math.hpp"

namespace pic {

/**
 * @brief The Vec class
 */
template<unsigned int N, class T>
class Vec
{
public:
    T data[N];

    /**
     * @brief Vec<N, T>
     */
    Vec<N, T>()
    {
    }

    /**
     * @brief Vec<N, T>
     * @param data0
     */
    Vec<N, T>(T data0)
    {
        assert(N >= 1);

        for(unsigned int i = 0; i < N; i++) {
            data[i] = data0;
        }
    }

    /**
     * @brief Vec<N, T>
     * @param data0
     * @param data1
     */
    Vec<N, T>(T data0, T data1)
    {
        assert(N >= 2);
        data[0] = data0;
        data[1] = data1;
    }

    /**
     * @brief Vec<N, T>
     * @param data0
     * @param data1
     * @param data2
     */
    Vec<N, T>(T data0, T data1, T data2)
    {
        assert(N >= 3);
        data[0] = data0;
        data[1] = data1;
        data[2] = data2;
    }

    /**
     * @brief operator []
     * @param index
     * @return
     */
    T &operator[](int index)
    {
        return data[index];
    }

    /**
     * @brief setZero
     */
    void setZero()
    {
        for(unsigned int i=0; i<N; i++) {
            data[i] = T(0);
        }
    }

    /**
     * @brief operator []
     * @param index
     * @return
     */
    const T &operator[](int index) const
    {
        return data[index];
    }

    /**
     * @brief equal
     * @param a
     * @return
     */
    bool equal(Vec<N, T> a)
    {
        for(unsigned int i = 0; i < N; i++) {
            if(a[i] != data[i]) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief distanceSq
     * @param x
     * @return
     */
    T distanceSq(Vec<N, T> &x)
    {
        T tmp = data[0] - x[0];
        T d2 = tmp * tmp;

        for(unsigned int i = 1; i < N; i++) {
            tmp = data[i] - x[i];
            d2 += tmp * tmp;
        }

        return d2;
    }

    /**
     * @brief lengthSq
     * @return
     */
    T lengthSq()
    {
        T l2 = data[0] * data[0];

        for(unsigned int i = 1; i < N; i++) {
            l2 += data[i] * data[i];
        }

        return l2;
    }
};

/**
 * @brief insideVecBBox
 * @param sample
 * @return
 */
template<unsigned int N>
bool insideVecBBox(const Vec<N, float> &sample)
{
    for(unsigned int i = 0; i < N; i++) {
        if((sample[i] < -1.0f) || (sample[i] > 1.0f)) {
            return false;
        }
    }

    return true;
}

template<unsigned int N>
Vec<N, float> normalize(Vec<N, float> x)
{
    float length = x.distanceSq();

    if(length > 0.0f) {
        length = sqrtf(length);
        for(unsigned int i = 0; i < N; i++) {
            x[i] /= length;
        }
    }

    return x;
}

/**
 * @brief randomPoint
 * @param m
 * @return
 */
template<unsigned int N>
Vec<N, float> randomPoint(std::mt19937 *m)
{
    Vec<N, float> x;

    for(unsigned int i = 0; i < N; i++) {
        x[i] = Random((*m)()) * 2.0f - 1.0f;
    }

    return x;
}

/**
 * @brief annulusSampling
 * @param m
 * @param center
 * @param radius
 * @return
 */
template<unsigned int N>
Vec<N, float> annulusSampling(std::mt19937 *m, Vec<N, float> center, float radius)
{
    Vec<N, float> x;

    while(true) {
        for(unsigned int i = 0; i < N; i++) {
            x[i] = Random((*m)()) * 4.0f - 2.0f;
        }

        float t = x.lengthSq();

        if((t < 1.0f) || (t > 4.0f)) {
            break;
        }
    }

    for(unsigned int i = 0; i < N; i++) {
        x[i] = x[i] * radius + center[i];
    }

    return x;
}

/**
 * @brief Vec2i
 */
typedef Vec<2, int> Vec2i;

/**
 * @brief Vec3i
 */
typedef Vec<3, int> Vec3i;

/**
 * @brief Vec4i
 */
typedef Vec<4, int> Vec4i;

/**
 * @brief Vec3f
 */
typedef Vec<3, float> Vec3f;

} // end namespace pic

#endif /* PIC_UTIL_VEC_HPP */

