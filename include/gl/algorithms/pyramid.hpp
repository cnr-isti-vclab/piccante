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

#ifndef PIC_GL_ALGORITHMS_PYRAMID_HPP
#define PIC_GL_ALGORITHMS_PYRAMID_HPP

#include "../../base.hpp"

#include "../../gl/image.hpp"

#include "../../gl/filtering/filter_gaussian_2d.hpp"
#include "../../gl/filtering/filter_sampler_2d.hpp"
#include "../../gl/filtering/filter_blend.hpp"
#include "../../gl/filtering/filter_op.hpp"

namespace pic {

/**
 * @brief The PyramidGL class
 */
class PyramidGL
{
protected:
    bool                lapGauss;
    int                 limitLevel;

    FilterGLGaussian2D  *flt_gauss;
    FilterGLSampler2D   *flt_sampler;
    FilterGLOp		*flt_add, *flt_sub;
    FilterGLBlend       *flt_blend;
    std::vector<FilterGL *> filters;

    ImageGLVec          trackerRec, trackerUp;

    /**
     * @brief initFilters
     */
    void initFilters();

    /**
     * @brief create
     * @param img
     * @param width
     * @param height
     * @param channels
     * @param lapGauss
     * @param limitLevel
     */
    void create(ImageGL *img, bool lapGauss, int limitLevel);

public:

    ImageGLVec stack;

    /**
     * @brief PyramidGL
     * @param img
     * @param lapGauss
     * @param limitLevel
     */
    PyramidGL(ImageGL *img, bool lapGauss, int limitLevel);

    /**
     * @brief PyramidGL
     * @param width
     * @param height
     * @param channels
     * @param lapGauss
     * @param limitLevel
     */
    PyramidGL(int width, int height, int channels, bool lapGauss, int limitLevel);

    ~PyramidGL();

    /**
     * @brief update
     * @param img
     */
    void update(ImageGL *img);

    /**
     * @brief setValue
     * @param value
     */
    void setValue(float value);

    /**
     * @brief mul
     * @param pyr
     */
    void mul(const PyramidGL *pyr);

    /**
     * @brief mulNeg
     * @param pyr
     */
    void mulNeg(const PyramidGL *pyr);

    /**
     * @brief add
     * @param pyr
     */
    void add(const PyramidGL *pyr);

    /**
     * @brief blend
     * @param pyr
     * @param weight
     */
    void blend(PyramidGL *pyr, PyramidGL *weight);

