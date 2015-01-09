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
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *SetupAux(ImageVec imgIn, Image *imgOut);

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
    static Image *AddAlpha(Image *imgIn, Image *imgOut, float value)
    {
        //Create an alpha channel
        Image *alpha = new Image(imgIn->frames, imgIn->width, imgIn->height, 1);
        *alpha = value;

        //Add the channel to the image
        ImageVec src;
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
    static Image *Execute(ImageVec imgIn, Image *imgOut)
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
    static Image *ExecuteTest(std::string nameIn, std::string nameOut)
    {
        Image imgIn(nameIn);

        FilterChannel filter(0);
        Image *outR = filter.Process(Single(&imgIn), NULL);

        filter.setChannel(1);
        Image *outG = filter.Process(Single(&imgIn), NULL);

        filter.setChannel(2);
        Image *outB = filter.Process(Single(&imgIn), NULL);

        ImageVec src;
        src.push_back(outR);
        src.push_back(outG);
        src.push_back(outB);

        FilterCombine filterC;
        Image *ret = filterC.Process(src, NULL);

        ret->Write(nameOut);
        return ret;
    }
};

Image *FilterCombine::SetupAux(ImageVec imgIn, Image *imgOut)
{
    if(imgOut == NULL) {
        int channels = 0;

        for(unsigned int i = 0; i < imgIn.size(); i++) {
            channels += imgIn[i]->channels;
        }

        imgOut = new Image(imgIn[0]->frames, imgIn[0]->width, imgIn[0]->height,
                              channels);
    }

    return imgOut;
}

void FilterCombine::ProcessBBox(Image *dst, ImageVec src, BBox *box)
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

