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

#ifndef PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP
#define PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP

#include "gl/filtering/filter_conv_1d.hpp"

namespace pic {

class FilterGLGaussian1D: public FilterGLConv1D
{
protected:
    float		sigma;

public:

    /**
     * @brief FilterGLGaussian1D
     * @param sigma
     * @param direction
     * @param target
     */
    FilterGLGaussian1D(float sigma, int direction, GLenum target);

    ~FilterGLGaussian1D();

    /**
     * @brief Update
     * @param sigma
     */
    void Update(float sigma);

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma
     * @return
     */
    static ImageGL *Execute(std::string nameIn, std::string nameOut, float sigma)
    {
        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLGaussian1D filter(sigma, true, GL_TEXTURE_2D);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageGL *imgOut = filter.Process(SingleGL(&imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gaussian 1D Filter on GPU time: %g ms\n",
               double(timeVal) / 100000000.0);

        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLGaussian1D::FilterGLGaussian1D(float sigma, int direction,
                                       GLenum target): FilterGLConv1D(NULL, direction, target)
{
    this->sigma = -1.0f;

    Update(sigma);
}

FilterGLGaussian1D::~FilterGLGaussian1D()
{
    if(weights != NULL) {
        delete weights;
        weights = NULL;
    }
}

void FilterGLGaussian1D::Update(float sigma)
{
    bool bChanges = false;

    if((this->sigma != sigma) && sigma > 0.0f) {
        this->sigma = sigma;
        bChanges = true;
    }

    //Precomputation of the Gaussian Kernel
    int halfKernelSize = PrecomputedGaussian::KernelSize(sigma) >> 1;
    int nSamples = halfKernelSize * 2 + 1;
    float sigma2 = 2.0f * sigma * sigma;

    if(bChanges || weights == NULL) {
        if(weights != NULL) {
            delete weights;
        }

        weights = new ImageGL(1, nSamples, 1, 1, IMG_CPU, 0);

        for(int i = -halfKernelSize; i <= halfKernelSize; i++) {
            weights->data[i + halfKernelSize] = expf(-float(i * i) / sigma2);
        }

        weights->generateTextureGL();
    }

    SetUniform();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP */

