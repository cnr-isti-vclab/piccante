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

#ifndef PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP
#define PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP

#include "filtering/filter.hpp"
#include "util/precomputed_gaussian.hpp"

namespace pic {

class FilterGaussian1D: public Filter
{
protected:
    float				sigma;
    PrecomputedGaussian *pg;
    int					dirs[3];

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

public:
    //Basic constructor
    FilterGaussian1D();
    ~FilterGaussian1D();

    //Standard constructor
    FilterGaussian1D(float sigma, int direction);

    //Reuse constructor
    FilterGaussian1D(PrecomputedGaussian *pg);

    //Change data for this pass
    void ChangePass(int pass, int tPass);

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, float sigma,
                             int direction)
    {
        FilterGaussian1D filter(sigma, direction);
        return filter.ProcessP(Single(imgIn), imgOut);
    }
};

//Basic constructor
PIC_INLINE FilterGaussian1D::FilterGaussian1D()
{
    pg = NULL;
}

//Basic constructor
PIC_INLINE FilterGaussian1D::FilterGaussian1D(float sigma, int direction = 1)
{
    this->sigma = sigma;
    dirs[ direction      % 3] = 1;
    dirs[(direction + 1) % 3] = 0;
    dirs[(direction + 2) % 3] = 0;
    pg = new PrecomputedGaussian(sigma);
}

//Reuse constructor
PIC_INLINE FilterGaussian1D::FilterGaussian1D(PrecomputedGaussian *pg)
{
    if(pg == NULL) {
        printf("Error no precomputed gaussian values.\n");
        return;
    }

    dirs[0] = 1;
    dirs[1] = 0;
    dirs[2] = 0;

    this->pg = pg;
}

PIC_INLINE FilterGaussian1D::~FilterGaussian1D()
{
    if(pg != NULL) {
        delete pg;
        pg = NULL;
    }
}

//Change data for this pass
PIC_INLINE void FilterGaussian1D::ChangePass(int pass, int tPass)
{
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

    dirs[pass % tMod] = 1;

    for(int i = 1; i < tMod; i++) {
        dirs[(pass + i) % tMod] = 0;
    }

//	printf("%d %d %d\n",dirs[0],dirs[1],dirs[2]);
}

//Process in a box
PIC_INLINE void FilterGaussian1D::ProcessBBox(ImageRAW *dst, ImageRAWVec src,
        BBox *box)
{
    int channels = dst->channels;

    ImageRAW *source = src[0];

    for(int m = box->z0; m < box->z1; m++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                //1D Filtering
                float *tmpDst = (*dst)(i, j, m);

                for(int l = 0; l < channels; l++) {
                    tmpDst[l] = 0.0f;
                }

                for(int k = 0; k < pg->kernelSize; k++) {
                    int tmpCoord = k - pg->halfKernelSize;
                    //Address cj
                    int cj = j + tmpCoord * dirs[0];
                    //Address ci
                    int ci = i + tmpCoord * dirs[1];
                    //Address cm
                    int cm = m + tmpCoord * dirs[2];

                    float *tmpSource = (*source)(ci, cj, cm);

                    for(int l = 0; l < channels; l++) {
                        tmpDst[l] += tmpSource[l] * pg->coeff[k];
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP */

