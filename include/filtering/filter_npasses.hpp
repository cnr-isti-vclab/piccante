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

#ifndef PIC_FILTERING_FILTER_NPASSES_HPP
#define PIC_FILTERING_FILTER_NPASSES_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterNPasses class
 */
class FilterNPasses: public Filter
{
protected:
    Image *imgAllocated;
    Image *imgTmpSame[2];
    ImageVec imgTmp;

    bool CheckSame(ImageVec imgIn);

    /**
     * @brief destroy
     */
    void destroy();

public:

    /**
     * @brief FilterNPasses
     */
    FilterNPasses();

    ~FilterNPasses();

    /**
     * @brief PreProcess
     * @param imgIn
     * @param imgOut
     */
    virtual void PreProcess(ImageVec imgIn, Image *imgOut){}

    /**
     * @brief setupAuxNGen
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual Image *setupAuxNGen(ImageVec imgIn, Image *imgOut);

    /**
     * @brief setupAuxNSame
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual Image *setupAuxNSame(ImageVec imgIn, Image *imgOut);

    /**
     * @brief ProcessGen
     * @param imgIn
     * @param imgOut
     * @param parallel
     * @return
     */
    Image *ProcessGen(ImageVec imgIn, Image *imgOut, bool parallel);

    /**
     * @brief ProcessSame
     * @param imgIn
     * @param imgOut
     * @param parallel
     * @return
     */
    Image *ProcessSame(ImageVec imgIn, Image *imgOut, bool parallel);

    /**
     * @brief insertFilter
     * @param flt
     */
    void insertFilter(Filter *flt);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @param parallel
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut, bool parallel);

    /**
     * @brief ProcessP
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessP(ImageVec imgIn, Image *imgOut);
};

PIC_INLINE FilterNPasses::FilterNPasses() : Filter()
{
    imgAllocated = NULL;

    for(int i = 0; i < 2; i++) {
        imgTmpSame[i] = NULL;
    }
}

PIC_INLINE FilterNPasses::~FilterNPasses()
{
    destroy();
}

PIC_INLINE void FilterNPasses::destroy()
{
    if(imgAllocated != NULL) {
        delete imgAllocated;
        imgAllocated = NULL;
    }
    
    imgTmpSame[0] = NULL;
    imgTmpSame[1] = NULL;

    for(unsigned int i=0; i<imgTmp.size(); i++) {
        delete imgTmp[i];
    }

    imgTmp.clear();
}

PIC_INLINE bool FilterNPasses::CheckSame(ImageVec imgIn)
{
    Image *tmp = imgIn[0];

    for(unsigned int i = 0; i < filters.size(); i++) {
        int width, height, channels, frames; 

        filters[i]->OutputSize(tmp, width, height, channels, frames);

        if( (tmp->width != width) || (tmp->height != height) ||
            (tmp->channels != channels) || (tmp->frames != frames) ) {
                return false;
        }
    }
    return true;
}

PIC_INLINE void FilterNPasses::insertFilter(Filter *flt)
{
    if(flt == NULL) {
        return;
    }

    if(flt->filters.size() > 0) {
        for(unsigned int i = 0; i < flt->filters.size(); i++) {
            insertFilter(flt->filters[i]);
        }
    } else {
        filters.push_back(flt);
    }
}

PIC_INLINE Image *FilterNPasses::setupAuxNGen(ImageVec imgIn,
        Image *imgOut)
{   
    if(imgTmp.size() == 0 ) {
        for( unsigned int i=0; i<filters.size(); i++) {
            imgTmp.push_back(NULL);
        }
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::setupAuxNSame(ImageVec imgIn,
        Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->allocateSimilarOne();
    }

    if(imgAllocated != NULL) {
        delete imgAllocated;
    }

    imgAllocated = imgOut->allocateSimilarOne();

    //Bug:
    if((filters.size() % 2) == 0) {
        imgTmpSame[1] = imgOut;
        imgTmpSame[0] = imgAllocated;
    } else {
        imgTmpSame[0] = imgOut;
        imgTmpSame[1] = imgAllocated;
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::ProcessGen(ImageVec imgIn, Image *imgOut,
        bool parallel = false)
{
    if(imgIn.empty() || filters.empty()) {
        return NULL;
    }

    setupAuxNGen(imgIn, NULL);

    int n =  int(filters.size()) - 1;
    
    for(int i = 0; i < n; i++) {
        filters[i]->changePass(i, imgIn[0]->frames);
        
        if(parallel) {
            imgTmp[i] = filters[i]->ProcessP(imgIn, imgTmp[i]);
        } else {
            imgTmp[i] = filters[i]->Process(imgIn, imgTmp[i]);
        }

        imgIn[0] = imgTmp[i];
    }

    filters[n]->changePass(n, imgIn[0]->frames);
    imgOut = filters[n]->Process(imgIn, imgOut);

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::ProcessSame(ImageVec imgIn, Image *imgOut,
        bool parallel = false)
{
    if((imgIn.size() <= 0) || (filters.size() < 1)) {
        return NULL;
    }

    //Setup
    imgOut = setupAuxNSame(imgIn, imgOut);

    //Pass 0
    filters[0]->changePass(0, imgIn[0]->frames);

    if(parallel) {
        filters[0]->ProcessP(imgIn, imgTmpSame[0]);
    } else {
        filters[0]->Process(imgIn, imgTmpSame[0]);
    }

    for(unsigned int i = 1; i < filters.size(); i++) {
        filters[i]->changePass(i, imgIn[0]->frames);

        imgIn[0] = imgTmpSame[(i + 1) % 2];

        if(parallel) {
            filters[i]->ProcessP(imgIn, imgTmpSame[i % 2]);
        } else {
            filters[i]->Process(imgIn, imgTmpSame[i % 2]);
        }
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::Process(ImageVec imgIn, 
        Image *imgOut, bool parallel = false)
{
    PreProcess(imgIn, imgOut);

    if(CheckSame(imgIn)) {
        return ProcessSame(imgIn, imgOut, parallel);
    } else {
        return ProcessGen(imgIn, imgOut, parallel);
    }
}
 
PIC_INLINE Image *FilterNPasses::ProcessP(ImageVec imgIn,
        Image *imgOut)
{
    return Process(imgIn, imgOut, true);
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NPASSES_HPP */

