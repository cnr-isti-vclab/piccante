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

#ifndef PIC_FILTERING_FILTER_BILATERAL_1D_HPP
#define PIC_FILTERING_FILTER_BILATERAL_1D_HPP

#include "../util/precomputed_gaussian.hpp"
#include "../util/std_util.hpp"
#include "../util/array.hpp"

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterBilateral1D class
 */
class FilterBilateral1D: public Filter
{
protected:
    PrecomputedGaussian *pg;
    int dirs[3];
    float sigma_r_sq_2;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    float sigma_s, sigma_r;

    /**
     * @brief FilterBilateral1D
     * @param sigma_s
     * @param sigma_r
     */
    FilterBilateral1D(float sigma_s, float sigma_r);

    /**
     * @brief update
     * @param sigma_s
     * @param sigma_r
     */
    void update(float sigma_s, float sigma_r);

    /**
     * @brief signature
     * @return
     */
    std::string signature()
    {
        return genBilString("1D", sigma_s, sigma_r);
    }

    /**
     * @brief changePass
     * @param pass
     * @param tPass
     */
    void changePass(int pass, int tPass);
};

PIC_INLINE FilterBilateral1D::FilterBilateral1D(float sigma_s, float sigma_r) : Filter()
{
    pg = NULL;
    update(sigma_s, sigma_r);
}

PIC_INLINE void FilterBilateral1D::update(float sigma_s, float sigma_r)
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s > 0.0f ? sigma_s : 1.0f;
    this->sigma_r = sigma_r > 0.0f ? sigma_r : 0.01f;
    this->sigma_r_sq_2 = this->sigma_r * this->sigma_r * 2.0f;

    //Precomputation of the Gaussian filter
    dirs[0] = dirs[1] = dirs[2] = 0;

    pg = delete_s(pg);

    pg = new PrecomputedGaussian(sigma_s);
}

PIC_INLINE void FilterBilateral1D::changePass(int pass, int tPass)
{
    /*	tPass++;
    	dirs[ pass%tPass] = 1;
    	for(int i=1;i<tPass;i++)
    		dirs[(pass+i)%tPass] = 0;
    */
    int tMod;

    if(tPass > 1) {
        tMod = 3;
    } else {
        if(tPass == 1) {
            tMod = 2;
        } else {
            printf("ERROR: FilterGaussian1D::ChangePass\n");
            return;
        }
    }

    dirs[ pass % tMod] = 1;

    for(int i = 1; i < tMod; i++) {
        dirs[(pass + i) % tMod] = 0;
    }

#ifdef PIC_DEBUG
    printf("%d %d %d\n", dirs[0], dirs[1], dirs[2]);
#endif
}

PIC_INLINE void FilterBilateral1D::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    Image *edge, *base;

    if(src.size() == 2) {
        //Joint/Cross Bilateral Filtering
        base = src[0];
        edge = src[1];
    } else {
        base = src[0];
        edge = src[0];
    }

    for(int m = box->z0; m < box->z1; m++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {

                float *tmpDst  = (*dst )(i, j, m);
                float *tmpEdge = (*edge)(i, j, m);

                Arrayf::assign(0.0f, tmpDst, dst->channels);

                float sum = 0.0f;

                for(int k = 0; k < pg->kernelSize; k++) {
                    //Spatial filtering
                    float weight = pg->coeff[k];

                    int tmpCoord = k - pg->halfKernelSize;

                    //Address cj
                    int cj = j + tmpCoord * dirs[0];
                    //Address ci
                    int ci = i + tmpCoord * dirs[1];
                    //Address cm
                    int cm = m + tmpCoord * dirs[2];


                    //Range filtering
                    float *curEdge = (*edge)(ci, cj, cm); 

                    float edgeDist = Arrayf::distanceSq(curEdge, tmpEdge, dst->channels);
                    edgeDist = expf(-edgeDist / sigma_r_sq_2);

                    //Weight
                    weight *= edgeDist;

                    //filter
                    float *curBase = (*base)(ci, cj, cm);
                    for(int l = 0; l < dst->channels; l++) {
                        tmpDst[l] += curBase[l] * weight;
                    }

                    sum += weight;
                }

                //Normalization
                if(sum > 0.0f) {
                    Arrayf::div(tmpDst, dst->channels, sum);
                } else {
                    float *base = (*edge)(i, j, m);
                    Arrayf::assign(base, dst->channels, tmpDst);
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_1D_HPP */

