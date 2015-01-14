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

#include "util/precomputed_gaussian.hpp"

#include "gl/filtering/filter_conv_1d.hpp"

namespace pic {

class FilterGLGaussian1D: public FilterGLConv1D
{
protected:
    float               sigma;
    PrecomputedGaussian *pg;

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
        imgIn.generateTextureGL(GL_TEXTURE_2D, false);

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

FilterGLGaussian1D::FilterGLGaussian1D(float sigma, int direction = 0,
                                       GLenum target = GL_TEXTURE_2D): FilterGLConv1D(NULL, direction, target)
{
    pg = NULL;

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

    if(pg != NULL) {
        delete pg;
    }

    pg = new PrecomputedGaussian(this->sigma);

    if(bChanges || weights == NULL) {
        if(weights != NULL) {
            delete weights;
        }

        weights = new ImageGL(1, pg->kernelSize, 1, 1, pg->coeff);
        weights->generateTextureGL();
    }

    SetUniform();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP */

