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

#ifndef PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP
#define PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterColorDistance class
 */
class FilterColorDistance: public Filter
{
protected:
    float	*refColor;
    float	sigma;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int width = dst->width;
        int channels = src[0]->channels;
        float *data = src[0]->data;

        float sigma2 = sigma * sigma * 2.0f;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int c1 = (c + i);
                int c2 = c1 * channels;

                float sum = 0.0f;

                for(int i = 0; i < channels; i++) {
                    float tmp = data[c2 + i] - refColor[i];
                    sum += tmp * tmp;
                }

                dst->data[c1] = expf(-sum / sigma2);
            }
        }
    }

    Image *SetupAux(ImageVec imgIn, Image *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterColorDistance
     * @param color
     * @param sigma
     */
    FilterColorDistance(float *color, float sigma)
    {
        refColor    = color;
        this->sigma = sigma;
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param color
     * @param sigma
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, float *color,
                             float sigma)
    {
        FilterColorDistance fltColDst(color, sigma);
        return fltColDst.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param fileInput
     * @param fileOutput
     * @param color
     * @param sigma
     * @return
     */
    static Image *Execute(std::string fileInput, std::string fileOutput,
                             float *color, float sigma)
    {
        Image imgIn(fileInput);
        Image *out = FilterColorDistance::Execute(&imgIn, NULL, color, sigma);
        out->Write(fileOutput);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP */

