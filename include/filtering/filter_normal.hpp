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

#ifndef PIC_FILTERING_FILTER_NORMAL_HPP
#define PIC_FILTERING_FILTER_NORMAL_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterNormal class
 */
class FilterNormal: public Filter
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
     * @brief FilterNormal
     */
    FilterNormal();

    /**
     * @brief FilterNormal
     * @param colorChannel
     */
    FilterNormal(int colorChannel);

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
};

PIC_INLINE FilterNormal::FilterNormal() : Filter()
{
    update(0);
}

PIC_INLINE FilterNormal::FilterNormal(int colorChannel) : Filter()
{
    update(colorChannel);
}

PIC_INLINE void FilterNormal::update(int colorChannel)
{
    if(colorChannel<0) {
        colorChannel = 0;
    }

    this->colorChannel = colorChannel;
}

PIC_INLINE void FilterNormal::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int width    = dst->width;
    int height   = dst->height;
    int channels = src[0]->channels;

    if(colorChannel>(channels - 1)) {
        colorChannel = 0;
    }

    float *data = src[0]->data;
    float gradX, gradY;


    for(int j = box->y0; j < box->y1; j++) {
        int ind = j * width;

        for(int i = box->x0; i < box->x1; i++) {
            //Convolution kernel
            int ind2 = (ind + i) * 3;

            //Positions
            int ci  = CLAMP(i + 1, width);
            int cj  = CLAMP(j + 1, height);
            int ci1 = CLAMP(i - 1, width);
            int cj1 = CLAMP(j - 1, height);

            //Grad X
            int tmpc  = (ind + ci) * channels;
            gradX = data[tmpc + colorChannel];

            tmpc   = (ind + ci1) * channels;
            gradX -= data[tmpc + colorChannel];

            //Grad Y
            tmpc  = (cj * width + i) * channels;
            gradY = data[tmpc + colorChannel];

            tmpc   = (cj1 * width + i) * channels;
            gradY -= data[tmpc + colorChannel];

            /*gx[0]=1.0f; gx[1]=0.0f; gx[2]=gradX;
            gy[1]=1.0f; gy[0]=0.0f; gy[2]=gradY;

            dst->data[ind2  ] = gx[1] * gy[2] - gy[1] * gx[2];
            dst->data[ind2+1] = gx[2] * gy[0] - gy[2] * gx[0];
            dst->data[ind2+2] = gx[0] * gy[1] - gy[0] * gx[1];*/

            dst->data[ind2    ] = gradX;
            dst->data[ind2 + 1] = gradY;
            dst->data[ind2 + 2] = 1.0f;

            float norm = gradX * gradX + gradY * gradY + 1.0f;

            if(norm > 0.0f) {
                norm = sqrtf(norm);
                dst->data[ind2    ] /= norm;
                dst->data[ind2 + 1] /= norm;
                dst->data[ind2 + 2] /= norm;
            } else {
                dst->data[ind2    ] = 0.0f;
                dst->data[ind2 + 1] = 0.0f;
                dst->data[ind2 + 2] = 0.0f;
            }
        }
    }
}

/*
void genLight(float *L, int x, int y, int width, int height)
{
    float xf = float(x) / float(width);
    float yf = float(y) / float(height);

    L[0] = (xf - 0.5f) * 2.0f;
    L[1] = (yf - 0.5f) * 2.0f;
    L[2] = 1.0f;

    float norm = L[0] * L[0] + L[1] * L[1] + L[2] * L[2];

    if(norm > 0.0f) {
        norm = 1.0f / sqrtf(norm);
        L[0] *= norm;
        L[1] *= norm;
        L[2] *= norm;
    };
};
*/


} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NORMAL_HPP */

