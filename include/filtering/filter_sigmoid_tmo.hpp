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

#ifndef PIC_FILTERING_FILTER_SIGMOID_TMO_HPP
#define PIC_FILTERING_FILTER_SIGMOID_TMO_HPP

#include "filtering/filter.hpp"

namespace pic {

enum SIGMOID_MODE {SIG_TMO, SIG_TMO_WP, SIG_SDM};

class FilterSigmoidTMO: public Filter
{
protected:
    bool			temporal;
    float			c, alpha, epsilon, wp, wp2;
    SIGMOID_MODE	type;

    float CalculateEpsilon(ImageRAWVec imgIn);

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);
    //SetupAux
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut);

public:
    //Basic constructors
    FilterSigmoidTMO();
    FilterSigmoidTMO(SIGMOID_MODE type, float alpha, float wp, float epsilon,
                     bool temporal);

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut)
    {
        FilterSigmoidTMO filter;
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static ImageRAW *Execute(std::string nameIn, std::string nameOut)
    {
        ImageRAW imgIn(nameIn);
        ImageRAW *imgOut = Execute(&imgIn, NULL);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Basic constructors
FilterSigmoidTMO::FilterSigmoidTMO()
{
    type = SIG_TMO;
    alpha = 0.18f;
    wp = 1e9f;
    wp2 = wp * wp;
    epsilon = -1.0f;
    temporal = false;
}

FilterSigmoidTMO::FilterSigmoidTMO(SIGMOID_MODE type, float alpha,
                                   float wp = 1e9f, float epsilon = -1.0f, bool temporal = false)
{
    this->type = type;
    this->alpha = alpha;
    this->wp = wp;
    this->wp2 = wp * wp;

    this->epsilon = epsilon;
    this->temporal = temporal;
}

float FilterSigmoidTMO::CalculateEpsilon(ImageRAWVec imgIn)
{
    float tmpEpsilon, retEpsilon;

    switch(type) {
    case SIG_TMO:
        tmpEpsilon = imgIn[0]->getLogMeanVal(NULL, NULL)[0];
        break;

    case SIG_TMO_WP:
        tmpEpsilon = imgIn[0]->getLogMeanVal(NULL, NULL)[0];
        break;

    case SIG_SDM:
        tmpEpsilon = 1.0f;
        break;

    default:
        tmpEpsilon = 1.0f;
    }

    if(temporal) {
        if(epsilon > 0.0f) {
            retEpsilon = (epsilon + tmpEpsilon) / 2.0f;
        } else {
            retEpsilon = tmpEpsilon;
        }
    } else {
        retEpsilon = tmpEpsilon;
    }

    return retEpsilon;
}

ImageRAW *FilterSigmoidTMO::SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
{
    if(epsilon <= 0.0f || temporal) {
        epsilon = CalculateEpsilon(imgIn);
    }

    //printf("%f %f %f\n",epsilon,alpha,wp2);
    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOne();
    }

    return imgOut;
}

//Process in a box
void FilterSigmoidTMO::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
{

    float *data, *dataFlt;

    if(src.size() == 2) {
        data    = src[0]->data;
        dataFlt = src[1]->data;
    } else {
        data    = src[0]->data;
        dataFlt = src[0]->data;
    }

    float *dataOut = dst->data;

    if(src[0]->channels == 3) {
        float alpha_over_epsilon = alpha / epsilon;

        for(int j = box->y0; j < box->y1; j++) {
            int js = j * src[0]->ystride;

            for(int i = box->x0; i < box->x1; i++) {
                int c = js + i * src[0]->xstride; //index

                float L		= 0.213f * data[c] + 0.715f * data[c + 1]	+ 0.072f * data[c + 2];

                if(L > 0.0f) {
                    float L_flt	 = 0.213f * dataFlt[c] + 0.715f * dataFlt[c + 1] + 0.072f *
                                   dataFlt[c + 2];
                    float Lm	 = L     * alpha_over_epsilon;
                    float Lm_flt = L_flt * alpha_over_epsilon;
                    float Ld = Lm / (1.0f + Lm_flt);

                    for(int k = 0; k < src[0]->channels; k++) {
                        int ck = c + k;
                        dataOut[ck] = (data[ck] * Ld) / L;
                    }
                } else {
                    for(int k = 0; k < src[0]->channels; k++) {
                        dataOut[c + k] = 0.0f;
                    }
                }
            }
        }
    } else {
        float Lm, Lm_Flt;

        for(int j = box->y0; j < box->y1; j++) {
            int js = j * src[0]->ystride;

            for(int i = box->x0; i < box->x1; i++) {
                int c = js + i * src[0]->xstride; //index

                for(int k = 0; k < src[0]->channels; k++) {
                    int ck = c + k;

                    switch(type) {
                    case SIG_TMO_WP: {
                        Lm		=	(data   [ck] * alpha) / epsilon;
                        Lm_Flt	=	(dataFlt[ck] * alpha) / epsilon;

                        dataOut[ck] = Lm * (1.0f + Lm / wp2) / (1.0f + Lm_Flt);
                        //						dataOut[ck] = (val*(val/wp2+epsilon)/epsilon)/(valFlt+epsilon);
                    }
                    break;

                    default: {
                        Lm		=	data   [ck] * alpha;
                        Lm_Flt	=	dataFlt[ck] * alpha;

                        dataOut[ck] = Lm / (Lm_Flt + epsilon);
                    }
                    break;
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SIGMOID_TMO_HPP */

