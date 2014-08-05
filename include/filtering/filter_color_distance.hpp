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

#ifndef PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP
#define PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterColorDistance: public Filter
{
protected:
    float	*refColor;
    float	sigma;

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
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

    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new ImageRAW(1, imgIn[0]->width, imgIn[0]->height, 1);
        }

        return imgOut;
    }

public:

    //Basic constructors
    FilterColorDistance(float *color, float sigma)
    {
        refColor    = color;
        this->sigma = sigma;
    }

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, float *color,
                             float sigma)
    {
        FilterColorDistance fltColDst(color, sigma);
        return fltColDst.ProcessP(Single(imgIn), imgOut);
    }

    //Filtering
    static ImageRAW *Execute(std::string fileInput, std::string fileOutput,
                             float *color, float sigma)
    {
        ImageRAW imgIn(fileInput);
        ImageRAW *out = FilterColorDistance::Execute(&imgIn, NULL, color, sigma);
        out->Write(fileOutput);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COLOR_DISTANCE_HPP */

