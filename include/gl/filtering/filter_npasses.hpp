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

#include "../../util/std_util.hpp"

#include "../../util/array.hpp"

#include "../../util/gl/fbo.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLNPasses class
 */
class FilterGLNPasses: public FilterGL
{
protected:
    ImageGL *imgAllocated;
    ImageGL *imgTmpSame[2];
    ImageGLVec imgTmp;

    /**
     * @brief PreProcess
     * @param imgIn
     * @param imgOut
     */
    virtual void PreProcess(ImageGLVec imgIn, ImageGL *imgOut){}

    /**
     * @brief setupAuxNGen
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *setupAuxNGen(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief setupAuxNSame
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *setupAuxNSame(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief getFilter
     * @param i
     * @return
     */
    virtual FilterGL* getFilter(int i);

    /**
     * @brief getIterations
     * @return
     */
    virtual int getIterations();

    /**
     * @brief releaseAux
     */
    void releaseAux();

    /**
     * @brief ProcessGen
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *ProcessGen(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief ProcessSame
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *ProcessSame(ImageGLVec imgIn, ImageGL *imgOut);

public:

    /**
     * @brief FilterGLNPasses
     */
    FilterGLNPasses();

    ~FilterGLNPasses();

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param frames
     * @param channels
     */
    void OutputSize(ImageGLVec imgIn, int &width, int &height, int &frames, int &channels);

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

PIC_INLINE FilterGLNPasses::FilterGLNPasses() : FilterGL()
{
    imgAllocated = NULL;

    for(int i = 0; i < 2; i++) {
        imgTmpSame[i] = NULL;
    }

    target = GL_TEXTURE_2D;
}

PIC_INLINE FilterGLNPasses::~FilterGLNPasses()
{
    release();
}

PIC_INLINE void FilterGLNPasses::releaseAux()
{
    delete_s(imgAllocated);

    imgTmpSame[0] = NULL;
    imgTmpSame[1] = NULL;

    stdVectorClear<ImageGL>(imgTmp);
}

PIC_INLINE FilterGL* FilterGLNPasses::getFilter(int i)
{
    int j = i % filters.size();
    return filters[j];
}

PIC_INLINE int FilterGLNPasses::getIterations()
{
    return int(filters.size());
}

PIC_INLINE void FilterGLNPasses::OutputSize(ImageGLVec imgIn, int &width, int &height, int &frames, int &channels)
{
    ImageGL *imgIn0 = new ImageGL(imgIn[0], false);

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
        imgIn0->allocateAux();
    }

    imgIn[0] = tmp;

    delete imgIn0;
}

PIC_INLINE ImageGL *FilterGLNPasses::setupAuxNGen(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    int width, height, frames, channels;
    OutputSize(imgIn, width, height, frames, channels);

    int n = getIterations();

    if(imgTmp.empty()) {
        setToANullVector<ImageGL>(imgTmp, n);
    } else {
        int tw, th, tf, tc;

        filters[0]->OutputSize(imgIn, tw, th, tf, tc);

        if(tw != imgTmp[0]->width ||
           th != imgTmp[0]->height ||
           tf != imgTmp[0]->frames ||
           tc != imgTmp[0]->channels) {
            stdVectorClear<ImageGL>(imgTmp);

            setToANullVector<ImageGL>(imgTmp, n);
        }
    }

    //output
    if(imgOut == NULL) {
        imgOut = new ImageGL(frames, width, height, channels, IMG_GPU, GL_TEXTURE_2D);
    } else {
        if(imgOut->height != height ||
           imgOut->width != width ||
           imgOut->channels != channels ||
           imgOut->frames != frames) {
           imgOut = new ImageGL(frames, width, height, channels, IMG_GPU, GL_TEXTURE_2D);
        }
    }

    return imgOut;
}

PIC_INLINE ImageGL *FilterGLNPasses::setupAuxNSame(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->allocateSimilarOneGL();
    } else {
        if(!imgOut->isSimilarType(imgIn[0])) {
            imgOut = imgIn[0]->allocateSimilarOneGL();
        }
    }

    if(imgAllocated == NULL) {
        imgAllocated = imgIn[0]->allocateSimilarOneGL();
    } else {
        if(!imgAllocated->isSimilarType(imgIn[0])) {
            delete imgAllocated;
            imgAllocated = imgIn[0]->allocateSimilarOneGL();
        }
    }

    int nIterations = getIterations();
    if((nIterations % 2) == 0) {
        imgTmpSame[0] = imgAllocated;
        imgTmpSame[1] = imgOut;
    } else {
        imgTmpSame[0] = imgOut;
        imgTmpSame[1] = imgAllocated;
    }

    return imgOut;
}

PIC_INLINE ImageGL *FilterGLNPasses::ProcessGen(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.empty() || filters.empty()) {
        return imgOut;
    }

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

PIC_INLINE ImageGL *FilterGLNPasses::ProcessSame(ImageGLVec imgIn, ImageGL *imgOut)
{
    if((imgIn.size() <= 0) || (filters.size() < 1)) {
        return imgOut;
    }

    //setup
    imgOut = setupAuxNSame(imgIn, imgOut);

    int n = getIterations();
    auto flt_0 = getFilter(0);
    flt_0->changePass(0, n);
    flt_0->Process(imgIn, imgTmpSame[0]);

    for(auto i = 1; i < n; i++) {
        auto flt_i = getFilter(0);
        flt_i->changePass(i, n);

        imgIn[0] = imgTmpSame[(i + 1) % 2];
        flt_i->Process(imgIn, imgTmpSame[i % 2]);
    }

    return imgOut;
}

PIC_INLINE ImageGL *FilterGLNPasses::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn.empty() || filters.empty()) {
        return imgOut;
    }

    PreProcess(imgIn, imgOut);

    int width, height, frames, channels;
    OutputSize(imgIn, width, height, frames, channels);

    bool bSame = (imgIn[0]->width == width) &&
                 (imgIn[0]->height == height) &&
                 (imgIn[0]->channels == channels) &&
                 (imgIn[0]->frames == frames);

    if(bSame) {
        imgOut = ProcessSame(imgIn, imgOut);
    } else {
        imgOut = ProcessGen(imgIn, imgOut);
    }

    return imgOut;
}

/*
if(fbo==NULL)
    fbo = new Fbo();

fbo->create(imgOut->width,imgOut->height,imgOut->frames, false, imgOut->getTexture());

for(unsigned int i=0;i<filters.size();i++)
    filters[i]->setFbo(fbo);*/

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_NPASSES_HPP */

