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

#ifndef PIC_POINT_SAMPLERS_SAMPLER_MONTE_CARLO_HPP
#define PIC_POINT_SAMPLERS_SAMPLER_MONTE_CARLO_HPP

#include <random>
#include "util/math.hpp"

namespace pic {

/**
 *
 */
template <unsigned int N>
void MonteCarloSampler(std::mt19937 *m, int nSamples, std::vector<float> &samples)
{
    for(int i = 0; i < nSamples; i++) {
        for(unsigned int j = 0; j < N; j++) {
            float val = Random((*m)()) * 2.0f - 1.0f;
            samples.push_back(val);
        }
    }
}

/**
 *
 */
template <unsigned int N>
void MonteCarloStratifiedSampler(std::mt19937 *m, int nSamples,
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
            float val = ((Random((*m)()) + k) / n_f) * 2.0f - 1.0f;
            samples.push_back(val);
            div *= n;
        }
    }
}

/**
 *
 */
template <unsigned int N>
void PatternMethodSampler(int nSamples, std::vector<float> &samples)
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

