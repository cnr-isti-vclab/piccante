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

#ifndef PIC_TONE_MAPPING_RAMAN_TMO_HPP
#define PIC_TONE_MAPPING_RAMAN_TMO_HPP

#include "../base.hpp"
#include "../util/std_util.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_bilateral_2dg.hpp"

#include "../tone_mapping/get_all_exposures.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The RamanTMO class
 */
class RamanTMO: public ToneMappingOperator
{
protected:
    FilterLuminance flt_lum;

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessAux(ImageVec imgIn, Image *imgOut)
    {
        if(imgIn.size() > 1) {
            return ProcessAuxStack(imgIn, imgOut);
        } else {

            std::vector<float> fstops = getAllExposuresUniform(imgIn[0]);
            ImageVec stack = getAllExposuresImages(imgIn[0], fstops);
            //pic::ImageVec stack = getAllExposures(imgIn[0]);

            imgOut = ProcessAuxStack(stack, imgOut);

            stdVectorClear<Image>(stack);

            return imgOut;
        }
    }

    /**
     * @brief ramanFunction
     * @param x
     * @param param
     * @return
     */
    static float ramanFunction(float x, std::vector<float>& param)
    {
        return fabsf(x) + param[0];
    }

    /**
     * @brief ProcessAuxStack
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessAuxStack(ImageVec imgIn, Image *imgOut)
    {
        int n = int(imgIn.size());

        if(n < 2 || !ImageVecCheck(imgIn, -1)) {
            return imgOut;
        }

        //As reported in Raman and Chaudhuri Eurographics 2009 short paper
        float K1 = 1.0f;
        float K2 = 0.1f;
        float C = 70.0f / 255.0f;

        int width  = imgIn[0]->width;
        int height = imgIn[0]->height;

        float sigma_s = K1 * MIN(width, height);

        updateImage(imgIn[0]);

        std::vector<float> param;
        param.push_back(C);

        if(images[2] == NULL) {
            //images[2] --> acc
            images[2] = new Image(1, width, height, 1);
        }

        images[2]->setZero();

        //accumulate into a Pyramid
        #ifdef PIC_DEBUG
            printf("Blending...");
        #endif

        imgOut->setZero();

        for(int j = 0; j < n; j++) {
            images[0] = flt_lum.Process(Single(imgIn[j]), images[0]);

            float min, max;

            images[0]->getMinVal(NULL, &min);
            images[0]->getMaxVal(NULL, &max);
            float sigma_r = K2 * (max - min);

            images[1] = FilterBilateral2DG::execute(images[0], images[1], sigma_s, sigma_r);
            *images[1] -= *images[0];
            images[1]->applyFunctionParam(ramanFunction, param);

            *images[2] += *images[1];

            //normalization
            auto tmp = imgIn[j]->clone();
            *tmp *= *images[1];

            *imgOut += *tmp;
        }

        *imgOut /= *images[2];

        #ifdef PIC_DEBUG
            printf(" ok\n");
        #endif

        return imgOut;
    }

public:

    /**
     * @brief RamanTMO
     */
    RamanTMO()
    {
        setToANullVector<Image>(images, 3);
    }

    ~RamanTMO()
    {
        release();
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image* execute(Image *imgIn, Image *imgOut)
    {
        RamanTMO rtmo;
        return rtmo.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief executeStack
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image* executeStack(ImageVec imgIn, Image *imgOut)
    {
        RamanTMO rtmo;
        return rtmo.Process(imgIn, imgOut);
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_RAMAN_TMO_HPP */

