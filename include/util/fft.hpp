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

#ifndef PIC_UTIL_FFT_HPP
#define PIC_UTIL_FFT_HPP

#include <string.h>
#include <complex>

#include "../base.hpp"
#include "../util/math.hpp"

namespace pic {

/**
 * @brief RE
 * @param x
 * @return
 */
PIC_INLINE unsigned int RE(unsigned int x) {
    return x << 1;
}

/**
 * @brief IM
 * @param x
 * @return
 */
PIC_INLINE unsigned int IM(unsigned int x) {
    return (x << 1) + 1;
}

/**
 * @brief complexf
 */
typedef std::complex<float> complexf;

/**
 * @brief complexd
 */
typedef std::complex<double> complexd;

/**
 * @brief DFT1D
 * @param in
 * @param n
 * @param out
 * @return
 */
PIC_INLINE float *DFT1D(float *in, unsigned int n, float *out = NULL)
{
    if(out == NULL) {
        out = new float[n * 2];
    }

    float n_f = float(n);

    for(unsigned int i = 0; i < n ; i++) {
        unsigned int re = RE(i);
        unsigned int im = IM(i);

        out[re] = 0.0f;
        out[im] = 0.0f;

        float i_f = float(i);
        for(unsigned int j = 0; j < n ; j++) {

            float angle = -C_PI_2 * i_f * float(j) / n_f;
            out[re] += in[j] * cosf(angle);
            out[im] += in[j] * sinf(angle);
        }
    }

    return out;
}

/**
 * @brief bitReversal
 * @param n
 * @param nbit
 * @return
 */
PIC_INLINE unsigned int bitReversal(unsigned int n, unsigned int nbit)
{
    unsigned int out = 0;
    for(int i = nbit; i>0; i--) {
        unsigned int bit = (n >> (i - 1)) & 0x00000001;
        out += bit << (nbit - i);
    }

    return out;
}

/**
 * @brief FFTIterative1D
 * @param in
 * @param n
 * @param out
 * @return
 */
PIC_INLINE float *FFTIterative1D(float *in, unsigned int n, float *out = NULL)
{
    if(out == NULL) {
        out = new float[n * 2];
    }

    unsigned int logn = std::log2(n);

    for(unsigned int i = 0; i< n; i++) {
        //bit reversal
        unsigned int i_rev = bitReversal(i, logn);
        out[RE(i_rev)] = in[i];
        out[IM(i_rev)] = 0.0f;
    }

    for(unsigned int s = 1; s <= logn; s++) {
        int m = 1 << s;
        float angle = -C_PI_2 / float(m);

        complexf omega_m = complexf(cosf(angle), sinf(angle));
        complexf omega = complexf(1.0f, 0.0f);

        for(unsigned int j = 0; j < (m / 2); j++) {
            for(unsigned int k = j; k < n; k += m) {
                unsigned int ind = k + m / 2;

                complexf t = omega * complexf(out[RE(ind)], out[IM(ind)]);
                complexf u = complexf(out[RE(k)], out[IM(k)]);

                complexf tmp0 = u + t;

                out[RE(k)] = tmp0.real();
                out[IM(k)] = tmp0.imag();

                complexf tmp1 = u - t;

                out[RE(ind)] = tmp1.real();
                out[IM(ind)] = tmp1.imag();
            }

            omega *= omega_m;
        }
    }

    return out;
}

/**
 * @brief fftTest
 */
PIC_INLINE void fftTest()
{
    int n = 16;
    float *values = new float[n];
    float *values_fft = new float[n * 2];
    float *values_dft = new float[n * 2];

    for(int i=0;i<n;i++) {
        values[i] = 0.0f;
    }

    values[1] = 1.0f;
    values[2] = 1.0f;

    FFTIterative1D(values, n, values_fft);

    printf("FFT\n");
    for(int i=0;i<n;i++) {
        printf("%3.3f %3.3f\n", values_fft[RE(i)], values_fft[IM(i)]);
    }

    DFT1D(values, n, values_dft);
    printf("DFT\n");
    for(int i=0;i<n;i++) {
        printf("%3.3f %3.3f\n", values_dft[RE(i)], values_dft[IM(i)]);
    }
}

} // end namespace pic

#endif /* PIC_UTIL_FFT_HPP */

