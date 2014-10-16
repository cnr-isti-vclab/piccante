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

#ifndef PIC_ALGORITHMS_PYRAMID_HPP
#define PIC_ALGORITHMS_PYRAMID_HPP

#include "image.hpp"
#include "filtering/filter_gaussian_2d.hpp"
#include "filtering/filter_sampler_2d.hpp"
#include "filtering/filter_sampler_2dsub.hpp"
#include "filtering/filter_sampler_2dadd.hpp"

namespace pic {

/**
 * @brief The Pyramid class
 */
class Pyramid
{
protected:
    bool    lapGauss;
    int     limitLevel;

    std::vector<Image *> trackerRec, trackerUp;

    void Create(Image *img, bool lapGauss, int limitLevel);

public:
    std::vector<Image *>  stack;

    /**
     * @brief Pyramid
     * @param img
     * @param lapGauss
     * @param limitLevel
     */
    Pyramid(Image *img, bool lapGauss, int limitLevel);

    /**
     * @brief Pyramid
     * @param width
     * @param height
     * @param channels
     * @param lapGauss
     * @param limitLevel
     */
    Pyramid(int width, int height, int channels, bool lapGauss, int limitLevel);

    ~Pyramid();

    /**
     * @brief SetLapGauss
     * @param lapGauss
     */
    void SetLapGauss(bool lapGauss)
    {
        this->lapGauss = lapGauss;
    }

    /**
     * @brief Update recomputes the pyramid given a compatible image, img.
     * @param img
     */
    void Update(Image *img);

    /**
     * @brief Mul is the mul operator ( *= ) between pyramids.
     * @param pyr
     */
    void Mul(const Pyramid *pyr);

    /**
     * @brief Mul is the add operator ( += ) between pyramids.
     * @param pyr
     */
    void Add(const Pyramid *pyr);

    /**
     * @brief Blend
     * @param pyr
     * @param weight
     */
    void Blend(Pyramid *pyr, Pyramid *weight);

    /**
     * @brief Reconstruct evaluates a Gaussian/Laplacian pyramid.
     * @param imgOut
     * @return
     */
    Image *Reconstruct(Image *imgOut);

    /**
     * @brief size
     * @return
     */
    int size()
    {
        return stack.size();
    }

    /**
     * @brief get
     * @param index
     * @return
     */
    Image *get(int index)
    {
        return stack[index % stack.size()];
    }
};

Pyramid::Pyramid(Image *img, bool lapGauss, int limitLevel = 0)
{
    Create(img, lapGauss, limitLevel);
}


Pyramid::Pyramid(int width, int height, int channels, bool lapGauss, int limitLevel = 0)
{
    Image *tmpImg = new Image(1, width, height, channels);
    tmpImg->SetZero();

    Create(tmpImg, lapGauss, limitLevel);

    delete tmpImg;

#ifdef PIC_DEBUG
    printf("Pyramid size: %lu\n", stack.size());
#endif
}

Pyramid::~Pyramid()
{
    for(unsigned int i = 0; i < stack.size(); i++) {
        if(stack[i] != NULL) {
            delete stack[i];
        }
    }
}

void Pyramid::Create(Image *img, bool lapGauss, int limitLevel = 0)
{
    this->lapGauss  = lapGauss;
    this->limitLevel = limitLevel;

    FilterGaussian2D	fltG(1.0f);
    FilterSampler2D		fltS(0.5f);
    FilterSampler2DSub	fSub;

    Image *tmpImg = img;

    int levels = MAX(log2(MIN(img->width, img->height)) - limitLevel, 1);

    Image *tmpG = NULL;
    Image *tmpD = NULL;

    for(int i = 0; i < levels; i++) {
        tmpG = fltG.ProcessP(Single(tmpImg), NULL);
        tmpD = fltS.ProcessP(Single(tmpG), NULL);

        if(lapGauss) {	//Laplacian Pyramid
            Image *tmpDiff = fSub.ProcessP(Double(tmpImg, tmpD), tmpG);
            stack.push_back(tmpDiff);
        } else {			//Gaussian Pyramid
            tmpG->Assign(tmpImg);
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
    printf("Pyramid size: %lu\n", stack.size());
#endif
}

Image *Pyramid::Reconstruct(Image *imgOut)
{
    if(stack.size() < 2) {
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = stack[0]->AllocateSimilarOne();
    }

    FilterSampler2DAdd fltAdd;
    int n = stack.size() - 1;
    Image *tmp = stack[n];

    if(trackerRec.empty()) {
        for(int i = n; i >= 2; i--) {
            Image *tmp2 = fltAdd.ProcessP(Double(stack[i - 1], tmp), NULL);
            trackerRec.push_back(tmp2);
            tmp = tmp2;
        }
    } else {
        int c = 0;

        for(int i = n; i >= 2; i--) {
            tmp = fltAdd.ProcessP(Double(stack[i - 1], tmp), trackerRec[c]);
            c++;
        }
    }

    fltAdd.ProcessP(Double(stack[0], tmp), imgOut);

    return imgOut;
}

void Pyramid::Update(Image *img)
{
    //TODO: check if the image and the pyramid are compatible
    if(img == NULL) {
        return;
    }

    if(!stack[0]->SimilarType(img)) {
        return;
    }

    FilterGaussian2D	fltG(1.0f);
    FilterSampler2D		fltS(0.5f);
    FilterSampler2DSub	fSub;

    Image *tmpImg = img;

    unsigned int levels = MAX(log2(MIN(img->width, img->height)) - limitLevel, 1);

    for(unsigned int i = 0; i < (levels - 1); i++) {
        fltG.ProcessP(Single(tmpImg), stack[i]);

        fltS.ProcessP(Single(stack[i]), trackerUp[i]);

        if(lapGauss) {	//Laplacian Pyramid
            stack[i] = fSub.ProcessP(Double(tmpImg, trackerUp[i]), stack[i]);
        } else {		//Gaussian Pyramid
            stack[i]->Assign(tmpImg);
        }

        tmpImg = trackerUp[i];
    }

    if(tmpImg != NULL) {
        stack[levels - 1]->Assign(tmpImg);
    }
}

void Pyramid::Mul(const Pyramid *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        if(pyr->stack[i]->channels == 1) {
            stack[i]->MulS(pyr->stack[i]);
        } else {
            stack[i]->Mul(pyr->stack[i]);
        }
    }
}

void Pyramid::Add(const Pyramid *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        stack[i]->Add(pyr->stack[i]);
    }
}

void Pyramid::Blend(Pyramid *pyr, Pyramid *weight)
{
    if((stack.size() != pyr->stack.size()) && (pyr->stack.size() > 0)) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        stack[i]->Blend(pyr->stack[i], weight->stack[i]);
    }
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_PYRAMID_HPP */