    /**
     * @brief reconstruct
     * @param imgOut
     * @return
     */
    ImageGL *reconstruct(ImageGL *imgOut);
};

PIC_INLINE PyramidGL::PyramidGL(ImageGL *img, bool lapGauss, int limitLevel = 1)
{
    flt_gauss = NULL;
    flt_sampler = NULL;
    flt_add = NULL;
    flt_sub = NULL;
    flt_blend = NULL;

    if(img != NULL) {
        create(img, lapGauss, limitLevel);
    }
}

PIC_INLINE PyramidGL::PyramidGL(int width, int height, int channels, bool lapGauss, int limitLevel = 1)
{
    flt_gauss = NULL;
    flt_sampler = NULL;
    flt_add = NULL;
    flt_sub = NULL;
    flt_blend = NULL;

    ImageGL *img = new ImageGL(1, width, height, channels, IMG_GPU, GL_TEXTURE_2D);
    *img = 0.0f;

    create(img, lapGauss, limitLevel);

    delete img;
}

PIC_INLINE PyramidGL::~PyramidGL()
{
    stdVectorClear<ImageGL>(stack);
    stdVectorClear<ImageGL>(trackerUp);
    stdVectorClear<ImageGL>(trackerRec);
    stdVectorClear<FilterGL>(filters);
}

PIC_INLINE void PyramidGL::initFilters()
{
    if(flt_gauss == NULL) {
        flt_gauss = new FilterGLGaussian2D(1.0f);
        filters.push_back(flt_gauss);
    }

    if(flt_sampler == NULL) {
        flt_sampler = new FilterGLSampler2D(0.5f);
        filters.push_back(flt_sampler);
    }

    if(flt_add == NULL) {
        flt_add  = FilterGLOp::CreateOpAdd(false);
        filters.push_back(flt_add);
    }

    if(flt_sub == NULL) {
        flt_sub  = FilterGLOp::CreateOpSub(false);
        filters.push_back(flt_sub);
    }

    if(flt_blend == NULL) {
        flt_blend = new FilterGLBlend();
        filters.push_back(flt_blend);
    }
}

PIC_INLINE void PyramidGL::create(ImageGL *img, bool lapGauss, int limitLevel = 1)
{
    this->lapGauss = lapGauss;

    if(limitLevel < 1) {
        limitLevel = 1;
    }

    this->limitLevel  = limitLevel;

    initFilters();

    int levels = MAX(log2(MIN(img->width, img->height)) - limitLevel, 1);

    ImageGL *tmpImg = img;
    ImageGL *tmpG   = NULL;
    ImageGL *tmpD   = NULL;

    //normal image case
    for(int i = 0; i < levels; i++) {

        tmpG = flt_gauss->Process(SingleGL(tmpImg), NULL);

        tmpD = flt_sampler->Process(SingleGL(tmpG), NULL);

        if(lapGauss) { //Laplacian Pyramid
            flt_sub->Process(DoubleGL(tmpImg, tmpD), tmpG);

            stack.push_back(tmpG);
        } else { //Gaussian Pyramid
            *tmpG = *tmpImg;

            stack.push_back(tmpG);
        }

        if(i < (levels - 1)) {
            trackerUp.push_back(tmpD);
        }

        tmpImg = tmpD;
    }

    if(tmpD != NULL) {
        stack.push_back(tmpD);
    }

#ifdef PIC_DEBUG
    printf("PyramidGL size: %lu\n", stack.size());
#endif
}

PIC_INLINE void PyramidGL::update(ImageGL *img)
{
    if(img == NULL) {
        return;
    }

    if(stack.empty()) {
        return;
    }

    if(!stack[0]->isSimilarType(img)) {
        return;
    }

    ImageGL *tmpG = NULL;
    ImageGL *tmpD = NULL;
    ImageGL *tmpImg = img;

    int levels = MAX(log2(MIN(tmpImg->width, tmpImg->height)) - limitLevel, 1);

    for(int i = 0; i < levels; i++) {
        tmpG = flt_gauss->Process(SingleGL(tmpImg), stack[i]);

        if(i == (levels - 1) ) {
            tmpD = flt_sampler->Process(SingleGL(tmpG), stack[i + 1]);
        } else {
            tmpD = flt_sampler->Process(SingleGL(tmpG), trackerUp[i]);
        }

        if(lapGauss) { //Laplacian Pyramid
            flt_sub->Process(DoubleGL(tmpImg, tmpD), tmpG);
        } else { //Gaussian Pyramid
            *tmpG = *tmpImg;
        }

        tmpImg = tmpD;
    }
}

PIC_INLINE ImageGL *PyramidGL::reconstruct(ImageGL *imgOut)
{
    if(stack.size() < 2) {
        return imgOut;
    }

    int n = int(stack.size()) - 1;
    ImageGL *tmp = stack[n];

    if(trackerRec.empty()) {
        for(int i = n; i >= 2; i--) {
            ImageGL *tmp2 = flt_add->Process(DoubleGL(stack[i - 1], tmp), NULL);
            trackerRec.push_back(tmp2);
            tmp = tmp2;
        }
    } else {
        int c = 0;

        for(int i = n; i >= 2; i--) {
            flt_add->Process(DoubleGL(stack[i - 1], tmp), trackerRec[c]);
            tmp = trackerRec[c];
            c++;
        }
    }

    imgOut = flt_add->Process(DoubleGL(stack[0], tmp), imgOut);

    return imgOut;
}

PIC_INLINE void PyramidGL::setValue(float value)
{
    if(stack.empty()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        *stack[i] = value;
    }
}

PIC_INLINE void PyramidGL::mul(const PyramidGL *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        *stack[i] *= *pyr->stack[i];
    }
}

PIC_INLINE void PyramidGL::add(const PyramidGL *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        *stack[i] += *pyr->stack[i];
    }
}

PIC_INLINE void PyramidGL::blend(PyramidGL *pyr, PyramidGL *weight)
{
    if(stack.size() != pyr->stack.size() ||
       stack.size() != weight->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        flt_blend->Process(TripleGL(stack[i], pyr->stack[i], weight->stack[i]), stack[i]);
    }
}

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_PYRAMID_HPP */

