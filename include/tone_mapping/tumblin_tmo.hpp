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

#ifndef PIC_TONE_MAPPING_TUMBLIN_TMO_HPP
#define PIC_TONE_MAPPING_TUMBLIN_TMO_HPP

#include "../base.hpp"

#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The TumblinTMO class
 */
class TumblinTMO: public ToneMappingOperator
{
protected:
    float Ld_Max, Ld_a, Lw_a, C_Max;
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

        float Lw_Max;
        images[0]->getMaxVal(NULL, &Lw_Max);

        float Lw_a_t;
        if(Lw_a > 0.0f) {
            Lw_a_t = Lw_a;
        } else {
            images[0]->getLogMeanVal(NULL, &Lw_a_t);
        }

        float gamma_w = StevenCSF(Lw_a_t);
        float gamma_d = StevenCSF(Ld_a);
        float gamma_wd = gamma_w / (1.855f + 0.4f * logf(Ld_a));
        float m = powf(C_Max, (gamma_wd - 1.0f) / 2.0f);

        float exponent = gamma_w / gamma_d;
        float scale_norm = Lw_a;
        float scale = Ld_a * m / Ld_Max;

        imgOut->assign(imgIn[0]);

        std::vector<float> param;
        param.push_back(exponent);
        param.push_back(scale_norm);
        param.push_back(scale);
        images[0]->applyFunctionParam(TumblinFun, param);

        (*imgOut) *= (*images[0]);

        return imgOut;
    }

    /**
     * @brief TumblinFun
     * @param x
     * @param param
     * @return
     */
    static float TumblinFun(float Lw, std::vector<float> &param)
    {
        if(Lw > 0.0f) {
            float Ld = powf(Lw / param[1], param[0]) * param[2];

            return Ld / Lw;
        } else {
            return 0.0f;
        }
    }

public:

    /**
     * @brief TumblinTMO
     * @param Ld_a
     * @param Ld_Max
     * @param C_Max
     * @param Lw_b
     */
    TumblinTMO(float Ld_a = 20.0f, float Ld_Max = 100.0f, float C_Max = 100.0f, float Lw_a = -1.0f) : ToneMappingOperator()
    {
        images.push_back(NULL);
        update(Ld_a, Ld_Max, C_Max, Lw_a);
    }

    ~TumblinTMO()
    {
        release();
    }

    /**
     * @brief update
     * @param Ld_a
     * @param Ld_Max
     * @param C_Max
     * @param Lw_a
     */
    void update(float Ld_a = 20.0f, float Ld_Max = 100.0f, float C_Max = 100.0f, float Lw_a = 1.0f)
    {
        this->Ld_a = Ld_a > 0.0f ? Ld_a : 20.0f;
        this->Ld_Max = Ld_Max > 0.0f ? Ld_Max : 100.0f;
        this->C_Max = C_Max > 0.0f ? C_Max : 100.0f;
        this->Lw_a = Lw_a > 0.0f ? Lw_a : 1.0f;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        TumblinTMO ttmo;
        return ttmo.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief StevenCSF
     * @param x
     * @return
     */
    static float StevenCSF(float x)
    {
        if(x <= 100.0f) {
            return 1.855f + 0.4f * log10f(x + 2.3e-5f);
        } else {
            return 2.655f;
        }
    }
};
} // end namespace pic

#endif /* PIC_TONE_MAPPING_TUMBLIN_TMO_HPP */

