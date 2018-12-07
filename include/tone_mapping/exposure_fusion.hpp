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

#ifndef PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP
#define PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP

#include "../base.hpp"
#include "../colors/saturation.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_laplacian.hpp"
#include "../filtering/filter_exposure_fusion_weights.hpp"
#include "../algorithms/pyramid.hpp"
#include "../util/std_util.hpp"
#include "../tone_mapping/get_all_exposures.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief ExposureFusion
 * @param imgIn
 * @param wC
 * @param wE
 * @param wS
 * @param imgOut
 * @return
 */

class ExposureFusion: public ToneMappingOperator
{
protected:
    FilterLuminance flt_lum;
    FilterExposureFusionWeights flt_weights;

    /**
     * @brief ifNegGetOne
     * @param x
     * @return
     */
    static float ifNegGetOne(float x)
    {
        return x > 0.0 ? x : 1.0f;
    }

    /**
     * @brief setNegToZero
     * @param x
     * @return
     */
    static float setNegToZero(float x)
    {
        return MAX(x, 0.0f);
    }

public:

    /**
     * @brief ExposureFusion
     * @param wC
     * @param wE
     * @param wS
     */
    ExposureFusion(float wC = 1.0f, float wE = 1.0f,
                   float wS = 1.0f)
    {

        setToANullVector<Image>(images, 3);

        update(wC, wE, wS);
    }

    ~ExposureFusion()
    {
        release();
    }

    /**
     * @brief update
     * @param wC
     * @param wE
     * @param wS
     */
    void update(float wC = 1.0f, float wE = 1.0f,
                float wS = 1.0f)
    {
        flt_weights.update(wC, wE, wS);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(Image *imgIn, Image *imgOut)
    {
        pic::ImageVec stack = getAllExposuresImages(imgIn);

        imgOut = ProcessStack(stack, imgOut);

        stdVectorClear<Image>(stack);

        return imgOut;
    }

    /**
     * @brief ProcessStack
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessStack(ImageVec imgIn, Image *imgOut)
    {
        int n = int(imgIn.size());

        if(n < 2 || !ImageVecCheck(imgIn, -1)) {
            return imgOut;
        }

        //compute weights values
        int channels = imgIn[0]->channels;
        int width = imgIn[0]->width;
        int height = imgIn[0]->height;

        updateImage(imgIn[0]);

        if(images[2] == NULL) {//images[2] --> acc
            images[2] = new Image(1, width, height, 1);
        }

        images[2]->setZero();

        for(int j = 0; j < n; j++) {
            #ifdef PIC_DEBUG
                printf("Processing image %d\n", j);
            #endif

            //images[0] --> lum
            images[0] = flt_lum.Process(Single(imgIn[j]), images[0]);

            //images[0] --> weights
            images[1] = flt_weights.Process(Double(images[0], imgIn[j]), images[1]);

            *images[2] += *images[1];
        }

        images[2]->applyFunction(ifNegGetOne);

        //accumulate Pyramid
        #ifdef PIC_DEBUG
            printf("Blending...");
        #endif

        Pyramid *pW   = new Pyramid(width, height, 1, false, 2);
        Pyramid *pI   = new Pyramid(width, height, channels, true, 2);
        Pyramid *pOut = new Pyramid(width, height, channels, true, 2);

        pOut->setValue(0.0f);

        for(int j = 0; j < n; j++) {
            images[0] = flt_lum.Process(Single(imgIn[j]), images[0]);
            images[1] = flt_weights.Process(Double(images[0], imgIn[j]), images[1]);

            //normalization
            *images[1] /= *images[2];

            pW->update(images[1]);
            pI->update(imgIn[j]);

            pI->mul(pW);

            pOut->add(pI);
        }

        #ifdef PIC_DEBUG
            printf(" ok\n");
        #endif

        //final result
        imgOut = pOut->reconstruct(imgOut);

      //  imgOut->applyFunction(setNegToZero);

        //free the memory
        delete pW;
        delete pOut;
        delete pI;

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_EXPOSURE_FUSION_HPP */

