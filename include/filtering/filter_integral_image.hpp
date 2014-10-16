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

#ifndef PIC_FILTERING_FILTER_INTEGRAL_IMAGE
#define PIC_FILTERING_FILTER_INTEGRAL_IMAGE

#include "filtering/filter.hpp"

namespace pic {

class FilterIntegralImage: public Filter
{
public:

    FilterIntegralImage() : Filter()
    {
    }

    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if(imgIn.size() < 1){
            return imgOut;
        }

        if(imgIn[0] == NULL) {
            return imgOut;
        }

        imgOut = SetupAux(imgIn, imgOut);

        int width = imgIn[0]->width;
        int height = imgIn[0]->height;
        int channels = imgIn[0]->channels;

        //setting up the first pixel (0,0)
        for(int k=0; k<channels; k++) {
            imgOut->data[k] = imgIn[0]->data[k];
        }

        //seting up the first row
        for(int j=1; j<width; j++) {
            int ind1 = j * channels;
            int ind2 = ind1 - channels;

            for(int k=0; k<channels; k++) {
                imgOut->data[ind1 + k] = imgIn[0]->data[ind1 + k] + imgOut->data[ind2 + k];
            }
        }

        //setting up the first column
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

    Image *ProcessP(ImageVec imgIn, Image *imgOut)
    {
        return Process(imgIn, imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_INTEGRAL_IMAGE_HPP */
