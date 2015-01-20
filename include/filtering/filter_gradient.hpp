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

#include "filtering/filter.hpp"

namespace pic {

enum GRADIENT_TYPE {G_SOBEL, G_PREWITT, G_NORMAL};

/**
 * @brief The FilterGradient class
 */
class FilterGradient: public Filter
{
protected:
    int				colorChannel;
    GRADIENT_TYPE	type;
    float			mask[3];

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
     * @brief Setup
     * @param colorChannel
     * @param type
     */
    void Setup(int colorChannel, GRADIENT_TYPE type);

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
        channels    = 3;
        frames      = imgIn->frames;
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param type
     * @param colorChannel
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut = NULL,
                             GRADIENT_TYPE type = G_SOBEL, int colorChannel = 0)
    {
        FilterGradient filter(colorChannel, type);
        return filter.ProcessP(Single(imgIn), imgOut);
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
        Image *out = FilterGradient::Execute(&imgIn, NULL);
        out->Write(fileOutput);
        return out;
    }
};

PIC_INLINE FilterGradient::FilterGradient()
{
    Setup(0, G_NORMAL);
}

PIC_INLINE FilterGradient::FilterGradient(int colorChannel,
        GRADIENT_TYPE type = G_NORMAL)
{
    Setup(colorChannel, type);
}

PIC_INLINE void FilterGradient::Setup(int colorChannel,
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

PIC_INLINE Image *FilterGradient::SetupAux(ImageVec imgIn,
        Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 3);
    }

    return imgOut;
}

PIC_INLINE void FilterGradient::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    //Filtering
    Image *img = src[0];

    int channels = img->channels;

    int tmpColorChannel = (channels == 1) ? 0 : colorChannel;

    for(int j = box->y0; j < box->y1; j++) {
        for(int i = box->x0; i < box->x1; i++) {
            float gradX = 0.0f;
            float gradY = 0.0f;

            for(int k = -1; k < 2; k++) {
                float val = mask[k + 1];

                gradX += val * (*img)(i + 1, j + k)[tmpColorChannel];
                gradX -= val * (*img)(i - 1, j + k)[tmpColorChannel];

                gradY += val * (*img)(i + k, j + 1)[tmpColorChannel];
                gradY -= val * (*img)(i + k, j - 1)[tmpColorChannel];
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
