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

#ifndef PIC_COLORS_COLOR_HPP
#define PIC_COLORS_COLOR_HPP

#include "../base.hpp"

#include "../util/math.hpp"
#include "../util/vec.hpp"

namespace pic {

/**
 * @brief scaleTau
 * @param in
 * @param tau
 */
template<uint N>
void scaleTau(Vec<N, float> &in, const Vec<N, float> &tau)
{
    for (int i = 0; i < N; i++) {
        in.data[i] *= expf(-tau.data[i]);
    }
}

/**
 * @brief scaleTau
 * @param in
 * @param sigma_t
 * @param tau
 */
template<uint N>
void scaleTau(Vec<N, float> &in, const Vec<N, float> &sigma_t, const Vec<N, float> &tau)
{
    for (int i = 0; i < N; i++) {
        in.data[i] *= expf(-tau.data[i] * sigma_t.data[i]);
    }
}

/**
 * @brief scaleTau
 * @param in
 * @param sigma_t
 * @param t
 */
template<uint N>
void scaleTau(Vec<N, float> &in, const Vec<N, float> &sigma_t, float t)
{
    for (int i = 0; i < N; i++) {
        in.data[i] *= expf(-sigma_t.data[i] * t);
    }
}

/**
 * @brief colorLuminance
 * @param in
 * @return
 */
template<uint N>
float colorLuminance(Vec<N, float> &in)
{
    return  0.213f * in.data[0] +
            0.715f * in.data[1] +
            0.072f * in.data[2];
}

/**
 * @brief colorSaturate
 * @param in
 */
template<uint N>
void colorSaturate(Vec<N, float> &in)
{
    for (int i = 0; i < N; i++) {
        in.data[i] = in.data[i] * 0.5f + 0.5f;
    }
}

/**
 * @brief importanceSampling
 * @param in
 * @param e
 * @param channel
 * @param pdf
 */
template<uint N>
void importanceSampling(Vec<N, float> &in, float e, int &channel, float &pdf)
{
    float sum = 0.0f;
    for(uint i = 0; i < N; i++) {
        sum += in.data[i];
    }

    if(sum > 0.0f) {
        float CDF[N];
        CDF[0] = in.data[0] / sum;
        for(uint i = 1; i < (N - 1); i++) {
            CDF[i] = (CDF[i - 1] + in.data[i]) / sum;
        }
        CDF[N - 1] = 1.0f; // sanity check

        for(uint i = 0; i < N; i++) {
            if(e <= CDF[i]) {
                channel = i;
                pdf = in.data[i] / sum;
            }
        }
    } else {
        channel = int(e * float(N - 1));
        pdf = 1.0f / float(N);
    }
}

/**
 * @brief convertToLDR
 * @param in
 * @param exposure
 * @param gammaCor
 * @param maxVal
 * @return
 */
template<uint N>
Vec<N, float> convertToLDR(Vec<N, float> &in, float exposure = 1.0f, float gammaCor = 2.2f, float maxVal = 255.0f)
{
    Vec<N, float> ret = in.clone();
    ret *= exposure;

    vecGamma(ret, 1.0f / gammaCor);
    ret *= maxVal;
    ret.clamp(0.0f, maxVal);

    return ret;
}

} // end namespace pic

#endif /* PIC_COLORS_COLOR_HPP */

