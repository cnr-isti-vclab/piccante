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

#ifndef PIC_POINT_SAMPLERS_SAMPLER_BRIDSON_HPP
#define PIC_POINT_SAMPLERS_SAMPLER_BRIDSON_HPP

#include <math.h>
#include <random>
#include "../util/math.hpp"
#include "../util/vec.hpp"

namespace pic {

/**
 * @brief checkNeighborsBruteForce
 * @param samples
 * @param x
 * @param radius
 * @return
 */
template<unsigned int N>
bool checkNeighborsBruteForce(std::vector< Vec<N, float> > &samples,
                              Vec<N, float> x, float radius)
{
    float radius_sq = radius * radius;

    for(unsigned int i = 0; i < samples.size(); i++) {
        if(x.distanceSq(samples[i]) < radius_sq) {
            return false;
        }
    }

    return true;
}

/**
 * @brief getBridsonSamples
 * @param m
 * @param radius
 * @param samples
 * @param kSamples
 */
template<unsigned int N>
void getBridsonSamples(std::mt19937 *m, float radius, std::vector<float> &samples,
                    int kSamples = 30)
{
    if(kSamples < 1) {
        kSamples = 30;
    }

    //Step 0: Creating an N-grid
//	Grid<N> grid(0.999f * radius / sqrtf(float(N)));

    //Step 1: Initial sample
    Vec<N, float> x0 = randomPoint<N>(m);

    std::vector< Vec<N, float> > vecSamples;
    std::vector<int> activeList;

    vecSamples.push_back(x0);
    activeList.push_back(0);
//	grid.setValue(0, x0);

    //Step 2: active list
    while(!activeList.empty()) {
        int i = (*m)() % activeList.size();

        int ind = activeList[i];

        bool bCheckSuccess = false;
        bool bFlag = true;

        int j = 0;

        while(bFlag) {
            //create samples inside the annulus around sample_i
            Vec<N, float> x = annulusSampling<N>(m, vecSamples[ind], radius);

            //check if the generated sample is in the bounding box
            if(insideVecBBox(x)) {
                //check if the sample does not have neighbors in grid with distance radius
                if(checkNeighborsBruteForce(vecSamples, x, radius)) {
                    vecSamples.push_back(x);
                    int value = int(vecSamples.size()) - 1;

                    activeList.push_back(value);
                    //                grid.setValue(value, x);
                    bCheckSuccess = true;
                }
            }

            j++;

            bFlag = (j < kSamples) && (!bCheckSuccess);
        }

        if(!bCheckSuccess) { //removing i-th sample from the active list
            if(activeList.size() > 1) {
                activeList[i] = activeList.back();
                activeList.pop_back();
            } else {
                activeList.pop_back();
            }
        }
    }

    for(unsigned int i = 0; i < vecSamples.size(); i++) {
        Vec<N, float> x = vecSamples[i];

        for(unsigned int k = 0; k < N; k++) {
            samples.push_back(x[k]);
        }
    }
}

} // end namespace pic

#endif /* PIC_POINT_SAMPLERS_SAMPLER_BRIDSON_HPP */

