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

#include "../util/math.hpp"

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
     * @brief setZero
     */
    void setZero()
    {
        for(unsigned int i = 0; i < N; i++) {
            data[i] = T(0);
        }
    }


    Vec<N, T> clone()
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, N * sizeof(T));
        return ret;
    }

    /**
     * @brief operator []
     * @param index
     * @return
     */
    const T &operator[](std::size_t i) const
    {
        return data[i];
    }

    /**
     * @brief operator []
     * @param index
     * @return
     */
    T &operator[](std::size_t i)
    {
        return data[i];
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
     * @brief getMax
     * @return
     */
    T getMax()
    {
        T ret = this->data[0];
        for (int i = 1; i < N; i++) {
            ret = this->data[i] > ret ? this->data[i] : ret;
        }
        return ret;
    }

    /**
     * @brief getMaxChannel
     * @return
     */
    int getMaxChannel()
    {
        float valMax = getMax();

        for (int i = 1; i < N; i++) {
            if (valMax == this->data[i]) {
                return i;
            }
        }

        return -1;
    }

    /**
     * @brief dot
     * @param a
     */
    T dot(Vec<N, T> a)
    {
        T out = T(0);
        for(unsigned int i=0; i<N; i++) {
            out += data[i] * a[i];
        }
        return out;
    }

    /**
     * @brief squaredSum
     * @return
     */
    T squaredSum()
    {
        T ret = this->data[0] * this->data[0];

        for(unsigned int i = 1; i < N; i++) {
            ret += this->data[i] * this->data[i];
        }

        return ret;
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
    * @brief clamp
    * @param min
    * @param max
    * @return
    */
    void clamp(T min, T max)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] = CLAMPi(this->data[i], min, max);
        }
    }

    /**
     * @brief lengthSq
     * @return
     */
    T lengthSq()
    {
        T out = data[0] * data[0];

        for(unsigned int i = 1; i < N; i++) {
            out += data[i] * data[i];
        }

        return out;
    }

    /*
    *
    *	Scalar Operands
    *
    */

    /**
     * @brief operator =
     * @param a
     */
    void operator =(const T &a)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] = a;
        }
    }

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const T &a)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] += a;
        }
    }

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const T *a)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] = a[i];
        }
    }

    /**
     * @brief operator +
     * @param a
     * @return
     */
    Vec<N, T> operator +(const T &a) const
    {
        Vec<N, T> ret = this->clone();
        ret += a;
        return ret;
    }

    /**
     * @brief operator -=
     * @param a
     */
    void operator -=(const T &a)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] -= a;
        }
    }

    /**
     * @brief operator -
     * @param a
     * @return
     */
    Vec<N, T> operator -(const T &a) const
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, sizeof(T) * N);
        ret -= a;
        return ret;
    }

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const T &a)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] *= a;
        }
    }

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const T *a)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] *= a[i];
        }
    }

    /**
     * @brief operator *
     * @param a
     * @return
     */
    Vec<N, T> operator *(const T &a) const
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, sizeof(T) * N);
        ret *= a;
        return ret;
    }

    /**
     * @brief operator /=
     * @param a
     */
    void operator /=(const float &a)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] /= a;
        }
    }

    /**
     * @brief operator /
     * @param a
     * @return
     */
    Vec<N, T> operator /(const float &a) const
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, sizeof(T) * N);
        ret /= a;
        return ret;
    }

    /*
    *
    *	Vec Operands
    *
    */

    /**
     * @brief operator +=
     * @param col
     */
    void operator +=(const Vec<N, T> &col)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] += col.data[i];
        }
    }

    /**
     * @brief operator +
     * @param col
     * @return
     */
    Vec<N, T> operator +(const Vec<N, T> &col) const
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, sizeof(T) * N);
        ret += col;
        return ret;
    }

    /**
     * @brief operator -=
     * @param col
     */
    void operator -=(const Vec<N, T> &col)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] -= col.data[i];
        }
    }

    /**
     * @brief operator -
     * @return
     */
    Vec<N, T> operator -() const
    {
        Vec<N, T> ret;
        for (int i = 0; i < N; i++) {
            ret.data[i] = -this->data[i];
        }

        return ret;
    }

    /**
     * @brief operator -
     * @param col
     * @return
     */
    Vec<N, T> operator -(const Vec<N, T> &col) const
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, sizeof(T) * N);
        ret -= col;
        return ret;
    }

    /**
     * @brief operator *=
     * @param col
     */
    void operator *=(const Vec<N, T> &col)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] *= col.data[i];
        }
    }

    /**
     * @brief operator *
     * @param col
     * @return
     */
    Vec<N, T> operator *(const Vec<N, T> &col)
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, sizeof(T) * N);
        ret *= col;
        return ret;
    }

    /**
     * @brief operator /=
     * @param col
     */
    void operator /=(Vec<N, T> &col)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] /= col.data[i];
        }
    }

    /**
     * @brief operator /
     * @param col
     * @return
     */
    Vec<N, T> operator /(Vec<N, T> &col) const
    {
        Vec<N, T> ret;
        memcpy(ret.data, this->data, sizeof(T) * N);
        ret /= col;
        return ret;
    }

    /**
     * @brief operator !=
     * @param col
     * @return
     */
    bool operator !=(Vec<N, T> &col)
    {
        bool ret = false;
        for (int i = 0; i < N; i++) {
            ret = ret || (this->data[i] != col.data[i]);
        }
        return ret;
    }

    /**
     * @brief operator ==
     * @param col
     * @return
     */
    bool operator ==(Vec<N, T> &col)
    {
        bool ret = true;
        for (int i = 0; i < N; i++) {
            ret = ret && (this->data[i] == col.data[i]);
        }
        return ret;
    }
};

