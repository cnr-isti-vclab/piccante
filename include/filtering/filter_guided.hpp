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

#include "../filtering/filter.hpp"

#include "../util/array.hpp"

#include "../util/matrix_3_x_3.hpp"

#include "../filtering/filter_guided_a_b.hpp"

#include "../util/math.hpp"

namespace pic {

/**
 * @brief The FilterGuided class
 */
class FilterGuided: public Filter
{
protected:

    int radius;
    float e_regularization, nPixels;
    Image *img_a_b;

    FilterGuidedAB flt;

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
    FilterGuided() : Filter()
    {
        update(5, 0.01f);
    }

    /**
     * @brief FilterGuided
     * @param radius
     * @param e_regularization
     */
    FilterGuided(int radius, float e_regularization) : Filter()
    {
        update(radius, e_regularization);
    }

    /**
     * @brief update
     * @param radius
     * @param e_regularization
     */
    void update(int radius, float e_regularization);

    /**
     * @brief FilterGuided::Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut);

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
        FilterGuided filter(radius, e_regularization);
        return filter.Process(Double(imgIn, guide), imgOut);
    }
};

PIC_INLINE void FilterGuided::update(int radius, float e_regularization)
{
    img_a_b = NULL;

    this->radius = radius;
    this->e_regularization = e_regularization;
    nPixels = float(radius * radius * 4);

    flt.update(radius, e_regularization);
}

PIC_INLINE void FilterGuided::Process1Channel(Image *I, Image *p, Image *q,
                                   BBox *box)
{
    float *a_b_mean = new float[img_a_b->channels];

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float *tmpQ = (*q)(i, j);
            float *tmpI = (*I)(i, j);

            BBox tmpBox(i - radius, i + radius, j - radius, j + radius);
            img_a_b->getMeanVal(&tmpBox, a_b_mean);

            for(int c = 0; c < p->channels; c++) {
                int index = c << 1;
                float a = a_b_mean[index];
                float b = a_b_mean[index + 1];
                tmpQ[c] = a * tmpI[0] + b;
            }
        }
    }

    delete[] a_b_mean;
}

PIC_INLINE void FilterGuided::Process3Channel(Image *I, Image *p,
        Image *q, BBox *box)
{
    float *a_b_mean = new float[img_a_b->channels];

    int shift = I->channels + 1;

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float *tmpQ = (*q)(i, j);
            float *tmpI = (*I)(i, j);

            BBox tmpBox(i - radius, i + radius, j - radius, j + radius);
            img_a_b->getMeanVal(&tmpBox, a_b_mean);

            for(int c = 0; c < p->channels; c++) {

                int index = c * shift;
                float *a = &a_b_mean[index];
                float b = a_b_mean[index + I->channels];

                float a_dot_I = Array<float>::dot(a, tmpI, I->channels);

                tmpQ[c] = a_dot_I + b;
            }

        }
    }
    delete[] a_b_mean;
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

    if(I->channels == 3) {
        Process3Channel(I, p, dst, box);
    } else {
        Process1Channel(I, p, dst, box);
    }
}

PIC_INLINE Image *FilterGuided::Process(ImageVec imgIn, Image *imgOut)
{
    if(!checkInput(imgIn)) {
        return imgOut;
    }

    imgOut = setupAux(imgIn, imgOut);

    if(imgOut == NULL) {
        return imgOut;
    }

    img_a_b = flt.Process(imgIn, img_a_b);

    return ProcessP(imgIn, imgOut);
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GUIDED_HPP */

