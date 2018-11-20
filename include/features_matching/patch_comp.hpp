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
    float *VALUES_COS, *VALUES_SIN;
    ImageSamplerBilinear isb;

    Image *img0, *img1, *img0_g, *img1_g;

    //patchsize
    int patchSize, halfPatchSize;
    float patchSize_sq;

    //stereo
    float  lambda, alpha;

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

        initStereo(img0, img1, NULL, NULL, patchSize, 0.05f, 0.05f);
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
              int patchSize, float lambda, float alpha)
    {
        setNULL();
        initStereo(img0, img1, img0_g, img1_g, patchSize, lambda, alpha);
    }

    ~PatchComp()
    {
        if(VALUES_COS != NULL) {
            delete[] VALUES_COS;
        }

        if(VALUES_SIN != NULL) {
            delete[] VALUES_SIN;
        }

        setNULL();
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

        lambda = -1.0f;
        alpha = -1.0f;

        VALUES_COS = NULL;
        VALUES_SIN = NULL;
        patchSize = -1;
        halfPatchSize = -1;
    }

    /**
     * @brief initStereo
     * @param img0
     * @param img1
     * @param patchSize
     */
    void initStereo(Image *img0,   Image *img1,
                    Image *img0_g, Image *img1_g,
                    int patchSize, float lambda, float alpha)
    {
        if(patchSize < 1) {
            return;
        }

        this->img0 = img0;
        this->img1 = img1;
        this->img0_g = img0_g;
        this->img1_g = img1_g;

        if(lambda < 0.0f) {
            lambda = 0.2f;
        }

        this->lambda = lambda;

        if(alpha < 0.0f || alpha > 1.0f) {
            alpha = 0.05f;
        }

        this->alpha = alpha;

        this->patchSize_sq = float(patchSize * patchSize);
        this->halfPatchSize = patchSize >> 1;
        this->patchSize = (halfPatchSize << 1) + 1;

        VALUES_SIN = new float[this->patchSize];
        VALUES_COS = new float[this->patchSize];
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
     * @brief improveStereo
     * @param x0
     * @param y0
     * @param x1
     * @param prevL
     * @param prevU
     * @param xb
     * @param db
     */
    void improveStereo(int x0, int y0, int x1,
                       float *prevL,
                       float *prevU,
                       float maxDisparity,
                       int &xb, float &db)
    {
        float dist = getSSDSmooth(x0, y0, x1, y0) / patchSize_sq;

        //regularization
        float reg = 0.0f;//float(x1 - x0);

        if(prevL[1] >= 0.0f) {
            float deltaL = fabsf(prevL[0] - x1);
            reg += deltaL / maxDisparity;
        }

        if(prevU[1] >= 0.0f) {
            float deltaU = fabsf(prevU[0] - x1);
            reg += deltaU / maxDisparity;
        }

        dist += lambda * reg;

        if(dist < db) {
            xb = x1;
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
    void improve(TransformData *td0, TransformData *td1, TransformData *tb)
    {
        float dist = getSSD(td0, td1);

        if(dist < tb->quality) {
            tb->x       = td0->x;
            tb->y       = td0->y;
            tb->angle   = td0->angle;
            tb->scale   = td0->scale;
            tb->gain    = td0->gain;
            tb->bias    = td0->bias;
            tb->quality = dist;
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
                float *tmp_img0_ij = (*img0)(x0 + j, y0 + i);
                float *tmp_img1_ij = (*img1)(x1 + j, y1 + i);

                float *tmp_img0_g_ij = (*img0_g)(x0 + j, y0 + i);
                float *tmp_img1_g_ij = (*img1_g)(x1 + j, y1 + i);


                //color term
                float err_col = 0.0f;
                for(int k = 0; k < img0->channels; k++) {
                    auto tmp = tmp_img1_ij[k] - tmp_img0_ij[k];
                    err_col += tmp * tmp;
                }

                err_col = sqrtf(err_col);

                //gradient term
                float err_grad = 0.0f;
                for(int k = 0; k < 2; k++) {
                    auto tmp = tmp_img0_g_ij[k] - tmp_img1_g_ij[k];
                    err_grad += tmp * tmp;
                }
                err_grad = sqrtf(err_grad);

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
    float getSSD(int x0, int y0,
                 int x1, int y1)
    {
        float ret = 0.0f;

        for(int i = -halfPatchSize; i <= halfPatchSize; i++) {
            for(int j = -halfPatchSize; j <= halfPatchSize; j++) {
                float *d0 = (*img0)(x0 + j, y0 + i);
                float *d1 = (*img1)(x1 + j, y1 + i);

                ret += Array<float>::dot(d0, d1, img0->channels);
            }
        }

        return ret;
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

        float ret = 0.0f;
        float col0[128];
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

            for(int j = 0; j < patchSize; j++) {
                x0f = xf + (VALUES_COS[j] - tmpix);
                y0f = yf + (VALUES_SIN[j] + tmpiy);

                x0f = CLAMPi(x0f, 0.0f, img0->width1f);
                y0f = CLAMPi(y0f, 0.0f, img0->height1f);

                isb.SampleImageUC(img0, x0f, y0f, col0);

                float *col1 = (*img1)(x1 + j - halfPatchSize, y1 + i - halfPatchSize);

                ret += Array<float>::dot(col0, col1, img0->channels);

                if(ret > threshold) {
                    return ret;
                }
            }
        }

        return ret;
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

        float ret = 0.0f;
        float x0f, y0f, tmp;

        float xf = float(td0->x);
        float yf = float(td0->y);

        for(int i = 0; i < patchSize; i++) {
            float val = float(i - halfPatchSize) * td0->scale;
            VALUES_COS[i] = val * cosAngle;
            VALUES_SIN[i] = val * sinAngle;
        }

        //compute statistics
        float col0[64];
        float col0_mu[64];
        float col0_s[64];
        float col1_mu[64];
        float col1_s[64];

        for(int i = 0; i < img0->channels; i++) {
            col0_mu[i] = 0.0f;
            col1_mu[i] = 0.0f;

            col0_s[i] = 0.0f;
            col1_s[i] = 0.0f;
        }

        for(int i = 0; i < patchSize; i++) {
            float tmpix = VALUES_SIN[i];//float(i-halfPatchSize)*sinAngle;
            float tmpiy = VALUES_COS[i];//float(i-halfPatchSize)*cosAngle;

            for(int j = 0; j < patchSize; j++) {
                x0f = xf + (VALUES_COS[j] - tmpix);
                y0f = yf + (VALUES_SIN[j] + tmpiy);

                x0f = CLAMPi(x0f, 0.0f, img0->width1f);
                y0f = CLAMPi(y0f, 0.0f, img0->height1f);

                isb.SampleImageUC(img0, x0f, y0f, col0);
                float *col1 = (*img1)(td1->x + j - halfPatchSize, td1->y + i - halfPatchSize);

                for(int k = 0; k < img1->channels; k++) {
                    //apply gain+bias
                    col0_mu[k] += col0[k];
                    col1_mu[k] += col1[k];

                    col0_s[k] += col0[k] * col0[k];
                    col1_s[k] += col1[k] * col1[k];
                }
            }
        }

        float n_sqf = float(patchSize * patchSize);
        for(int i = 0; i < img0->channels; i++) {
            col0_mu[i] /= n_sqf;
            col1_mu[i] /= n_sqf;

            col0_s[i] /= n_sqf;
            col1_s[i] /= n_sqf;

            col0_s[i] -= (col0_mu[i] * col0_mu[i]);
            col1_s[i] -= (col1_mu[i] * col1_mu[i]);
        }

        //perform SSD
        for(int i = 0; i < patchSize; i++) {
            float tmpix = VALUES_SIN[i];//float(i-halfPatchSize)*sinAngle;
            float tmpiy = VALUES_COS[i];//float(i-halfPatchSize)*cosAngle;

            for(int j = 0; j < patchSize; j++) {
                x0f = xf + (VALUES_COS[j] - tmpix);
                y0f = yf + (VALUES_SIN[j] + tmpiy);

                x0f = CLAMPi(x0f, 0.0f, img0->width1f);
                y0f = CLAMPi(y0f, 0.0f, img0->height1f);

                isb.SampleImageUC(img0, x0f, y0f, col0);

                float *col1 = (*img1)(td1->x + j - halfPatchSize, td1->y + i - halfPatchSize);

                for(int k = 0; k < img1->channels; k++) {
                    //apply gain + bias
                    float gain = col0_s[k] > 0.0f ? (col1_s[k] / col0_s[k]) : 1.0f;
                    float bias = col0_mu[k] - gain * col1_mu[k];
                    tmp = col1[k] - (col0[k] * gain + bias);
                    ret += tmp * tmp;
                }

                if(ret > threshold) {
                    return ret;
                }
            }
        }

        return ret;
    }

    /**
     * @brief distance computes the distance between two transformations.
     * @param x0 is the x coordinate of point p0 = (x0, y0)
     * @param y0 is the y coordinate of point p0 = (x0, y0)
     * @param d0 is the transformation of p0
     * @param x1 is the x coordinate of point p1 = (x1, y1)
     * @param y1 is the y coordinate of point p1 = (x1, y1)
     * @param d1 is the transformation of p1
     * @param width is the width of the target image
     * @param height is the height of the target image
     * @return
     */
    static float distance(float x0, float y0, TransformData *d0,
                          float x1, float y1, TransformData *d1,
                          float width, float height)
    {
        float tx, ty, dist, div;

        //float dx0 = float(d0->x) - x0) / width;
        //float dy0 = float(d0->y) - y0) / height;

        float dx1 = float(d1->x - x1) / width;
        float dy1 = float(d1->y - y1) / height;

        float x0d1 = x0 + dx1;
        float y0d1 = y0 + dy1;

        tx = d0->x - x0d1;
        ty = d0->y - y0d1;

        dist = (tx * tx + ty * ty);

        tx = d1->x - x0d1;
        ty = d1->y - y0d1;

        div = (tx * tx + ty * ty);

        if(div > 0.0f) {
            dist /= div;
        } else {
            dist = FLT_MAX;
        }

        return dist;
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_PATCH_COMP_HPP */

