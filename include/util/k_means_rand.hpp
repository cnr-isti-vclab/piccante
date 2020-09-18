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

#ifndef PIC_UTIL_K_MEANS_RAND_HPP
#define PIC_UTIL_K_MEANS_RAND_HPP

#include <vector>
#include <set>
#include <chrono>

#include "../base.hpp"
#include "../util/array.hpp"
#include "../util/math.hpp"
#include "../util/std_util.hpp"
#include "../util/k_means.hpp"

namespace pic{

template<class T>
class KMeansRand: public KMeans<T>
{
protected:

    T* initCenters(T *samples, int nSamples, int nDim, T* centers)
    {
        std::mt19937 m(42);
        if(centers == NULL) {
            centers = new T[this->k * nDim];
        }

        std::set< uint > chosen;
        for(uint i = 0; i < this->k; i++) {

           bool bCheck = true;
           while(bCheck) {
               uint index = m() % nSamples;
               if(chosen.find(index) == chosen.end()) {

                   chosen.insert(index);
                   Array<T>::assign(&samples[index * nDim], nDim, &centers[i * nDim]);
                   bCheck = false;
               }
           }
        }
        return centers;
    }

public:

    KMeansRand(uint k, uint maxIter) : KMeans<T>(k, maxIter)
    {
    }

    static T* execute(T *samples, int nSamples, int nDim,
                      T* centers, int k,
                      std::vector< std::set<uint> *> &labels,
                      uint maxIter = 100)
    {

        KMeansRand<T> km(k, maxIter);

        return km.Process(samples, nSamples, nDim, centers, labels);
    }
};

} //end namespace pic

#endif // PIC_UTIL_K_MEANS_RAND_HPP
