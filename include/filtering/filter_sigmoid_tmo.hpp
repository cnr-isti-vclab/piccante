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

#include "../filtering/filter.hpp"

namespace pic {

enum SIGMOID_MODE {SIG_TMO, SIG_TMO_WP, SIG_SDM};

/**
 * @brief The FilterSigmoidTMO class
 */
class FilterSigmoidTMO: public Filter
{
protected:
    bool temporal;
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
        FilterSigmoidTMO filter;
        return filter.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterSigmoidTMO::FilterSigmoidTMO() : Filter()
{
    update(SIG_TMO, 0.18f, 1e9f, -1.0f, false);
}

PIC_INLINE FilterSigmoidTMO::FilterSigmoidTMO(SIGMOID_MODE type, float alpha,
                                   float wp = 1e9f, float epsilon = -1.0f, bool temporal = false) : Filter()
{
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
        tmpEpsilon = imgIn[0]->getLogMeanVal(NULL, NULL)[0];
        break;

    case SIG_TMO_WP:
        tmpEpsilon = imgIn[0]->getLogMeanVal(NULL, NULL)[0];
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

    float *data, *dataFlt;

    if(src.size() == 2) {
        data    = src[0]->data;
        dataFlt = src[1]->data;
    } else {
        data    = src[0]->data;
        dataFlt = src[0]->data;
    }

    float *dataOut = dst->data;

    if(src[0]->channels == 3) {
        float lum_weights[] = {0.213f, 0.715f, 0.072f};

        float alpha_over_epsilon = alpha / epsilon;

        for(int j = box->y0; j < box->y1; j++) {
            int js = j * src[0]->ystride;

            for(int i = box->x0; i < box->x1; i++) {
                int c = js + i * src[0]->xstride; //index

                float L = Arrayf::dot(data, lum_weights, 3);

                if(L > 0.0f) {

                    float L_flt = Arrayf::dot(dataFlt, lum_weights, 3);
                    float Lm	 = L     * alpha_over_epsilon;
                    float Lm_flt = L_flt * alpha_over_epsilon;

                    float Ld = Lm / (1.0f + Lm_flt);

                    for(int k = 0; k < src[0]->channels; k++) {
                        int ck = c + k;
                        dataOut[ck] = (data[ck] * Ld) / L;
                    }
                } else {
                    Arrayf::assign(0.0f, dataOut, src[0]->channels);
                }
            }
        }
    } else {
        float Lm, Lm_Flt;

        for(int j = box->y0; j < box->y1; j++) {
            int js = j * src[0]->ystride;

            for(int i = box->x0; i < box->x1; i++) {
                int c = js + i * src[0]->xstride; //index

                for(int k = 0; k < src[0]->channels; k++) {
                    int ck = c + k;

                    switch(type) {

                    case SIG_TMO_WP: {
                        Lm =     (data[ck]    * alpha) / epsilon;
                        Lm_Flt = (dataFlt[ck] * alpha) / epsilon;

                        dataOut[ck] = Lm * (1.0f + Lm / wp_sq) / (1.0f + Lm_Flt);
                    }
                    break;

                    default: {
                        Lm = data[ck] * alpha;
                        Lm_Flt = dataFlt[ck] * alpha;

                        dataOut[ck] = Lm / (Lm_Flt + epsilon);
                    }
                    break;
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SIGMOID_TMO_HPP */

