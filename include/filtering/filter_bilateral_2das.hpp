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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DAS_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DAS_HPP

#include <random>

#include "../base.hpp"
#include "../util/std_util.hpp"
#include "../util/precomputed_gaussian.hpp"
#include "../filtering/filter_sampling_map.hpp"
#include "../point_samplers/sampler_random_m.hpp"

namespace pic {

/**
 * @brief The FilterBilateral2DAS class
 */
class FilterBilateral2DAS: public Filter
{
protected:
    float sigma_s, sigma_r, sigma_r_sq_2;
    PrecomputedGaussian *pg;
    ImageSamplerBilinear isb;
    int seed;
    Image *samplingMap;

    MRSamplers<2> *ms;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    /**
     * @brief FilterBilateral2DAS
     */
    FilterBilateral2DAS();

    /**
     * @brief FilterBilateral2DAS
     * @param type
     * @param sigma_s
     * @param sigma_r
     * @param mult
     */
    FilterBilateral2DAS(float sigma_s, float sigma_r, int mult, SAMPLER_TYPE type);

    ~FilterBilateral2DAS();

    /**
     * @brief update
     * @param sigma_s
     * @param sigma_r
     * @param mult
     * @param type
     */
    void update(float sigma_s, float sigma_r, int mult, SAMPLER_TYPE type);

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *setupAux(ImageVec imgIn, Image *imgOut)
    {
        FilterSamplingMap fsm(sigma_s);

        samplingMap = fsm.Process(imgIn, samplingMap);
        float maxVal;
        samplingMap->getMaxVal(NULL, &maxVal);
        *samplingMap /= maxVal;

        return allocateOutputMemory(imgIn, imgOut, bDelete);
    }

    /**
     * @brief Signature
     * @return
     */
    std::string signature()
    {
        return genBilString("AS", sigma_s, sigma_r);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float sigma_s, float sigma_r)
    {
        FilterBilateral2DAS flt(sigma_s, sigma_r, 1, ST_DARTTHROWING);
        imgOut = flt.Process(Single(imgIn), imgOut);
        return imgOut;
    }
};

PIC_INLINE FilterBilateral2DAS::FilterBilateral2DAS() : Filter()
{
    seed = 1;
    pg = NULL;
    ms = NULL;
    samplingMap = NULL;
}

PIC_INLINE FilterBilateral2DAS::FilterBilateral2DAS(float sigma_s,
        float sigma_r, int mult = 1, SAMPLER_TYPE type = ST_BRIDSON) : Filter()
{
    seed = 1;
    pg = NULL;
    ms = NULL;
    samplingMap = NULL;

    update(sigma_s, sigma_r, mult, type);
}

PIC_INLINE FilterBilateral2DAS::~FilterBilateral2DAS()
{
    pg = delete_s(pg);
    ms = delete_s(ms);
    samplingMap = delete_s(samplingMap);
}

PIC_INLINE void FilterBilateral2DAS::update(float sigma_s,
        float sigma_r, int mult = 1, SAMPLER_TYPE type = ST_BRIDSON)
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s > 0.0f ? sigma_s : 1.0f;
    this->sigma_r = sigma_r > 0.0f ? sigma_r : 0.01f;
    this->sigma_r_sq_2 = this->sigma_r * this->sigma_r * 2.0f;

    //precompute the Gaussian Kernel
    pg = delete_s(pg);
    pg = new PrecomputedGaussian(sigma_s);

    //Poisson samples
    Vec2i window = Vec2i(pg->halfKernelSize, pg->halfKernelSize);

    ms = delete_s(ms);
    if(mult > 0) {
        ms = new MRSamplers<2>(type, window, pg->halfKernelSize * mult, 3, 64);
    } else if(mult < 0) {
        mult = -mult;
        ms = new MRSamplers<2>(type, window, pg->halfKernelSize / mult, 3, 64);
    }

    seed = 1;
}

PIC_INLINE void FilterBilateral2DAS::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int channels = dst->channels;

    //filter
    Image *edge, *base;

    switch(src.size()) {
        case 1: {
            base = src[0];
            edge = src[0];
        } break;

        default: {
            base = src[0];
            edge = src[1];
        } break;

    }

    RandomSampler<2> *ps;
    float valOut;

    //Mersenne Twister
    std::mt19937 m(seed);

    for(int i = box->y0; i < box->y1; i++) {
        float x = float(i) / dst->heightf;

        for(int j = box->x0; j < box->x1; j++) {

            //convolve with the kernel
            float *dst_data = (*dst)(j, i);
            float *edge_data = (*edge)(j, i);

            Arrayf::assign(0.0f, dst_data, channels);

            ps = ms->getSampler(&m);

            //calculate the number of samples
            float y = float(j) / dst->widthf;
            isb.SampleImage(samplingMap, x, y, &valOut);

            float tmpValOut = 1.0f - valOut; //+valOut[1]+valOut[2])/3.0f;
            float levelVal = CLAMPi(tmpValOut, 0.0f, 0.9f) * float(ps->levelsR.size());

            int levelInt = int(floorf(levelVal));
            int nSamples = ps->levelsR[levelInt];

            int levelsRsize = int(ps->levelsR.size()) - 1;
            if(levelInt < levelsRsize) {
                if((levelVal - float(levelInt)) > 0.0f) {
                    nSamples += int(float(ps->levelsR[levelInt + 1] - ps->levelsR[levelInt]) *
                                (levelVal - float(levelInt)));
                }
            }

            if((nSamples % 2) == 1) {
                nSamples++;
            }

            nSamples = MIN(nSamples, pg->halfKernelSize * pg->halfKernelSize * 2);

            float sum = 0.0f;
            for(int k = 0; k < nSamples; k += 2) {
                //fetch addresses
                int cj = j + ps->samplesR[k    ];
                int ci = i + ps->samplesR[k + 1];

                //
                //Spatial Gaussian kernel
                //
                float G1 = pg->coeff[ps->samplesR[k    ] + pg->halfKernelSize] *
                           pg->coeff[ps->samplesR[k + 1] + pg->halfKernelSize];


                float *cur_edge = (*edge)(cj, ci);

                //
                //Range Gaussian Kernel
                //
                float tmp = Arrayf::distanceSq(cur_edge, edge_data, channels);
                float G2 = expf(-tmp / sigma_r_sq_2);

                //Weight
                float weight = G1 * G2;
                sum += weight;

                //filter
                float *base_data_ci_cj = (*base)(cj, ci);

                for(int l = 0; l < channels; l++) {
                    dst_data[l] += base_data_ci_cj[l] * weight;
                }
            }

            //normalization
            if(sum > 0.0f) {
                Arrayf::div(dst_data, channels, sum);
            } else {
                float *base_data = (*base)(j, i);
                Arrayf::assign(base_data, channels, dst_data);
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DAS_HPP */
