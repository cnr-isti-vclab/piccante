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

#ifndef PIC_UTIL_PRECOMPUTED_DIFF_OF_GAUSSIANS_HPP
#define PIC_UTIL_PRECOMPUTED_DIFF_OF_GAUSSIANS_HPP

#include "../util/precomputed_gaussian.hpp"

namespace pic {

/**
 * @brief The PrecomputedDiffOfGaussians class
 */
class  PrecomputedDiffOfGaussians
{
protected:
    /**
     * @brief precomputeCoefficients precomputes a Gaussian kernel.
     */
    void precomputeCoefficients()
    {
        halfKernelSize = kernelSize >> 1;
        kernelSize = (halfKernelSize << 1) + 1;
        coeff = new float[kernelSize];

        float sigma1_sq = sigma1 * sigma1;
        float sigma1_sq_2 = sigma1_sq * 2.0f;

        float sigma2_sq = sigma2 * sigma2;
        float sigma2_sq_2 = sigma2_sq * 2.0f;

        float C1 = sigma1 * sqrtf(2.0f * C_PI);
        float C2 = sigma2 * sqrtf(2.0f * C_PI);

        for(int i = 0; i < kernelSize; i++) {
            int i_s = i - halfKernelSize;
            float i_sq_f = -float(i_s * i_s);

            float G1 = expf(i_sq_f / sigma1_sq_2) / C1;
            float G2 = expf(i_sq_f / sigma2_sq_2) / C2;

            coeff[i] = G1 - G2;
        }
    }

public:
    float sigma1, sigma2;
    int	  kernelSize, halfKernelSize;
    float *coeff;

    /**
     * @brief PrecomputedDiffOfGaussians
     */
    PrecomputedDiffOfGaussians()
    {
        kernelSize = halfKernelSize = 0;
        sigma1 = 0.0f;
        sigma2 = 0.0f;
        coeff = NULL;
    }

    /**
     * @brief PrecomputedDiffOfGaussians
     * @param sigma1
     * @param sigma2
     */
    PrecomputedDiffOfGaussians(float sigma1, float sigma2)
    {

        calculateKernel(sigma1, sigma2);
    }

    ~PrecomputedDiffOfGaussians()
    {
        if(coeff != NULL) {
            delete[] coeff;
        }

        coeff = NULL;
    }

    /**
     * @brief calculateKernel computes a Gaussian kernel of size sigma
     * @param sigma
     */
    void calculateKernel(float sigma1, float sigma2)
    {
        this->sigma1 = sigma1;
        this->sigma2 = sigma2;

        //compute the sigma for the size of the kernel
        kernelSize = PrecomputedGaussian::getKernelSize(MAX(sigma1, sigma2));

        //precompute Gaussian coefficients
        precomputeCoefficients();
    }
};

} // end namespace pic

#endif /* PIC_UTIL_PRECOMPUTED_DIFF_OF_GAUSSIANS_HPP */

