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

#ifndef PIC_TONE_MAPPING_FERWERDA_TMO_HPP
#define PIC_TONE_MAPPING_FERWERDA_TMO_HPP

#include "../base.hpp"

#include "../util/array.hpp"

#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The FerwerdaTMO class
 */
class FerwerdaTMO: public ToneMappingOperator
{
protected:
    float Ld_Max, Ld_a, Lw_a;
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
        images[0] = flt_lum.Process(imgIn, images[0]);

        if(Lw_a < 0.0f) {
            float maxVal;
            images[0]->getMaxVal(NULL, &maxVal);
            Lw_a = maxVal / 2.0f;
        }

        float mC = Tp(Ld_a) / Tp(Lw_a);
        float mR = Ts(Ld_a) / Ts(Lw_a);
        float k = WalravenValetonK(Lw_a);

        int channels = imgIn[0]->channels;
        float *scale = new float[channels];

        if(channels == 3) {
            scale[0] = 1.05f;
            scale[1] = 0.97f;
            scale[2] = 1.27f;
        } else {
           Arrayf::assign(1.0f, scale, channels);
        }

        for(int i = 0; i < channels; i++) {
            scale[i] *= (mR * k);
        }

        #pragma omp parallel for
        for(int i = 0; i < imgIn[0]->size(); i += channels) {

            int indexL = i / channels;

            for(int j = 0; j < channels; j++) {
                int index = i + j;
                imgOut->data[index] = imgIn[0]->data[index] * mC +
                                      images[0]->data[indexL] * scale[j];
            }
        }

        //NOTE: this is done to have values in [0,1] and not in cd/m^2!
        *imgOut /= Ld_Max;

        return imgOut;
    }

public:

    /**
     * @brief FerwerdaTMO
     * @param Ld_Max
     * @param Ld_a
     * @param Lw_a
     */
    FerwerdaTMO(float Ld_Max = 100.0f, float Ld_a = 50.0f, float Lw_a = 50.0f) : ToneMappingOperator()
    {
        images.push_back(NULL);
        update(Ld_Max, Ld_a, Lw_a);
    }

    ~FerwerdaTMO()
    {
        release();
    }

    /**
     * @brief update
     * @param Ld_Max
     * @param Ld_a
     * @param Lw_a
     */
    void update(float Ld_Max = 100.0f, float Ld_a = 50.0f, float Lw_a = 50.0f)
    {
        this->Ld_Max = Ld_Max > 0.0f ? Ld_Max : 100.0f;
        this->Ld_a = Ld_a > 0.0f ? Ld_a : (this->Ld_Max / 2.0f);
        this->Lw_a = Lw_a;
    }

    /**
     * @brief Ts computes the gamma function used in Ferwerda TMO for Scotopic levels (rods' cells).
     * @param x
     * @return
     */
    static float Ts(float x)
    {
        float t = log10f(x);
        float y;

        if(t <= -3.94f) {
            y = -2.86f;
        } else {
            if(t >= -1.44) {
                y = t - 0.395f;
            } else {
                y = powf(0.405f * t + 1.6f, 2.18f) - 2.86f;
            }
        }

        y = powf(10.0f, y);

        return y;
    }

    /**
     * @brief Tp computes the gamma function used in Ferwerda TMO for Photopic levels (cones' cells).
     * @param x
     * @return
     */
    static float Tp(float x)
    {
        float t = log10f(x);
        float y;

        if(t <= -2.6f) {
            y = -0.72f;
        } else {
            if(t >= 1.9f) {
                y = t - 1.255f;
            } else {
                y = powf(0.249f * t + 0.65f, 2.7f) - 0.72f;
            }
        }

        y = powf(10.0f, y);

        return y;
    }

    /**
     * @brief WalravenValetonK
     * @param Lw_a is the world adaptation luminance in cd/m^2
     * @param sigma
     * @return
     */
    static float WalravenValetonK(float Lw_a, float sigma = 100.0f)
    {
        float k = (sigma - Lw_a / 4.0f) / (sigma + Lw_a);
        return (k > 0.0f) ?  k : 0.0f;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FerwerdaTMO ftmo(200.0f, -1.0f, -1.0f);
        return ftmo.Process(Single(imgIn), imgOut);
    }
};
} // end namespace pic

#endif /* PIC_TONE_MAPPING_FERWERDA_TMO_HPP */

