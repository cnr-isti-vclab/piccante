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
#include "../util/std_util.hpp"

namespace pic{

template<class T>
class KMeans
{
protected:

    T* getMean(T *samples, T *out, int nDim, std::set<uint> *cluster)
    {
        Array<T>::assign(T(0), out, nDim);

        int count = 0;
         for (auto it = cluster->begin(); it != cluster->end(); it++) {
             int i = *it;
             Array<T>::add(&samples[i * nDim], nDim, out);
             count++;
         }

         if(count > 0) {
             Array<T>::div(out, nDim, T(count));
         }

         return out;
    }

    uint assignLabel(T* sample_j, int nDim, T* centers)
    {
        T dist = Array<T>::distanceSq(sample_j, &centers[0], nDim);
        uint label = 0;

        for(uint i = 1; i < k; i++) {
            T *center_i = &centers[i * nDim];

            T tmp_dist = Array<T>::distanceSq(sample_j, center_i, nDim);

            if(tmp_dist < dist) {
                dist = tmp_dist;
                label = i;
            }
        }

        return label;
    }

    virtual T* initCenters(T *samples, int nSamples, int nDim, T* centers)
    {
        if(centers == NULL) {
            centers = new T[k * nDim];
        }

        std::mt19937 m(42);

        T *tMin = new T[nDim];
        T *tMax = new T[nDim];

        for(int j = 0; j < nDim; j++) {
            T s = samples[j];
            tMin[j] = s;
            tMax[j] = s;
        }

        for(int i = 1; i < nSamples; i++) {
            int index = i * nDim;
            for(int j = 0; j < nDim; j++) {
                T s = samples[index + j];

                tMin[j] = MIN(tMin[j], s);
                tMax[j] = MAX(tMax[j], s);
            }
        }

        for(uint i = 0; i < k; i++) {
            int index = i * nDim;
            for(int j = 0; j < nDim; j++) {
                centers[index + j] = T(getRandom(m()) * (tMax[j] - tMin[j]) + tMin[j]);
            }
        }

        delete[] tMin;
        delete[] tMax;

        return centers;
    }

    uint k, maxIter;

public:

    KMeans(uint k, uint maxIter)
    {
        setup(k, maxIter);
    }

    void setup(uint k, uint maxIter = 100)
    {
        this->k = k;
        this->maxIter = maxIter;
    }
    T* Process(T *samples, int nSamples, int nDim,
               T* centers,
               std::vector< std::set<uint> *> &labels)
    {
        if(nSamples < k) {
            return NULL;
        }

        labels.clear();
        for(uint i = 0; i < k; i++) {
            labels.push_back(new std::set<uint>);
        }

        centers = initCenters(samples, nSamples, nDim, centers);

        for(uint i = 0; i < nSamples; i++) {
            T *sample_i = &samples[i * nDim];

            uint label = assignLabel(sample_i, nDim, centers);
            labels[label]->insert(i);
        }

        T *mean = new T[k * nDim];

        for(uint i = 0; i < maxIter; i++) {
            bool bNoChanges = true;

            for(uint j = 0; j < k; j++) {
                //compute new means
                int index = j * nDim;
                std::set<uint> *tmp = labels.at(j);
                getMean(samples, &mean[index], nDim, tmp);

                //update centers
                float dist = Arrayf::distanceSq(&centers[index], &mean[index], nDim);

                Arrayf::assign(&mean[index], nDim, &centers[index]);

                if(dist > 1e-6f) {
                    bNoChanges = false;
                }
            }

            if(bNoChanges) {
                #ifdef PIC_DEBUG
                    printf("Max iterations: %d\n", i);
                #endif

                delete[] mean;
                return centers;
            } else {
                //clear labels
                for(uint j = 0; j < k; j++) {
                    labels[j]->clear();
                }

                //re-assign labels
                for(uint j = 0; j < nSamples; j++) {
                    T *sample_j = &samples[j * nDim];
                    uint label = assignLabel(sample_j, nDim, centers);
                    labels[label]->insert(j);
                }
            }
        }

        delete[] mean;

        return centers;
    }

    static T* execute(T *samples, int nSamples, int nDim,
                      T* centers, int k,
                      std::vector< std::set<uint> *> &labels,
                      uint maxIter = 100)
    {

        KMeans<T> km(k, maxIter);

        return km.Process(samples, nSamples, nDim, centers, labels);
    }

    static T* select(T *samples, int nSamples, int nDim,
              std::vector< std::set<uint> *> &labels,
              uint &k,
              float threshold = 1e-2f,
              uint maxIter = 100)
    {
        T *centers = NULL;
        k = 1;
        T prevErr;
        bool bFlag = true;
        while(bFlag) {
            k++;

            #ifdef PIC_DEBUG
                printf("k: %d\n", k);
            #endif

            labels.clear();
            centers = delete_vec_s(centers);

            centers = KMeans::execute(samples, nSamples, nDim, NULL, k, labels, maxIter);

            T err = T(0);
            for(uint i = 0; i < labels.size(); i++) {
                T *center_i = &centers[i * nDim];

                std::set<uint> * cluster = labels.at(i);
                for (std::set<uint>::iterator it = cluster->begin(); it != cluster->end(); it++) {
                    int i = *it;
                    err += Array<T>::distanceSq(&samples[i * nDim], center_i, nDim);
                }

            }

            if(k > 2) {
                float relErr = fabsf(float(err - prevErr)) / float(prevErr);

                 #ifdef PIC_DEBUG
                    printf("%f %f %f\n", err, prevErr, relErr);
                #endif

                if(relErr < threshold) {
                    bFlag = false;
                }
            }

            prevErr = err;
        }

        return centers;
    }

};

} //end namespace pic

#endif // PIC_UTIL_K_MEANS_HPP
