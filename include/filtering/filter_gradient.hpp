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

#ifndef PIC_FILTERING_FILTER_GRADIENT_HPP
#define PIC_FILTERING_FILTER_GRADIENT_HPP

#include "../filtering/filter.hpp"

namespace pic {

enum GRADIENT_TYPE {G_SOBEL, G_PREWITT, G_NORMAL};

/**
 * @brief The FilterGradient class
 */
class FilterGradient: public Filter
{
protected:
    int colorChannel;
    GRADIENT_TYPE type;
    float mask[3];

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    /**
     * @brief FilterGradient
     */
    FilterGradient();

    /**
     * @brief FilterGradient
     * @param colorChannel
     * @param type
     */
    FilterGradient(int colorChannel, GRADIENT_TYPE type);

    /**
     * @brief update
     * @param colorChannel
     * @param type
     */
    void update(int colorChannel, GRADIENT_TYPE type);

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
     * @param type
     * @param colorChannel
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut = NULL,
                             GRADIENT_TYPE type = G_SOBEL, int colorChannel = 0)
    {
        FilterGradient filter(colorChannel, type);
        return filter.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterGradient::FilterGradient() : Filter()
{
    update(0, G_NORMAL);
}

PIC_INLINE FilterGradient::FilterGradient(int colorChannel,
        GRADIENT_TYPE type = G_NORMAL) : Filter()
{
    update(colorChannel, type);
}

PIC_INLINE void FilterGradient::update(int colorChannel,
                                           GRADIENT_TYPE type = G_NORMAL)
{
    this->colorChannel = colorChannel;
    this->type = type;

    switch(type) {
    case G_SOBEL: {
        mask[0] = 1.0f;
        mask[1] = 2.0f;
        mask[2] = 1.0f;
    }
    break;

    case G_PREWITT: {
        mask[0] = 1.0f;
        mask[1] = 1.0f;
        mask[2] = 1.0f;
    }
    break;

    case G_NORMAL: {
        mask[0] = 0.0f;
        mask[1] = 1.0f;
        mask[2] = 0.0f;
    }
    break;

    }
}

PIC_INLINE void FilterGradient::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    Image *img = src[0];

    int channel = (img->channels == 1) ? 0 : colorChannel;

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float gradX = 0.0f;
            float gradY = 0.0f;

            for(int k = -1; k < 2; k++) {
                float val = mask[k + 1];

                gradX += val * (*img)(i + 1, j + k)[channel];
                gradX -= val * (*img)(i - 1, j + k)[channel];

                gradY += val * (*img)(i + k, j + 1)[channel];
                gradY -= val * (*img)(i + k, j - 1)[channel];
            }

            float *dst_data = (*dst)(i, j);

            dst_data[0] = gradX;
            dst_data[1] = gradY;
            dst_data[2] = sqrtf(gradX * gradX + gradY * gradY);
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GRADIENT_HPP */
