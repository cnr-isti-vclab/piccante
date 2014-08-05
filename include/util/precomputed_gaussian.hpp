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

