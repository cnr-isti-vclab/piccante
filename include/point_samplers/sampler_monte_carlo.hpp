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

#ifndef PIC_POINT_SAMPLERS_SAMPLER_MONTE_CARLO_HPP
#define PIC_POINT_SAMPLERS_SAMPLER_MONTE_CARLO_HPP

#include <random>

#include "../base.hpp"

#include "../util/math.hpp"

namespace pic {

/**
 * @brief getMonteCarloSamples
 * @param m
 * @param nSamples
 * @param samples
 */
template <unsigned int N> PIC_INLINE
void getMonteCarloSamples(std::mt19937 *m, int nSamples, std::vector<float> &samples)
{
    for(int i = 0; i < nSamples; i++) {
        for(unsigned int j = 0; j < N; j++) {
            float val = getRandom((*m)()) * 2.0f - 1.0f;
            samples.push_back(val);
        }
    }
}

/**
 * @brief getMonteCarloStratifiedSamples
 * @param m
 * @param nSamples
 * @param samples
 */
template <unsigned int N> PIC_INLINE
void getMonteCarloStratifiedSamples(std::mt19937 *m, int nSamples,
                                 std::vector<float> &samples)
{
    int n = int(powf(float(nSamples), 1 / float(N))) + 1; //int(sqrtf(nSamples))+1;
    float n_f = float(n);
    nSamples = n;

    for(unsigned int i = 1; i < N; i++) {
        nSamples *= n;
    }

    for(int i = 0; i < nSamples; i++) {
        int div = 1;

        for(unsigned int j = 0; j < N; j++) {
            int k = (i / div) % n;
            float val = ((getRandom((*m)()) + k) / n_f) * 2.0f - 1.0f;
            samples.push_back(val);
            div *= n;
        }
    }
}

/**
 * @brief getPatternMethodSampler
 * @param nSamples
 * @param samples
 */
template <unsigned int N> PIC_INLINE
void getPatternMethodSamples(int nSamples, std::vector<float> &samples)
{
    int n = int(powf(float(nSamples), 1 / float(N))) + 1; //int(sqrtf(nSamples))+1;
    nSamples = n;

    for(unsigned int i = 1; i < N; i++) {
        nSamples *= n;
    }

    for(int i = 0; i < nSamples; i++) {
        int div = 1;

        for(unsigned int j = 0; j < N; j++) {
            int k = ((i / div)) % n;
            float val = (float(k) / float(n)) * 2.0f - 1.0f;
            samples.push_back(val);
            div *= n;
        }
    }
}

} // end namespace pic

#endif /* PIC_POINT_SAMPLERS_SAMPLER_MONTE_CARLO_HPP */

