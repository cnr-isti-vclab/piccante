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

#include "../../base.hpp"

#include "../../util/array.hpp"

#include "../../util/gl/fbo.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

class FilterGLNPasses: public FilterGL
{
protected:
    bool bSameImageSize;

    //same size image
    ImageGL *imgAllocated;
    ImageGL *imgTmp[2];

    //different size image
    ImageGLVec imgTmpVec;

public:
    /**
     * @brief FilterGLNPasses
     */
    FilterGLNPasses();

    ~FilterGLNPasses();

    /**
     * @brief setupAuxN
     * @param imgIn
     * @param imgOut
     * @return
     */
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
    Fbo *getFbo()
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

PIC_INLINE FilterGLNPasses::FilterGLNPasses(): FilterGL()
{
    bSameImageSize = true;
    imgAllocated = NULL;
    imgTmp[0] = imgTmp[1] = NULL;
    target = GL_TEXTURE_2D;
}

PIC_INLINE FilterGLNPasses::~FilterGLNPasses()
{
    if(imgAllocated != NULL) {
        delete imgAllocated;
        imgAllocated = NULL;
    }

    if(!imgTmpVec.empty()) {
        imgTmpVec.clear();
    }
}

PIC_INLINE void FilterGLNPasses::insertFilter(FilterGL *flt)
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

PIC_INLINE ImageGL *FilterGLNPasses::setupAuxN(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(bSameImageSize) {
        //output
        if(imgOut == NULL) {
            imgOut = imgIn[0]->allocateSimilarOneGL();
        } else {
            if(!imgOut->isSimilarType(imgIn[0])) {
                imgOut = imgIn[0]->allocateSimilarOneGL();
            }
        }

        //temporary buffer
        if(imgAllocated == NULL) {
            imgAllocated = imgOut->allocateSimilarOneGL();
        } else {
            if(!imgAllocated->isSimilarType(imgIn[0])) {
                delete imgAllocated;
                imgAllocated = imgOut->allocateSimilarOneGL();
            }
        }

        //temporary + output
        if((filters.size() % 2) == 0) {
            imgTmp[0] = imgAllocated;
            imgTmp[1] = imgOut;
        } else {
            imgTmp[0] = imgOut;
            imgTmp[1] = imgAllocated;
        }
    } else {        
    }

    /*
    if(fbo==NULL)
    	fbo = new Fbo();

    fbo->create(imgOut->width,imgOut->height,imgOut->frames, false, imgOut->getTexture());

    for(unsigned int i=0;i<filters.size();i++)
    	filters[i]->setFbo(fbo);*/

    return imgOut;
}

PIC_INLINE ImageGL *FilterGLNPasses::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.size() < 1 || imgIn[0] == NULL || filters.empty()) {
        return imgOut;
    }

    //allocate FBOs
    imgOut = setupAuxN(imgIn, imgOut);

    if(bSameImageSize) {
        filters[0]->changePass(0, int(filters.size()));
        filters[0]->Process(imgIn, imgTmp[0]);

        for(unsigned int i = 1; i < filters.size(); i++) {
            filters[i]->changePass(i, int(filters.size()));
            imgIn[0] = imgTmp[(i + 1) % 2];
            filters[i]->Process(imgIn, imgTmp[i % 2]);
        }
    } else {
        if(imgTmpVec.empty()) {
            for(unsigned int i = 0; i < (filters.size() - 1); i++) {
                imgTmpVec.push_back(NULL);
            }

            imgTmpVec.push_back(imgOut);
        }

        filters[0]->changePass(0, int(filters.size()));
        imgTmpVec[0] = filters[0]->Process(imgIn, imgTmpVec[0]);

        for(unsigned int i = 1; i < filters.size(); i++) {
            filters[i]->changePass(i, int(filters.size()));

            imgIn[0] = imgTmpVec[i - 1];
            imgTmpVec[i] = filters[i]->Process(imgIn, imgTmpVec[i]);
        }
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_NPASSES_HPP */

