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

#ifndef PIC_FILTERING_FILTER_MOSAIC_HPP
#define PIC_FILTERING_FILTER_MOSAIC_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterMosaic: public Filter
{
protected:

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
    {
        if(src[0]->channels != 3){
            return;
        }

        int width = dst->width;
        float *data = src[0]->data;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;
            int mody = j % 2;

            for(int i = box->x0; i < box->x1; i++) {
                int modx = i % 2;

                //indecies
                int c1 = c + i;
                int c3 = c1 * 3;

                if(mody == 0 && modx == 0) { //Red
                    dst->data[c1] = data[c3];
                }

                if(mody == 0 && modx == 1) { //Green
                    dst->data[c1] = data[c3 + 1];
                }

                if(mody == 1 && modx == 0) { //Green
                    dst->data[c1] = data[c3 + 1];
                }

                if(mody == 1 && modx == 1) { //Blue
                    dst->data[c1] = data[c3 + 2];
                }
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
    FilterMosaic() {}

    /**Output size*/
    void OutputSize(ImageRAW *imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn->width;
        height      = imgIn->height;
        channels    = 1;
        frames      = imgIn->frames;
    }

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut)
    {
        FilterMosaic flt;
        return flt.ProcessP(Single(imgIn), imgOut);
    }

    //Filtering
    static ImageRAW *Execute(std::string fileInput, std::string fileOutput)
    {
        ImageRAW imgIn(fileInput);
        ImageRAW *out = FilterMosaic::Execute(&imgIn, NULL);
        out->Write(fileOutput);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MOSAIC_HPP */

