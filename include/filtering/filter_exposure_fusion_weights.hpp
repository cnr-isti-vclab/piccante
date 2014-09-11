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

#ifndef PIC_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS
#define PIC_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterExposureFusionWeights class
 */
class FilterExposureFusionWeights: public Filter
{
protected:
    float wC, wE, wS;


    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
    {
        int width    = src[0]->width;
        int channels = src[0]->channels;
        float *data  = src[0]->data;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int indOut = c + i;
                int ind = indOut * channels;

                float sum = 0.0f;
                for(int k=0;k<transformChannels;k++)
                {
                    sum += data[ind + k] * weights[k];
                }

                dst->data[indOut] = sum;
            }
        }
    }

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new ImageRAW(1, imgIn[0]->width, imgIn[0]->height, 1);
        } else {
            if((imgIn[0]->width  != imgOut->width)  ||
               (imgIn[0]->height != imgOut->height) ||
               (imgOut->channels != 1)) {
                imgOut = new ImageRAW(1, imgIn[0]->width, imgIn[0]->height, 1);
            }
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterExposureFusionWeights
     * @param type
     */
    FilterExposureFusionWeights(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f)
    {
        this->wC = wC > 0.0f ? wC : 1.0f;
        this->wE = wE > 0.0f ? wE : 1.0f;
        this->wS = wS > 0.0f ? wS : 1.0f;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageRAW *imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn->width;
        height      = imgIn->height;
        channels    = 1;
        frames      = imgIn->frames;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_EXPOSURE_FUSION_WEIGHTS */

