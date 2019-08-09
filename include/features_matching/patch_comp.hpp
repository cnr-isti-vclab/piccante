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

#ifndef PIC_FEATURES_MATCHING_PATCH_COMP_HPP
#define PIC_FEATURES_MATCHING_PATCH_COMP_HPP

#include "../image.hpp"

#include "../util/math.hpp"
#include "../util/std_util.hpp"
#include "../util/array.hpp"

#include "../image_samplers/image_sampler_bilinear.hpp"

#include "../features_matching/transform_data.hpp"

namespace pic {

/**
 * @brief The PatchComp class
 */
class PatchComp
{
protected:
    ImageSamplerBilinear isb;

    Image *img0, *img1, *img0_g, *img1_g;

    //patchsize
    int patchSize, halfPatchSize;
    float patchSize_sq;

    //stereo
    float alpha;

public:

    /**
     * @brief PatchComp
     */
    PatchComp()
    {
        setNULL();
    }

    /**
     * @brief PatchComp
     * @param img0
     * @param img1
     * @param patchSize
     */
    PatchComp(Image *img0, Image *img1, int patchSize)
    {
        setNULL();

        setup(img0, img1, NULL, NULL, patchSize, 0.05f);
    }

    /**
     * @brief PatchComp
     * @param img0
     * @param img1
     * @param patchSize
     * @param alpha
     */
    PatchComp(Image *img0, Image *img1,
              Image *img0_g, Image *img1_g,
              int patchSize, float alpha)
    {
        setNULL();
        setup(img0, img1, img0_g, img1_g, patchSize, alpha);
    }

    /**
     * @brief setNULL
     */
    void setNULL()
    {
        img0   = NULL;
        img0_g = NULL;
        img1   = NULL;
        img1_g = NULL;

        alpha = -1.0f;

        patchSize = -1;
        halfPatchSize = -1;
    }

    /**
     * @brief setup
     * @param img0
     * @param img1
     * @param patchSize
     */
    void setup(Image *img0,   Image *img1,
               Image *img0_g, Image *img1_g,
               int patchSize, float alpha)
    {
        if(patchSize < 1) {
            return;
        }

        this->img0 = img0;
        this->img1 = img1;
        this->img0_g = img0_g;
        this->img1_g = img1_g;

        this->alpha = CLAMPi(alpha, 0.0f, 1.0f);

        if(this->patchSize != patchSize) {
            this->patchSize_sq = float(patchSize * patchSize);
            this->halfPatchSize = patchSize >> 1;
            this->patchSize = (halfPatchSize << 1) + 1;
        }
    }

    /**
     * @brief getSSDSmooth
     * @param x0
     * @param y0
     * @param x1
     * @param y1
     * @return
     */
    float getSSDSmooth(int x0, int y0, int x1, int y1)
    {
        float alpha_i = 1.0f - alpha;

        float ret = 0.0f;

        for(int i = -halfPatchSize; i <= halfPatchSize; i++) {
            for(int j = -halfPatchSize; j <= halfPatchSize; j++) {
                float *img0_ij = (*img0)(x0 + j, y0 + i);
                float *img1_ij = (*img1)(x1 + j, y1 + i);

                float *img0_g_ij = (*img0_g)(x0 + j, y0 + i);
                float *img1_g_ij = (*img1_g)(x1 + j, y1 + i);

                //color term
                float err_col = sqrtf(Arrayf::distanceSq(img1_ij, img0_ij, img0->channels));

                //gradient term
                float err_grad = sqrtf(Arrayf::distanceSq(img0_g_ij, img1_g_ij, 2));

                //err term
                ret += alpha_i * err_col + alpha * err_grad;

            }            
        }

        return ret;
    }

    /**
     * @brief getSSD
     * @param x0
     * @param y0
     * @param x1
     * @param y1
     * @return
     */
    float getSSD(int x0, int y0, int x1, int y1)
    {
        float ret = 0.0f;
        for(int i = -halfPatchSize; i <= halfPatchSize; i++) {
            for(int j = -halfPatchSize; j <= halfPatchSize; j++) {
                float *img0_ij = (*img0)(x0 + j, y0 + i);
                float *img1_ij = (*img1)(x1 + j, y1 + i);

                ret += Arrayf::distanceSq(img0_ij, img1_ij, img0->channels);
            }
        }

        return ret;
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_PATCH_COMP_HPP */

