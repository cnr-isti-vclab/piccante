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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DF_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DF_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterBilateral2DF: public Filter
{
protected:
    float sigma_s, sigma_r;

    PrecomputedGaussian *pg;

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    //Basic constructors
    FilterBilateral2DF();
    //Init constructors
    FilterBilateral2DF(float sigma_s, float sigma_r);

    std::string Signature()
    {
        return GenBilString("F", sigma_s, sigma_r);
    }

    //Filtering
    static Image *Execute(Image *imgIn, Image *imgOut,
                             float sigma_s, float sigma_r)
    {
        //Filtering
        FilterBilateral2DF filter(sigma_s, sigma_r);
        long t0 = timeGetTime();
        Image *out = filter.ProcessP(Single(imgIn), imgOut);
        long t1 = timeGetTime();
        printf("Full Bilateral Filter time: %ld\n", t1 - t0);
        return out;
    }

    //Filtering
    static Image *Execute(std::string nameIn,
                             std::string nameOut,
                             float sigma_s, float sigma_r)
    {
        //Load the image
        Image imgIn(nameIn);

        //Filter
        Image *imgOut = FilterBilateral2DF::Execute(&imgIn, NULL, sigma_s, sigma_r);

        //Write image out
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Basic constructor
FilterBilateral2DF::FilterBilateral2DF()
{
    pg = NULL;
}

//Init constructors
FilterBilateral2DF::FilterBilateral2DF(float sigma_s, float sigma_r)
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    //Precomputation of the Gaussian filter
    pg = new PrecomputedGaussian(sigma_s);
}

//Process in a box
void FilterBilateral2DF::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int channels = dst->channels;

    //Filtering
    Image *edge, *base;

    if(src.size() == 2) {
        //Joint/Cross Bilateral Filtering
        base = src[0];
        edge = src[1];
    } else {
        base = src[0];
        edge = src[0];
    }

    float sigma_r2 = (2.0f * sigma_r * sigma_r);

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            //Convolution kernel
            float *tmpDst = (*dst)(i, j);
            float sum = 0.0f;

            float *tmpEdge = (*edge)(i, j);

            for(int m = 0; m < channels; m++) {
                tmpDst[m]  = 0.0;
            }

            for(int k = 0; k < pg->kernelSize; k++) {
                int cj = j + k - pg->halfKernelSize;

                for(int l = 0; l < pg->kernelSize; l++) {
                    //Spatial filtering
                    float Gauss1 = pg->coeff[k] * pg->coeff[l];

                    //Address
                    int ci = i + l - pg->halfKernelSize;

                    //Range filtering
                    float tmp = 0.0;
                    float *tmpEdge2 = (*edge)(ci, cj);

                    for(int m = 0; m < channels; m++) {
                        float tmp3 = tmpEdge2[m] - tmpEdge[m];
                        tmp += tmp3 * tmp3;
                    }

                    float Gauss2 = expf(-tmp / sigma_r2);

                    //Weight
                    float weight = Gauss1 * Gauss2;

                    //Filtering
                    float *tmpBase = (*base)(ci, cj);

                    for(int m = 0; m < channels; m++) {
                        tmpDst[m] += tmpBase[m] * weight;
                    }

                    sum += weight;
                }
            }

            //Normalization
            bool sumTest = sum > 0.0f;

            float *tmpBase = (*base)(i, j);

            for(int m = 0; m < channels; m++) {
                tmpDst[m] = sumTest ? tmpDst[m] / sum : tmpBase[m];
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DF_HPP */

