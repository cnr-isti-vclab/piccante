/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_UTIL_VEC_HPP
#define PIC_UTIL_VEC_HPP

#include <random>
#include <assert.h>

#include "util/math.hpp"

namespace pic {

template<unsigned int N, class T>
class Vec
{
public:
    T data[N];

    Vec<N, T>()
    {
    }

    Vec<N, T>(T data0)
    {
        assert(N >= 1);

        for(unsigned int i = 0; i < N; i++) {
            data[i] = data0;
        }
    }

    Vec<N, T>(T data0, T data1)
    {
        assert(N >= 2);
        data[0] = data0;
        data[1] = data1;
    }

    Vec<N, T>(T data0, T data1, T data2)
    {
        assert(N >= 3);
        data[0] = data0;
        data[1] = data1;
        data[2] = data2;
    }

    T &operator[](int index)
    {
        return data[index];
    }

    void setZero()
    {
        for(unsigned int i=0; i<N; i++) {
            data[i] = T(0);
        }
    }

    const T &operator[](int index) const
    {
        return data[index];
    }

    bool equal(Vec<N, T> a)
    {
        for(unsigned int i = 0; i < N; i++) {
            if(a[i] != data[i]) {
                return false;
            }
        }

        return true;
    }

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

    T lengthSq()
    {
        T l2 = data[0] * data[0];

        for(unsigned int i = 1; i < N; i++) {
            l2 += data[i] * data[i];
        }

        return l2;
    }
};

float Randomf()
{
    return float(rand() % RAND_MAX) / float(RAND_MAX);
}

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

template<unsigned int N>
Vec<N, float> randomPoint(std::mt19937 *m)
{
    Vec<N, float> x;

    for(unsigned int i = 0; i < N; i++) {
        x[i] = Random((*m)()) * 2.0f - 1.0f;
    }

    return x;
}

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

} // end namespace pic

#endif /* PIC_UTIL_VEC_HPP */

