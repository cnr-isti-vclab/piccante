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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DS_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DS_HPP

#include <random>

#include "../base.hpp"
#include "../util/string.hpp"
#include "../util/std_util.hpp"

#include "../filtering/filter.hpp"
#include "../util/precomputed_gaussian.hpp"
#include "../point_samplers/sampler_random_m.hpp"

namespace pic {

/**
 * @brief The FilterBilateral2DS class
 */
class FilterBilateral2DS: public Filter
{
protected:
    float sigma_s, sigma_r, sigma_r_sq_2;
    PrecomputedGaussian *pg;
    MRSamplers<2> *ms;
    int seed;
    int nSamples;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterBilateral2DS
     */
    FilterBilateral2DS()
    {
        seed = 1;
        pg = NULL;
        ms = NULL;
    }

    ~FilterBilateral2DS()
    {
        delete_s(pg);
        delete_s(ms);
    }

    /**
     * @brief FilterBilateral2DS
     * @param nameFile
     * @param sigma_r
     */
    FilterBilateral2DS(std::string nameFile, float sigma_r);

    /**
     * @brief FilterBilateral2DS
     * @param type
     * @param sigma_s
     * @param sigma_r
     * @param mult
     */
    FilterBilateral2DS(float sigma_s, float sigma_r, int mult, SAMPLER_TYPE type);

    /**
     * @brief update
     * @param sigma_s
     * @param sigma_r
     * @param mult
     * @param type
     */
    void update(float sigma_s, float sigma_r, int mult, SAMPLER_TYPE type);

    /**
     * @brief signature
     * @return
     */
    std::string signature()
    {
        return genBilString("S", sigma_s, sigma_r);
    }

    /**
     * @brief Write
     * @param filename
     * @return
     */
    bool Write(std::string filename);

    /**
     * @brief Read
     * @param filename
     * @return
     */
    bool Read(std::string filename);

    /**
     * @brief execute
     * @param imgIn
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *execute(Image *imgIn,
                             float sigma_s, float sigma_r)
    {
        //create the filter
        FilterBilateral2DS filter(sigma_s, sigma_r, 1, ST_BRIDSON);
        //filter
        Image *imgOut = filter.Process(Single(imgIn), NULL);
        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgEdge
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgEdge,
                             float sigma_s, float sigma_r)
    {
        FilterBilateral2DS filter(sigma_s, sigma_r, 1, ST_BRIDSON);
        Image *imgOut;

        if(imgEdge == NULL) {
            imgOut = filter.Process(Single(imgIn), NULL);
        } else {
            imgOut = filter.Process(Double(imgIn, imgEdge), NULL);
        }
        return imgOut;
    }

    /**
     * @brief getK
     * @param kernelSize
     * @return
     */
    static inline float getK(int kernelSize)
    {
        //	float ret = 0.9577f/(0.6466f*float(kernelSize)-0.9175f)+0.4505;
        float ret = 0.4055f / (0.6437f * float(kernelSize) - 1.1083f) + 0.7347f;
        ret = (ret > 0.0f) ? ret : 3.0f;

        return ret;
    }

    /**
     * @brief getK2
     * @param kernelSize
     * @return
     */
    static inline float getK2(int kernelSize)
    {
        float ret = 0.3233f / (0.5053f * float(kernelSize) - 0.8272f) + 0.7366f;
        ret = (ret > 0.0f) ? ret : 2.5f;
        return ret;
    }

    /**precomputeKernels
     * @brief PrecomputedKernels
     */
    static void precomputeKernels()
    {
        for(int i = 0; i < 6; i++) {
            float sigma_s = powf(2.0f, float(i));

            int nSamples = PrecomputedGaussian::getKernelSize(sigma_s);
            int nSamplesDiv2  = nSamples / 2;
            int nMaxSamples = nSamplesDiv2 * nSamplesDiv2;
            int oldNSamples = -1;

            printf("Compute kernel sigma_s: %f\n", sigma_s);

            for(int j = 1; j <= 16; j++) {
                printf("Multiplier: %d\n", j);
                nSamples = MIN((nSamplesDiv2 * j), nMaxSamples);

                if(nSamples == oldNSamples) {
                    break;
                }

                FilterBilateral2DS f2DS(sigma_s, 0.01f, j, ST_BRIDSON);
                f2DS.Write("kernel_" + fromNumberToString(sigma_s) + "_" + fromNumberToString(j) + ".txt");
                oldNSamples = nSamples;
            }
        }
    }
};

PIC_INLINE FilterBilateral2DS::FilterBilateral2DS(std::string nameFile,
        float sigma_r) : Filter()
{
    ms = NULL;
    pg = NULL;
    Read(nameFile);
    this->sigma_r = sigma_r;
}

PIC_INLINE FilterBilateral2DS::FilterBilateral2DS(
        float sigma_s, float sigma_r, int mult = 1, SAMPLER_TYPE type = ST_BRIDSON) : Filter()
{
    pg = NULL;
    ms = NULL;
    update(sigma_s, sigma_r, mult, type);
}

PIC_INLINE void FilterBilateral2DS::update( float sigma_s,
        float sigma_r, int mult = 1, SAMPLER_TYPE type = ST_BRIDSON)
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s > 0.0f ? sigma_s : 1.0f;
    this->sigma_r = sigma_r > 0.0f ? sigma_r : 0.01f;
    this->sigma_r_sq_2 = this->sigma_r * this->sigma_r * 2.0f;

    //Precomputation of the Gaussian Kernel
    pg = delete_s(pg);
    pg = new PrecomputedGaussian(sigma_s);//, sigma_r);

    //Poisson samples
    int nMaxSamples = pg->halfKernelSize * pg->halfKernelSize;

    int nSamples = int(lround(float(pg->kernelSize)) * getK(int(sigma_s))) * mult;

    nSamples = MIN(nSamples, nMaxSamples);
    //	nSamples = MIN(	(pg->halfKernelSize*mult), nMaxSamples);

#ifdef PIC_DEBUG
    printf("Nsamples: %d %f\n", nSamples, sigma_s);
#endif

    Vec2i window = Vec2i(pg->halfKernelSize, pg->halfKernelSize);

    ms = delete_s(ms);
    ms = new MRSamplers<2>(type, window, nSamples, 1, 64);

    seed = 1;
}

PIC_INLINE void FilterBilateral2DS::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *edge, *base;

    switch(src.size()) {
    //cross bilateral filter
    case 2:
        base = src[0];
        edge = src[1];
        break;

    default:
        base = src[0];
        edge = src[0];
    }

    int channels = dst->channels;
    int edgeChannels = edge->channels;

    //Mersenne Twister
    std::mt19937 m(seed);

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float *dst_data  = (*dst )(i, j);
            float *edge_data = (*edge)(i, j);

            Array<float>::assign(0.0f, dst_data, channels);

            RandomSampler<2> *ps = ms->getSampler(&m);
            int nSamples = int(ps->samplesR.size());

            float sum = 0.0f;
            for(int k = 0; k < nSamples; k += 2) {
                //fetch addresses
                int ci = i + ps->samplesR[k    ];
                int cj = j + ps->samplesR[k + 1];

                //
                //fetch the precomputed Spatial Gaussian kernel
                //
                float G1 = pg->coeff[ps->samplesR[k    ] + pg->halfKernelSize] *
                           pg->coeff[ps->samplesR[k + 1] + pg->halfKernelSize];

                float *edge_data_ci_cj = (*edge)(ci, cj);

                //
                //compute the Range Gaussian kernel
                //
                float acc_delta_range_sq = Arrayf::distanceSq(edge_data_ci_cj, edge_data, edgeChannels);

                float G2 = exp(-acc_delta_range_sq / sigma_r_sq_2);

                //
                //compute the final weight
                //
                float weight = G1 * G2;
                sum += weight;

                float *base_data_ci_cj = (*base)(ci, cj);

                //filter
                for(int l = 0; l < channels; l++) {
                    dst_data[l] += base_data_ci_cj[l] * weight;
                }
            }

            //normalization
            if(sum > 0.0f) {
                Arrayf::div(dst_data, channels, sum);
            } else {
                float *base_data = (*base)(i, j);
                Arrayf::assign(base_data, channels, dst_data);
            }

        }
    }
}

PIC_INLINE bool FilterBilateral2DS::Write(std::string nameFile)
{
    //TODO: add the writing of (sigms_s, sigma_r)
    return ms->Write(nameFile);
}

PIC_INLINE bool FilterBilateral2DS::Read(std::string filename)
{
    //TODO: add the reading of (sigms_s, sigma_r)
    //Precomputation of the Gaussian Kernel
    pg = delete_s(pg);
    pg = new PrecomputedGaussian(sigma_s);

    ms = delete_s(ms);
    ms = new MRSamplers<2>();
    return ms->Read(filename);
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DS_HPP */

