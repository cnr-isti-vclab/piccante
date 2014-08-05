/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_FILTERING_FILTER_BILATERAL_1D_HPP
#define PIC_FILTERING_FILTER_BILATERAL_1D_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterBilateral1D: public Filter
{
protected:
    PrecomputedGaussian *pg;
    int					dirs[3];

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

public:
    float sigma_s, sigma_r;

    //Standard constructor
    FilterBilateral1D(float sigma_s, float sigma_r);

    void Update(float sigma_s, float sigma_r);

    std::string Signature()
    {
        return GenBilString("1D", sigma_s, sigma_r);
    }

    //Change data for this pass
    void ChangePass(int pass, int tPass);
};

//Basic constructor
FilterBilateral1D::FilterBilateral1D(float sigma_s, float sigma_r)
{
    pg = NULL;
    Update(sigma_s, sigma_r);
}

///Update:
void FilterBilateral1D::Update(float sigma_s, float sigma_r)
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    //Precomputation of the Gaussian filter
    dirs[0] = dirs[1] = dirs[2] = 0;

    if(pg!=NULL)
        delete pg;

    pg = new PrecomputedGaussian(sigma_s);
}

//Change data for this pass
void FilterBilateral1D::ChangePass(int pass, int tPass)
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

//Process in a box
void FilterBilateral1D::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
{
    int channels = dst->channels;

    double inv_sigma_r2 = 1.0 / (2.0 * sigma_r * sigma_r);

    //Filtering
    ImageRAW *edge, *base;

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

                for(int l = 0; l < channels; l++) {
                    tmpDst[l] = 0.0;
                }

                double sum = 0.0;

                for(int k = 0; k < pg->kernelSize; k++) {
                    //Spatial filtering
                    float gauss1 = pg->coeff[k];

                    int tmpCoord = k - pg->halfKernelSize;

                    //Address cj
                    int cj = j + tmpCoord * dirs[0];
                    //Address ci
                    int ci = i + tmpCoord * dirs[1];
                    //Address cm
                    int cm = m + tmpCoord * dirs[2];

                    //Range filtering
                    float *curEdge = (*edge)(ci, cj, cm); 
                    double tmp = 0.0;
                    for(int l = 0; l < channels; l++) {
                        double diff = curEdge[l] - tmpEdge[l];
                        tmp += diff * diff;
                    }

                    double gauss2 = exp(-tmp * inv_sigma_r2);

                    //Weight
                    tmp = gauss1 * gauss2;

                    //Filtering
                    float *curBase = (*base)(ci, cj, cm); 
                    for(int l = 0; l < channels; l++) {
                        tmpDst[l] += curBase[l] * tmp;
                    }

                    sum += tmp;
                }

                //Normalization
                bool sumTest = sum > 0.0;

                for(int l = 0; l < channels; l++) {
                    tmpDst[l] = sumTest ? tmpDst[l] / sum : 0.0f;
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_1D_HPP */

