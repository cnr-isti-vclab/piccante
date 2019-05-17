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

#ifndef PIC_FILTERING_FILTER_GRADIENT_HARRIS_OPT_HPP
#define PIC_FILTERING_FILTER_GRADIENT_HARRIS_OPT_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGradientHarrisOPT class
 */
class FilterGradientHarrisOPT: public Filter
{
protected:
    int colorChannel;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterGradientHarrisOPT
     * @param colorChannel
     */
    FilterGradientHarrisOPT(int colorChannel);

    /**
     * @brief update
     * @param colorChannel
     */
    void update(int colorChannel);

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
        channels    = 3;
        frames      = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param colorChannel
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut = NULL, int colorChannel = 0)
    {
        FilterGradientHarrisOPT filter(colorChannel);
        return filter.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterGradientHarrisOPT::FilterGradientHarrisOPT(int colorChannel) : Filter()
{
    this->colorChannel = 0;
    update(colorChannel);
}

PIC_INLINE void FilterGradientHarrisOPT::update(int colorChannel)
{
    if(colorChannel > -1) {
        this->colorChannel = colorChannel;
    }
}

PIC_INLINE void FilterGradientHarrisOPT::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *img = src[0];

    int channel = (img->channels == 1) ? 0 : colorChannel;

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float I_x = (*img)(i + 1, j)[channel] - (*img)(i - 1, j)[channel];
            float I_y = (*img)(i, j + 1)[channel] - (*img)(i, j - 1)[channel];

            float *dst_data = (*dst)(i, j);

            dst_data[0] = I_x * I_x;
            dst_data[1] = I_y * I_y;
            dst_data[2] = I_x * I_y;
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GRADIENT_HARRIS_OPT_HPP */
