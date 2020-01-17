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

#ifndef PIC_TONE_MAPPING_WARD_GLOBAL_TMO_HPP
#define PIC_TONE_MAPPING_WARD_GLOBAL_TMO_HPP

#include "../base.hpp"

#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The WardGlobalTMO class
 */
class WardGlobalTMO: public ToneMappingOperator
{
protected:
    float Ld_Max;
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

        float Lw_a;
        images[0]->getLogMeanVal(NULL, &Lw_a);

        float m = (1.219f + powf(Ld_Max * 0.5f, 0.4f)) /
                  (1.219f + powf(Lw_a , 0.4f));
        m = powf(m, 2.5f);

        float scale = m / Ld_Max;

        imgOut->assign(imgIn[0]);

        (*imgOut) *= scale;

        return imgOut;
    }

public:

    /**
     * @brief WardGlobalTMO
     * @param Ld_Max
     */
    WardGlobalTMO(float Ld_Max = 100.0f) : ToneMappingOperator()
    {
        images.push_back(NULL);
        update(Ld_Max);
    }

    ~WardGlobalTMO()
    {
        release();
    }

    /**
     * @brief update
     * @param Ld_Max
     */
    void update(float Ld_Max = 100.0f)
    {
        this->Ld_Max = Ld_Max > 0.0f ? Ld_Max : 100.0f;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        WardGlobalTMO wgtmo;
        return wgtmo.Process(Single(imgIn), imgOut);
    }
};
} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_GLOBAL_TMO_HPP */

