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

#ifndef PIC_FILTERING_FILTER_SIGMOID_TMO_HPP
#define PIC_FILTERING_FILTER_SIGMOID_TMO_HPP

#include "../util/array.hpp"
#include "../util/std_util.hpp"

#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"

namespace pic {

enum SIGMOID_MODE {SIG_TMO, SIG_TMO_WP, SIG_SDM};

/**
 * @brief The FilterSigmoidTMO class
 */
class FilterSigmoidTMO: public Filter
{
protected:
    bool temporal;
    float *lum_weights, *lum_weights_flt;
    float c, alpha, epsilon, wp, wp_sq;
    SIGMOID_MODE type;

    /**
     * @brief calculateEpsilon
     * @param imgIn
     * @return
     */
    float calculateEpsilon(ImageVec imgIn);

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
\
    /**
     * @brief FilterSigmoidTMO
     */
    FilterSigmoidTMO();

    /**
     * @brief FilterSigmoidTMO
     * @param type
     * @param alpha
     * @param wp
     * @param epsilon
     * @param temporal
     */
    FilterSigmoidTMO(SIGMOID_MODE type, float alpha, float wp, float epsilon,
                     bool temporal);

    ~FilterSigmoidTMO()
    {
        delete_s(lum_weights);
        delete_s(lum_weights_flt);
    }

    /**
     * @brief update
     * @param type
     * @param alpha
     * @param wp
     * @param epsilon
     * @param temporal
     */
    void update(SIGMOID_MODE type, float alpha, float wp, float epsilon,
                     bool temporal);

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        if(epsilon <= 0.0f || temporal) {
            epsilon = calculateEpsilon(imgIn);
        }

        lum_weights = delete_s(lum_weights);
        lum_weights_flt = delete_s(lum_weights_flt);

        lum_weights = FilterLuminance::computeWeights(LT_CIE_LUMINANCE, imgIn[0]->channels, NULL);

        int whichImage = (imgIn.size() > 1) ? 1 : 0;
        lum_weights_flt = FilterLuminance::computeWeights(LT_CIE_LUMINANCE, imgIn[whichImage]->channels, NULL);

        width       = imgIn[0]->width;
        height      = imgIn[0]->height;
        channels    = imgIn[0]->channels;
        frames      = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FilterSigmoidTMO filter(SIG_TMO, 0.18f, 1e9f, -1.0f, false);
        return filter.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterSigmoidTMO::FilterSigmoidTMO() : Filter()
{
    lum_weights = NULL;
    lum_weights_flt = NULL;
    update(SIG_TMO, 0.18f, 1e6f, -1.0f, false);
}

PIC_INLINE FilterSigmoidTMO::FilterSigmoidTMO(SIGMOID_MODE type, float alpha,
                                   float wp, float epsilon, bool temporal) : Filter()
{
    lum_weights = NULL;
    lum_weights_flt = NULL;
    update(type, alpha, wp, epsilon, temporal);
}

PIC_INLINE void FilterSigmoidTMO::update(SIGMOID_MODE type, float alpha,
                                   float wp = 1e9f, float epsilon = -1.0f, bool temporal = false)
{
    this->type = type;
    this->alpha = alpha;
    this->wp = wp;
    this->wp_sq = wp * wp;

    this->epsilon = epsilon;
    this->temporal = temporal;
}

PIC_INLINE float FilterSigmoidTMO::calculateEpsilon(ImageVec imgIn)
{
    float tmpEpsilon, retEpsilon;

    switch(type) {
    case SIG_TMO:
        imgIn[0]->getLogMeanVal(NULL, &tmpEpsilon);
        break;

    case SIG_TMO_WP:
        imgIn[0]->getLogMeanVal(NULL, &tmpEpsilon);
        break;

    case SIG_SDM:
        tmpEpsilon = 1.0f;
        break;

    default:
        tmpEpsilon = 1.0f;
    }

    if(temporal) {
        if(epsilon > 0.0f) {
            retEpsilon = (epsilon + tmpEpsilon) / 2.0f;
        } else {
            retEpsilon = tmpEpsilon;
        }
    } else {
        retEpsilon = tmpEpsilon;
    }

    return retEpsilon;
}

PIC_INLINE void FilterSigmoidTMO::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{

    Image *img, *img_flt;

    img = src[0];

    if(src.size() > 1) {
        img_flt = src[1];
    } else {
        img_flt = src[0];
    }

    float alpha_over_epsilon = alpha / epsilon;

    for(int j = box->y0; j < box->y1; j++) {

        for(int i = box->x0; i < box->x1; i++) {

            float *p = (*img)(i, j);
            float *p_flt = (*img_flt)(i, j);

            float *dstOut = (*dst)(i, j);

            float L = Arrayf::dot(p, lum_weights, img->channels);

            if(L > 0.0f) {
                float L_flt = Arrayf::dot(p_flt, lum_weights_flt, img_flt->channels);

                float Lm = L * alpha_over_epsilon;
                float Lm_flt = L_flt * alpha_over_epsilon;
                float Ld;

                if(type == SIG_TMO_WP) {
                    Ld = L * (1.0f + L / wp_sq) / (1.0f + Lm_flt);
                } else {
                    Ld = Lm / (1.0f + Lm_flt);
                }

                for(int k = 0; k < dst->channels; k++) {
                    dstOut[k] = (p[k] * Ld) / L;
                }
            } else {
                Arrayf::assign(0.0f, dstOut, dst->channels);
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SIGMOID_TMO_HPP */

