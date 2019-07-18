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

#include "../../base.hpp"

#include "../../util/std_util.hpp"

#include "../../util/precomputed_gaussian.hpp"

#include "../../gl/filtering/filter_conv_1d.hpp"

namespace pic {

class FilterGLGaussian1D: public FilterGLConv1D
{
protected:
    float sigma;
    PrecomputedGaussian *pg;
    bool bWeightsOwn;

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
     * @brief releaseAux
     */
    void releaseAux()
    {
        pg = delete_s(pg);

        if(bWeightsOwn) {
            weights = delete_s(weights);
        }
    }

    /**
     * @brief update
     * @param sigma
     */
    void update(float sigma);

    /**
     * @brief execute
     * @param nameIn
     * @param nameOut
     * @param sigma
     * @return
     */
    static ImageGL *execute(std::string nameIn, std::string nameOut, float sigma)
    {
        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);

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
    bWeightsOwn = false;
    this->sigma = -1.0f;

    update(sigma);
}

FilterGLGaussian1D::~FilterGLGaussian1D()
{
    release();
}

void FilterGLGaussian1D::update(float sigma)
{
    bool bChanges = false;

    if((this->sigma != sigma) && sigma > 0.0f) {
        this->sigma = sigma;
        bChanges = true;
    }

    if(pg != NULL) {
        pg = delete_s(pg);
    }

    if(pg == NULL) {        
        pg = new PrecomputedGaussian(this->sigma);
    }

    if(bChanges || weights == NULL) {
        weights = delete_s(weights);

        weights = new ImageGL(1, pg->kernelSize, 1, 1, pg->coeff);
        weights->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);
        bWeightsOwn = true;
    }
    setUniform();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_1D_HPP */

