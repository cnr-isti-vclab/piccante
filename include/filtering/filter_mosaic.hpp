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

/**
 * @brief The FilterMosaic class
 */
class FilterMosaic: public Filter
{
protected:

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
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

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *SetupAux(ImageVec imgIn, Image *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
        }

        return imgOut;
    }

public:
    /**
     * @brief FilterMosaic
     */
    FilterMosaic()
    {

    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(Image *imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn->width;
        height      = imgIn->height;
        channels    = 1;
        frames      = imgIn->frames;
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut)
    {
        FilterMosaic flt;
        return flt.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param fileInput
     * @param fileOutput
     * @return
     */
    static Image *Execute(std::string fileInput, std::string fileOutput)
    {
        Image imgIn(fileInput);
        Image *out = FilterMosaic::Execute(&imgIn, NULL);
        out->Write(fileOutput);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MOSAIC_HPP */

