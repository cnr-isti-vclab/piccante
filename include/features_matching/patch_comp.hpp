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

#include "image.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"

#include "features_matching/transform_data.hpp"

namespace pic {

/**
 * @brief The PatchComp class
 */
class PatchComp
{
protected:
    float					*VALUES_COS;
    float					*VALUES_SIN;

    ImageSamplerBilinear	isb;
    Image					*img0, *img1;
    int						patchSize, halfPatchSize;

    //stereo
    Image					*lap0, *lap1;
    float                   gamma_c_2, gamma_l_2;

public:

    /**
     * @brief PatchComp
     */
    PatchComp()
    {
        VALUES_COS = NULL;
        VALUES_SIN = NULL;
        halfPatchSize = -1;
    }

    /**
     * @brief PatchComp
     * @param img0
     * @param img1
     * @param patchSize
     */
    PatchComp(Image *img0, Image *img1, int patchSize)
    {
        VALUES_COS = NULL;
        VALUES_SIN = NULL;

        Init(img0, img1, patchSize);
    }

    /**
     * @brief PatchComp
     * @param img0
     * @param img1
     * @param lap0
     * @param lap1
     * @param patchSize
     * @param gamma_c
     * @param gamma_l
     */
    PatchComp(Image *img0, Image *img1, Image *lap0, Image *lap1, int patchSize, float gamma_c, float gamma_l)
    {
        VALUES_COS = NULL;
        VALUES_SIN = NULL;

        this->gamma_c_2 = gamma_c * gamma_c * 2.0f;
        this->gamma_l_2 = gamma_l * gamma_l * 2.0f;

        this->lap0 = lap0;
        this->lap1 = lap1;

        Init(img0, img1, patchSize);
    }

    ~PatchComp()
    {
        if(VALUES_COS != NULL) {
            delete[] VALUES_COS;
        }

        if(VALUES_SIN != NULL) {
            delete[] VALUES_SIN;
        }
    }

    /**
     * @brief Init
     * @param img0
     * @param img1
     * @param patchSize
     */
    void Init(Image *img0, Image *img1, int patchSize)
    {
        this->img0 = img0;
        this->img1 = img1;

        halfPatchSize = patchSize >> 1;
        this->patchSize = (halfPatchSize << 1) + 1;

        VALUES_SIN = new float[this->patchSize];
        VALUES_COS = new float[this->patchSize];
    }

    /**
     * @brief improveStereo
     * @param x0
     * @param y
     * @param x1
     * @param xb
     * @param db
     */
    void improveStereo(int x0, int y, int x1, int &xb, float &db)
    {
        float dist = getSSDSmooth(x0, y, x1, y);

        if(dist < db) {
            xb = x0;
            db = dist;
        }
    }

    /**
     * @brief improve
     * @param x0
     * @param y0
     * @param x1
     * @param y1
     * @param xb
     * @param yb
     * @param db
     */
    void improve(int x0, int y0,
                      int x1, int y1,
                      int &xb, int &yb, float &db)
    {

        float dist = getSSD(x0, y0, x1, y1);

        if(dist < db) {
            xb = x0;
            yb = y0;
            db = dist;
        }
    }

    /**
     * @brief improve
     * @param x0
     * @param y0
     * @param angle
     * @param scale
     * @param x1
     * @param y1
     * @param xb
     * @param yb
     * @param ab
     * @param sb
     * @param db
     */
    void improve(int x0, int y0, float angle, float scale,
                 int x1, int y1,
                 int &xb, int &yb, float &ab, float &sb, float &db)
    {

        float dist = getSSD(x0, y0, angle, scale, x1, y1, db);

        if(dist < db) {
            xb = x0;
            yb = y0;
            ab = angle;
            sb = scale;
            db = dist;
        }
    }

    /**
     * @brief improve
     * @param td0
     * @param td1
     * @param tb
     * @param db
     */
    void improve(TransformData *td0, TransformData *td1, TransformData *tb,
                 float &db)
    {
        float dist = getSSD(td0, td1, db);

        if(dist < db) {
            tb->x		= td0->x;
            tb->y		= td0->y;
            tb->angle	= td0->angle;
            tb->scale	= td0->scale;
            tb->gain	= td0->gain;
            tb->bias	= td0->bias;
            db			= dist;
        }
    }


