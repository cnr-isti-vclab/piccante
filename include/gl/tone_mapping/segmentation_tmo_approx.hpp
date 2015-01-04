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

#ifndef PIC_GL_TONE_MAPPING_SEGMENTATION_TMO_APPROX_HPP
#define PIC_GL_TONE_MAPPING_SEGMENTATION_TMO_APPROX_HPP

#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_redux.hpp"
#include "gl/filtering/filter_remove_nuked.hpp"
#include "gl/filtering/filter_iterative.hpp"
#include "gl/filtering/filter_bilateral_2ds.hpp"
#include "gl/filtering/filter_op.hpp"

namespace pic {

class SegmentationGL
{
protected:
    FilterGLLuminance		lum;
    FilterGLRedux			*min, *max;
    FilterGLRemoveNuked		*fltNuked;
    FilterGLIterative		*fltIt;
    FilterGLBilateral2DS	*fltBil;
    FilterGLOp				*fltSeg;
    ImageGL				*L, *imgIn_flt;

    float					perCent, nLayer;
    int						iterations;

public:
    ImageGLVec			stack;
    float					minVal, maxVal;

    SegmentationGL()
    {
        min = FilterGLRedux::CreateMinPos();
        max = FilterGLRedux::CreateMax();
        fltNuked = new FilterGLRemoveNuked(0.9f);
        fltBil = NULL;
        fltIt  = NULL;
        fltSeg = NULL;

        nLayer = 0.0f;
        iterations = 0;

        L = NULL;
        imgIn_flt = NULL;

        maxVal = FLT_MAX;
        minVal = 0.0f;

        perCent  = 0.005f;
    }

    ~SegmentationGL()
    {
        if(imgIn_flt != NULL) {
            delete imgIn_flt;
        }

        if(L != NULL) {
            delete L;
        }

        delete min;
        delete max;
        delete fltIt;
        delete fltBil;
        delete fltSeg;
        delete fltNuked;
    }

    void ComputeStatistics(Image *imgIn)
    {
        float nLevels, area;

        nLevels		= log10f(maxVal) - log10f(minVal) + 1.0f;
        nLayer		= ((maxVal - minVal) / nLevels) / 4.0f;
        area		= imgIn->widthf * imgIn->heightf * perCent;
        iterations	= MAX(int(sqrtf(area)) / 8, 1);
    }

    ImageGL *Compute(ImageGL *imgIn, ImageGL *imgOut)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(!imgIn->isValid()) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = new ImageGL(1, imgIn->width, imgIn->height, 1, IMG_GPU, GL_TEXTURE_2D);
        }

        //Compute luminance
        L = lum.Process(SingleGL(imgIn), L);

        //Get min value
        if(stack.empty()) {
            FilterGLRedux::CreateData(L->width, L->height, L->channels, stack, 1);
        }

        ImageGL *min_val = min->Redux(L, stack);
        min_val->loadToMemory();
        minVal = min_val->data[0];

        //Get max value
        ImageGL *max_val = max->Redux(L, stack);
        max_val->loadToMemory();
        maxVal = max_val->data[0];

        //Statistics
        ComputeStatistics(imgIn);

        //Iterative bilateral filtering
        if(fltIt == NULL) {
            fltBil = new FilterGLBilateral2DS(1.0f, nLayer);
            fltIt  = new FilterGLIterative(fltBil, iterations);
        }

        imgIn_flt = fltIt->Process(SingleGL(imgIn), imgIn_flt);
        lum.Process(SingleGL(imgIn_flt), L);

        //Thresholding
        if(fltSeg == NULL) {
            fltSeg = FilterGLOp::CreateOpSegmentation(false, floor(log10f(minVal)));
        }

        fltSeg->Process(SingleGL(L), L);

        //Removing nuked pixels
        fltNuked->Process(SingleGL(L), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_SEGMENTATION_TMO_APPROX_HPP */

