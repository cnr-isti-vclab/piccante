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

#ifndef PIC_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS
#define PIC_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS

#include "filtering/filter.hpp"

#include "colors/saturation.hpp"

namespace pic {

/**
 * @brief The FilterExposureFusionWeights class
 */
class FilterExposureFusionWeights: public Filter
{
protected:
    float wC, wE, wS;
    float mu, sigma2;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int channels = src[0]->channels;
        int width = src[0]->width;
        int height = src[0]->height;
        int tmp;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int ind = c + i;

                //Saturation
                float pSat = computeSaturation(&src[0]->data[ind * channels], channels);

                //Contrast
                float pCon = -4.0f * src[1]->data[ind];

                tmp   = CLAMP(i + 1, width);
                pCon += src[1]->data[c + tmp];

                tmp   = CLAMP(i - 1, width);
                pCon += src[1]->data[c + tmp];

                tmp   = CLAMP(j + 1, height);
                pCon += src[1]->data[tmp * width + i];

                tmp   = CLAMP(j - 1, height);
                pCon += src[1]->data[tmp * width + i];

                pCon = fabsf(pCon);

                //Well-exposedness
                float tmpL = src[1]->data[ind] - mu;
                float pWE = expf(-(tmpL * tmpL) / sigma2);

                //Final weights
                dst->data[ind] =  powf(pCon, wC) *
                                  powf(pWE,  wE) *
                                  powf(pSat, wS);
            }
        }
    }

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *SetupAux(ImageVec imgIn, Image *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
        } else {
            if((imgIn[0]->width  != imgOut->width)  ||
               (imgIn[0]->height != imgOut->height) ||
               (imgOut->channels != 1)) {
                imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
            }
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterExposureFusionWeights
     * @param type
     */
    FilterExposureFusionWeights(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f)
    {
        float sigma = 0.2f;

        mu = 0.5f;
        sigma2 = 2.0f * sigma * sigma;

        this->wC = wC > 0.0f ? wC : 1.0f;
        this->wE = wE > 0.0f ? wE : 1.0f;
        this->wS = wS > 0.0f ? wS : 1.0f;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(Image *imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn->width;
        height      = imgIn->height;
        channels    = 1;
        frames      = imgIn->frames;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS */

