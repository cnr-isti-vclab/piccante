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

#ifndef PIC_POINT_SAMPLERS_SAMPLER_DART_THROWING_HPP
#define PIC_POINT_SAMPLERS_SAMPLER_DART_THROWING_HPP

#include <random>

#include "../util/vec.hpp"
#include "../util/math.hpp"

namespace pic {

const int CONST_DARTTHROWING = 5000;

/**
 * @brief getDartThrowingSamples
 * @param m
 * @param radius2
 * @param nSamples
 * @param samples
 */
template<unsigned int N>
void getDartThrowingSamples(std::mt19937 *m, float radius2, int nSamples,
                         std::vector<float> &samples)
{
    float dist2, delta;
    Vec<N, float> val;

    int counter = 0;

    while(counter < (nSamples * CONST_DARTTHROWING)) {
        for(unsigned int j = 0; j < N; j++) {
            val[j] = ( getRandom((*m)()) * 2.0f - 1.0f);
        }

        bool bFlag = true;

        for(unsigned int i = 0; i < samples.size(); i += N) {
            dist2 = 0.0f;

            for(unsigned int j = 0; j < N; j++) {
                delta = val[j] - samples[i + j];
                dist2 += delta * delta;
            }

            bFlag = dist2 >= radius2;

            if(!bFlag) {
                break;
            }
        }

        if(bFlag) {
            if(val.lengthSq() <= 1.0f) {
                for(unsigned int j = 0; j < N; j++) {
                    samples.push_back(val[j]);
                }
            }
        }

        counter++;
    }
}

} // end namespace pic

#endif /* PIC_POINT_SAMPLERS_SAMPLER_DART_THROWING_HPP */

