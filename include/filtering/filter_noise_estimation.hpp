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

#ifndef PIC_FILTERING_FILTER_NOISE_ESTIMATION_HPP
#define PIC_FILTERING_FILTER_NOISE_ESTIMATION_HPP

#include "../util/math.hpp"

#include "../filtering/filter.hpp"

#include "../filtering/filter_conv_2d.hpp"

namespace pic {

/**
 * @brief The FilterNoiseEstimation class
 */
class FilterNoiseEstimation: public Filter
{
protected:
    FilterConv2D flt;
    Image *img_conv;
    float *data;

public:

    /**
     * @brief FilterNoiseEstimation
     * @param type
     */
    FilterNoiseEstimation() : Filter()
    {
        data = new float[9];

        data[0] =  1.0f;
        data[1] = -2.0f;
        data[2] =  1.0f;

        data[3] = -2.0f;
        data[4] =  4.0f;
        data[5] = -2.0f;

        data[6] =  1.0f;
        data[7] = -2.0f;
        data[8] =  1.0f;

        img_conv = new Image(1, 3, 3, 1, data);
    }

    ~FilterNoiseEstimation()
    {
        if(img_conv != NULL) {
            delete img_conv;
        }

        if(data != NULL) {
            delete[] data;
        }
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if(!checkInput(imgIn)) {
            return imgOut;
        }

        imgOut = flt.Process(Double(imgIn[0], img_conv), imgOut);

        if(imgOut == NULL) {
            return imgOut;
        }

        imgOut->applyFunction(square);

        *imgOut /= 36.0f;

        return imgOut;
    }

    /**
     * @brief getNoiseEstimation
     * @param imgNoise
     * @return
     */
    static float *getNoiseEstimation(Image *imgNoise, float *ret)
    {
        if(ret == NULL) {
            ret = new float[imgNoise->channels];
        }

        BBox box = imgNoise->getFullBox();

        box.x0++;
        box.x1--;
        box.y0++;
        box.y1--;

        ret = imgNoise->getMeanVal(&box, ret);

        return ret;
    }


    /**
     * @brief getNoiseEstimation
     * @param imgNoise
     * @return
     */
    static float *getNoiseEstimation(Image *img, Image *imgNoise, float *ret)
    {

        FilterNoiseEstimation flt;
        imgNoise = flt.execute(img, imgNoise);

        ret = getNoiseEstimation(imgNoise, ret);

        return ret;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FilterNoiseEstimation flt;
        return flt.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NOISE_ESTIMATION_HPP */

