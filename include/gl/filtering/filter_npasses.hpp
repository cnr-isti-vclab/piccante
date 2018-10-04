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

#ifndef PIC_GL_FILTERING_FILTER_NPASSES_HPP
#define PIC_GL_FILTERING_FILTER_NPASSES_HPP

#include "../../gl/filtering/filter.hpp"

namespace pic {

class FilterGLNPasses: public FilterGL
{
protected:
    ImageGL *imgAllocated;
    ImageGL *imgTmp[2];

    void initShaders() {}
    void FragmentShader() {}

public:
    /**
     * @brief FilterGLNPasses
     */
    FilterGLNPasses();
    ~FilterGLNPasses();

    virtual ImageGL *setupAuxN(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief insertFilter
     * @param flt
     */
    void insertFilter(FilterGL *flt);

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

FilterGLNPasses::FilterGLNPasses(): FilterGL()
{
    imgAllocated = NULL;
    imgTmp[0] = imgTmp[1] = NULL;
    target = GL_TEXTURE_2D;
}

FilterGLNPasses::~FilterGLNPasses()
{
    if(imgAllocated != NULL) {
        delete imgAllocated;
        imgAllocated = NULL;
    }
}
void FilterGLNPasses::insertFilter(FilterGL *flt)
{
    if(flt == NULL) {
        return;
    }

    if(flt->filters.size() > 0) {
        for(unsigned int i = 0; i < flt->filters.size(); i++) {
            insertFilter(flt->filters[i]);
        }
    } else {

#ifdef PIC_DEBUG
        printf("Add Single Filter\n");
#endif
        filters.push_back(flt);
    }
}

ImageGL *FilterGLNPasses::setupAuxN(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->allocateSimilarOneGL();
    }

    if(imgAllocated == NULL) {
        imgAllocated = imgOut->allocateSimilarOneGL();
    }

    if((filters.size() % 2) == 0) {
        imgTmp[1] = imgOut;

        if(imgTmp[0] == NULL) {
            imgTmp[0] = imgAllocated;
        }
    } else {
        imgTmp[0] = imgOut;

        if(imgTmp[1] == NULL) {
            imgTmp[1] = imgAllocated;
        }
    }

    /*
    if(fbo==NULL)
    	fbo = new Fbo();

    fbo->create(imgOut->width,imgOut->height,imgOut->frames, false, imgOut->getTexture());

    for(unsigned int i=0;i<filters.size();i++)
    	filters[i]->setFbo(fbo);*/

    return imgOut;
}

ImageGL *FilterGLNPasses::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.size() < 1 || imgIn[0] == NULL) {
        return imgOut;
    }

    //allocate FBOs
    imgOut = setupAuxN(imgIn, imgOut);

    filters[0]->changePass(0, int(filters.size()));
    filters[0]->Process(imgIn, imgTmp[0]);

    for(auto i = 1; i < filters.size(); i++) {
        filters[i]->changePass(i, int(filters.size()));
        imgIn[0] = imgTmp[(i + 1) % 2];
        filters[i]->Process(imgIn, imgTmp[i % 2]);
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_NPASSES_HPP */

