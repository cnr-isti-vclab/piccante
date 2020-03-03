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

#ifndef PIC_TONE_MAPPING_SCHLICK_TMO_HPP
#define PIC_TONE_MAPPING_SCHLICK_TMO_HPP

#include "../base.hpp"

#include "../util/array.hpp"
#include "../util/indexed_array.hpp"

#include "../image.hpp"
#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The SchlickTMO class
 */
class SchlickTMO: public ToneMappingOperator
{
protected:
    std::string mode;
    int nBit;
    float k, p, L0;
    FilterLuminance flt_lum;

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param imgOut
     * @return
     */    
    Image *ProcessAux(ImageVec imgIn, Image *imgOut)
    {
        updateImage(imgIn[0]);

        //compute luminance and its statistics
        float LMin, LMax;

        images[0] = flt_lum.Process(imgIn, images[0]);

        IntCoord ret;
        IndexedArray<float>::findSimple(images[0]->data, images[0]->size(), IndexedArray<float>::bFuncNotNeg, ret, 1);
        LMin = IndexedArray<float>::percentile(images[0]->data, ret, 0.01f);

        images[0]->getMaxVal(NULL, &LMax);

        int channels = imgIn[0]->channels;

        bool bNonUniform = (mode.compare("nonuniform") == 0);

        float p_prime;
        if((mode.compare("automatic") == 0) || bNonUniform) {
            int nValues = 1 << nBit;
            p_prime = L0 * LMax / (float(nValues) * LMin);
        } else {
            p_prime = p;
        }

        float cSqrtLminLmax = sqrtf(LMin * LMax);

        #pragma omp parallel for
        for(int i = 0; i < images[0]->size(); i++) {

            float Lw = images[0]->data[i];

            if(Lw > 0.0f) {
                float p_prime_w = p_prime;

                if(bNonUniform) {
                    p_prime_w *= (1.0f - k + k * Lw / cSqrtLminLmax);
                }

                float Ld = (p_prime_w * Lw) / ((p_prime_w - 1.0f) * Lw + LMax);

                int index = i * channels;
                for(int j = 0; j < channels; j++) {
                    int k = index + j;
                    imgOut->data[k] = (imgIn[0]->data[k] * Ld) / Lw;
                }
            }
        }

        return imgOut;
    }

public:

    /**
     * @brief SchlickTMO
     * @param mode valid values are: "nonuniform", "automatic", "standard"
     * @param p is a model parameter which takes values in [1,+inf].
     * @param nBit the number of bits of the output LDR display
     * @param L0 is lowest value of the LDR monitor that can be perceived by the HVS.
     * @param k is a value in [0,1].
     */
    SchlickTMO(std::string mode, float p, int nBit, float L0, float k) : ToneMappingOperator()
    {
        images.push_back(NULL);
        update(mode, p, nBit, L0, k);
    }

    ~SchlickTMO()
    {
        release();
    }

    /**
     * @brief update
     * @param mode valid values are: "nonuniform", "automatic", "standard"
     * @param p is a model parameter which takes values in [1,+inf].
     * @param nBit the number of bits of the output LDR display
     * @param L0 is lowest value of the LDR monitor that can be perceived by the HVS.
     * @param k is a value in [0,1].
     */
    void update(std::string mode = "automatic", float p = 200.0f, int nBit = 8, float L0 = 1.0f, float k = 0.5f)
    {
        this->mode = mode;
        this->k = CLAMPi(k, 0.0f, 1.0f);
        this->nBit = nBit < 1 ? 8 : nBit;
        this->p = p < 1.0f ? 200.0f : p;
        this->L0 = L0 < 0.0f ? 1.0f : L0;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        SchlickTMO stmo("automatic", 1.0f / 0.005f, 8, 1.0f, 0.5f);
        return stmo.Process(Single(imgIn), imgOut);
    }
};
} // end namespace pic

#endif /* PIC_TONE_MAPPING_SCHLICK_TMO_HPP */

