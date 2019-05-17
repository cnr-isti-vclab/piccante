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

#include "../util/std_util.hpp"

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
    Image *setupAuxNGen(ImageVec imgIn, Image *imgOut);

    /**
     * @brief setupAuxNSame
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *setupAuxNSame(ImageVec imgIn, Image *imgOut);

    /**
     * @brief getFilter
     * @param i
     * @return
     */
    virtual Filter* getFilter(int i);

    /**
     * @brief getIterations
     * @return
     */
    virtual int getIterations();

    /**
     * @brief release
     */
    void release();

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

public:

    /**
     * @brief FilterNPasses
     */
    FilterNPasses();

    ~FilterNPasses();

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut);
};

PIC_INLINE FilterNPasses::FilterNPasses() : Filter()
{
    imgAllocated = NULL;

    for(int i = 0; i < 2; i++) {
        imgTmpSame[i] = NULL;
    }

    imgTmp.clear();
}

PIC_INLINE FilterNPasses::~FilterNPasses()
{
    release();
}

PIC_INLINE void FilterNPasses::release()
{
    if(imgAllocated != NULL) {
        delete imgAllocated;
        imgAllocated = NULL;
    }

    imgTmpSame[0] = NULL;
    imgTmpSame[1] = NULL;

    stdVectorClear<Image>(imgTmp);
}

PIC_INLINE Filter* FilterNPasses::getFilter(int i)
{
    int j = i % filters.size();
    return filters[j];
}

PIC_INLINE int FilterNPasses::getIterations()
{
    return int(filters.size());
}

PIC_INLINE void FilterNPasses::OutputSize(ImageVec imgIn, int &width, int &height, int &frames, int &channels)
{
    Image *imgIn0 = new Image(imgIn[0], false);

    auto *tmp = imgIn[0];
    imgIn[0] = imgIn0;

    int n = getIterations();

    for(int i = 0; i < n; i++) {
        auto flt_i = getFilter(i);
        flt_i->changePass(i, n);
        flt_i->OutputSize(imgIn, width, height, channels, frames);

        imgIn0->width = width;
        imgIn0->height = height;
        imgIn0->channels = channels;
        imgIn0->frames = frames;
    }

    imgIn[0] = tmp;

    delete imgIn0;
}

PIC_INLINE Image *FilterNPasses::setupAuxNGen(ImageVec imgIn,
        Image *imgOut)
{   
    int width, height, frames, channels;
    OutputSize(imgIn, width, height, frames, channels);

    int n = getIterations();

    if(imgTmp.empty()) {
        setToANullVector<Image>(imgTmp, n);
    } else {
        int tw, th, tf, tc;

        filters[0]->OutputSize(imgIn, tw, th, tf, tc);

        if(tw != imgTmp[0]->width ||
           th != imgTmp[0]->height ||
           tf != imgTmp[0]->frames ||
           tc != imgTmp[0]->channels) {

            stdVectorClear<Image>(imgTmp);

            setToANullVector<Image>(imgTmp, n);
        }
    }

    //output
    if(imgOut == NULL) {
        imgOut = new Image(frames, width, height, channels);
    } else {
        if(imgOut->height != height ||
           imgOut->width != width ||
           imgOut->channels != channels ||
           imgOut->frames != frames) {
           imgOut = new Image(frames, width, height, channels);
        }
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::setupAuxNSame(ImageVec imgIn,
        Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->allocateSimilarOne();
    } else {
        if(!imgOut->isSimilarType(imgIn[0])) {
            imgOut = imgIn[0]->allocateSimilarOne();
        }
    }

    if(imgAllocated == NULL) {
        imgAllocated = imgIn[0]->allocateSimilarOne();
    } else {
        if(!imgAllocated->isSimilarType(imgIn[0])) {
            delete imgAllocated;
            imgAllocated = imgIn[0]->allocateSimilarOne();
        }
    }

    if((getIterations() % 2) == 0) {
        imgTmpSame[0] = imgAllocated;
        imgTmpSame[1] = imgOut;
    } else {
        imgTmpSame[0] = imgOut;
        imgTmpSame[1] = imgAllocated;
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::ProcessGen(ImageVec imgIn, Image *imgOut,
        bool parallel = false)
{
    imgOut = setupAuxNGen(imgIn, imgOut);

    int n = getIterations();
    int n2 = n - 1;
    
    for(int i = 0; i < n2; i++) {
        auto flt_i = getFilter(i);
        flt_i->changePass(i, n);

        imgTmp[i] = flt_i->Process(imgIn, imgTmp[i]);

        imgIn[0] = imgTmp[i];
    }

    auto flt_n = getFilter(n2);
    flt_n->changePass(n2, n);

    imgOut = filters[n2]->Process(imgIn, imgOut);

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::ProcessSame(ImageVec imgIn, Image *imgOut,
        bool parallel = false)
{
    //setup
    imgOut = setupAuxNSame(imgIn, imgOut);

    int n = getIterations();
    auto flt_0 = getFilter(0);
    flt_0->changePass(0, n);

    flt_0->Process(imgIn, imgTmpSame[0]);

    for(int i = 1; i < n; i++) {
        auto flt_i = getFilter(i);
        flt_i->changePass(i, n);

        imgIn[0] = imgTmpSame[(i + 1) % 2];

        flt_i->Process(imgIn, imgTmpSame[i % 2]);
    }

    return imgOut;
}

PIC_INLINE Image *FilterNPasses::Process(ImageVec imgIn, 
        Image *imgOut)
{
    if(imgIn.empty() || filters.empty()) {
        return imgOut;
    }

    PreProcess(imgIn, imgOut);

    int width, height, frames, channels;
    OutputSize(imgIn, width, height, frames, channels);

    bool bSame = (imgIn[0]->width == width) &&
                 (imgIn[0]->height == height) &&
                 (imgIn[0]->frames == frames) &&
                 (imgIn[0]->channels == channels);

    if(bSame) {
        imgOut = ProcessSame(imgIn, imgOut);
    } else {
        imgOut = ProcessGen(imgIn, imgOut);
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NPASSES_HPP */

