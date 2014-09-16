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

#ifndef PIC_FILTERING_FILTER_COMBINE_HPP
#define PIC_FILTERING_FILTER_COMBINE_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterCombine class
 */
class FilterCombine: public Filter
{
protected:

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut);

public:

    /**
     * @brief FilterCombine
     */
    FilterCombine() {}

    /**
     * @brief AddAlpha
     * @param imgIn
     * @param imgOut
     * @param value
     * @return
     */
    static ImageRAW *AddAlpha(ImageRAW *imgIn, ImageRAW *imgOut, float value)
    {
        //Create an alpha channel
        ImageRAW *alpha = new ImageRAW(imgIn->frames, imgIn->width, imgIn->height, 1);
        alpha->Assign(value);

        //Add the channel to the image
        ImageRAWVec src;
        src.push_back(imgIn);
        src.push_back(alpha);

        FilterCombine filterC;
        imgOut = filterC.Process(src, imgOut);

        delete alpha;
        return imgOut;
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static ImageRAW *Execute(ImageRAWVec imgIn, ImageRAW *imgOut)
    {
        FilterCombine filterC;
        return filterC.Process(imgIn, imgOut);
    }

    /**
     * @brief ExecuteTest
     * @param nameIn
     * @param nameOut
     * @return
     */
    static ImageRAW *ExecuteTest(std::string nameIn, std::string nameOut)
    {
        ImageRAW imgIn(nameIn);

        FilterChannel filter(0);
        ImageRAW *outR = filter.Process(Single(&imgIn), NULL);

        filter.setChannel(1);
        ImageRAW *outG = filter.Process(Single(&imgIn), NULL);

        filter.setChannel(2);
        ImageRAW *outB = filter.Process(Single(&imgIn), NULL);

        ImageRAWVec src;
        src.push_back(outR);
        src.push_back(outG);
        src.push_back(outB);

        FilterCombine filterC;
        ImageRAW *ret = filterC.Process(src, NULL);

        ret->Write(nameOut);
        return ret;
    }
};

ImageRAW *FilterCombine::SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
{
    if(imgOut == NULL) {
        int channels = 0;

        for(unsigned int i = 0; i < imgIn.size(); i++) {
            channels += imgIn[i]->channels;
        }

        imgOut = new ImageRAW(imgIn[0]->frames, imgIn[0]->width, imgIn[0]->height,
                              channels);
    }

    return imgOut;
}

void FilterCombine::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
{
    for(int p = box->z0; p < box->z1; p++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                int c  = p * dst->tstride + j * dst->ystride + i * dst->xstride;
                int k2 = 0;

                for(unsigned int im = 0; im < src.size(); im++) {
                    int c2 = p * src[im]->tstride + j * src[im]->ystride + i * src[im]->xstride;

                    for(int k = 0; k < src[im]->channels; k++) {
                        dst->data[c + k2] = src[im]->data[c2 + k];
                        k2++;
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COMBINE_HPP */

