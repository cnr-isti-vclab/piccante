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

#include "gl/image.hpp"

#include "gl/filtering/filter_gaussian_2d.hpp"
#include "gl/filtering/filter_sampler_2d.hpp"
#include "gl/filtering/filter_blend.hpp"
#include "gl/filtering/filter_op.hpp"

namespace pic {

/**
 * @brief The PyramidGL class
 */
class PyramidGL
{
protected:
    bool                lapGauss;
    int                 limitLevel;

    FilterGLGaussian2D  *fltG;
    FilterGLSampler2D   *fltS;
    FilterGLOp			*fltAdd, *fltSub, *fltMul, *fltId;
    FilterGLBlend       *fltBlend;

    ImageGLVec          trackerRec, trackerUp;

    /**
     * @brief InitFilters
     */
    void InitFilters()
    {
        fltG = new FilterGLGaussian2D(1.0f);
        fltS = new FilterGLSampler2D(0.5f);
        fltId   = FilterGLOp::CreateOpIdentity(false);
        fltAdd  = FilterGLOp::CreateOpAdd(false);
        fltMul  = FilterGLOp::CreateOpMul(false);
        fltSub  = FilterGLOp::CreateOpSub(false);
        fltBlend = new FilterGLBlend();
    }

    /**
     * @brief Create
     * @param img
     * @param lapGauss
     * @param limitLevel
     */
    void Create(ImageGL *img, bool lapGauss, int limitLevel);

public:
    std::vector<ImageGL *>	stack;

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
     * @brief Update
     * @param img
     */
    void Update(ImageGL *img);

    /**
     * @brief SetValue
     * @param value
     */
    void SetValue(float value);

    /**
     * @brief Mul
     * @param pyr
     */
    void Mul(const PyramidGL *pyr);

    /**
     * @brief MulNeg
     * @param pyr
     */
    void MulNeg(const PyramidGL *pyr);

    /**
     * @brief Add
     * @param pyr
     */
    void Add(const PyramidGL *pyr);

    /**
     * @brief Blend
     * @param pyr
     * @param weight
     */
    void Blend(PyramidGL *pyr, PyramidGL *weight);

    /**
     * @brief Reconstruct
     * @param imgOut
     * @return
     */
    ImageGL *Reconstruct(ImageGL *imgOut);
};

PyramidGL::PyramidGL(ImageGL *img, bool lapGauss, int limitLevel = 0)
{
    Create(img, lapGauss, limitLevel);
}

PyramidGL::PyramidGL(int width, int height, int channels, bool lapGauss, int limitLevel = 0)
{
    ImageGL *tmpImg = new ImageGL(1, width, height, channels, IMG_GPU, GL_TEXTURE_2D);
    *tmpImg = 0.0f;

    Create(tmpImg, lapGauss, limitLevel);
}

PyramidGL::~PyramidGL()
{
    for(unsigned int i = 0; i < stack.size(); i++) {
        if(stack[i] != NULL) {
            delete stack[i];
        }
    }
}

void PyramidGL::Create(ImageGL *img, bool lapGauss, int limitLevel = 0)
{
    this->lapGauss = lapGauss;
    this->limitLevel  = limitLevel;

    InitFilters();

    ImageGL *tmpImg = img;
    ImageGL *tmpG   = NULL;
    ImageGL *tmpD   = NULL;

    int levels = MAX(log2(MIN(tmpImg->width, tmpImg->height)) - limitLevel, 1);

    for(int i = 0; i < levels; i++) {
        tmpG = fltG->Process(SingleGL(tmpImg), NULL);
        tmpD = fltS->Process(SingleGL(tmpG), NULL);

        if(lapGauss) {  //Laplacian Pyramid
            fltSub->Process(DoubleGL(tmpImg, tmpD), tmpG);
            stack.push_back(tmpG);
        } else {        //Gaussian Pyramid
            fltId->Process(SingleGL(tmpImg), tmpG);
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

void PyramidGL::Update(ImageGL *img)
{
    if(img == NULL) {
        return;
    }

    if(stack.empty()) {
        return;
    }

    if(!stack[0]->SimilarType(img)) {
        return;
    }

    ImageGL *tmpImg = img;

    int levels = MAX(log2(MIN(tmpImg->width, tmpImg->height)) - limitLevel, 1);

    for(int i = 0; i < levels; i++) {
        stack[i] = fltG->Process(SingleGL(tmpImg), stack[i]);
        trackerUp[i] = fltS->Process(SingleGL(stack[i]), trackerUp[i]);

        if(lapGauss) {	//Laplacian Pyramid
            stack[i] = fltSub->Process(DoubleGL(tmpImg, trackerUp[i]), stack[i]);
        } else {		//Gaussian Pyramid
            stack[i] = fltId->Process(SingleGL(tmpImg), stack[i]);
        }

        tmpImg = trackerUp[i];
    }

    fltId->Process(SingleGL(tmpImg), stack[stack.size() - 1]);

}

ImageGL *PyramidGL::Reconstruct(ImageGL *imgOut)
{
    if(stack.size() < 2) {
        return imgOut;
    }

    int n = stack.size() - 1;
    ImageGL *tmp = stack[n];

    if(trackerRec.empty()) {
        for(int i = n; i >= 2; i--) {
            ImageGL *tmp2 = fltAdd->Process(DoubleGL(stack[i - 1], tmp), NULL);
            trackerRec.push_back(tmp2);
            tmp = tmp2;
        }
    } else {
        int c = 0;

        for(int i = n; i >= 2; i--) {
            tmp = fltAdd->Process(DoubleGL(stack[i - 1], tmp), trackerRec[c]);
            c++;
        }
    }

    imgOut = fltAdd->Process(DoubleGL(stack[0], tmp), imgOut);

    return imgOut;
}

void PyramidGL::SetValue(float value)
{
    if(stack.empty()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        *stack[i] = value;
    }
}

void PyramidGL::Mul(const PyramidGL *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        *stack[i] *= *pyr->stack[i];
        //fltMul->Process(DoubleGL(stack[i], pyr->stack[i]), stack[i]);
    }
}

void PyramidGL::Add(const PyramidGL *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        *stack[i] += *pyr->stack[i];
//        fltAdd->Process(DoubleGL(stack[i], pyr->stack[i]), stack[i]);
    }
}

void PyramidGL::Blend(PyramidGL *pyr, PyramidGL *weight)
{
    if(stack.size() != pyr->stack.size() ||
       stack.size() != weight->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        fltBlend->Process(TripleGL(stack[i], pyr->stack[i], weight->stack[i]), stack[i]);
    }
}

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_PYRAMID_HPP */

