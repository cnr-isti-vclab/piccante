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

#ifndef PIC_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP
#define PIC_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP

#include "../base.hpp"
#include "../filtering/filter.hpp"
#include "../filtering/filter_iterative.hpp"

namespace pic {

/**
 * @brief The FilterAnsiotropicDiffusion class
 */
class FilterAnsiotropicDiffusion: public Filter
{
protected:
    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

    float k, k_sq, delta_t;
    unsigned int mode;

public:

    /**
     * @brief FilterAnsiotropicDiffusion
     * @param k
     * @param mode
     */
    FilterAnsiotropicDiffusion(float k, unsigned int mode);

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param k
     * @param mode
     * @param iterations
     * @return
     */
    static Image *execute(ImageVec imgIn, Image *imgOut,
                                          float k, unsigned int mode, unsigned int iterations)
    {
        FilterAnsiotropicDiffusion ansio_flt(k, mode);
        FilterIterative iter_flt(&ansio_flt, iterations);
        imgOut = iter_flt.Process(imgIn, imgOut);
        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma_s
     * @param sigma_r
     * @param maxIterations
     * @return
     */
    static Image *execute(ImageVec imgIn, Image *imgOut,
                                          float sigma_s, float sigma_r, int maxIterations = -1)
    {
        if(sigma_s <= 0.0f) {
            sigma_s = 1.0f;
        }

        if(sigma_r <= 0.0f) {
            sigma_r = 0.05f;
        }

        int iterations = 0;

        if(maxIterations > 0) {
            iterations = maxIterations;
        } else {
            iterations = int(ceilf(5.0f * sigma_s));
        }

        FilterAnsiotropicDiffusion ansio_flt(sigma_r, 2);
        FilterIterative iter_flt(&ansio_flt, iterations);
        imgOut = iter_flt.Process(imgIn, imgOut);
        return imgOut;
    }

};

PIC_INLINE FilterAnsiotropicDiffusion::FilterAnsiotropicDiffusion(float k,
        unsigned int mode)
{
    if(k <= 0.0f) {
        k = 0.11f;
    }

    if(mode > 2 || mode < 0) {
        mode = 0;
    }

    delta_t = 1.0f / 7.0f;

    this->k = k;
    this->k_sq = k * k;

    this->mode = mode;
}

PIC_INLINE void FilterAnsiotropicDiffusion::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *img = src[0];
    int channels = img->channels;

    float *gN = new float [channels];
    float *gS = new float [channels];
    float *gW = new float [channels];
    float *gE = new float [channels];

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {

            float *dst_data  = (*dst)(i, j);
            float *img_data  = (*img)(i, j);

            float *img_dataN = (*img)(i + 1, j    );
            float *img_dataS = (*img)(i - 1, j    );
            float *img_dataW = (*img)(i    , j - 1);
            float *img_dataE = (*img)(i    , j + 1);

            float cN = 0.0f;
            float cS = 0.0f;
            float cW = 0.0f;
            float cE = 0.0f;

            for(int p = 0; p < channels; p++) {
                gN[p] = img_dataN[p] - img_data[p];
                gS[p] = img_dataS[p] - img_data[p];
                gW[p] = img_dataW[p] - img_data[p];
                gE[p] = img_dataE[p] - img_data[p];

                cN += gN[p] * gN[p];
                cS += gS[p] * gS[p];
                cW += gW[p] * gW[p];
                cE += gE[p] * gE[p];
            }

            switch(mode) {
                case 1:
                {
                    cN = 1.0f / (1.0f + cN / k_sq);
                    cS = 1.0f / (1.0f + cS / k_sq);
                    cW = 1.0f / (1.0f + cW / k_sq);
                    cE = 1.0f / (1.0f + cE / k_sq);
                } break;

                case 2:
                {
                    float t;
                    t = 1.0f - expf(-3.315f / powf(cN / k_sq, 8.0f));
                    cN = cN > 0.0f ? t : 1.0f;

                    t = 1.0f - expf(-3.315f / powf(cS / k_sq, 8.0f));
                    cS = cS > 0.0f ? t : 1.0f;

                    t = 1.0f - expf(-3.315f / powf(cW / k_sq, 8.0f));
                    cW = cW > 0.0f ? t : 1.0f;

                    t = 1.0f - expf(-3.315f / powf(cE / k_sq, 8.0f));
                    cE = cE > 0.0f ? t : 1.0f;
                } break;

                default:
                {
                    cN = expf(-cN / k_sq);
                    cS = expf(-cS / k_sq);
                    cW = expf(-cW / k_sq);
                    cE = expf(-cE / k_sq);
                } break;
            }

            for(int p = 0; p < channels; p++) {
                dst_data[p] = img_data[p] + delta_t *
                        (cN * gN[p] + cS * gS[p] + cW * gW[p] + cE * gE[p]);
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP */

