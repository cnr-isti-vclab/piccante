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

#ifndef PIC_GL_TONE_MAPPING_EXPOSURE_FUSION_TMO_HPP
#define PIC_GL_TONE_MAPPING_EXPOSURE_FUSION_TMO_HPP

#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_exposure_fusion_weights.hpp"
#include "gl/filtering/filter_op.hpp"

namespace pic {

/**
 * @brief The ExposureFusionGL class
 */
class ExposureFusionGL
{
protected:
    FilterGLLuminance  *flt_lum;
    FilterGLExposureFusionWeights *flt_weights;
    FilterGLOp         *remove_negative, *convert_zero_to_one;
    ImageGL            *lum, *acc, *weights;
    PyramidGL          *pW, *pI, *pOut;

public:
    /**
     * @brief ExposureFusionGL
     */
    ExposureFusionGL()
    {
        flt_lum = new FilterGLLuminance();
        flt_weights = NULL;

        remove_negative = new FilterGLOp("max(I0, vec4(0.0))", true, NULL, NULL);

        convert_zero_to_one = new FilterGLOp("I0.x > 0.0 ? I0 : vec4(1.0)", true, NULL, NULL);

        lum = NULL;
        acc = NULL;
        weights = NULL;

        pW = NULL;
        pI = NULL;
        pOut = NULL;
    }

    ~ExposureFusionGL()
    {
        if(flt_lum != NULL) {
            delete flt_lum;
            flt_lum = NULL;
        }
    }

    /**
     * @brief Process
     * @param imgIn
     * @param wC
     * @param wE
     * @param wC
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, float wC = 1.0f, float wE = 1.0f, float wS = 1.0f, ImageGL *imgOut = NULL)
    {
        int n = imgIn.size();

        if(n < 2) {
            return imgOut;
        }

        //Computing weights values
        int width = imgIn[0]->width;
        int height = imgIn[0]->height;
        int channels = imgIn[0]->channels;

        if(acc == NULL) {
            acc = new ImageGL(1, width, height, 1, IMG_GPU, GL_TEXTURE_2D);
        }

        *acc = 0.0f;

        if(flt_weights == NULL) {
            flt_weights = new FilterGLExposureFusionWeights(wC, wE, wS);
        }

        for(int j = 0; j < n; j++) {
            #ifdef PIC_DEBUG
                printf("Processing image %d\n", j);
            #endif

            lum = flt_lum->Process(SingleGL(imgIn[j]), lum);

            weights = flt_weights->Process(DoubleGL(lum, imgIn[j]), weights);

            *acc += *weights;
        }

        convert_zero_to_one->Process(SingleGL(acc), acc);

        //Accumulation Pyramid
        #ifdef PIC_DEBUG
            printf("Blending...");
        #endif

        if(pW == NULL) {
            pW = new PyramidGL(width, height, 1, false);
        }

        if(pI == NULL) {
            pI = new PyramidGL(width, height, channels, true);
        }

        if(pOut == NULL) {
            pOut = new PyramidGL(width, height, channels, true);
        }

        pOut->SetValue(0.0f);

        for(int j = 0; j < n; j++) {
            lum = flt_lum->Process(SingleGL(imgIn[j]), lum);
            weights = flt_weights->Process(DoubleGL(lum, imgIn[j]), weights);

            //normalization
            *weights /= *acc;

            pW->Update(weights);
            pI->Update(imgIn[j]);

            pI->Mul(pW);

            pOut->Add(pI);
        }

        #ifdef PIC_DEBUG
            printf(" ok\n");
        #endif

        //final result
        imgOut = pOut->Reconstruct(imgOut);
        imgOut = remove_negative->Process(SingleGL(imgOut), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_EXPOSURE_FUSION_TMO_HPP */

