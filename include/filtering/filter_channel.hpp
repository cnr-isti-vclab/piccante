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

#ifndef PIC_FILTERING_FILTER_CHANNEL_HPP
#define PIC_FILTERING_FILTER_CHANNEL_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterChannel class
 */
class FilterChannel: public Filter
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
        int tmpChannel;
        tmpChannel = MAX(channel, 0);
        tmpChannel = MIN(channel, src[0]->channels);

        for(int p = box->z0; p < box->z1; p++) {
            for(int j = box->y0; j < box->y1; j++) {
                for(int i = box->x0; i < box->x1; i++) {

                    float *dst_data =	(*dst)(i, j, p);
                    float *data =	(*src[0])(i, j, p);

                    dst_data[0] = data[tmpChannel];
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
            imgOut = new Image(imgIn[0]->frames, imgIn[0]->width, imgIn[0]->height, 1);
        }

        return imgOut;
    }

    int channel;

public:

    /**
     * @brief FilterChannel
     * @param channel
     */
    FilterChannel(int channel)
    {
        setChannel(channel);
    }

    /**
     * @brief setChannel
     * @param channel
     */
    void setChannel(int channel)
    {
        this->channel = channel;
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param channel
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, int channel = 0)
    {
        FilterChannel fltCh(channel);
        return fltCh.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Test
     */
    static void Test()
    {
        Image imgIn(1, 512, 512, 3);
        imgIn = 1.0f;

        FilterChannel filter(0);
        Image *outR = filter.Process(Single(&imgIn), NULL);

        filter.setChannel(1);
        Image *outG = filter.Process(Single(&imgIn), NULL);

        filter.setChannel(2);
        Image *outB = filter.Process(Single(&imgIn), NULL);

        outR->Write("channel_R.pfm");
        outG->Write("channel_G.pfm");
        outB->Write("channel_B.pfm");
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CHANNEL_HPP */

