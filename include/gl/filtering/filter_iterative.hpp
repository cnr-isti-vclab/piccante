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
    ImageRAWGL *imgTmp[2];
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
    virtual ImageRAWGL *SetupAuxN(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

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

    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);
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

ImageRAWGL *FilterGLIterative::SetupAuxN(ImageRAWGLVec imgIn,
        ImageRAWGL *imgOut)
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

ImageRAWGL *FilterGLIterative::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
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

