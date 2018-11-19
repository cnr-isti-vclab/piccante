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

#ifndef PIC_FILTERING_FILTER_NON_LOCAL_MEANS_HPP
#define PIC_FILTERING_FILTER_NON_LOCAL_MEANS_HPP

#include <random>

#include "../filtering/filter.hpp"
#include "../features_matching/patch_comp.hpp"
#include "../filtering/filter_bilateral_2ds.hpp"
#include "../util/array.hpp"
#include "../point_samplers/sampler_random_m.hpp"

namespace pic {

/**
 * @brief The FilterNonLocalMeans class
 */
class FilterNonLocalMeans: public Filter
{
protected:
    float sigma_r;
    double sigma_r_sq_2;
    int kernelSize;
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
     * @brief FilterNonLocalMeans
     */
    FilterNonLocalMeans()
    {
        seed = 1;
        ms = NULL;
    }

    /**
     * @brief FilterNonLocalMeans
     * @param sigma_s
     * @param sigma_r
     */
    FilterNonLocalMeans(int kernelSize, float sigma_r);

    /**
     * @brief update
     * @param sigma_r
     */
    void update(int kernelSize, float sigma_r);

};

PIC_INLINE FilterNonLocalMeans::FilterNonLocalMeans(int kernelSize, float sigma_r)
{
    update(kernelSize, sigma_r);
}

PIC_INLINE void FilterNonLocalMeans::update(int kernelSize, float sigma_r)
{
    //protected values are assigned/computed
    this->sigma_r = sigma_r;
    sigma_r_sq_2 = double(sigma_r * sigma_r) * 2.0;

    int halfKernelSize = kernelSize >> 1;
    this->kernelSize = (halfKernelSize << 1) + 1;

    int nSamples = int(lround(float(this->kernelSize)) * FilterBilateral2DS::getK(kernelSize));


    Vec2i window = Vec2i(halfKernelSize, halfKernelSize);
    ms = new MRSamplers<2>(ST_BRIDSON, window, nSamples, 1, 64);

    seed = 1;
}

PIC_INLINE void FilterNonLocalMeans::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    int width = dst->width;
    int height = dst->height;
    int channels = dst->channels;

    //Mersenne Twister
    std::mt19937 m(seed);

    PatchComp pc(src[0], src[0], kernelSize);

    float kernelSize_sq = float(kernelSize * kernelSize);

    float weight = sigma_r_sq_2 * kernelSize_sq * dst->channelsf;

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {

            RandomSampler<2> *ps = ms->getSampler(&m);
            int nSamples = int(ps->samplesR.size());

            float *tmp_dst = (*dst)(i, j);

            float tot = 0.0f;
            Array<float>::assign(0.0f, tmp_dst, dst->channels);

            for(int k = 0; k < nSamples; k += 2) {
                //fetch addresses
                int ci = CLAMP(i + ps->samplesR[k    ],  width);
                int cj = CLAMP(j + ps->samplesR[k + 1], height);

                float *tmp_src = (*src[0])(ci, cj);

                float w = expf(-pc.getSSD(i, j, ci, cj) / weight);
                tot += w;

                for(int c = 0; c < dst->channels; c++) {
                    tmp_dst[c] += tmp_src[c] * w;
                }

            }

            float *tmp_src = (*src[0])(i, j);
            bool sumTest = tot > 0.0f;

            for(int c = 0; c < dst->channels; c++) {
                tmp_dst[c] = sumTest ? tmp_dst[c] : tmp_src[c];
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NON_LOCAL_MEANS_HPP */