    /**
     * @brief improve
     * @param x0
     * @param y0
     * @param x1
     * @param y1
     * @param xb
     * @param yb
     * @param db
     * @param tmpAnn
     * @param n
     */
    void improve(int x0, int y0,
                 int x1, int y1,
                 int &xb, int &yb, float &db,
                 float *tmpAnn, int n)
    {

        float dist = getSSD(x0, y0, x1, y1);

        if(dist < db) {
            for(int i = (n - 3); i > 3; i -= 3) {
                tmpAnn[i  ] = tmpAnn [i - 3];
                tmpAnn[i + 1] = tmpAnn [i - 2];
                tmpAnn[i + 2] = tmpAnn [i - 1];
            }

            tmpAnn[0] = float(x0);
            tmpAnn[0] = float(y0);
            tmpAnn[0] = dist;

            xb = x0;
            yb = y0;
            db = dist;
        } else { //insertion of the value
            for(int i = 0; i < n; i += 3) {
                if((x0 == int(tmpAnn[i])) &&
                   (y0 == int(tmpAnn[i + 1]))) {
                    break;
                } else {
                    if(dist < tmpAnn[i + 2]) { //shift
                        for(int j = (n - 3); j > i; j -= 3) {
                            tmpAnn[j  ] = tmpAnn [j - 3];
                            tmpAnn[j + 1] = tmpAnn [j - 2];
                            tmpAnn[j + 2] = tmpAnn [j - 1];
                        }

                        tmpAnn[i  ] = float(x0);
                        tmpAnn[i + 1] = float(y0);
                        tmpAnn[i + 2] = dist;
                        break;
                    }
                }
            }
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
        float *tmp_img0 = (*img0)(x0, y0);
        float *tmp_img1 = (*img1)(x1, y1);
        float *tmp_lap0 = (*lap0)(x0, y0);
        float *tmp_lap1 = (*lap1)(x1, y1);

        float E = 0.0f;
        float norm = 0.0f;

        for(int i = -halfPatchSize; i <= halfPatchSize; i++) {
            for(int j = -halfPatchSize; j <= halfPatchSize; j++) {
                float *tmp_img0_ij = (*img0)(x0 + j, y0 + i);
                float *tmp_img1_ij = (*img1)(x1 + j, y1 + i);
                float *tmp_lap0_ij = (*lap0)(x0 + j, y0 + i);
                float *tmp_lap1_ij = (*lap1)(x1 + j, y1 + i);

                float e0 = 0.0f;

                float dc0 = 0.0f;
                float dc1 = 0.0f;
                float dl0 = 0.0f;
                float dl1 = 0.0f;

                float tmp;
                for(int k = 0; k < img0->channels; k++) {
                    tmp = tmp_img0[k] - tmp_img0_ij[k];
                    dc0 += tmp * tmp;

                    tmp = tmp_img1[k] - tmp_img1_ij[k];
                    dc1 += tmp * tmp;

                    tmp = tmp_lap0[k] - tmp_lap0_ij[k];
                    dl0 += tmp * tmp;

                    tmp = tmp_lap1[k] - tmp_lap1_ij[k];
                    dl1 += tmp * tmp;

                    e0 += fabsf(tmp_img0_ij[k] - tmp_img1_ij[k]);
                }

                float w0 = expf( - (dc0 / gamma_c_2) - (dl0 / gamma_l_2));
                float w1 = expf( - (dc1 / gamma_c_2) - (dl1 / gamma_l_2));

                E += w0 * w1 * e0;
                norm += w0 * w1;
            }
        }

        if(norm > 0.0f) {
            E /= norm;
        }

        return E;
    }

    /**
     * @brief getSSD
     * @param x0
     * @param y0
     * @param x1
     * @param y1
     * @return
     */
    float getSSD(int x0, int y0,
                 int x1, int y1)
    {
        float val = 0.0f;

        for(int i = -halfPatchSize; i <= halfPatchSize; i++) {
            for(int j = -halfPatchSize; j <= halfPatchSize; j++) {
                float *tmpData0 = (*img0)(x0 + j, y0 + i);
                float *tmpData1 = (*img1)(x1 + j, y1 + i);
                
                for(int k = 0; k < img0->channels; k++) {
                    float tmp = tmpData0[k] - tmpData1[k];
                    val += tmp * tmp;
                }
            }
        }

        return val;
    }

    /**
     * @brief getSSD
     * @param x0
     * @param y0
     * @param a0
     * @param s0
     * @param x1
     * @param y1
     * @param threshold
     * @return
     */
    float getSSD(int x0, int y0, float a0, float s0,
                    int x1, int y1,
                    float threshold = FLT_MAX) {

        float cosAngle = cosf(a0);
        float sinAngle = sinf(a0);

        float val = 0.0f;
        float col[128];
        float x0f, y0f, tmp;

        float xf = float(x0);
        float yf = float(y0);

        for(int i = 0; i < patchSize; i++) {
            float val = s0 * float(i - halfPatchSize);
            VALUES_COS[i] = val * cosAngle;
            VALUES_SIN[i] = val * sinAngle;
        }

        for(int i = 0; i < patchSize; i++) {
            float tmpix = VALUES_SIN[i];//float(i-halfPatchSize)*sinAngle;
            float tmpiy = VALUES_COS[i];//float(i-halfPatchSize)*cosAngle;

            int tmpInd = CLAMP((y1 + i - halfPatchSize), img1->height) * img1->width;

            for(int j = 0; j < patchSize; j++) {
                x0f = xf + (VALUES_COS[j] - tmpix);
                y0f = yf + (VALUES_SIN[j] + tmpiy);

                x0f = CLAMPi(x0f, 0.0f, img0->width1f);
                y0f = CLAMPi(y0f, 0.0f, img0->height1f);

                isb.SampleImageUC(img0, x0f, y0f, col);

                int ind = (tmpInd + CLAMP((x1 + j - halfPatchSize),
                                            img1->width)) * img1->channels;

                for(int k = 0; k < img1->channels; k++) {
                    tmp = img1->data[ind] - col[k];
                    val += tmp * tmp;
                    ind++;
                }

                if(val > threshold) {
                    return val;
                }
            }
        }

        return val;
    }

    /**
     * @brief getSSD
     * @param td0
     * @param td1
     * @param threshold
     * @return
     */
    float getSSD(TransformData * td0, TransformData * td1,
                    float threshold = FLT_MAX) {
        float cosAngle = cosf(td0->angle);
        float sinAngle = sinf(td0->angle);

        float val = 0.0f;
        float col[128];
        float x0f, y0f, tmp;

        float xf = float(td0->x);
        float yf = float(td0->y);

        for(int i = 0; i < patchSize; i++) {
            float val = float(i) * td0->scale;
            VALUES_COS[i] = val * cosAngle;
            VALUES_SIN[i] = val * sinAngle;
        }

        for(int i = 0; i < patchSize; i++) {
            float tmpix = VALUES_SIN[i];//float(i-halfPatchSize)*sinAngle;
            float tmpiy = VALUES_COS[i];//float(i-halfPatchSize)*cosAngle;

            int tmpInd = CLAMP((td1->y + i - halfPatchSize), img1->height) * img1->width;

            for(int j = 0; j < patchSize; j++) {
                x0f = xf + (VALUES_COS[j] - tmpix);
                y0f = yf + (VALUES_SIN[j] + tmpiy);

                x0f = CLAMPi(x0f, 0.0f, img0->width1f);
                y0f = CLAMPi(y0f, 0.0f, img0->height1f);

                isb.SampleImageUC(img0, x0f, y0f, col);

                int ind = (tmpInd + CLAMP((td1->x + j - halfPatchSize),
                                            img1->width)) * img1->channels;

                for(int k = 0; k < img1->channels; k++) {
                    //apply gain+bias
                    float col_k = CLAMPi((col[k] * td0->gain + td0->bias), 0.0f, 1.0f);
                    tmp = img1->data[ind] - col_k;
                    val += tmp * tmp;
                    ind++;
                }

                if(val > threshold) {
                    return val;
                }
            }
        }

        return val;
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_PATCH_COMP_HPP */

