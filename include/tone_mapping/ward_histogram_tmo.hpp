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

#ifndef PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP
#define PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP

#include "image_raw.hpp"
#include "histogram.hpp"
#include "util/array.hpp"
#include "filtering/filter_sampler_2d.hpp"

namespace pic {

inline ImageRAW *WardHistogramTMO(ImageRAW *imgIn, ImageRAW *imgOut = NULL,
                                  int nBin = 256, float LdMax = 100.0f, float LdMin = 1.0f)
{
    if(imgIn == NULL) {
        return NULL;
    }

    if(imgOut == NULL) {
        imgOut = imgIn->Clone();
    }

    if(LdMax <= 0.0f) {
        LdMax = 100.0f;
    }

    if(LdMin <= 0.0f) {
        LdMin = 1.0f;
    }

    int maxCoord = MAX(imgIn->width, imgIn->height);

    float maxCoordf       = 2.0f * float(maxCoord) * 0.75f;
    float viewAngleWidth  = 2.0f * atanf(imgIn->width / maxCoordf);
    float viewAngleHeight = 2.0f * atanf(imgIn->height / maxCoordf);

    int fScaleX = (2.0f * tanf(viewAngleWidth / 2.0f) / 0.01745f);
    int fScaleY = (2.0f * tanf(viewAngleHeight / 2.0f) / 0.01745f);

    ImageRAW *L = FilterLuminance::Execute(imgIn, NULL, LT_CIE_LUMINANCE);	//Luminance

    ImageSamplerBilinear isb;
    ImageRAW *Lscaled = FilterSampler2D::Execute(L, NULL, fScaleX, fScaleY, &isb);

    float LMin = Lscaled->getGT(0.0f);
    float LMax = Lscaled->getMaxVal()[0];
    float LlMax = logf(LMax);
    float LlMin = logf(LMin);

    float LldMax = logf(LdMax);
    float LldMin = logf(LdMin);

    Histogram h;
    h.Calculate(Lscaled, VS_LOG_E, nBin);
    h.Ceiling();

    unsigned int *Pcum = NULL;

    Pcum = Array<unsigned int>::cumsum(h.bin, Pcum, nBin);
    float  maxPcumf = float(Pcum[nBin - 1]);
    float *PcumNorm = new float[nBin];
    float *x        = new float[nBin];

    for(int i = 0; i < nBin; i++) {
        PcumNorm[i] = float(Pcum[i]) / maxPcumf;
        x[i] = (LlMax - LlMin) * float(i) / float(nBin - 1) + LlMin;
    }

    #pragma omp parallel for
    for(int i = 0; i < L->size(); i++) {
        float tmp_L =  L->data[i];
        float LLog = logf(tmp_L);
        float Ld = expf(LldMin + (LldMax - LldMin) * Array<float>::interp(x, PcumNorm, nBin, LLog));
        float L_old = tmp_L;
        L->data[i] = (Ld - LdMin) / ((LdMax - LdMin) * L_old);
    }

    imgOut->MulS(L);
    imgOut->removeSpecials();

    delete L;
    delete[] Pcum;
    delete[] x;
    delete[] PcumNorm;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP */

