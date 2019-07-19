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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DF_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DF_HPP

#include "../base.hpp"

#include "../util/std_util.hpp"

#include "../util/array.hpp"

#include "../util/precomputed_gaussian.hpp"

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterBilateral2DF class
 */
class FilterBilateral2DF: public Filter
{
protected:
    float sigma_s, sigma_r, sigma_r_sq_2;

    PrecomputedGaussian *pg;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    /**
     * @brief FilterBilateral2DF
     */
    FilterBilateral2DF();

    /**
     * @brief FilterBilateral2DF
     * @param sigma_s
     * @param sigma_r
     */
    FilterBilateral2DF(float sigma_s, float sigma_r);

    ~FilterBilateral2DF();

    /**
     * @brief signature
     * @return
     */
    std::string signature()
    {
        return genBilString("F", sigma_s, sigma_r);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut,
                             float sigma_s, float sigma_r)
    {
        //filter
        FilterBilateral2DF filter(sigma_s, sigma_r);
        Image *out = filter.Process(Single(imgIn), imgOut);
        return out;
    }
};

PIC_INLINE FilterBilateral2DF::FilterBilateral2DF() : Filter()
{
    pg = NULL;
}

PIC_INLINE FilterBilateral2DF::FilterBilateral2DF(float sigma_s, float sigma_r) : Filter()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s > 0.0f ? sigma_s : 1.0f;
    this->sigma_r = sigma_r > 0.0f ? sigma_r : 0.01f;
    this->sigma_r_sq_2 = this->sigma_r * this->sigma_r * 2.0f;

    //Precomputation of the Gaussian filter
    pg = new PrecomputedGaussian(sigma_s);
}

FilterBilateral2DF::~FilterBilateral2DF()
{
    pg = delete_s(pg);
}

PIC_INLINE void FilterBilateral2DF::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int channels = dst->channels;

    //Filtering
    Image *edge, *base;

    if(src.size() > 1) {
        //Joint/Cross Bilateral Filtering
        base = src[0];
        edge = src[1];
    } else {
        base = src[0];
        edge = src[0];
    }

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            //Convolution kernel
            float *dst_data = (*dst)(i, j);

            float *data_edge = (*edge)(i, j);

            Arrayf::assign(0.0f, dst_data, channels);

            float sum = 0.0f;
            for(int k = 0; k < pg->kernelSize; k++) {
                int cj = j + k - pg->halfKernelSize;

                for(int l = 0; l < pg->kernelSize; l++) {
                    int ci = i + l - pg->halfKernelSize;

                    //Spatial weight
                    float G1 = pg->coeff[k] * pg->coeff[l];

                    //Range weight
                    float *cur_edge = (*edge)(ci, cj);

                    float tmp = Arrayf::distanceSq(data_edge, cur_edge, edge->channels);
                    float G2 = expf(-tmp / sigma_r_sq_2);

                    //Weight
                    float weight = G1 * G2;

                    //filter
                    float *base_data_cur = (*base)(ci, cj);

                    for(int m = 0; m < channels; m++) {
                        dst_data[m] += base_data_cur[m] * weight;
                    }

                    sum += weight;
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

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DF_HPP */

