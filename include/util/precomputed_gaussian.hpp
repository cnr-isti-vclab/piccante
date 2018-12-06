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

#ifndef PIC_UTIL_PRECOMPUTED_GAUSSIAN_HPP
#define PIC_UTIL_PRECOMPUTED_GAUSSIAN_HPP

namespace pic {

#include <math.h>

#include "../util/array.hpp"

/**
 * @brief The PrecomputedGaussian class
 */
class  PrecomputedGaussian
{
protected:
    /**
     * @brief precomputeCoefficients precomputes a Gaussian kernel.
     */
    void precomputeCoefficients()
    {
        halfKernelSize = kernelSize >> 1;
        kernelSize = (halfKernelSize << 1) + 1;

        if(coeff != NULL) {
            delete[] coeff;
            coeff = NULL;
        }

        coeff = new float[kernelSize];

        float sigma_sq_2 = (2.0f * sigma * sigma);

        float sum = 0.0f;
        for(int i = 0; i < kernelSize; i++) {
            int i_s = i - halfKernelSize;
            i_s *= i_s;
            coeff[i] = expf(-float(i_s) / sigma_sq_2);
            sum += coeff[i];
        }

        //normalize the kernel
        if(sum > 0.0f) {
            Arrayf::div(coeff, kernelSize, sum);
        }
    }

public:
    float sigma;
    int	  kernelSize, halfKernelSize;
    float *coeff;

    /**
     * @brief PrecomputedGaussian
     */
    PrecomputedGaussian()
    {
        kernelSize = halfKernelSize = 0;
        sigma = 0.0f;
        coeff = NULL;
    }

    /**
     * @brief PrecomputedGaussian
     * @param sigma
     */
    PrecomputedGaussian(float sigma)
    {
        coeff = NULL;
        calculateKernel(sigma);
    }

    ~PrecomputedGaussian()
    {
        if(coeff != NULL) {
            delete[] coeff;
            coeff = NULL;
        }
    }

    /**
     * @brief calculateKernel computes a Gaussian kernel of size sigma
     * @param sigma
     */
    void calculateKernel(float sigma, int kernelSize = -1)
    {
        this->sigma = sigma;

        //the sigma for the size of the kernel
        if(kernelSize < 3) {
            this->kernelSize = PrecomputedGaussian::getKernelSize(sigma);
        } else {
            this->kernelSize = kernelSize;
        }

        //precompute Gaussian coefficients
        precomputeCoefficients();
    }

    /**
     * @brief KernelSize computes the size of a kernel in pixel give its sigma.
     * @param sigma is the sigma value of a Gaussian kernel.
     * @return It returns the size of the kernel in pixels.
     */
    static int getKernelSize(float sigma)
    {
        int kernelSize = int(ceilf(sigma * 5.0f));
        return (kernelSize > 3) ? kernelSize : 3;
    }
};

} // end namespace pic

#endif /* PIC_UTIL_PRECOMPUTED_GAUSSIAN_HPP */