/**
 * @brief insideVecBBox
 * @param sample
 * @return
 */
template<unsigned int N>
PIC_INLINE bool insideVecBBox(const Vec<N, float> &sample)
{
    for(unsigned int i = 0; i < N; i++) {
        if((sample[i] < -1.0f) || (sample[i] > 1.0f)) {
            return false;
        }
    }

    return true;
}

template<unsigned int N>
PIC_INLINE Vec<N, float> normalize(Vec<N, float> x)
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
PIC_INLINE Vec<N, float> randomPoint(std::mt19937 *m)
{
    Vec<N, float> x;

    for(unsigned int i = 0; i < N; i++) {
        x[i] = Random((*m)()) * 2.0f - 1.0f;
    }

    return x;
}

template<unsigned int N>
PIC_INLINE void printVec(Vec<N, float> x)
{
    printf("[");
    for(unsigned int i = 0; i < N; i++) {
        printf("%f ", x[i]);
    }
    printf("]\n");
}

/**
 * @brief annulusSampling
 * @param m
 * @param center
 * @param radius
 * @return
 */
template<unsigned int N>
PIC_INLINE Vec<N, float> annulusSampling(std::mt19937 *m, Vec<N, float> center, float radius)
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

template<unsigned int N, class T>
void transferFromVecToPlain(std::vector< Vec<N, T> > &in, std::vector< T > &out)
{
    for(auto i = 0; i < in.size(); i++) {
        for(auto j = 0; j < N; j++) {
            out.push_back(in[i][j]);
        }
    }
}

template<unsigned int N, class T>
void transferFromPlainToVec(std::vector< T > &in, std::vector< Vec<N, T> > &out)
{
    for(auto i = 0; i < in.size(); i+= N) {
        Vec<N, T> tmp;
        for(auto j = 0; j < N; j++) {
            tmp[j] = in[i + j];
        }

        out.push_back(tmp);
    }
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
 * @brief Vec2f
 */
typedef Vec<2, float> Vec2f;

/**
 * @brief Vec3f
 */
typedef Vec<3, float> Vec3f;

} // end namespace pic

#endif /* PIC_UTIL_VEC_HPP */

