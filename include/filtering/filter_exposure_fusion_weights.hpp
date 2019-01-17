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

#include "../filtering/filter.hpp"

#include "../colors/saturation.hpp"

namespace pic {

/**
 * @brief The FilterExposureFusionWeights class
 */
class FilterExposureFusionWeights: public Filter
{
protected:
    float wC, wE, wS;
    float mu, sigma_sq_2;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *pCur0 = (*src[0])(i, j);
                float *pCur1 = (*src[1])(i, j);

                //saturation
                float pSat = computeSaturation(pCur1, src[1]->channels);

                //contrast
                float *pCurN0 = (*src[0])(i, j + 1);
                float *pCurS0 = (*src[0])(i, j - 1);
                float *pCurE0 = (*src[0])(i + 1, j);
                float *pCurW0 = (*src[0])(i - 1, j);

                float pCon = fabsf(-4.0f * pCur0[0] +
                        pCurN0[0] + pCurS0[0] + pCurE0[0] + pCurW0[0]);

                //well-exposedness
                float pExp = 0.0f;
                for(int c = 0; c < src[1]->channels; c++) {
                    float delta = pCur1[c] - mu;
                    pExp += delta * delta;
                }
                pExp = expf(-pExp / sigma_sq_2);

                //final weights
                float *out = (*dst)(i, j);
                out[0] = powf(pCon, wC) * powf(pExp, wE) * powf(pSat, wS) + 1e-12f;
                out[0] = CLAMPi(out[0], 0.0f, 1.0f);
            }
        }
    }

public:

    /**
     * @brief FilterExposureFusionWeights
     * @param wC
     * @param wE
     * @param wS
     */
    FilterExposureFusionWeights(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f) : Filter()
    {
        update(wC, wE, wS);
        minInputImages = 2;
    }

    /**
     * @brief update
     * @param wC
     * @param wE
     * @param wS
     */
    void update(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f)
    {
        float sigma = 0.2f;

        mu = 0.5f;
        sigma_sq_2 = 2.0f * sigma * sigma;

        this->wC = wC > 0.0f ? MIN(wC, 1.0f) : 1.0f;
        this->wE = wE > 0.0f ? MIN(wE, 1.0f) : 1.0f;
        this->wS = wS > 0.0f ? MIN(wS, 1.0f) : 1.0f;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS */

