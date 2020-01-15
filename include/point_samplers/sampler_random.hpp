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

#ifndef PIC_POINT_SAMPLERS_SAMPLER_RANDOM_HPP
#define PIC_POINT_SAMPLERS_SAMPLER_RANDOM_HPP

#include <vector>
#include <set>

#include <iostream>
#include <fstream>
#include <random>

#include "../base.hpp"
#include "../image.hpp"
#include "../util/math.hpp"
#include "../util/point_samplers.hpp"

#include "../point_samplers/sampler_monte_carlo.hpp"
#include "../point_samplers/sampler_dart_throwing.hpp"
#include "../point_samplers/sampler_bridson.hpp"

namespace pic {

/**
 * @brief The RandomSampler class
 */
template <unsigned int N>
class RandomSampler
{
protected:
    SAMPLER_TYPE type;
    std::mt19937 *m;
    std::set<int> track;

public:
    //Samples
    std::vector<float>	samples;
    std::vector<int>	samplesR;

    //Boundaries for each level
    std::vector<int>	levels;
    std::vector<int>	levelsR;

    Vec<N, int> window;
    int nSamples;

    /**
     * @brief RandomSampler
     */
    RandomSampler()
    {

    }

    /**
     * @brief RandomSampler
     * @param type
     * @param window
     * @param nSamples
     * @param nLevels
     */
    RandomSampler(SAMPLER_TYPE type, Vec<N, int> window, int nSamples, int nLevels, unsigned int seed);

    /**
     * @brief update
     * @param type
     * @param window
     * @param nSamples
     * @param nLevels
     */
    void update(SAMPLER_TYPE type, Vec<N, int> window, int nSamples, int nLevels);

    /**
     * @brief render2Int
     */
    void render2Int();

    /**
     * @brief wrap
     * @param alpha
     */
    void wrap(float alpha);

    /**
     * @brief cutRescale
     * @param cutDim
     */
    void cutRescale(unsigned int cutDim);

    /**
     * @brief getSamplesPerLevel
     * @param level
     * @return
     */
    int getSamplesPerLevel(int level);

    /**
     * @brief getSampleAt
     * @param level
     * @param i
     * @param x
     * @param y
     */
    void getSampleAt(int level, int i, int &x, int &y);

    /**
     * @brief Write
     * @param name
     * @param level
     */
    void Write(std::string name, int level);

    /**
     * @brief generateFigureRS
     * @param nameOut
     * @param type
     * @param window
     * @param nSamples
     * @param nLevels
     */
    static void generateFigureRS(std::string nameOut, SAMPLER_TYPE type, int window,
                                 int nSamples, int nLevels)
    {
        Vec<2, int> w = Vec<2, int>(window, window);
        RandomSampler<2> *p2Ds = new RandomSampler<2>(type, w, nSamples, nLevels, 0);

        for(int i = 0; i < p2Ds->levelsR.size(); i++) {
            std::string str = nameOut;
            std::stringstream sstr;
            sstr << i;
            str = str + sstr.str() + ".pfm";
            p2Ds->Write(str, i);
        }
    }

    /**
     * @brief Generate
     * @param type
     * @param window
     */
    static void Generate(SAMPLER_TYPE type, int window)
    {
        int c = 1;

        for(int i = 1; i <= 5; i++) {
            RandomSampler<N> *p2Ds = new RandomSampler<N>(type, window * c, window * c,
                    1); //2*c,1);
            printf("Samples expected: %d \t Real Samples: %d\n", (window * 2)*c,
                   p2Ds->samplesR.size() / N);
            std::string str = "test_poisson_sampler_";
            std::stringstream sstr;
            sstr << i;
            str = str + sstr.str() + ".pfm";
            p2Ds->Write(str, 0);
            c *= 2;
        }
    }
};

template <unsigned int N> PIC_INLINE RandomSampler<N>::RandomSampler(
    SAMPLER_TYPE type, Vec<N, int> window, int nSamples, int nLevels, unsigned int seed)
{
    m = new std::mt19937(seed);
    update(type, window, nSamples, nLevels);
}

template <unsigned int N> PIC_INLINE void RandomSampler<N>::cutRescale(
    unsigned int cutDim)
{
    if(cutDim >= N) {
#ifdef PIC_DEBUG
        printf("cutRescale: not cuts.\n");
#endif
        return;
    }

    float cutValue = float(window[cutDim]) / float(window[0]);

#ifdef PIC_DEBUG
    printf("CutSize: %f\n", cutValue);
#endif

    std::vector<float> tmpCutSamples;
    std::vector<int> tmpCutLevels;

    int prevCutSamples = 0;

    for(unsigned int i = 0; i < levels.size(); i++) {
        int start, end;

        if(i == 0) {
            start = 0;
        } else {
            start = levels[i - 1];
        }

        end = levels[i];

        for(int j = start; j < end; j += N) {
            //cut
            if(fabsf(samples[j + cutDim]) <= cutValue) {
                //rescale
                samples[j + cutDim] /= cutValue;

                for(unsigned int k = 0; k < N; k++) {
                    tmpCutSamples.push_back(samples[j + k]);
                }
            }
        }

        int tmpCutSamples_size = int(tmpCutSamples.size());
        if(prevCutSamples != tmpCutSamples_size) {
            tmpCutLevels.push_back(int(tmpCutSamples.size()));
            prevCutSamples = int(tmpCutSamples.size());
        }
    }

    samples.clear();
    levels.clear();

    samples.insert(samples.begin(), tmpCutSamples.begin(), tmpCutSamples.end());
    levels.insert(levels.begin(), tmpCutLevels.begin(), tmpCutLevels.end());
}

template <unsigned int N> PIC_INLINE void RandomSampler<N>::wrap(float alpha)
{
    float x, y, ang, r, r2;

    for(int i = 0; i < samples.size(); i += 2) {
        x = samples[i];
        y = samples[i + 1];
        ang = atan2f(y, x);
        r = sqrtf(x * x + y * y);
        r2 = powf(r, alpha);
        samples[i]   = (r2 * cosf(ang));
        samples[i + 1] = (r2 * sinf(ang));
    }
}

template <unsigned int N> PIC_INLINE void RandomSampler<N>::update(
    SAMPLER_TYPE type, Vec<N, int> window, int nSamples, int nLevels)
{
    //Resetting vectors
    samples.clear();
    samplesR.clear();
    levels.clear();
    levelsR.clear();

    nSamples = MAX(nSamples, 1);

    this->type = type;
    this->window = window;
    this->nSamples = nSamples;

    float radius = PoissonRadius(nSamples);

    for(int i = 0; i < nLevels; i++) {
        float factor = powf(2.0f, float(i));
        float tmpRadius = radius * factor;

        switch(type) {
        case ST_BRIDSON:
            getBridsonSamples< N >(m, tmpRadius, samples);
            break;

        case ST_DARTTHROWING:
            getDartThrowingSamples< N >(m, tmpRadius * tmpRadius, nSamples, samples);
            break;

        case ST_MONTECARLO:
            getMonteCarloSamples< N >(m, nSamples, samples);
            break;

        case ST_MONTECARLO_S:
            getMonteCarloStratifiedSamples< N >(m, nSamples, samples);
            break;

        case ST_PATTERN:
            getPatternMethodSamples< N >(nSamples, samples);
            break;
        }

        levels.push_back(int(samples.size()));
    }

    //generate integer addresses
    cutRescale(2);
    render2Int();
}

template <unsigned int N>  PIC_INLINE void RandomSampler<N>::render2Int()
{
    if(samplesR.size() > 0 || samples.size() > 0) {
        samplesR.clear();
        levelsR.clear();
        track.clear();
    }

    Vec<N, float> window_f;

    for(unsigned int i = 0; i < N; i++) {
        window_f[i] = float(window[i]);
    }

    int x, coord;

    //int prevSamplesR = 0;
    for(uint i = 0; i < levels.size(); i++) {
        int start, end;

        if(i == 0) {
            start = 0;
        } else {
            start = levels[i - 1];
        }

        end = levels[i];

        for(int j = start; j < end; j += N) {
            coord = 0;//rounding

            for(unsigned int k = 0; k < N; k++) {
                x = int(lround(samples[j + k] * window_f[k]));
                coord += x * powint(window[k], k);
            }

            //Is the value in the track list?
            if(track.find(coord) == track.end()) {
                track.insert(coord);

                for(unsigned int k = 0; k < N; k++) { //final rounding
                    x = int(lround(samples[j + k] * window_f[k]));
                    samplesR.push_back(x);
                }
            }
        }

        levelsR.push_back(int(samplesR.size()));
    }

#ifdef PIC_DEBUG
    printf("render2Int: Original: %d \t Rendered: %d\n", int(samples.size() / N),
           int(track.size()));
#endif
}

template <unsigned int N>PIC_INLINE void RandomSampler<N>::Write(
    std::string name, int level)
{
    Image img(1, window[0] * 2 + 1, window[1] * 2 + 1, 1);
    img.setZero();

    int start, end;

    if(level == 0) {
        start = 0;
    } else {
        start = levelsR[level - 1];
    }

    end = levelsR[level];

    int x, y;

    for(int i = start; i < end; i += N) {
        x = samplesR[i    ] + window[0];
        y = samplesR[i + 1] + window[1];
        img.data[y * img.width + x] += 1.0;
    }

    img.Write(name);
}

template <unsigned int N> PIC_INLINE int RandomSampler<N>::getSamplesPerLevel(int level)
{
    if(level<0) {
        return -1;
    }

    if(level == 0) {
        return (levelsR[level] / N);
    } else {
        return ( levelsR[level] - levelsR[level - 1]) / N;
    }
}

template <unsigned int N> PIC_INLINE void RandomSampler<N>::getSampleAt(int level, int i, int &x, int &y)
{
    int start, end;

    if(level == 0) {
        start = 0;
    } else {
        start = levelsR[level - 1];
    }

    end = levelsR[level];

    i *= int(N);
    i = CLAMPi(i + start, start, end-1);

    x = samplesR[i    ] + window[0];
    y = samplesR[i + 1] + window[1];
}

template <unsigned int N>
PIC_INLINE void ConvertVectorToPlus1(std::vector<RandomSampler<N> > &rsVec,
                                     RandomSampler < N + 1 > &rsOut)
{
    //Copy data
    int halfSize = rsVec.size() / 2;

    for(int i = -halfSize; i <= halfSize; i++) {
        for(int k = 0; k < rsVec[i].samplesR.size(); k += N) {
            for(int l = 0; l < N; l++) {
                rsOut.samplesR.push_back(rsVec[i].samplesR[k + l]);
            }

            rsOut.samplesR.push_back(i);
        }
    }

    //Window assignment
    Vec < N + 1, int > window;

    for(int i = 0; i < N; i++) {
        window[i] = rsVec[0].window[i];
    }

    window[N] = halfSize;
    rsOut.window = window;
}



} // end namespace pic

#endif /* PIC_POINT_SAMPLERS_SAMPLER_RANDOM_HPP */
