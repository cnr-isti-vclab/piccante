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

#ifndef PIC_GL_ALGORITHMS_PYRAMID_HPP
#define PIC_GL_ALGORITHMS_PYRAMID_HPP

#include "gl/image_raw.hpp"

#include "gl/filtering/filter_gaussian_2d.hpp"
#include "gl/filtering/filter_sampler_2d.hpp"
#include "gl/filtering/filter_op.hpp"

namespace pic {

/**
 * @brief The PyramidGL class
 */
class PyramidGL
{
protected:
    bool					lapGauss;
    int						limitLevel;

    FilterGLGaussian2D		*fltG;
    FilterGLSampler2D		*fltS;
    FilterGLOp				*fltSub, *fltId, *fltMul, *fltAdd;

    void InitFilters()
    {
        fltG = new FilterGLGaussian2D(1.0f);
        fltS = new FilterGLSampler2D(0.5f);
        fltSub  = FilterGLOp::CreateOpSub(false);
        fltMul  = FilterGLOp::CreateOpMul(false);
        fltAdd  = FilterGLOp::CreateOpAdd(false);
        fltId   = FilterGLOp::CreateOpIdentity(false);
    }

    std::vector<ImageRAWGL *> trackerRec, trackerUp;

    void Create(ImageRAWGL *img, bool lapGauss, int limitLevel);

public:
    std::vector<ImageRAWGL *>	stack;

    /**
     * @brief PyramidGL
     * @param img
     * @param lapGauss
     * @param limitLevel
     */
    PyramidGL(ImageRAWGL *img, bool lapGauss, int limitLevel);

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
    void Update(ImageRAWGL *img);

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
    ImageRAWGL *Reconstruct(ImageRAWGL *imgOut);
};

PyramidGL::PyramidGL(ImageRAWGL *img, bool lapGauss, int limitLevel = 0)
{
    Create(img, lapGauss, limitLevel);
}

PyramidGL::PyramidGL(int width, int height, int channels, bool lapGauss, int limitLevel = 0)
{
    ImageRAWGL *tmpImg = new ImageRAWGL(1, width, height, channels, IMG_GPU, GL_TEXTURE_2D);

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

void PyramidGL::Create(ImageRAWGL *img, bool lapGauss, int limitLevel = 0)
{
    this->lapGauss = lapGauss;
    this->limitLevel  = limitLevel;

    InitFilters();

    ImageRAWGL *tmpImg = img;
    ImageRAWGL *tmpG   = NULL;
    ImageRAWGL *tmpD   = NULL;

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

ImageRAWGL *PyramidGL::Reconstruct(ImageRAWGL *imgOut)
{
    if(stack.size() < 2) {
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(1, stack[0]->width, stack[0]->height,
                                stack[0]->channels, IMG_GPU, GL_TEXTURE_2D);
    }

    int n = stack.size() - 1;
    ImageRAWGL *tmp = stack[n];

    if(trackerRec.empty()) {
        for(int i = n; i >= 2; i--) {
            ImageRAWGL *tmp2 = fltAdd->Process(DoubleGL(stack[i - 1], tmp), NULL);
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

    fltAdd->Process(DoubleGL(stack[0], tmp), imgOut);

    return imgOut;
}

void PyramidGL::Mul(const PyramidGL *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        fltMul->Process(DoubleGL(stack[i], pyr->stack[i]), stack[i]);
    }
}

void PyramidGL::Add(const PyramidGL *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        fltAdd->Process(DoubleGL(stack[i], pyr->stack[i]), stack[i]);
    }
}

void PyramidGL::Blend(PyramidGL *pyr, PyramidGL *weight)
{
    if(stack.size() != pyr->stack.size() ||
       stack.size() != weight->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        //fltBlend->Process(Triple(stack[i], pyr->stack[i], weight->stack[i]), stack[i]);
    }
}

void PyramidGL::Update(ImageRAWGL *img)
{
    if(img == NULL) {
        return;
    }

    if(stack.size() < 1) {
        return;
    }

    ImageRAWGL *tmpImg = img;

    int levels = MAX(log2(MIN(tmpImg->width, tmpImg->height)) - limitLevel, 1);

    for(int i = 0; i < (levels - 1); i++) {
        fltG->Process(SingleGL(tmpImg),		stack[i]);

        fltS->Process(SingleGL(stack[i]),	trackerUp[i]);

        if(lapGauss) {	//Laplacian Pyramid
            fltSub->Process(DoubleGL(tmpImg, trackerUp[i]), stack[i]);
        } else {		//Gaussian Pyramid
            fltId->Process(SingleGL(tmpImg), stack[i]);
        }

        tmpImg = trackerUp[i];
    }

    if(tmpImg != NULL) {
        fltId->Process(SingleGL(tmpImg), stack[levels - 1]);
    }
}

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_PYRAMID_HPP */

