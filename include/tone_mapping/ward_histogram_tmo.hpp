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

#include "../base.hpp"
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
    Image *ProcessAux(ImageVec imgIn, Image *imgOut)
    {
        updateImage(imgIn[0]);

        images[0] = flt_lum.Process(imgIn, images[0]);

        int fScaleX, fScaleY;
        getScaleFiltering(imgIn[0], fScaleX, fScaleY);
        flt_smp.update(fScaleX, fScaleY, &isb);
        images[1] = flt_smp.Process(Single(images[0]), images[1]);

        //compute min and max luminance
        float LMin, LMax;
        images[1]->getMinVal(NULL, &LMin);
        images[1]->getMaxVal(NULL, &LMax);

        float log_LMin = logf(LMin + epsilon);
        float log_LMax = logf(LMax + epsilon);

        float log_LdMin = logf(LdMin + epsilon);
        float log_LdMax = logf(LdMax + epsilon);

        float delta_Ld = LdMax - LdMin;
        float delta_log_L = (log_LMax - log_LMin);
        float delta_log_Ld = log_LdMax - log_LdMin;

        //compute the histogram with ceiling
        h.calculate(images[1], VS_LOG_E, nBin);

        if(bCeiling) {
            h.ceiling(delta_log_L / (float(nBin) * delta_log_Ld));
        }

        Pcum = Array<uint>::cumsum(h.bin, nBin, Pcum);
        float maxPcumf = float(Pcum[nBin - 1]);

        for(int i = 0; i < nBin; i++) {
            PcumNorm[i] = float(Pcum[i]) / maxPcumf;
            x[i] = delta_log_L * float(i) / float(nBin - 1) + log_LMin;
        }

        #pragma omp parallel for
        for(int i = 0; i < images[0]->size(); i++) {
            float L_w =  images[0]->data[i];

            float log_L_w = logf(L_w + epsilon);
            float Ld = expf(delta_log_Ld * Arrayf::interp(x, PcumNorm, nBin, log_L_w) + log_LdMin) - epsilon;

            float scale = (MAX(Ld, 0.0f) - LdMin) / (delta_Ld * L_w);
            scale = MAX(scale, 0.0f);

            int index = i * imgOut->channels;
            for(int j = 0; j < imgOut->channels; j++) {
                int k = index + j;
                imgOut->data[k] = imgIn[0]->data[k] * scale;
            }
        }

        imgOut->removeSpecials();

        return imgOut;
    }

    /**
     * @brief allocate
     * @param nBin
     */
    void allocate(int nBin = 256)
    {
        nBin = nBin > 16 ? nBin : 256;

        if(this->nBin == nBin) {
            return;
        }

        releaseAux();

        Pcum = new unsigned int[nBin];
        PcumNorm = new float[nBin];
        x = new float[nBin];

        this->nBin = nBin;
    }

    int nBin;
    float LdMin, LdMax;
    ImageSamplerBilinear isb;
    Histogram h;
    float epsilon;

    unsigned int *Pcum;
    float *PcumNorm, *x;

    bool bCeiling;
    FilterLuminance flt_lum;
    FilterSampler2D flt_smp;

public:

    /**
     * @brief WardHistogramTMO
     * @param nBin is the number of bins of the histogram
     * @param LdMin is the minimum luminance of the LDR display in cd/m^2
     * @param LdMax is the maximum luminance of the LDR display in cd/m^2
     * @param bCeiling enables histogram ceiling or not
     */
    WardHistogramTMO(int nBin = 256, float LdMin = 1.0f, float LdMax = 100.0f, bool bCeiling = true) : ToneMappingOperator()
    {
        this->Pcum = NULL;
        this->PcumNorm = NULL;
        this->x = NULL;
        this->nBin = 0;
        this->bCeiling = bCeiling;

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
     * @param nBin is the number of bins of the histogram
     * @param LdMin is the minimum luminance of the LDR display in cd/m^2
     * @param LdMax is the maximum luminance of the LDR display in cd/m^2
     */
    void update(int nBin = 256, float LdMin = 1.0f, float LdMax = 100.0f)
    {
        allocate(nBin);

        epsilon = 1e-6f;

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
        WardHistogramTMO wtmo(100, 1.0f, 200.0f);
        return wtmo.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP */

