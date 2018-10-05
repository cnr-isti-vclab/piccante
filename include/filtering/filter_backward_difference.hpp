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

#ifndef PIC_FILTERING_FILTER_BACKWARD_DIFFERENCE_HPP
#define PIC_FILTERING_FILTER_BACKWARD_DIFFERENCE_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterBackwardDifference class
 */
class FilterBackwardDifference: public Filter
{
protected:

    /**
     * @brief f
     * @param data
     */
    void f(FilterFData *data)
    {
        float *in   = (*data->src[0])(data->x,     data->y);
        float *inXm = (*data->src[0])(data->x + 1, data->y);
        float *inYm = (*data->src[0])(data->x,     data->y + 1);

        for(int k = 0; k < data->dst->channels; k++) {
            int tmp = k << 1;
            data->out[tmp  ]   = inXm[k] - in[k];
            data->out[tmp + 1] = inYm[k] - in[k];
        }
    }

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     *
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        //Filtering
        Image *img = src[0];
        int channels = img->channels;

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *dst_data   = (*dst)(i  , j);

                float *img_data   = (*img)(i  , j);
                float *img_dataXm = (*img)(i + 1, j);
                float *img_dataYm = (*img)(i  , j + 1);

                for(int k = 0; k < channels; k++) {

                }
            }
        }
    }*/

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *setupAux(ImageVec imgIn, Image *imgOut)
    {
        if(imgIn.empty()) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height,
                    2 * imgIn[0]->channels);
        }

        return imgOut;
    }

public:
    /**
     * @brief FilterBackwardDifference
     */
    FilterBackwardDifference()
    {

    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FilterBackwardDifference filter;
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param fileInput
     * @param fileOutput
     * @return
     */
    static Image *execute(std::string fileInput, std::string fileOutput)
    {
        Image imgIn(fileInput);
        Image *out = FilterBackwardDifference::execute(&imgIn, NULL);
        out->Write(fileOutput);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BACKWARD_DIFFERENCE_HPP */

