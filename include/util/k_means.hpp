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

#include "util/vec.hpp"

namespace pic{

/**
 * @brief kMeansAssignLabel
 * @param j
 * @param data
 * @param nData
 * @param centers
 * @return
 */
template<unsigned int N, class T>
unsigned int kMeansAssignLabel( unsigned int j, std::vector< Vec<N, T> > &data,
                                std::vector<unsigned int> &centers)
{
    Vec<N, T> data_j = data[j];

    unsigned int label = 0;
    T dist = data_j.distanceSq(data[centers[0]]);

    for(unsigned int i = 1; i < centers.size(); i++) {
        Vec<N, T> data_i = data[centers[i]];

        T tmp_dist = data_j.distanceSq(data_i);

        if(tmp_dist < dist) {
            dist = tmp_dist;
            label = i;
        }
    }

    return label;
}

/**
 * @brief kMeansUpdateClusterCenter
 * @param mean
 * @param data
 * @param cluster
 * @return
 */
template<unsigned int N, class T>
unsigned int kMeansUpdateClusterCenter( Vec<N, T> mean, std::vector< Vec<N, T> > &data,
                                  std::set<unsigned int> *cluster)
{
    T dist = mean.distanceSq(data[*cluster->begin()]);
    unsigned int index = 0;

    for (std::set<unsigned int>::iterator it = cluster->begin(); it != cluster->end(); it++) {
        T tmp_dist = mean.distanceSq(data[*it]);

        if(tmp_dist < dist) {
            dist = tmp_dist;
            index = *it;
        }
    }

    return index;
}

/**
  * @brief kMeansComputeMean
  * @param data
  * @param cluster
  * @return
  */
template<unsigned int N, class T>
Vec<N, T>  kMeansComputeMean(std::vector< Vec<N, T> > &data, std::set<unsigned int> *cluster)
{
     Vec<N, T> mean;
     mean.setZero();

     unsigned int count = 0;
     for (std::set<unsigned int>::iterator it = cluster->begin(); it != cluster->end(); it++) {
         mean.add(data[*it]);
         count++;
     }

     if(count > 0) {
         mean.div(T(count));
     }

     return mean;
}

/**
 * @brief KMeans
 * @param data
 * @param nData
 * @param k
 * @param maxIter
 */
template<unsigned int N, class T>
bool kMeans(std::vector< Vec<N, T> > &data, unsigned int k, unsigned int maxIter,
            std::vector<unsigned int> &centers, std::vector< std::set<unsigned int> *> &labels)
{
    centers.clear();
    labels.clear();

    unsigned int n = data.size();

    if(n < k) {
        return false;
    }

    std::set<unsigned int> centers_set;
    while(centers_set.size() < k) {
        unsigned int value = rand() % n;
        if(centers_set.find(value) == centers_set.end()) {
            centers_set.insert(value);
            centers.push_back(value);
            labels.push_back(new std::set<unsigned int>);
        }
    }

    for(unsigned int i = 0; i < n; i++) {
        unsigned int label = kMeansAssignLabel(i, data, centers);
        labels[label]->insert(i);
    }

    for(unsigned int i = 0; i < maxIter; i++) {
        bool bNoChanges = true;

        for(unsigned int j = 0; j < k; j++) {
            //computing new means
            Vec<N, T> mean = kMeansComputeMean(data, labels[j]);

            //updating centers
            unsigned int index = kMeansUpdateClusterCenter(mean, data, labels[j]);

            if(index != centers[j]) {
                centers[j] = index;
                bNoChanges = false;
            }
        }

        if(bNoChanges) {
            return true;
        } else {
            //clearing labels
            for(unsigned int j = 0; j < k; j++) {
                labels[j]->clear();
            }

            //re-assigning labels
            for(unsigned int j = 0; j < n; j++) {
                unsigned int label = kMeansAssignLabel(j, data, centers);
                labels[label]->insert(j);
            }
        }
    }

    return true;
}

}

#endif // PIC_UTIL_K_MEANS_HPP
