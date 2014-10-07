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

#ifndef PIC_UTIL_MATH_HPP
#define PIC_UTIL_MATH_HPP

#include <math.h>
#include <random>

namespace pic {

//Natural logarithm of 2
const float C_LOG_NAT_2			= 0.69314718055994530941723212145818f;

//Reciprocal natural logarithm of 2
const float C_INV_LOG_NAT_2		= 1.4426950408889634073599246810019f;

//Epsilon
const float C_EPSILON			= 1e-6f;

//Square root of 2
const float C_SQRT_2            = 1.4142135623730950488016887242097f;

//PI/4
const float C_PI_025            = 0.78539816339744830961566084581988f;
//PI/2
const float C_PI_05             = 1.5707963267948966192313216916398f;
//PI
const float C_PI                = 3.1415926535897932384626433832795f;
//(PI *2)
const float C_PI_2              = 6.283185307179586476925286766559f;
// 1 / (PI *2)
const float C_INV_PI_2          = 0.159154943091895335768883763f;
//PI times 4
const float C_PI_4              = 12.566370614359172953850573533118f;
//One over PI times 4
const float C_INV_PI_4          = 0.07957747154594766788444188168626f;
//PI*PI*2
const float C_PI_2_2            = 19.739208802178717237668981999752f;
// 1/PI
const float C_INV_PI            = 0.31830988618379067153776526745f;
// 180/PI
const float C_ONE_80_OVER_PI    = 57.295779513082320876798154814105f;
// PI/180
const float C_PI_OVER_ONE_80    = 0.017453292519943295769236907685f;

#ifndef MIN
    #define MIN(a, b)           (a < b ? a : b)
#endif

#ifndef MAX
    #define MAX(a, b)           (a > b ? a : b)
#endif

#ifndef CLAMP
    #define CLAMP(x, a)         (x >= a ? (a - 1) : (x < 0 ? 0 : x))
#endif

#ifndef CLAMPi
    #define CLAMPi(x, a, b)     (x <  a ? a       : (x > b ? b : x))
#endif

/**isnan: is it a NaN?*/
#ifndef isnan
template< typename T > inline bool isnan(T value)
{
    return value != value ;
}
#endif

/**isnan: is it a Inf value?*/
#ifndef isnan
template< typename T > inline bool isinf(T value)
{
    return std::numeric_limits<T>::has_infinity &&
           (value == std::numeric_limits<T>::infinity() ||
            value == -std::numeric_limits<T>::infinity());
}
#endif

/**
 * @brief equalf checks if two float values are the same or not.
 * @param a is the first value to be checked.
 * @param b is the second value to be checked.
 * @return This function returns true if a and b are similar,
 * false otherwise.
 */
inline bool equalf(float a, float b)
{
    return ( fabsf(a - b) < C_EPSILON);
}

/**
 * @brief Random returns a number in [0, 2^32 - 1] to a float in [0, 1]
 * @param n
 * @return
 */
inline float Random(unsigned int n)
{
    return float(n) / 4294967295.0f;
}

/**SFunction: a cubic s-function*/
inline float SFunction(float x)
{
    float x2 = x * x;
    return 3.0f * x2 - 2.0f * x2 * x;
}

/**SCurve5: a quintic S-Shape: 6x^5-15x^4+10x^3*/
float SCurve5(float t)
{
    float t2 = t * t;
    float t4 = t2 * t2;

    return (6.0f * t - 15.0f) * t4 + 10.0f * t2 * t;
}

//Clamp a value, x, in the bound [a,b]
template< class T >
inline T Clamp(T x, T a, T b)
{
    if(x > b) {
        return b;
    }

    if(x < a) {
        return a;
    }

    return x;
}

/**lround: double rounding*/
inline long lround(double x)
{
    if(x > 0.0) {
        return (x - floor(x) < 0.5) ? (long)floor(x) : (long)ceil(x);
    } else {
        return (x - floor(x) <= 0.5) ? (long)floor(x) : (long)ceil(x);
    }
}

/**lround: float rounding*/
inline float lround(float x)
{
    if(x > 0.0f) {
        return (x - floorf(x) < 0.5f)  ? floorf(x) : ceilf(x);
    } else {
        return (x - floorf(x) <= 0.5f) ? floorf(x) : ceilf(x);
    }
}


/**lerp: Linear interpolation*/
inline float lerp(float t, float x0, float x1)
{
    return x0 + t * (x1 - x0);
}

/**SmoothStep: smoothes a value from a to b using a cube S-Shape: -2x^3+3x^2*/
inline float SmoothStep(float a, float b, float value)
{
    float x = Clamp<float>((value - a) / (b - a), 0.0f, 1.0f);
    return  x * x * (-2.0f * x + 3.0f);
}

/**Deg2Rad: from degrees to radiants*/
inline float Deg2Rad(float deg)
{
    return deg * C_PI_OVER_ONE_80;
}

/**Rad2Deg: from radiants to degrees*/
inline float Rad2Deg(float rad)
{
    return rad * C_ONE_80_OVER_PI;
}

/**log2: logarithm in base 2 for integers*/
inline int log2(int n)
{
    int val = 1;
    int lg  = 0;

    while(val < n) {
        val = val << 1;
        lg++;
    }

    if(val != n) {
        lg--;
    }

    return lg;
}

/**pow2: power of two for integers*/
inline int pow2(int n)
{
    return 1 << n;
}

/**logPlus: log10f adding an 1.0*/
float logPlus(float x)
{
    return log10f(x + 1.0f);
}

/**expMinus: pow10f removing an 1.0*/
inline float expMinus(float x)
{
    float tmp = powf(10.0f, x) - 1.0f;
    return MAX(tmp, 0.0f);
}

/**logPlusEpsilon: log10f plus a small epsilon*/
float logPlusEpsilon(float x)
{
    return log10f(x + 1e-5f);
}

/**log2f: logarithm in base 2 for floating point*/
inline float log2f(float x)
{
    return logf(x) * C_INV_LOG_NAT_2;
}

/**pow2f: power of two for floating point*/
inline float pow2f(float x)
{
    return powf(2.0f, x);
}

/**NegZero: a small value is set to zero*/
inline float NegZero(float val)
{
    return (fabsf(val) < 1e-6f) ? 0.0f : val;
}

/*powint: x^b for integers*/
static int powint(int x, int b)
{
    int ret = 1;
    
    for(int i = 0; i < b; i++) {
        ret *= x;
    }

    return ret;
}

/**
 * @brief getPermutation computes a permutation.
 * @param m is a Mersenne Twister random number generator.
 * @param perm is the array where to store the permutation.
 * @param nPerm is the size of perm.
 * @param n is the number of object to permutate.
 */
void getPermutation(std::mt19937 &m, unsigned int *perm, int nPerm, int n)
{
    int index = 0;
    while(index < nPerm) {
        perm[index] = m() % n;
        bool flag = true;

        for(int j = 0; j < (index - 1); j++) {
            if(perm[index] == perm[j]) {
                flag = false;
                break;
            }
        }

        if(flag) {
            index++;
        }
    }
}

} // end namespace pic

#endif
