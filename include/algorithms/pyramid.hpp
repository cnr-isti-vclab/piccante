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

#include "image_raw.hpp"
#include "filtering/filter_gaussian_2d.hpp"
#include "filtering/filter_sampler_2d.hpp"
#include "filtering/filter_sampler_2dsub.hpp"
#include "filtering/filter_sampler_2dadd.hpp"

namespace pic {

class Pyramid
{
protected:
    bool    lapGauss;
    int     limitLevel;

    std::vector<ImageRAW *> trackerRec, trackerUp;

    void Create(ImageRAW *img, bool lapGauss, int limitLevel);

public:
    std::vector<ImageRAW *>  stack;

    Pyramid(ImageRAW *img, bool lapGauss, int limitLevel);
    Pyramid(int width, int height, int channels, bool lapGauss, int limitLevel);

    ~Pyramid();

    void SetLapGauss(bool lapGauss)
    {
        this->lapGauss = lapGauss;
    }

    void Update(ImageRAW *img);
    void Mul(const Pyramid *pyr);
    void MulSNeg(const Pyramid *pyr);
    void Add(const Pyramid *pyr);
    void Blend(Pyramid *pyr, Pyramid *weight);

    ImageRAW *Reconstruct(ImageRAW *imgOut);

    int size()
    {
        return stack.size();
    }

    ImageRAW *get(int index)
    {
        return stack[index % stack.size()];
    }
};

Pyramid::Pyramid(ImageRAW *img, bool lapGauss, int limitLevel = 0)
{
    Create(img, lapGauss, limitLevel);
}


Pyramid::Pyramid(int width, int height, int channels, bool lapGauss, int limitLevel = 0)
{
    ImageRAW *tmpImg = new ImageRAW(1, width, height, channels);
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

void Pyramid::Create(ImageRAW *img, bool lapGauss, int limitLevel = 0)
{
    this->lapGauss  = lapGauss;
    this->limitLevel = limitLevel;

    FilterGaussian2D	fltG(1.0f);
    FilterSampler2D		fltS(0.5f);
    FilterSampler2DSub	fSub;

    ImageRAW *tmpImg = img;

    int levels = MAX(log2(MIN(img->width, img->height)) - limitLevel, 1);

    ImageRAW *tmpG = NULL;
    ImageRAW *tmpD = NULL;

    for(int i = 0; i < levels; i++) {
        tmpG = fltG.ProcessP(Single(tmpImg), NULL);
        tmpD = fltS.ProcessP(Single(tmpG), NULL);

        if(lapGauss) {	//Laplacian Pyramid
            ImageRAW *tmpDiff = fSub.ProcessP(Double(tmpImg, tmpD), tmpG);
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

/**Reconstruct: evaluates a Gaussian/Laplacian pyramid*/
ImageRAW *Pyramid::Reconstruct(ImageRAW *imgOut)
{
    if(stack.size() < 2) {
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = stack[0]->AllocateSimilarOne();
    }

    FilterSampler2DAdd fltAdd;
    int n = stack.size() - 1;
    ImageRAW *tmp = stack[n];

    if(trackerRec.empty()) {
        for(int i = n; i >= 2; i--) {
            ImageRAW *tmp2 = fltAdd.ProcessP(Double(stack[i - 1], tmp), NULL);
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

/**Update: recomputes the pyramid given a compatible img*/
void Pyramid::Update(ImageRAW *img)
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

    ImageRAW *tmpImg = img;

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

/**Mul: mul operator ( *= ) between pyramids; */
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

/**Add: add operator ( += ) between pyramids; */
void Pyramid::Add(const Pyramid *pyr)
{
    if(stack.size() != pyr->stack.size()) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        stack[i]->Add(pyr->stack[i]);
    }
}

/**MulSNeg: mul operator ( *= ) between pyramids; */
void Pyramid::MulSNeg(const Pyramid *pyr)
{
    if((stack.size() != pyr->stack.size()) && (pyr->stack.size() > 0)) {
        return;
    }

    for(unsigned int i = 0; i < stack.size(); i++) {
        stack[i]->MulSNeg(pyr->stack[i]);
    }
}

/**Blend*/
void Pyramid::Blend(Pyramid *pyr, Pyramid *weight)
{
    MulSNeg(weight);
    pyr->Mul(weight);
    Add(pyr);
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_PYRAMID_HPP */

