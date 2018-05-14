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

#ifndef PIC_UTIL_K_MEANS_HPP
#define PIC_UTIL_K_MEANS_HPP

#include <vector>
#include <set>
#include <chrono>

#include "../base.hpp"
#include "../util/array.hpp"
#include "../util/math.hpp"

namespace pic{

/**
 * @brief kMeansAssignLabel
 * @param sample_j
 * @param nDim
 * @param centers
 * @param k
 * @return
 */
template<class T>
PIC_INLINE unsigned int kMeansAssignLabel( T* sample_j, int nDim,
                                T* centers, int k)
{
    T dist = Array<T>::distanceSq(sample_j, &centers[0], nDim);
    unsigned int label = 0;

    for(unsigned int i = 1; i < k; i++) {
        T *center_i = &centers[i * nDim];

        T tmp_dist = Array<T>::distanceSq(sample_j, center_i, nDim);

        if(tmp_dist < dist) {
            dist = tmp_dist;
            label = i;
        }
    }

    return label;
}

/**
 * @brief kMeansComputeMean
 * @param samples
 * @param out
 * @param nDim
 * @param cluster
 * @return
 */
template<class T>
PIC_INLINE T* kMeansComputeMean(T *samples, T *out, int nDim, std::set<unsigned int> *cluster)
{
    Array<T>::set(out, nDim, T(0));

    int count = 0;
     for (std::set<unsigned int>::iterator it = cluster->begin(); it != cluster->end(); it++) {
         int i = *it;
         Array<T>::add(&samples[i * nDim], out, nDim);
         count++;
     }

     if(count > 0) {
         Array<T>::div(out, nDim, T(count));
     }

     return out;
}

/**
 * @brief kMeanscomputeRandomCenters
 * @param samples
 * @param nSamples
 * @param nDim
 * @param centers
 * @return
 */
template<class T>
PIC_INLINE T* kMeanscomputeRandomCenters(T *samples, int nSamples, int nDim, int k, T* centers)
{
    if(centers != NULL) {
        delete[] centers;
    }

    centers = new T[k * nDim];

        std::mt19937 m(std::chrono::system_clock::now().time_since_epoch().count());

        T *tMax = new T[nDim];
        T *tMin = new T[nDim];

        for(int j = 0; j < nDim; j++) {
            tMax[j] = -FLT_MAX;
            tMin[j] =  FLT_MAX;
        }

        for(int i = 0; i < nSamples; i++) {
            int index = i * nDim;
            for(int j = 0; j < nDim; j++) {
                T s = samples[index + j];
                tMax[j] = MAX(tMax[j], s);
                tMin[j] = MIN(tMin[j], s);
            }
        }

        for(int i = 0; i < k; i++) {
            for(int j = 0; j < nDim; j++) {
                 centers[i * nDim + j] = T(Random(m()) * (tMax[j] - tMin[j]) + tMin[j]);
            }
        }

    return centers;
}


/**
 * @brief KMeans
 * @param data
 * @param nData
 * @param k
 * @param maxIter
 */
template<class T>
PIC_INLINE T* kMeans(T *samples, int nSamples, int nDim,
          unsigned int k, T *centers,
          std::vector< std::set<unsigned int> *> &labels,
          unsigned int maxIter = 100)
{    
    if(nSamples < k) {
        return NULL;
    }

    labels.clear();
    for(unsigned int i = 0; i < k; i++) {
        labels.push_back(new std::set<unsigned int>);
    }

    if(centers == NULL) {
        centers = kMeanscomputeRandomCenters<T>(samples, nSamples, nDim, k, NULL);
    }

    for(unsigned int i = 0; i < nSamples; i++) {
        T *sample_i = &samples[i * nDim];

        unsigned int label = kMeansAssignLabel(sample_i, nDim, centers, k);
        labels[label]->insert(i);
    }

    T *mean = new T[k * nDim];

    for(unsigned int i = 0; i < maxIter; i++) {
        bool bNoChanges = true;

        for(unsigned int j = 0; j < k; j++) {
            //compute new means
            int index = j * nDim;
            std::set<unsigned int> *tmp = labels.at(j);
            kMeansComputeMean(samples, &mean[index], nDim, tmp);

            //update centers
            float dist = Array<float>::distanceSq(&centers[index], &mean[index], nDim);

            Array<float>::assign(&mean[index], &centers[index], nDim);

            if(dist > 1e-6f) {
                bNoChanges = false;
            }
        }

        if(bNoChanges) {
            #ifdef PIC_DEBUG
                printf("Max iterations: %d\n", i);
            #endif
            return centers;
        } else {
            //clear labels
            for(unsigned int j = 0; j < k; j++) {
                labels[j]->clear();
            }

            //re-assign labels
            for(unsigned int j = 0; j < nSamples; j++) {
                T *sample_j = &samples[j * nDim];
                unsigned int label = kMeansAssignLabel(sample_j, nDim, centers, k);
                labels[label]->insert(j);
            }
        }
    }

    return centers;
}

/**
 * @brief kMeansSelect
 * @param samples
 * @param nSamples
 * @param nDim
 * @param k
 * @param labels
 * @param threshold
 * @param maxIter
 * @return
 */
template<class T>
PIC_INLINE  T* kMeansSelect(T *samples, int nSamples, int nDim,
                unsigned int &k,
                std::vector< std::set<unsigned int> *> &labels,
                float threshold = 1e-2f,
                unsigned int maxIter = 100)
{

    T *centers = NULL;

    k = 1;
    T prevErr;
    bool bFlag = true;
    while(bFlag) {
        k++;
        printf("k: %d\n", k);
        labels.clear();
        if(centers != NULL) {
            delete[] centers;
        }

        centers = kMeans<T>(samples, nSamples, nDim, k, NULL, labels, maxIter);

        T err = T(0);
        for(int i = 0; i < labels.size(); i++) {
            T *center_i = &centers[i * nDim];

            std::set<unsigned int> * cluster = labels.at(i);
            for (std::set<unsigned int>::iterator it = cluster->begin(); it != cluster->end(); it++) {
                int i = *it;
                err += Array<T>::distanceSq(&samples[i * nDim], center_i, nDim);
            }

        }

        if(k > 2) {
            float relErr = fabsf(float(err - prevErr)) / float(prevErr);
            printf("%f %f %f\n", err, prevErr, relErr);
            if(relErr < threshold) {
                bFlag = false;
            }
        }

        prevErr = err;
    }

    return centers;
}

}

#endif // PIC_UTIL_K_MEANS_HPP
