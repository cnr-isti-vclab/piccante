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

#ifndef PIC_UTIL_PRECOMPUTED_DIFFERENCE_OF_GAUSSIANS_HPP
#define PIC_UTIL_PRECOMPUTED_DIFFERENCE_OF_GAUSSIANS_HPP

#include "util/precomputed_gaussian.hpp"
#include "util/math.hpp"

namespace pic {

/**
 * @brief The PrecomputedDifferenceOfGaussians class
 */
class PrecomputedDifferenceOfGaussians
{
protected:
    /**
     * @brief PrecomputeCoefficients
     */
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

    /**
     * @brief PrecomputedDifferenceOfGaussians
     */
    PrecomputedDifferenceOfGaussians()
    {
        kernelSize = halfKernelSize = 0;
        sigma_1 = 0.0f;
        sigma_2 = 0.0f;
        coeff = NULL;
    }

    /**
     * @brief PrecomputedDifferenceOfGaussians
     * @param sigma_1
     * @param sigma_2
     */
    PrecomputedDifferenceOfGaussians(float sigma_1, float sigma_2)
    {
        CalculateKernel(sigma_1, sigma_2);
    }

    ~PrecomputedDifferenceOfGaussians()
    {
        if(coeff != NULL) {
            delete[] coeff;
        }

        coeff = NULL;
    }

    /**
     * @brief CalculateKernel
     * @param sigma_1
     * @param sigma_2
     */
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

