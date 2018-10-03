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

#ifndef PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP
#define PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP

#include "../../gl/filtering/filter_npasses.hpp"
#include "../../gl/filtering/filter_gaussian_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLGaussian2D class
 */
class FilterGLGaussian2D: public FilterGLNPasses
{
protected:
    FilterGLGaussian1D		*filter;

    void initShaders() {}
    void FragmentShader() {}

public:
    /**
     * @brief FilterGLGaussian2D
     */
    FilterGLGaussian2D();
    ~FilterGLGaussian2D();

    /**
     * @brief FilterGLGaussian2D
     * @param sigma
     */
    FilterGLGaussian2D(float sigma);

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
        imgIn.generateTextureGL(GL_TEXTURE_2D, false);

        FilterGLGaussian2D *filter = new FilterGLGaussian2D(sigma);

#ifdef PIC_DEBUG
        GLuint testTQ1 = glBeginTimeQuery();
#endif

        ImageGL *imgOut = filter->Process(SingleGL(&imgIn), NULL);

#ifdef PIC_DEBUG
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gaussian 2D Filter on GPU time: %g ms\n",
               float(timeVal) / 100000000.0f);
#endif

        //Read from the GPU
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLGaussian2D::FilterGLGaussian2D(): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;

    filter = new FilterGLGaussian1D(1.0f, 0, target);
    InsertFilter(filter);
    InsertFilter(filter);
}

FilterGLGaussian2D::FilterGLGaussian2D(float sigma): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;

    filter = new FilterGLGaussian1D(sigma, 0, target);
    InsertFilter(filter);
    InsertFilter(filter);
}

FilterGLGaussian2D::~FilterGLGaussian2D()
{
    if(filter != NULL) {
       delete filter;
        filter = NULL;
    }
}

void FilterGLGaussian2D::update(float sigma)
{
    filter->update(sigma);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP */

