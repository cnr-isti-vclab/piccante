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

#ifndef PIC_FILTERING_FILTER_DCT_1D_HPP
#define PIC_FILTERING_FILTER_DCT_1D_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterDCT1D class
 */
class FilterDCT1D: public Filter
{
protected:
    int     dirs[3];
    float   *coeff, sqr[2];
    int     nCoeff;
    bool    bForward;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterDCT1D
     * @param nCoeff
     * @param bForward
     */
    FilterDCT1D(int nCoeff, bool bForward);

    ~FilterDCT1D();

    /**
     * @brief setForward
     */
    void setForward()
    {
        this->bForward = true;

        if(coeff != NULL) {
            delete[] coeff;
            coeff = NULL;
        }

        coeff = createCoefficientsTransform(nCoeff);
    }

    /**
     * @brief setInverse
     */
    void setInverse()
    {
        this->bForward = false;

        if(coeff != NULL) {
            delete[] coeff;
            coeff = NULL;
        }

        coeff = createCoefficientsInverse(nCoeff);
    }

    /**
     * @brief createCoefficientsTransform
     * @param size
     * @return
     */
    static float *createCoefficientsTransform(int size)
    {
        if(size < 1) {
            return NULL;
        }

        float size2 = float(size * 2);
        float *ret = new float[size * size];
        int val;

        int ind = 0;

        for(int u = 0; u < size; u++) {
            for(int x = 0; x < size; x++) {
                val		 = u * (2 * x + 1);
                ret[ind] = cosf(C_PI * float(val) / size2);
                ind++;
            }
        }

        return ret;
    }

    /**
     * @brief createCoefficientsInverse
     * @param size
     * @return
     */
    static float *createCoefficientsInverse(int size)
    {
        if(size < 1) {
            return NULL;
        }

        float size2 = float(size * 2);
        float *ret = new float[size * size];
        int val;

        float 	sqr[2];
        sqr[0] = sqrtf(1.0f / float(size));
        sqr[1] = sqrtf(2.0f / float(size));

        int ind = 0;

        for(int x = 0; x < size; x++) {
            for(int u = 0; u < size; u++) {
                val		 = u * (2 * x + 1);
                ret[ind] = cosf(C_PI * float(val) / size2);

                if(u == 0) {
                    ret[ind] *= sqr[0];
                } else {
                    ret[ind] *= sqr[1];
                }

                ind++;
            }
        }

        return ret;
    }

    /**
     * @brief changePass
     * @param pass
     * @param tPass
     */
    void changePass(int pass, int tPass);

    /**
     * @brief changePass
     * @param x
     * @param y
     * @param z
     */
    void changePass(int x, int y, int z);
};

PIC_INLINE FilterDCT1D::FilterDCT1D(int nCoeff, bool bForward)
{
    this->coeff = NULL;
    this->nCoeff = nCoeff;

    if(bForward) {
        setForward();
    } else {
        setInverse();
    }

    sqr[0] = sqrtf(1.0f / float(nCoeff));
    sqr[1] = sqrtf(2.0f / float(nCoeff));

    dirs[0] = 1;
    dirs[1] = 0;
    dirs[2] = 0;
}

PIC_INLINE FilterDCT1D::~FilterDCT1D()
{
    if(coeff != NULL) {
        delete[] coeff;
    }
}

PIC_INLINE void FilterDCT1D::changePass(int pass, int tPass)
{
    int tMod;

    if(tPass > 1) {
        tMod = 3;
    } else {
        if(tPass == 1) {
            tMod = 2;
        } else {
            printf("ERROR: FilterDCT1D::changePass");
            return;
        }
    }

    dirs[pass % tMod] = 1;

    for(int i = 1; i < tMod; i++) {
        dirs[(pass + i) % tMod] = 0;
    }

}

PIC_INLINE void FilterDCT1D::changePass(int x, int y, int z)
{
    dirs[0] = y;
    dirs[1] = x;
    dirs[2] = z;
}

PIC_INLINE void FilterDCT1D::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int channels = dst->channels;

    Image *source = src[0];

    for(int m = box->z0; m < box->z1; m++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmpDst = (*dst)(i, j, m);

                for(int l = 0; l < channels; l++) {
                    tmpDst[l] = 0.0f;
                }

                int ind = (j * dirs[0] + i * dirs[1] + m * dirs[2]) % nCoeff;
                int ind2 = ind * nCoeff;

                for(int k = 0; k < nCoeff; k++) { //1D Filtering
                    int k2 = k - ind;
                    //Address cj
                    int cj = j + k2 * dirs[0];
                    //Address ci
                    int ci = i + k2 * dirs[1];
                    //Address cm
                    int cm = m + k2 * dirs[2];

                    float *tmpSource = (*source)(ci, cj, cm);

                    float tmpCoeff = coeff[ind2];

                    for(int l = 0; l < channels; l++) {
                        tmpDst[l] += tmpSource[l] * tmpCoeff;
                    }

                    ind2++;
                }

                if(bForward) {
                    int select = (ind == 0) ? 0 : 1;
                    for(int l = 0; l < channels; l++) {
                        tmpDst[l] *= sqr[select];
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DCT_1D_HPP */

