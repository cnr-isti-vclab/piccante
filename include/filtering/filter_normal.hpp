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

#ifndef PIC_FILTERING_FILTER_NORMAL_HPP
#define PIC_FILTERING_FILTER_NORMAL_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterNormal: public Filter
{
protected:
    int colorChannel;

    //Setup Aux
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut);

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

public:
    //Basic constructors
    FilterNormal();
    FilterNormal(int colorChannel);

    void Update(int colorChannel); 

    /**Output size*/
    void OutputSize(ImageRAW *imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn->width;
        height      = imgIn->height;
        channels    = 3;
        frames      = imgIn->frames;
    }
};

FilterNormal::FilterNormal()
{
    Update(0);
}

//Basic constructor
FilterNormal::FilterNormal(int colorChannel)
{
    Update(colorChannel);
}

void FilterNormal::Update(int colorChannel)
{
    if(colorChannel<0) {
        colorChannel = 0;
    }

    this->colorChannel = colorChannel;
}

//Processing
ImageRAW *FilterNormal::SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
{
    if(imgOut == NULL) {
        imgOut = new ImageRAW(1, imgIn[0]->width, imgIn[0]->height, 3);
    }

    return imgOut;
}

//Process in a box
void FilterNormal::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
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

void TestFilterNormal()
{
    ImageRAW imgIn("depth0.pfm");

    FilterNormal *filter = new FilterNormal(1);

    ImageRAW *out = filter->Process(Single(&imgIn), NULL);

    float *L = new float[3];

    for(int i = 0; i < out->width; i++) {
        for(int j = 0; j < out->height; j++) {
            int c = (j * out->width + i) * 3;
            genLight(L, i, j, out->width, out->height);

            float val =
                fabsf(L[0] * out->data[c] +
                      L[1] * out->data[c + 1] +
                      L[2] * out->data[c + 2]) / (imgIn.data[c] * imgIn.data[c]);

            out->data[c] = val;
            out->data[c + 1] = val;
            out->data[c + 2] = val;
        }
    }

    out->Write("normal.pfm");
};*/

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NORMAL_HPP */

