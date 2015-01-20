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

#ifndef PIC_UTIL_MATH_HPP
#define PIC_UTIL_MATH_HPP

#include <math.h>
#include <random>
#include <stdlib.h>

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


#ifndef isnan

/**
 * @brief isnan is it a NaN?
 * @param value
 * @return
 */
template< typename T > inline bool isnan(T value)
{
    return value != value ;
}

#endif


#ifndef isnan

/**
 * @brief isinf is it a Inf value?
 * @param value
 * @return
 */
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
 * @brief Randombase returns a number in [0, 1] based on rand().
 * @return It returns a random number in [0, 1].
 */
float Randombase()
{
    return float(rand() % RAND_MAX) / float(RAND_MAX);
}

/**
 * @brief Random returns a number in [0, 2^32 - 1] to a float in [0, 1].
 * @param n is a 32-bit unsigned integer number.
 * @return It returns n as a normalized float in [0, 1].
 */
inline float Random(unsigned int n)
{
    return float(n) / 4294967295.0f;
}

/**
 * @brief RandomInt
 * @param n
 * @param a
 * @param b
 * @return
 */
inline int RandomInt(unsigned int n, int a, int b)
{
    if(a < b) {
        return n % (b - a);
    } else {
        return 0;
    }
}

/**
 * @brief SFunction evaluates a cubic s-function.
 * @param x is a value in [0.0, 1.0]
 * @return it returns 3 x^2 - 2 x^3
 */
inline float SFunction(float x)
{
    float x2 = x * x;
    return 3.0f * x2 - 2.0f * x2 * x;
}

/**
 * @brief SCurve5 evaluates a quintic S-Shape: 6x^5-15x^4+10x^3
 * @param x is a value in [0.0, 1.0]
 * @return
 */
float SCurve5(float x)
{
    float x2 = x * x;
    float x4 = x2 * x2;

    return (6.0f * x - 15.0f) * x4 + 10.0f * x2 * x;
}

/**
 * @brief Clamp clamps a value, x, in the bound [a,b].
 * @param x
 * @param a
 * @param b
 * @return
 */
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

/**
 * @brief lround rounds double numbers properly.
 * @param x is a scalar.
 * @return
 */
inline long lround(double x)
{
    if(x > 0.0) {
        return (x - floor(x) <  0.5) ? (long)floor(x) : (long)ceil(x);
    } else {
        return (x - floor(x) <= 0.5) ? (long)floor(x) : (long)ceil(x);
    }
}

/**
 * @brief lround rounds float numbers properly.
 * @param x is a scalar.
 * @return
 */
inline float lround(float x)
{
    if(x > 0.0f) {
        return (x - floorf(x) < 0.5f)  ? floorf(x) : ceilf(x);
    } else {
        return (x - floorf(x) <= 0.5f) ? floorf(x) : ceilf(x);
    }
}

/**
 * @brief lerp evaluates linear interpolation
 * @param t is a value in [0.0, 1.0].
 * @param x0 is the min value.
 * @param x1 is the max value.
 * @return it returns x0 + t * (x1 - x0)
 */
inline float lerp(float t, float x0, float x1)
{
    return x0 + t * (x1 - x0);
}

/**
 * @brief SmoothStep smoothes a value from a to b using a cube S-Shape.
 * @param a is the min value.
 * @param b is the max value.
 * @param value is a value in [0.0, 1.0].
 * @return It returns - 2 x^3 + 3 x^2.
 */
inline float SmoothStep(float a, float b, float value)
{
    float x = Clamp<float>((value - a) / (b - a), 0.0f, 1.0f);
    return  x * x * (-2.0f * x + 3.0f);
}

/**
 * @brief Deg2Rad converts angles expressed in degrees into angles expressed in radians.
 * @param deg is a value of an angle expressed in degrees.
 * @return It returns an ang expressed in radians.
 */
inline float Deg2Rad(float deg)
{
    return deg * C_PI_OVER_ONE_80;
}

/**
 * @brief Rad2Deg converts angles expressed in radians into angles expressed in degrees.
 * @param rad is a value of an angle expressed in radians.
 * @return It returns an ang expressed in degrees.
 */
inline float Rad2Deg(float rad)
{
    return rad * C_ONE_80_OVER_PI;
}

/**
 * @brief log2 computes logarithm in base 2 for integers.
 * @param n is an integer value.
 * @return It returns log2 of n.
 */
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

/**
 * @brief pow2 computes 2^n.
 * @param n is a positive exponent.
 * @return It returns 2^n.
 */
inline int pow2(int n)
{
    return 1 << n;
}

/**
 * @brief log10Plus computes log10 of a value plus 1.
 * @param x is a value for which the log10 needs to be computed.
 * @return It returns log10(x + 1).
 */
float log10Plus(float x)
{
    return log10f(x + 1.0f);
}

/**
 * @brief expMinus
 * @param x
 * @return
 */
inline float expMinus(float x)
{
    float tmp = powf(10.0f, x) - 1.0f;
    return MAX(tmp, 0.0f);
}

/**
 * @brief log10PlusEpsilon
 * @param x
 * @return
 */
float log10PlusEpsilon(float x)
{
    return log10f(x + 1e-5f);
}

/**
 * @brief log2f logarithm in base 2 for floating point
 * @param x
 * @return
 */
inline float log2f(float x)
{
    return logf(x) * C_INV_LOG_NAT_2;
}

/**
 * @brief pow2f
 * @param x
 * @return
 */
inline float pow2f(float x)
{
    return powf(2.0f, x);
}

/**
 * @brief powint computes power function for integer values.
 * @param x is the base.
 * @param b is the exponent.
 * @return it returns x^b.
 */
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
