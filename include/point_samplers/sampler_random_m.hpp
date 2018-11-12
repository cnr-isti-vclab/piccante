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

#ifndef PIC_POINT_SAMPLERS_SAMPLER_RANDOM_M_HPP
#define PIC_POINT_SAMPLERS_SAMPLER_RANDOM_M_HPP

#include <random>

#include "../base.hpp"
#include "../util/math.hpp"

#include "../point_samplers/sampler_random.hpp"

namespace pic {

/**
 * @brief The MRSamplers class
 */
template<unsigned int N>
class MRSamplers
{
protected:
    RandomSampler<N> **samplers;
    int nSamplers;
    int oldSamples;
    Vec<N, int> oldWindow;
    SAMPLER_TYPE type;

public:
    int nLevels;

    /**
     * @brief MRSamplers
     */
    MRSamplers();

    /**
     * @brief MRSamplers
     * @param type
     * @param window
     * @param nSamples
     * @param nLevels
     * @param nSamplers
     */
    MRSamplers(SAMPLER_TYPE type, Vec<N, int> window, int nSamples, int nLevels,
               int nSamplers);

    /**
     * @brief update
     * @param window
     * @param nSamples
     * @return
     */
    bool update(Vec<N, int> window, int nSamples);

    /**
     * @brief getSampler gets a sampler at a given level
     * @param m
     * @return
     */
    RandomSampler<N> *getSampler(std::mt19937 *m);

    /**
     * @brief Write saves into an existing file
     * @param name
     * @return
     */
    bool Write(std::string name);

    /**
     * @brief Read
     * @param name
     * @return
     */
    bool Read(std::string name);
};

template <unsigned int N>
PIC_INLINE MRSamplers<N>::MRSamplers()
{
    samplers = NULL;
    nSamplers = -1;
    oldWindow = 0;
    oldSamples = -1;
    nLevels = -1;
}

template <unsigned int N>
PIC_INLINE MRSamplers<N>::MRSamplers(
    SAMPLER_TYPE type, Vec<N, int> window, int nSamples, int nLevels, int nSamplers)
{
    this->type = type;
    this->nSamplers = nSamplers;
    this->nLevels = nLevels;
    oldSamples = nSamples;
    oldWindow = window;

    samplers = new RandomSampler< N > *[nSamplers];

    #pragma omp parallel for

    for(int i = 0; i < nSamplers; i++) {
        samplers[i] = new RandomSampler< N >(type, window, nSamples, nLevels, 0);
    }
}

template <unsigned int N>
PIC_INLINE bool MRSamplers<N>::update(Vec<N, int> window, int nSamples)
{
    if(window.equal(oldWindow) && (oldSamples == nSamples)) {
        return false;
    }

    //#pragma omp parallel for
    for(int i = 0; i < nSamplers; i++) {
        samplers[i]->update(type, window, nSamples, nLevels);
    }

    oldWindow = window;
    oldSamples = nSamples;
    return true;
}

template <unsigned int N>
PIC_INLINE RandomSampler<N> *MRSamplers<N>::getSampler(std::mt19937 *m)
{
    return samplers[(*m)() % nSamplers];
}

template <unsigned int N>
PIC_INLINE bool MRSamplers<N>::Read(std::string name)
{
    std::ifstream file;

    file.open(name.c_str(), std::ios::in);

    if(!file.is_open()) {
        return false;
    }

    std::string tmp;
    file >> tmp;
    file >> nSamplers;

    file >> tmp;
    file >> oldSamples;

    file >> tmp;
    file >> nLevels;

    file >> tmp;
    for(unsigned int i = 0; i < N; i++) {
        file >> oldWindow[i];
    }

    if(nSamplers < 1) {
        file.close();

        return false;
    }

    if(samplers != NULL) {
        delete[] samplers;
    }

    samplers = new RandomSampler<N> *[nSamplers];

    for(int i = 0; i < nSamplers; i++) {
        samplers[i] = new RandomSampler<N>();
        unsigned int samplesR = 0;

        file >> tmp;
        file >> samplesR;

        int value;
        for(unsigned int j=0; j<samplesR; j++) {
            file >> value;
            samplers[i]->samplesR.push_back(value);
        }
    }
    /*
    
    int nSamplesR;
    char tmp[512];
    fscanf(file, "%s", tmp);
    fscanf(file, "%d", &nSamplesR);

    for(int i = 0; i < nSamplesR; i++) {
        int tmpValue;
        fscanf(file, "%d", &tmpValue);
        samplesR.push_back(tmpValue);
    }

    levelsR.push_back(nSamplesR);**/


    file.close();

     return true;
}

template <unsigned int N>
PIC_INLINE bool MRSamplers<N>::Write(std::string name)
{
    std::ofstream file;

    file.open(name.c_str(), std::ios::out);

    if(!file.is_open()) {
        return false;
    }

    //general parameters
    file << "nSamplers: " << nSamplers << " oldSamples: " << oldSamples << " nLevels: " << nLevels << std::endl;

    file << "oldWindow: ";
    for(unsigned int i = 0; i < N; i++) {
        file << oldWindow[i] << " ";
    }
    file << std::endl;

    //write samplers
    for(int i = 0; i < nSamplers; i++) {
        RandomSampler< N > *rs = samplers[i];
        //write samples
        file << "nSamplesR: " << rs->samplesR.size() << std::endl;
        for(unsigned int j=0; j< rs->samplesR.size(); j++) {
            file << rs->samplesR[j] << " ";
        }
        file << std::endl;
    }

    file.close();

    return true;
}

} // end namespace pic

#endif /* PIC_POINT_SAMPLERS_SAMPLER_RANDOM_M_HPP */

