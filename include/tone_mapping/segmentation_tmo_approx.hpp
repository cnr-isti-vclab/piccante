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

#ifndef PIC_TONE_MAPPING_SEGMENTATION_TMO_APPROX_HPP
#define PIC_TONE_MAPPING_SEGMENTATION_TMO_APPROX_HPP

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_iterative.hpp"
#include "../filtering/filter_bilateral_2ds.hpp"

namespace pic {

/**
 * @brief The Segmentation class
 */
class Segmentation
{
protected:
    FilterIterative *fltIt;
    FilterBilateral2DS *fltBil;
    Image *L, *imgIn_flt;

    int	 iterations;
    float perCent, nLayer;

public:
    float minVal, maxVal;

    /**
     * @brief Segmentation
     */
    Segmentation()
    {
        nLayer = 0.0f;
        iterations = 0;

        fltBil = NULL;
        fltIt  = NULL;

        L = NULL;
        imgIn_flt = NULL;

        maxVal = FLT_MAX;
        minVal = 0.0f;

        perCent  = 0.005f;
    }

    ~Segmentation()
    {
        if(imgIn_flt != NULL) {
            delete imgIn_flt;
        }

        if(L != NULL) {
            delete L;
        }

        if(fltIt != NULL) {
            delete fltIt;
        }

        if(fltBil != NULL) {
            delete fltBil;
        }
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
        iterations	= MAX(int(sqrtf(area)) / 2, 1);
    }

    /**
     * @brief executeSegmentationBilatearal
     * @param imgIn
     * @return
     */
    Image *executeSegmentationBilatearal(Image *imgIn)
    {
        computeStatistics(imgIn);

        //Create filters
        if(fltIt == NULL) {
            fltBil = new FilterBilateral2DS(1.0f, nLayer);
            fltIt  = new FilterIterative(fltBil, iterations);
        }

#ifdef PIC_DEBUG
        printf("Layer: %f iterations: %d\n", nLayer, iterations);
#endif
        //Iterative bilateral filtering
        Image *imgOut = fltIt->Process(Single(imgIn), imgIn_flt);

        //imgOut->Write("filtered.pfm");
        return imgOut;
    }

    /**
     * @brief executeSuperPixels
     * @param imgIn
     * @param nSuperPixels
     * @return
     */
    Image *executeSuperPixels(Image *imgIn, int nSuperPixels = 4096)
    {
        Slic sp;
        sp.execute(imgIn, nSuperPixels);
        Image *imgOut = sp.getMeanImage(NULL);
        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *execute(Image *imgIn, Image *imgOut)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(!imgIn->isValid() || (imgIn->channels != 3)) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn->width, imgIn->height, 1);
        }

        //compute luminance
        FilterLuminance::execute(imgIn, imgOut, LT_CIE_LUMINANCE);

        //get min and max value
        maxVal = imgOut->getMaxVal()[0];
        minVal = imgOut->getMinVal()[0] + 1e-9f;

        Image *imgIn_flt = executeSegmentationBilatearal(imgIn);

        //threshold
        float minShift = floorf(log10f(minVal));
        float *data = imgIn_flt->data;

        #pragma omp parallel for

        for(int i = 0; i < imgIn_flt->size(); i += imgIn_flt->channels) {
            float Lum = 0.213f * data[i] + 0.715f * data[i + 1] + 0.072f * data[i + 2];

            if(Lum > 0.0f) {
                imgOut->data[i / 3] = floorf(log10f(Lum));
            } else {
                imgOut->data[i / 3] = minShift;
            }
        }

        #ifdef PIC_DEBUG
            imgOut->Write("Segmentation.pfm");
        #endif

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_SEGMENTATION_TMO_APPROX_HPP */

