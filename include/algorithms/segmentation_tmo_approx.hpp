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

#ifndef PIC_ALGORITHMS_SEGMENTATION_TMO_APPROX_HPP
#define PIC_ALGORITHMS_SEGMENTATION_TMO_APPROX_HPP

#include "../util/array.hpp"
#include "../util/std_util.hpp"

#include "../algorithms/superpixels_slic.hpp"

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

    float perCent;

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param bBilateral
     * @param nSuperPixels
     * @return
     */
    Image *ProcessAux(Image *imgIn, bool bBilateral, int nSuperPixels = 4096)
    {
        if(bBilateral) {
            float minVal, maxVal;
            imgIn->getMinVal(NULL, &minVal);
            imgIn->getMaxVal(NULL, &maxVal);

            float nLevels = log10f(maxVal) - log10f(minVal) + 1.0f;
            float nLayer = ((maxVal - minVal) / nLevels) / 4.0f;
            float area = imgIn->widthf * imgIn->heightf * perCent;
            int iterations = MAX(int(sqrtf(area)) / 2, 1);

            //create filters
            if(fltIt == NULL) {
                fltBil = new FilterBilateral2DS(1.0f, nLayer);
                fltIt  = new FilterIterative(fltBil, iterations);
            }

    #ifdef PIC_DEBUG
            printf("Layer: %f iterations: %d\n", nLayer, iterations);
    #endif
            //Iterative bilateral filtering
            Image *imgOut = fltIt->Process(Single(imgIn), imgIn_flt);

            return imgOut;
        } else {
            Slic sp;
            sp.execute(imgIn, nSuperPixels);
            Image *imgOut = sp.getMeanImage(NULL);
            return imgOut;
        }
    }

public:

    /**
     * @brief Segmentation
     */
    Segmentation()
    {
        fltBil = NULL;
        fltIt  = NULL;
        L = NULL;
        imgIn_flt = NULL;

        perCent  = 0.005f;
    }

    ~Segmentation()
    {
        delete_s(imgIn_flt);
        delete_s(L);
        delete_s(fltIt);
        delete_s(fltBil);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(Image *imgIn, Image *imgOut)
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

        Image *imgIn_flt = ProcessAux(imgIn, true, 4096);

        //threshold
        float *data = imgIn_flt->data;
        int channels = imgIn_flt->channels;

        float *weights = FilterLuminance::computeWeights(LT_CIE_LUMINANCE, channels, NULL);

        #pragma omp parallel for
        for(int i = 0; i < imgIn_flt->size(); i += channels) {
            float L = Arrayf::dot(weights, &data[i], channels) + 1e-7f;
            imgOut->data[i / 3] = floorf(log10f(L));
        }

        delete_vec_s<float>(weights);

        #ifdef PIC_DEBUG
            imgOut->Write("Segmentation.pfm");
        #endif

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_SEGMENTATION_TMO_APPROX_HPP */

