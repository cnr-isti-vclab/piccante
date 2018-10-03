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

#ifndef PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP
#define PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP

#include "../image.hpp"
#include "../histogram.hpp"
#include "../util/array.hpp"
#include "../filtering/filter_sampler_2d.hpp"

namespace pic {

class WardHistogramTMO
{
protected:
    int nBin;
    float LdMin, LdMax;
    ImageSamplerBilinear isb;

public:

    /**
     * @brief WardHistogramTMO
     * @param nBin
     * @param LdMin
     * @param LdMax
     */
    WardHistogramTMO(int nBin = 256, float LdMin = 1.0f, float LdMax = 100.0f)
    {
        update(nBin, LdMin, LdMax);
    }

    /**
     * @brief update
     * @param nBin
     * @param LdMin
     * @param LdMax
     */
    void update(int nBin = 256, float LdMin = 1.0f, float LdMax = 100.0f)
    {
        this->nBin = nBin > 1 ? nBin : 256;
        this->LdMax = LdMax > 0.0f ? LdMax : 100.0f;
        this->LdMin = LdMin > 0.0f ? LdMin : 1.0f;

        if(this->LdMin > this->LdMax) {
            LdMin = 1.0f;
            LdMax = 100.0f;
        }

    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *execute(Image *imgIn, Image *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->clone();
        }

        int maxCoord = MAX(imgIn->width, imgIn->height);

        float maxCoordf       = 2.0f * float(maxCoord) * 0.75f;
        float viewAngleWidth  = 2.0f * atanf(imgIn->width / maxCoordf);
        float viewAngleHeight = 2.0f * atanf(imgIn->height / maxCoordf);

        int fScaleX = int((2.0f * tanf(viewAngleWidth / 2.0f) / 0.01745f));
        int fScaleY = int((2.0f * tanf(viewAngleHeight / 2.0f) / 0.01745f));

        Image *L = FilterLuminance::execute(imgIn, NULL, LT_CIE_LUMINANCE);	//Luminance

        Image *Lscaled = FilterSampler2D::execute(L, NULL, fScaleX, fScaleY, &isb);

        float LMin = Lscaled->getGT(0.0f);
        float LMax = Lscaled->getMaxVal()[0];
        float LlMax = logf(LMax);
        float LlMin = logf(LMin);

        float LldMax = logf(LdMax);
        float LldMin = logf(LdMin);

        Histogram h;
        h.calculate(Lscaled, VS_LOG_E, nBin);
        h.ceiling();

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

        *imgOut *= *L;
        imgOut->removeSpecials();

        delete L;
        delete[] Pcum;
        delete[] x;
        delete[] PcumNorm;

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP */

