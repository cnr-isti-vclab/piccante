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

#ifndef PIC_FILTERING_FILTER_GUIDED_HPP
#define PIC_FILTERING_FILTER_GUIDED_HPP

#include "filtering/filter.hpp"

#include "util/math.hpp"

namespace pic {

/**
 * @brief The FilterGuided class
 */
class FilterGuided: public Filter
{
protected:

    int		radius;
    float	e_regularization, nPixels;

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
     * @brief FilterGuided
     */
    FilterGuided()
    {
        Update(3, 0.1f);
    }

    /**
     * @brief FilterGuided
     * @param radius
     * @param e_regularization
     */
    FilterGuided(int radius, float e_regularization)
    {
        Update(radius, e_regularization);
    }

    /**
     * @brief Update
     * @param radius
     * @param e_regularization
     */
    void Update(int radius, float e_regularization);

    /**
     * @brief Execute
     * @param imgIn
     * @param guide
     * @param imgOut
     * @param radius
     * @param e_regularization
     * @return
     */
    static Image *Execute(Image *imgIn, Image *guide, Image *imgOut,
                             int radius, float e_regularization)
    {
        FilterGuided filter(radius, e_regularization);
        return filter.ProcessP(Double(imgIn, guide), imgOut);
    }
};

void FilterGuided::Update(int radius, float e_regularization)
{
    this->radius = radius;
    this->e_regularization = e_regularization;
    nPixels = float(radius * radius * 4);
}

void FilterGuided::Process1Channel(Image *I, Image *p, Image *q,
                                   BBox *box)
{
    float I_mean, I_var;
    float *p_mean = new float [p->channels];

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float *tmpQ = (*q)(i, j);
            float *tmpI   = (*I)(i, j);

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
                tmpQ[c] = a * tmpI[0] + b;
            }
        }
    }

    delete[] p_mean;
}

PIC_INLINE void FilterGuided::Process3Channel(Image *I, Image *p,
        Image *q, BBox *box)
{
    int channels = p->channels;

    float *buf = new float [channels * 4];

    float *I_mean	= &buf[0];
    float *p_mean	= &buf[channels    ];
    float *a		= &buf[channels * 2];
    float *tmp_A	= &buf[channels * 3];

    Matrix3x3 cov, inv;

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float *tmpQ = (*q)(i, j);
            float *tmpI = (*I)(i, j);

            BBox tmpBox(i - radius, i + radius, j - radius, j + radius);

            I->getMeanVal(&tmpBox, I_mean);
            I->getCovMtxVal(I_mean, &tmpBox, cov.data);

            //regularization
            cov.Add(e_regularization);
            //invert matrix
            cov.Inverse(&inv);

            p->getMeanVal(&tmpBox, p_mean);

            for(int c = 0; c < channels; c++) {

                for(int n = 0; n < channels; n++) {
                    tmp_A[n] = 0.0f;
                }

                for(int k = -radius; k < radius; k++) {
                    for(int l = -radius; l < radius; l++) {
                        float *I_i = (*I)(i + l, j + k);
                        float *p_i = (*p)(i + l, j + k);

                        for(int n = 0; n < channels; n++) {
                            tmp_A[n] += I_i[n] * p_i[c] - I_mean[n] * p_mean[c];
                        }
                    }
                }

                for(int n = 0; n < channels; n++) {
                    tmp_A[n] /= nPixels;
                }

                //multiply for inverted matrix
                inv.Mul(tmp_A, a);

                float a_dot_I_mean = 0.0f;

                for(int n = 0; n < channels; n++) {
                    a_dot_I_mean += a[n] * I_mean[n];
                }

                float b = p_mean[c] - a_dot_I_mean;

                float a_dot_I = 0.0f;

                for(int n = 0; n < channels; n++) {
                    a_dot_I += a[n] * tmpI[n];
                }

                tmpQ[c] = a_dot_I + b;
            }
        }
    }

    delete[] buf;
}

PIC_INLINE void FilterGuided::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *I, *p;

    if(src.size() == 2) {
        p = src[0];

        if(src[1] != NULL) {
            I = src[1];
        } else {
            I = src[0];
        }
    } else {
        I = src[0];
        p = src[0];
    }

    if(I->channels == 1) {
        Process1Channel(I, p, dst, box);
    }

    if(I->channels == 3) {
        Process3Channel(I, p, dst, box);
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GUIDED_HPP */

