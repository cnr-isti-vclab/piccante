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

#ifndef PIC_UTIL_STD_UTIL_HPP
#define PIC_UTIL_STD_UTIL_HPP

#include <vector>

namespace pic {

/**
 * @brief filterInliers
 * @param vec
 * @param inliers
 * @param vecOut
 */
template<class T>
inline void filterInliers(std::vector< T > &vec, std::vector< unsigned int > &inliers, std::vector< T > &vecOut)
{
    vecOut.clear();

    if(!inliers.empty()) {
        for(unsigned int i = 0; i < inliers.size(); i++) {
            vecOut.push_back(vec[inliers[i]]);
        }
    } else {
        vecOut.assign(vec.begin(), vec.end());
    }
}

/**
 * @brief stdVectorClear
 * @param vec
 */
template<class T>
inline void stdVectorClear(std::vector<T *> &vec)
{
    for(unsigned int i = 0; i < vec.size(); i++) {
        T *tmp = vec[i];

        if(tmp != NULL) {
            delete tmp;
        }

        vec[i] = NULL;
    }

    vec.clear();
}


/**
 * @brief stdVectorArrayClear
 * @param vec
 */
template<class T>
inline void stdVectorArrayClear(std::vector<T *> &vec)
{
    for(unsigned int i = 0; i < vec.size(); i++) {
        T *tmp = vec[i];

        if(tmp != NULL) {
            delete[] tmp;
        }
    }

    vec.clear();
}

/**
 * @brief setToANullVector
 * @param vec
 * @param n
 */
template<class T>
inline void setToANullVector(std::vector< T* > &vec, unsigned int n)
{
    if(!vec.empty()) {
        return;
    }

    for(unsigned int i = 0; i < n; i++) {
        vec.push_back(NULL);
    }
}

/**
 * @brief release
 * @param data
 * @return
 */
template<class T>
inline T* releasePtr(T *data)
{
    if(data != NULL) {
        delete data;
        data = NULL;
    }

    return data;
}

/**
 * @brief delete_s
 * @param data
 * @return
 */
template<class T>
inline T* delete_s(T *data)
{
    if(data != NULL) {
        delete data;
        data = NULL;
    }
    return data;
}

/**
 * @brief delete_vec_s
 * @param data
 * @return
 */
template<class T>
inline T* delete_vec_s(T *data)
{
    if(data != NULL) {
        delete[] data;
        data = NULL;
    }
    return data;
}


} // end namespace pic

#endif // PIC_UTIL_STD_UTIL_HPP
