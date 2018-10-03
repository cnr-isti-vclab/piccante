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

#include "../../gl/filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter_remove_nuked.hpp"
#include "../../gl/filtering/filter_iterative.hpp"
#include "../../gl/filtering/filter_bilateral_2ds.hpp"
#include "../../gl/filtering/filter_op.hpp"

namespace pic {

/**
 * @brief The SegmentationGL class
 */
class SegmentationGL
{
protected:
    FilterGLLuminance       *flt_lum;
    FilterGLRemoveNuked     *flt_nuked;
    FilterGLIterative       *flt_it;
    FilterGLBilateral2DS    *flt_bil;
    FilterGLOp              *flt_seg;
    ImageGL                 *L, *imgIn_flt;

    float                   perCent, nLayer;
    int                     iterations;

public:
    ImageGLVec stack;
    float      minVal, maxVal;

    /**
     * @brief SegmentationGL
     */
    SegmentationGL()
    {
        flt_nuked = NULL;
        flt_lum = NULL;
        flt_bil = NULL;
        flt_it  = NULL;
        flt_seg = NULL;

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
            imgIn_flt = NULL;
        }

        if(L != NULL) {
            delete L;
            L = NULL;
        }

        delete flt_it;
        delete flt_bil;
        delete flt_seg;
        delete flt_nuked;
    }

    /**
     * @brief computeStatistics
     * @param imgIn
     */
    void computeStatistics(Image *imgIn)
    {
        float nLevels, area;

        nLevels		= log10f(maxVal) - log10f(minVal) + 1.0f;
        nLayer		= ((maxVal - minVal) / nLevels) / 4.0f;
        area		= imgIn->widthf * imgIn->heightf * perCent;
        iterations	= MAX(int(sqrtf(area)) / 8, 1);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(!imgIn->isValid()) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = new ImageGL(1, imgIn->width, imgIn->height, 1, IMG_GPU, GL_TEXTURE_2D);
        }

        //compute luminance
        if(flt_lum == NULL) {
            flt_lum = new FilterGLLuminance();
        }

        L = flt_lum->Process(SingleGL(imgIn), L);

        L->getMinVal(&minVal);
        L->getMaxVal(&maxVal);

        //iterative bilateral filtering
        if(flt_it == NULL) {
            flt_bil = new FilterGLBilateral2DS(1.0f, nLayer);
            flt_it  = new FilterGLIterative(flt_bil, iterations);
        }

        imgIn_flt = flt_it->Process(SingleGL(imgIn), imgIn_flt);
        L = flt_lum->Process(SingleGL(imgIn_flt), L);

        //threshold
        if(flt_seg == NULL) {
            flt_seg = FilterGLOp::CreateOpSegmentation(false, floor(log10f(minVal)));
        }

        flt_seg->Process(SingleGL(L), L);

        //remove nuked pixels
        if(flt_nuked == NULL) {
            flt_nuked = new FilterGLRemoveNuked(0.9f);
        }
        flt_nuked->Process(SingleGL(L), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_SEGMENTATION_TMO_APPROX_HPP */

