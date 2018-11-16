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

#ifndef PIC_FILTERING_FILTER_GUIDED_A_B_HPP
#define PIC_FILTERING_FILTER_GUIDED_A_B_HPP

#include "../filtering/filter.hpp"

#include "../util/array.hpp"

#include "../util/matrix_3_x_3.hpp"

#include "../util/math.hpp"

namespace pic {

/**
 * @brief The FilterGuidedAB class
 */
class FilterGuidedAB: public Filter
{
protected:

    int radius;
    float e_regularization, nPixels;

    /**
     * @brief Process1Channel
     * @param I
     * @param p
     * @param q
     * @param box
     */
    void Process1Channel(Image *I, Image *p, Image *q, BBox *box);

    /**
     * @brief Process3Channel
     * @param I
     * @param p
     * @param q
     * @param box
     */
    void Process3Channel(Image *I, Image *p, Image *q, BBox *box);

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterGuidedAB
     */
    FilterGuidedAB() : Filter()
    {
        update(8, 0.01f);
    }

    /**
     * @brief FilterGuidedAB
     * @param radius
     * @param e_regularization
     */
    FilterGuidedAB(int radius, float e_regularization) : Filter()
    {
        update(radius, e_regularization);
    }

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
        width = imgIn[0]->width;
        height = imgIn[0]->height;
        channels = getp(imgIn)->channels * (getI(imgIn)->channels + 1);
        frames = imgIn[0]->frames;
    }

    /**
     * @brief getI
     * @param imgIn
     * @return
     */
    Image *getI(ImageVec &imgIn)
    {
        auto n = imgIn.size();
        if(n == 1) {
            return imgIn[0];
        } else {
            if(n > 1) {
                return imgIn[1];
            } else {
                return NULL;
            }
        }
    }

    /**
     * @brief getp
     * @param imgIn
     * @return
     */
    Image *getp(ImageVec &imgIn)
    {
        auto n = imgIn.size();
        if(n == 1) {
            return imgIn[0];
        } else {
            if(n > 1) {
                return imgIn[0];
            } else {
                return NULL;
            }
        }
    }

    /**
     * @brief update
     * @param radius
     * @param e_regularization
     */
    void update(int radius, float e_regularization);

    /**
     * @brief execute
     * @param imgIn
     * @param guide
     * @param imgOut
     * @param radius
     * @param e_regularization
     * @return
     */
    static Image *execute(Image *imgIn, Image *guide, Image *imgOut,
                             int radius, float e_regularization)
    {
        FilterGuidedAB filter(radius, e_regularization);
        return filter.Process(Double(imgIn, guide), imgOut);
    }
};

PIC_INLINE void FilterGuidedAB::update(int radius, float e_regularization)
{
    this->radius = radius;
    this->e_regularization = e_regularization;
    nPixels = float(radius * radius * 4);
}

PIC_INLINE void FilterGuidedAB::Process1Channel(Image *I, Image *p, Image *q,
                                   BBox *box)
{
    float I_mean, I_var;
    float *p_mean = new float [p->channels];

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float *tmpQ = (*q)(i, j);

            BBox tmpBox(i - radius, i + radius, j - radius, j + radius);

            I->getMeanVal(&tmpBox, &I_mean);
            I->getVarianceVal(&I_mean, &tmpBox, &I_var);

            p->getMeanVal(&tmpBox, p_mean);

            for(int c = 0; c < p->channels; c++) {
                float I_mean_p_mean = I_mean * p_mean[c];
                float a = 0.0f;

                for(int k = -radius; k < radius; k++) {
                    for(int l = -radius; l < radius; l++) {
                        float *I_i = (*I)(i + l, j + k);
                        float *p_i = (*p)(i + l, j + k);
                        a += I_i[0] * p_i[c] - I_mean_p_mean;
                    }
                }

                a /= (nPixels * (I_var + e_regularization));
                float b = p_mean[c] - a * I_mean;

                int index = c << 1;
                tmpQ[index] = a;
                tmpQ[index + 1] = b;
            }
        }
    }

    delete[] p_mean;
}

PIC_INLINE void FilterGuidedAB::Process3Channel(Image *I, Image *p,
        Image *q, BBox *box)
{
    float *I_mean = new float[I->channels];
    float *p_mean = new float[p->channels];

    float *a = new float[I->channels];
    float *tmp_A = new float[I->channels];

    Matrix3x3 cov, inv;

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float *tmpQ = (*q)(i, j);

            BBox tmpBox(i - radius, i + radius, j - radius, j + radius);

            I->getMeanVal(&tmpBox, I_mean);
            I->getCovMtxVal(I_mean, &tmpBox, cov.data);

            //regularization
            cov.add(e_regularization);
            //invert matrix
            cov.inverse(&inv);

            p->getMeanVal(&tmpBox, p_mean);

            int index = 0;
            for(int c = 0; c < p->channels; c++) {

                Array<float>::assign(0.0f, tmp_A, I->channels);

                for(int k = -radius; k < radius; k++) {
                    for(int l = -radius; l < radius; l++) {
                        float *I_i = (*I)(i + l, j + k);
                        float *p_i = (*p)(i + l, j + k);

                        for(int n = 0; n < I->channels; n++) {
                            tmp_A[n] += I_i[n] * p_i[c] - I_mean[n] * p_mean[c];
                        }
                    }
                }

                Array<float>::div(tmp_A, I->channels, nPixels);

                //multiply for inverted matrix
                a = inv.mul(tmp_A, a);

                float a_dot_I_mean = Array<float>::dot(a, I_mean, I->channels);
                //float a_dot_I = Array<float>::dot(a, tmpI, channels);

                for(int n = 0; n < I->channels; n++) {
                    tmpQ[index] = a[n];
                    index++;
                }

                //b
                tmpQ[index] = p_mean[c] - a_dot_I_mean;
                index++;
            }
        }
    }
}

PIC_INLINE void FilterGuidedAB::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *I = getI(src);
    Image *p = getp(src);

    if(I->channels == 1) {
        Process1Channel(I, p, dst, box);
    }

    if(I->channels == 3) {
        Process3Channel(I, p, dst, box);
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GUIDED_A_B_HPP */

