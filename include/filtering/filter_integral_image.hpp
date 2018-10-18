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

#ifndef PIC_FILTERING_FILTER_INTEGRAL_IMAGE
#define PIC_FILTERING_FILTER_INTEGRAL_IMAGE

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterIntegralImage class
 */
class FilterIntegralImage: public Filter
{
public:

    /**
     * @brief FilterIntegralImage
     */
    FilterIntegralImage() : Filter()
    {
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if(imgIn.empty()){
            return imgOut;
        }

        if(imgIn[0] == NULL) {
            return imgOut;
        }

        imgOut = setupAux(imgIn, imgOut);

        int width = imgIn[0]->width;
        int height = imgIn[0]->height;
        int channels = imgIn[0]->channels;

        //set up the first pixel (0,0)
        for(int k = 0; k < channels; k++) {
            imgOut->data[k] = imgIn[0]->data[k];
        }

        //set up the first row
        for(int j=1; j<width; j++) {
            int ind1 = j * channels;
            int ind2 = ind1 - channels;

            for(int k=0; k<channels; k++) {
                imgOut->data[ind1 + k] = imgIn[0]->data[ind1 + k] + imgOut->data[ind2 + k];
            }
        }

        //set up the first column
        int c1 = width * channels;
        for(int i=1; i<height; i++){
            int ind1 = i * c1;
            int ind2 = ind1 - c1;
            for(int k=0; k<channels; k++) {
                imgOut->data[ind1 + k] = imgIn[0]->data[ind1 + k] + imgOut->data[ind2 + k];
            }
        }

        int c2 = (width + 1) * channels;

        for(int i=1; i<height; i++) {
            int ind = i * width;
            for(int j=1; j<width; j++) {
                int ind1 = (ind + j) * channels;
                int ind2 = ind1 - channels;
                int ind3 = ind1 - c1;
                int ind4 = ind1 - c2;

                for(int k=0; k<channels; k++) {
                    imgOut->data[ind1 + k] = imgIn[0]->data[ind1 + k] +
                                             imgOut->data[ind2 + k] +
                                             imgOut->data[ind3 + k] -
                                             imgOut->data[ind4 + k];
                }
            }
        }

        return imgOut;
    }

    /**
     * @brief ProcessP
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessP(ImageVec imgIn, Image *imgOut)
    {
        return Process(imgIn, imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_INTEGRAL_IMAGE_HPP */
