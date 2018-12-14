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
#include "../image_vec.hpp"
#include "../histogram.hpp"
#include "../util/array.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_sampler_2d.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

class WardHistogramTMO: public ToneMappingOperator
{
protected:

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessAux(Image *imgIn, Image *imgOut)
    {
        updateImage(imgIn);

        images[0] = flt_lum.Process(Single(imgIn), images[0]);

        int fScaleX, fScaleY;
        getScaleFiltering(imgIn, fScaleX, fScaleY);
        flt_smp.update(fScaleX, fScaleY, &isb);
        images[1] = flt_smp.Process(Single(images[0]), images[1]);

        float LMin, LMax;
        images[1]->getMinVal(NULL, &LMin);
        images[1]->getMaxVal(NULL, &LMax);

        float LlMin = logf(LMin + epsilon);
        float LlMax = logf(LMax + epsilon);

        float LldMin = logf(LdMin + epsilon);
        float LldMax = logf(LdMax + epsilon);

        h.calculate(images[1], VS_LOG_E, nBin);
        h.ceiling();

        Pcum = Array<unsigned int>::cumsum(h.bin, nBin, Pcum);
        float maxPcumf = float(Pcum[nBin - 1]);

        float delta = (LlMax - LlMin);
        for(int i = 0; i < nBin; i++) {
            PcumNorm[i] = float(Pcum[i]) / maxPcumf;
            x[i] = delta * float(i) / float(nBin - 1) + LlMin;
        }

        #pragma omp parallel for
        for(int i = 0; i < images[0]->size(); i++) {
            float L_w =  images[0]->data[i];

            float LLog = logf(L_w + epsilon);
            float Ld = expf(LldMin + (LldMax - LldMin) * Arrayf::interp(x, PcumNorm, nBin, LLog)) - epsilon;

            images[0]->data[i] = (MAX(Ld, 0.0f) - LdMin) / ((LdMax - LdMin) * L_w);
        }

        *imgOut *= *images[0];
        imgOut->removeSpecials();

        return imgOut;
    }

    /**
     * @brief allocate
     * @param nBin
     */
    void allocate(int nBin)
    {
        nBin = nBin > 1 ? nBin : 256;

        if((nBin != this->nBin) || (Pcum == NULL)) {
            releaseAux();
        }

        if(Pcum == NULL) {
            Pcum = new unsigned int[nBin];
        }

        if(PcumNorm == NULL) {
            PcumNorm = new float[nBin];
        }

        if(x == NULL) {
            x = new float[nBin];
        }

        this->nBin = nBin;
    }

    int nBin;
    float LdMin, LdMax;
    ImageSamplerBilinear isb;
    Histogram h;
    float epsilon;

    unsigned int *Pcum;
    float *PcumNorm, *x;

    FilterLuminance flt_lum;
    FilterSampler2D flt_smp;

public:

    /**
     * @brief WardHistogramTMO
     * @param nBin
     * @param LdMin
     * @param LdMax
     */
    WardHistogramTMO(int nBin = 256, float LdMin = 1.0f, float LdMax = 100.0f) : ToneMappingOperator()
    {
        this->Pcum = NULL;
        this->PcumNorm = NULL;
        this->x = NULL;
        this->nBin = 0;

        images.clear();
        images.push_back(NULL);
        images.push_back(NULL);

        update(nBin, LdMin, LdMax);
    }

    ~WardHistogramTMO()
    {
        release();
    }

    /**
     * @brief releaseAux
     */
    void releaseAux()
    {
        Pcum = delete_vec_s(Pcum);
        PcumNorm = delete_vec_s(PcumNorm);
        x = delete_vec_s(x);
    }

    /**
     * @brief update
     * @param nBin
     * @param LdMin
     * @param LdMax
     */
    void update(int nBin = 256, float LdMin = 1.0f, float LdMax = 100.0f)
    {
        allocate(nBin);

        epsilon = 1e-6;

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
    static Image* execute(Image *imgIn, Image *imgOut)
    {
        WardHistogramTMO wtmo(256, 1.0f, 100.0f);
        return wtmo.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP */

