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

#ifndef PIC_GL_FILTERING_FILTER_ITERATIVE_HPP
#define PIC_GL_FILTERING_FILTER_ITERATIVE_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLIterative class
 */
class FilterGLIterative: public FilterGL
{
protected:
    ImageGL *imgTmp[2];
    int			iterations;

    void InitShaders() {}
    void FragmentShader() {}

public:

    /**
     * @brief FilterGLIterative
     */
    FilterGLIterative();

    /**
     * @brief FilterGLIterative
     * @param flt
     * @param iterations
     */
    FilterGLIterative(FilterGL *flt, int iterations);

    /**
     * @brief SetupAuxN
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual ImageGL *SetupAuxN(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief Update
     * @param flt
     * @param iterations
     */
    void Update(FilterGL *flt, int iterations);

    /**
     * @brief getFbo
     * @return
     */
    Fbo  *getFbo()
    {
        return filters.back()->getFbo();
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

FilterGLIterative::FilterGLIterative(): FilterGL()
{
    imgTmp[0] = imgTmp[1] = NULL;
    iterations = 0;
    target = GL_TEXTURE_2D;
}

FilterGLIterative::FilterGLIterative(FilterGL *flt, int iterations): FilterGL()
{
    imgTmp[0] = imgTmp[1] = NULL;
    target = GL_TEXTURE_2D;

    Update(flt, iterations);
}

void FilterGLIterative::Update(FilterGL *flt, int iterations)
{
    if(iterations > 0) {
        this->iterations = iterations;
    }

    if(flt == NULL) {
        return;
    }

    if(filters.size() > 0) {
        filters.clear();
    }

    filters.push_back(flt);
}

ImageGL *FilterGLIterative::SetupAuxN(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOneGL();
    }

    /*
    if(fbo==NULL)
    	fbo = new Fbo();

    fbo->create(imgOut->width,imgOut->height,imgOut->frames, false, imgOut->getTexture());
    filters[0]->setFbo(fbo);*/

    if((iterations % 2) == 0) {
        imgTmp[1] = imgOut;

        if(imgTmp[0] == NULL) {
            imgTmp[0] = imgOut->AllocateSimilarOneGL();
        }
    } else {
        imgTmp[0] = imgOut;

        if(imgTmp[1] == NULL) {
            imgTmp[1] = imgOut->AllocateSimilarOneGL();
        }
    }

    return imgOut;
}

ImageGL *FilterGLIterative::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.size() < 1 || imgIn[0] == NULL) {
        return imgOut;
    }

    //Allocate FBOs
    imgOut = SetupAuxN(imgIn, imgOut);

    filters[0]->Process(imgIn, imgTmp[0]);

    for(int i = 1; i < iterations; i++) {
        imgIn[0] = imgTmp[(i + 1) % 2];
        filters[0]->Process(imgIn, imgTmp[i % 2]);
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_ITERATIVE_HPP */

