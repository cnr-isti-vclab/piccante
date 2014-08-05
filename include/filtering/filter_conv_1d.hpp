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

#ifndef PIC_FILTERING_FILTER_CONV_1D_HPP
#define PIC_FILTERING_FILTER_CONV_1D_HPP

#include "filtering/filter.hpp"
#include "util/precomputed_gaussian.hpp"

namespace pic {

class FilterConv1D: public Filter
{
protected:
    int					dirs[3];
    float				*data;
    int					n;

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

public:
    //Basic constructor
    FilterConv1D(float *data, int n);
    ~FilterConv1D();

    //Change data for this pass
    void ChangePass(int pass, int tPass);
    void ChangePass(int x, int y, int z);

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, float *data, int n,
                             bool XorY = true)
    {
        FilterConv1D filter(data, n);

        if(XorY) {
            filter.ChangePass(1, 0, 0);
        } else {
            filter.ChangePass(0, 1, 0);
        }

        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /** */
    static float *getFilterMean(int kernelSize) 
    {
        float *kernel = new float[kernelSize];

        float val = 1.0f/float(kernelSize);

        for(int i=0;i<kernelSize;i++) {
            kernel[i] = val;
        }

        return kernel;
    }
};

//Basic constructor
FilterConv1D::FilterConv1D(float *data, int n)
{
    if(data == NULL || n < 0) {
        return;
    }

    this->data = data;
    this->n = n;

    dirs[0] = 1;
    dirs[1] = 0;
    dirs[2] = 0;
}

FilterConv1D::~FilterConv1D()
{
    data = NULL;
    n = -1;
}

//Change data for this pass
void FilterConv1D::ChangePass(int pass, int tPass)
{

    int tMod;

    if(tPass > 1) {
        tMod = 3;
    } else {
        if(tPass == 1) {
            tMod = 2;
        } else {
            printf("ERROR: FilterConv1D::ChangePass");
            return;
        }
    }

    dirs[pass % tMod] = 1;

    for(int i = 1; i < tMod; i++) {
        dirs[(pass + i) % tMod] = 0;
    }

//	printf("%d %d %d\n",dirs[0],dirs[1],dirs[2]);
}

void FilterConv1D::ChangePass(int x, int y, int z)
{
    dirs[0] = y;
    dirs[1] = x;
    dirs[2] = z;
}

//Process in a box
void FilterConv1D::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
{
    int channels = dst->channels;

    ImageRAW *source = src[0];

    int halfKernelSize = n >> 1;

    for(int m = box->z0; m < box->z1; m++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *tmpDst = (*dst)(i, j, m);

                for(int l = 0; l < channels; l++) {
                    tmpDst[l] = 0.0f;
                }

                for(int k = 0; k < n; k++) { //1D Filtering
                    int tmpCoord = k - halfKernelSize;

                    //Address cj
                    int cj = j + tmpCoord * dirs[0];
                    //Address ci
                    int ci = i + tmpCoord * dirs[1];
                    //Address cm
                    int cm = m + tmpCoord * dirs[2];

                    float *tmpSource = (*source)(ci, cj, cm);

                    for(int l = 0; l < channels; l++) {
                        tmpDst[l] += tmpSource[l] * data[k];
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_1D_HPP */

