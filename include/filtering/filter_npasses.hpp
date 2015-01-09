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

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterNPasses class
 */
class FilterNPasses: public Filter
{
protected:
    Image	*imgTmpSame[2];
    ImageVec imgTmp;

    bool        bSame;

    void CheckSame(ImageVec imgIn);

public:

    /**
     * @brief FilterNPasses
     */
    FilterNPasses();

    ~FilterNPasses();

    /**
     * @brief Destroy
     */
    void Destroy();

    /**
     * @brief PreProcess
     * @param imgIn
     * @param imgOut
     */
    virtual void PreProcess(ImageVec imgIn, Image *imgOut){}

    /**
     * @brief SetupAuxNGen
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual Image *SetupAuxNGen(ImageVec imgIn, Image *imgOut);

    /**
     * @brief SetupAuxNSame
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual Image *SetupAuxNSame(ImageVec imgIn, Image *imgOut);

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
     * @brief InsertFilter
     * @param flt
     */
    void InsertFilter(Filter *flt);

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

PIC_INLINE FilterNPasses::FilterNPasses()
{
    bSame = true;

    for(int i = 0; i < 2; i++) {
        imgTmpSame[i] = NULL;
    }
}

PIC_INLINE FilterNPasses::~FilterNPasses()
{
    Destroy();
}

PIC_INLINE void FilterNPasses::Destroy()
{
    if((filters.size() % 2) == 0) {
        if(imgTmpSame[0] != NULL) {
            delete imgTmpSame[0];
        }
    } else {
        if(imgTmpSame[1] != NULL) {
            delete imgTmpSame[1];
        }
    }
    
    imgTmpSame[0] = NULL;
    imgTmpSame[1] = NULL;

    for(unsigned int i=0; i<imgTmp.size(); i++) {
        delete imgTmp[i];
    }

    imgTmp.clear();
}

PIC_INLINE void FilterNPasses::CheckSame(ImageVec imgIn)
{
    bSame = true;

    Image *tmp = imgIn[0];

    for(unsigned int i = 0; i < filters.size(); i++) {
        int width, height, channels, frames; 

        filters[i]->OutputSize(tmp, width, height, channels, frames);

        if( (tmp->width != width) || (tmp->height != height) ||
            (tmp->channels != channels) || (tmp->frames != frames) ) {
                bSame = false;
                break;
        }
    }
}

PIC_INLINE void FilterNPasses::InsertFilter(Filter *flt)
{
    if(flt == NULL) {
        return;
    }

    if(flt->filters.size() > 0) {
        for(unsigned int i = 0; i < flt->filters.size(); i++) {
            InsertFilter(flt->filters[i]);
        }
    } else {
        filters.push_back(flt);
    }
}

PIC_INLINE Image *FilterNPasses::SetupAuxNGen(ImageVec imgIn,
        Image *imgOut)
{   
    if(imgTmp.size() == 0 ) {
        for( unsigned int i=0; i<filters.size(); i++) {
            imgTmp.push_back(NULL);
        }
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::SetupAuxNSame(ImageVec imgIn,
        Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOne();
    }

    //Bug:
    if((filters.size() % 2) == 0) {
        imgTmpSame[1] = imgOut;

        if(imgTmpSame[0] == NULL) {
            imgTmpSame[0] = imgOut->AllocateSimilarOne();
        } else {
            if(!imgOut->SimilarType(imgTmpSame[0])) {
                delete imgTmpSame[0];
                imgTmpSame[0] = imgOut->AllocateSimilarOne();
            }
        }
    } else {
        imgTmpSame[0] = imgOut;

        if(imgTmpSame[1] == NULL) {
            imgTmpSame[1] = imgOut->AllocateSimilarOne();
        } else {
            if(!imgOut->SimilarType(imgTmpSame[1])) {
                delete imgTmpSame[1];
                imgTmpSame[1] = imgOut->AllocateSimilarOne();
            }
        }
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::ProcessGen(ImageVec imgIn, Image *imgOut,
        bool parallel = false)
{
    if((imgIn.size() <= 0) || (filters.size() < 1)) {
        return NULL;
    }

    SetupAuxNGen(imgIn, NULL);

    unsigned int n =  filters.size() - 1;
    
    for(unsigned int i = 0; i < n; i++) {
        filters[i]->ChangePass(i, imgIn[0]->frames);
        
        if(parallel) {
            imgTmp[i] = filters[i]->ProcessP(imgIn, imgTmp[i]);
        } else {
            imgTmp[i] = filters[i]->Process(imgIn, imgTmp[i]);
        }

        imgIn[0] = imgTmp[i];
    }

    filters[n]->ChangePass(n, imgIn[0]->frames);
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
    imgOut = SetupAuxNSame(imgIn, imgOut);

    //Pass 0
    filters[0]->ChangePass(0, imgIn[0]->frames);

    if(parallel) {
        filters[0]->ProcessP(imgIn, imgTmpSame[0]);
    } else {
        filters[0]->Process(imgIn, imgTmpSame[0]);
    }

    for(unsigned int i = 1; i < filters.size(); i++) {
        filters[i]->ChangePass(i, imgIn[0]->frames);

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

    CheckSame(imgIn);

    if(bSame) {
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

