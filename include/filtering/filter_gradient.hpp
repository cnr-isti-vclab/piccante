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

#ifndef PIC_FILTERING_FILTER_GRADIENT_HPP
#define PIC_FILTERING_FILTER_GRADIENT_HPP

#include "filtering/filter.hpp"

namespace pic {

enum GRADIENT_TYPE {G_SOBEL, G_PREWITT, G_NORMAL};

class FilterGradient: public Filter
{
protected:
    int				colorChannel;
    GRADIENT_TYPE	type;
    float			mask[3];

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

    Image *SetupAux(ImageVec imgIn, Image *imgOut);

    void CreateMask();

public:
    //Basic constructors
    FilterGradient(int colorChannel, GRADIENT_TYPE type);

    void Setup(int colorChannel, GRADIENT_TYPE type);
    
    /**Output size*/
    void OutputSize(Image *imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn->width;
        height      = imgIn->height;
        channels    = 3;
        frames      = imgIn->frames;
    }

    //Filtering
    static Image *Execute(Image *imgIn, Image *imgOut = NULL,
                             GRADIENT_TYPE type = G_SOBEL, int colorChannel = 0)
    {
        FilterGradient filter(colorChannel, type);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    //Filtering
    static Image *Execute(std::string fileInput, std::string fileOutput)
    {
        Image imgIn(fileInput);
        Image *out = FilterGradient::Execute(&imgIn, NULL);
        out->Write(fileOutput);
        return out;
    }
};

//Basic constructor
PIC_INLINE FilterGradient::FilterGradient(int colorChannel,
        GRADIENT_TYPE type = G_NORMAL)
{
    this->colorChannel = colorChannel;
    this->type = type;
    CreateMask();
}

PIC_INLINE void FilterGradient::Setup(int colorChannel,
                                      GRADIENT_TYPE type = G_NORMAL)
{
    this->colorChannel = colorChannel;
    this->type = type;
    CreateMask();
}

PIC_INLINE void FilterGradient::CreateMask()
{
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

//Processing
PIC_INLINE Image *FilterGradient::SetupAux(ImageVec imgIn,
        Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 3);
    }

    return imgOut;
}

//Process in a box
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

/*			//Convolution kernel
			c = ind + i*dst->xstride;
			//Positions
			ci	=	CLAMP(i+1,width);
			cj	=	CLAMP(j+1,height);
			ci1	=	CLAMP(i-1,width);
			cj1	=	CLAMP(j-1,height);

			//Grad X
			tmpc   = j*src[0]->ystride + ci*src[0]->xstride;
			gradX  = data[tmpc+colorChannel];

			tmpc   = j*src[0]->ystride + ci*src[0]->xstride;
			gradX -= data[tmpc+colorChannel];

			//Grad Y
			tmpc   = cj*src[0]->ystride + i*src[0]->xstride;
			gradY  = data[tmpc+colorChannel];

			tmpc   = cj1*src[0]->ystride + i*src[0]->xstride;
			gradY -= data[tmpc+colorChannel];
			*/

