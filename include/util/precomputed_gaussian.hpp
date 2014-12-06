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

class  PrecomputedGaussian
{
protected:
    /**PrecomputeCoefficients: precomputes a Gaussian Kernel*/
    void PrecomputeCoefficients()
    {
        halfKernelSize = kernelSize >> 1;
        kernelSize = (halfKernelSize << 1) + 1;
        coeff = new float[kernelSize];

        float sum = 0.0f;
        float sigma2 = (2.0f * sigma * sigma);

        for(int i = 0; i < kernelSize; i++) {
            int i_s = i - halfKernelSize;
            i_s *= i_s;
            coeff[i] = expf(-float(i_s) / sigma2);
            sum += coeff[i];
        }

        //Kernel's normalization
        if(sum > 0.0f) {
            for(int i = 0; i < kernelSize; i++) {
                coeff[i] /= sum;
            }
        }
    }

public:
    float sigma;
    int	  kernelSize, halfKernelSize;
    float *coeff;

    //Constructor
    PrecomputedGaussian()
    {
        kernelSize = halfKernelSize = 0;
        sigma = 0.0f;
        coeff = NULL;
    }

    ~PrecomputedGaussian()
    {
        if(coeff != NULL) {
            delete[] coeff;
        }

        coeff = NULL;
    }

    //Constructor
    PrecomputedGaussian(float sigma)
    {
        CalculateKernel(sigma);
    }

    //Computing a Gaussian kernel of size sigma1
    void CalculateKernel(float sigma)
    {
        this->sigma = sigma;
        //The sigma for the size of the kernel
        kernelSize = PrecomputedGaussian::KernelSize(sigma);

        //Precompute Gaussian coefficients
        PrecomputeCoefficients();
    }

    //Static kernel size
    static int KernelSize(float sigma)
    {
        int kernelSize = int(ceilf(sigma * 5.0f));
        return (kernelSize > 3) ? kernelSize : 3;
    }
};

} // end namespace pic

#endif /* PIC_UTIL_PRECOMPUTED_GAUSSIAN_HPP */

