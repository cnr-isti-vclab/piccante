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

#include "../../util/math.hpp"

#include "../../gl/filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter_exposure_fusion_weights.hpp"
#include "../../gl/filtering/filter_op.hpp"

namespace pic {

/**
 * @brief The ExposureFusionGL class
 */
class ExposureFusionGL
{
protected:
    std::vector<FilterGL *> filters;

    FilterGLLuminance  *flt_lum;
    FilterGLExposureFusionWeights *flt_weights;
    FilterGLOp *remove_negative, *convert_zero_to_one;    

    ImageGL *lum, *acc, *weights;

    PyramidGL *pW, *pI, *pOut;

    float wC, wS, wE;

    /**
     * @brief allocateFilters
     */
    void allocateFilters()
    {
        flt_lum = new FilterGLLuminance();
        filters.push_back(flt_lum);

        remove_negative = new FilterGLOp("max(I0, vec4(0.0))", true, NULL, NULL);
        filters.push_back(remove_negative);

        convert_zero_to_one = new FilterGLOp("I0.x > 0.0 ? I0 : vec4(1.0)", true, NULL, NULL);
        filters.push_back(convert_zero_to_one);

        flt_weights = new FilterGLExposureFusionWeights(wC, wE, wS);
        filters.push_back(flt_weights);
    }

public:
    /**
     * @brief ExposureFusionGL
     */
    ExposureFusionGL(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f)
    {
        update(wC, wE, wS);

        flt_lum =  NULL;
        flt_weights = NULL;
        convert_zero_to_one = NULL;
        remove_negative = NULL;

        lum = NULL;
        acc = NULL;
        weights = NULL;

        pW = NULL;
        pI = NULL;
        pOut = NULL;
    }

    ~ExposureFusionGL()
    {
        if(lum != NULL) {
            delete lum;
            lum = NULL;
        }

        if(acc != NULL) {
            delete acc;
            acc = NULL;
        }

        if(weights != NULL) {
            delete weights;
            weights = NULL;
        }

        if(pW != NULL) {
            delete pW;
            pW = NULL;
        }

        if(pI != NULL) {
            delete pI;
            pI = NULL;
        }

        if(pOut != NULL) {
            delete pOut;
            pOut = NULL;
        }

        for(unsigned int i = 0; i < filters.size(); i++) {
            if(filters[i] != NULL) {
                delete filters[i];
                filters[i] = NULL;
            }
        }
    }

    /**
     * @brief update
     * @param wC weight for preserving contrast
     * @param wE weight for preserving exposure
     * @param wS weight for preserving saturation
     */
    void update(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f)
    {
        this->wC = CLAMPi(wC, 0.0f, 1.0f);
        this->wE = CLAMPi(wE, 0.0f, 1.0f);
        this->wS = CLAMPi(wS, 0.0f, 1.0f);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *execute(ImageGLVec imgIn, ImageGL *imgOut = NULL)
    {
        int n = int(imgIn.size());

        if(n < 2) {
            return imgOut;
        }

        //compute weights values
        int width = imgIn[0]->width;
        int height = imgIn[0]->height;
        int channels = imgIn[0]->channels;

        if(acc == NULL) {
            acc = new ImageGL(1, width, height, 1, IMG_GPU, GL_TEXTURE_2D);
        }

        *acc = 0.0f;

        if(flt_lum == NULL) {
            allocateFilters();
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

        //accumulate on a pyramid
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

        pOut->setValue(0.0f);

        for(int j = 0; j < n; j++) {
            lum = flt_lum->Process(SingleGL(imgIn[j]), lum);
            weights = flt_weights->Process(DoubleGL(lum, imgIn[j]), weights);

            //normalization
            *weights /= *acc;

            pW->update(weights);
            pI->update(imgIn[j]);

            pI->mul(pW);

            pOut->add(pI);
        }

        #ifdef PIC_DEBUG
            printf(" ok\n");
        #endif

        //final result
        imgOut = pOut->reconstruct(imgOut);
        imgOut = remove_negative->Process(SingleGL(imgOut), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_EXPOSURE_FUSION_TMO_HPP */

