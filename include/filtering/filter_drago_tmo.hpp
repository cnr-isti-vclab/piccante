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

#ifndef PIC_FILTERING_FILTER_DRAGO_TMO_HPP
#define PIC_FILTERING_FILTER_DRAGO_TMO_HPP

#include "filtering/filter.hpp"
#include "filtering/filter_luminance.hpp"

namespace pic {

class FilterDragoTMO: public Filter
{
protected:
    float constant1, constant2, Lw_Max_scaled, Lw_a_scaled;
    float b, Ld_Max, Lw_Max, Lw_a;

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

    //SetupAux
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut);

public:
    //Basic constructors
    FilterDragoTMO();
    FilterDragoTMO(float Ld_Max, float b, float Lw_Max, float Lwa);

    void Update(float Ld_Max, float b, float Lw_Max, float Lwa);
};

//Basic constructors
FilterDragoTMO::FilterDragoTMO()
{
    Update(100.0f, 0.95f, 1e6f, 0.5f);
}


FilterDragoTMO::FilterDragoTMO(float Ld_Max, float b, float Lw_Max,
                               float Lw_a)
{
    Update(Ld_Max, b, Lw_Max, Lw_a);
}

void FilterDragoTMO::Update(float Ld_Max, float b, float Lw_Max,
                            float Lw_a)
{
    //protected values are assigned/computed
    if(Ld_Max > 0.0f) {
        this->Ld_Max = Ld_Max;
    } else {
        this->Ld_Max = 100.0f;
    }

    if(b > 0.0f) {
        this->b = b;
    } else {
        this->b = 0.95f;
    }

    if(Lw_Max > 0.0f) {
        this->Lw_Max = Lw_Max;
    } else {
        this->Lw_Max = 1e6f;
    }

    if(Lw_a > 0.0f) {
        this->Lw_a = Lw_a;
    } else {
        this->Lw_a = 0.5f;
    }

    //constants
    Lw_a_scaled  = this->Lw_a / powf(1.0f + b - 0.85f, 5.0f);
    Lw_Max_scaled = this->Lw_Max / Lw_a_scaled;

    constant1 = log(b) / log(0.5);
    constant2 = (Ld_Max / 100.0f) / (log10f(1.0f + Lw_Max_scaled));
}

ImageRAW *FilterDragoTMO::SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
{
    if(imgIn.size() < 2) {
        #ifdef PIC_DEBUG
            printf("ERROR: FilterDragoTMO::SetupAux");
        #endif
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOne();
    }

    return imgOut;
}

//Process in a box
void FilterDragoTMO::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
{
    int channels = src[0]->channels;

    for(int j = box->y0; j < box->y1; j++) {

        for(int i = box->x0; i < box->x1; i++) {

            float *dataIn  = (*src[0])(i, j);
            float *dataLum = (*src[1])(i, j);
            float *dataOut = (*dst   )(i, j);

            if(dataLum[0] > 0.0f) {
                float L_scaled = dataLum[0] / Lw_a_scaled;

                float tmp	= powf((L_scaled / Lw_Max_scaled), constant1);
                float Ld	= constant2 * logf(1.0 + L_scaled) / logf(2.0f + 8.0f * tmp);

                for(int k = 0; k < channels; k++) {
                    dataOut[k] = (dataIn[k] * Ld) / dataLum[0];
                }
            } else {
                for(int k = 0; k < src[0]->channels; k++) {
                    dataOut[k] = 0.0f;
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DRAGO_TMO_HPP */

