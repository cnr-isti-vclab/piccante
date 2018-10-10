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

#include "../../base.hpp"

#include "../../util/gl/fbo.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLIterative class
 */
class FilterGLIterative: public FilterGL
{
protected:
    bool bSameImageSize;

    //same size image
    ImageGL *imgTmp[2];

    //different size image
    ImageGLVec imgTmpVec;

    int iterations;

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
     * @brief setupAuxN
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual ImageGL *setupAuxN(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief update
     * @param flt
     * @param iterations
     */
    void update(FilterGL *flt, int iterations);

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

PIC_INLINE FilterGLIterative::FilterGLIterative(): FilterGL()
{
    imgTmp[0] = imgTmp[1] = NULL;
    iterations = 0;
    target = GL_TEXTURE_2D;
}

PIC_INLINE FilterGLIterative::FilterGLIterative(FilterGL *flt, int iterations): FilterGL()
{
    imgTmp[0] = imgTmp[1] = NULL;
    target = GL_TEXTURE_2D;

    update(flt, iterations);
}

PIC_INLINE void FilterGLIterative::update(FilterGL *flt, int iterations)
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

    //check if the filters keep the same image size
    bSameImageSize = true;
}

PIC_INLINE ImageGL *FilterGLIterative::setupAuxN(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(bSameImageSize) {
        if(imgOut == NULL) {
            imgOut = imgIn[0]->allocateSimilarOneGL();
        }

        /*
        if(fbo==NULL)
            fbo = new Fbo();

        fbo->create(imgOut->width,imgOut->height,imgOut->frames, false, imgOut->getTexture());
        filters[0]->setFbo(fbo);*/

        if((iterations % 2) == 0) {
            imgTmp[1] = imgOut;

            if(imgTmp[0] == NULL) {
                imgTmp[0] = imgOut->allocateSimilarOneGL();
            }
        } else {
            imgTmp[0] = imgOut;

            if(imgTmp[1] == NULL) {
                imgTmp[1] = imgOut->allocateSimilarOneGL();
            }
        }
    } else {
        if(imgOut == NULL) {
        }
    }

    return imgOut;
}

PIC_INLINE ImageGL *FilterGLIterative::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.size() < 1 || imgIn[0] == NULL || iterations < 1) {
        return imgOut;
    }

    //allocate FBOs
    imgOut = setupAuxN(imgIn, imgOut);

    if(bSameImageSize) {
        filters[0]->Process(imgIn, imgTmp[0]);

        for(int i = 1; i < iterations; i++) {
            imgIn[0] = imgTmp[(i + 1) % 2];
            filters[0]->Process(imgIn, imgTmp[i % 2]);
        }
    } else {
        if(imgTmpVec.empty()) {
            for(unsigned int i = 0; i < (filters.size() - 1); i++) {
                imgTmpVec.push_back(NULL);
            }

            imgTmpVec.push_back(imgOut);
        }

        imgTmpVec[0] = filters[0]->Process(imgIn, imgTmpVec[0]);

        for(unsigned int i = 1; i < filters.size(); i++) {
            imgIn[0] = imgTmpVec[i - 1];
            imgTmpVec[i] = filters[0]->Process(imgIn, imgTmpVec[i]);
        }
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_ITERATIVE_HPP */

