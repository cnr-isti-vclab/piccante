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

#ifndef PIC_FILTERING_FILTER_CHANNEL_HPP
#define PIC_FILTERING_FILTER_CHANNEL_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief SingleInt
 * @param v0
 * @return
 */
PIC_INLINE std::vector<int> SingleInt(int v0)
{
    std::vector<int> ret;
    ret.push_back(MAX(v0, 0));
    return ret;
}

/**
 * @brief TripleInt
 * @param v0
 * @param v1
 * @param v2
 * @return
 */
PIC_INLINE std::vector<int> TripleInt(int v0, int v1, int v2)
{
    std::vector<int> ret;
    ret.push_back(MAX(v0, 0));
    ret.push_back(MAX(v1, 0));
    ret.push_back(MAX(v2, 0));
    return ret;
}

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
        int totChannels =  CLAMPi(int(channels_vec.size()), 0, src[0]->channels);

        for(int p = box->z0; p < box->z1; p++) {
            for(int j = box->y0; j < box->y1; j++) {
                for(int i = box->x0; i < box->x1; i++) {

                    float *dataOut = (*dst)(i, j, p);
                    float *dataIn = (*src[0])(i, j, p);

                    for(int k = 0; k < totChannels; k++) {
                        int index = channels_vec[k];
                        dataOut[k] = dataIn[index];
                    }
                }
            }
        }
    }

    std::vector<int> channels_vec;

public:

    /**
     * @brief FilterChannel
     * @param channels_vec
     */
    FilterChannel(std::vector<int> channels_vec) : Filter()
    {
        update(channels_vec);
    }

    /**
     * @brief update
     * @param channels_vec
     */
    void update(std::vector<int> channels_vec)
    {
        this->channels_vec = channels_vec;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn[0]->width;
        height      = imgIn[0]->height;
        channels    = MAX(1, int(this->channels_vec.size()));
        frames      = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param channel
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int channel = 0)
    {
        FilterChannel fltCh(SingleInt(channel));
        return fltCh.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param channel
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, std::vector<int> channels_vec)
    {
        FilterChannel fltCh(channels_vec);
        return fltCh.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief removeAlpha
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *removeAlpha(Image *imgIn, Image *imgOut)
    {
        imgOut = execute(imgIn, imgOut, TripleInt(0, 1, 2));
        return imgOut;
    }

    /**
     * @brief test
     */
    static void test()
    {
        Image imgIn(1, 512, 512, 3);
        imgIn = 1.0f;

        Image *outR = execute(&imgIn, NULL, SingleInt(0));
        Image *outG = execute(&imgIn, NULL, SingleInt(1));
        Image *outB = execute(&imgIn, NULL, SingleInt(2));

        outR->Write("channel_R.pfm");
        outG->Write("channel_G.pfm");
        outB->Write("channel_B.pfm");
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CHANNEL_HPP */

