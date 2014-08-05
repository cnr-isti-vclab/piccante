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

#ifndef PIC_UTIL_PRECOMPUTED_DIFFERENCE_OF_GAUSSIANS_HPP
#define PIC_UTIL_PRECOMPUTED_DIFFERENCE_OF_GAUSSIANS_HPP

#include "util/precomputed_gaussian.hpp"
#include "util/math.hpp"

namespace pic {

class PrecomputedDifferenceOfGaussians
{
protected:
    /**PrecomputeCoefficients: precomputes a Gaussian Kernel*/
    void PrecomputeCoefficients()
    {
        halfKernelSize = kernelSize >> 1;
        kernelSize = (halfKernelSize << 1) + 1;
        coeff = new float[kernelSize];

        float sigma1_2 = (2.0f * sigma_1 * sigma_1);
        float sigma2_2 = (2.0f * sigma_2 * sigma_2);

        float sqrt2_pi = sqrtf(2.0f * C_PI);
        float c1 = sigma_1 * sqrt2_pi;
        float c2 = sigma_2 * sqrt2_pi;

        for(int i = 0; i < kernelSize; i++) {
            int i_s = i - halfKernelSize;
            float i_s_f = float(i_s * i_s);

            float g1 = expf(-float(i_s_f) / sigma1_2) / c1;
            float g2 = expf(-float(i_s_f) / sigma2_2) / c2;

            coeff[i] = g1 - g2 ;
        }
    }

public:
    float sigma_1, sigma_2;
    int	  kernelSize, halfKernelSize;
    float *coeff;

    //Constructor
    PrecomputedDifferenceOfGaussians()
    {
        kernelSize = halfKernelSize = 0;
        sigma_1 = 0.0f;
        sigma_2 = 0.0f;
        coeff = NULL;
    }

    ~PrecomputedDifferenceOfGaussians()
    {
        if(coeff != NULL) {
            delete[] coeff;
        }

        coeff = NULL;
    }

    //Constructor
    PrecomputedDifferenceOfGaussians(float sigma_1, float sigma_2)
    {
        CalculateKernel(sigma_1, sigma_2);
    }

    //Computing a Gaussian kernel of size sigma1
    void CalculateKernel(float sigma_1, float sigma_2)
    {
        this->sigma_1 = sigma_1;
        this->sigma_2 = sigma_2;

        //The sigma for the size of the kernel
        kernelSize = PrecomputedGaussian::KernelSize(sigma_1 > sigma_2 ? sigma_1 :
                     sigma_2);

        //Precompute Gaussian coefficients
        PrecomputeCoefficients();
    }
};

} // end namespace pic

#endif /* PIC_UTIL_PRECOMPUTED_GAUSSIAN_HPP */

