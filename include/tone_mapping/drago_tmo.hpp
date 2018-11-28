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

#ifndef PIC_TONE_MAPPING_DRAGO_TMO_HPP
#define PIC_TONE_MAPPING_DRAGO_TMO_HPP

#include "../base.hpp"

#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_drago_tmo.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The DragoTMO class
 */
class DragoTMO: public ToneMappingOperator
{
protected:
    float Ld_Max, b;
    FilterLuminance flt_lum;
    FilterDragoTMO flt_drg;

public:

    /**
     * @brief DragoTMO
     * @param Ld_Max
     * @param b
     */
    DragoTMO(float Ld_Max = 100.0f, float b = 0.95f) : ToneMappingOperator()
    {
        images.push_back(NULL);
        update(Ld_Max, b);
    }

    ~DragoTMO()
    {
        release();
    }

    /**
     * @brief update
     * @param Ld_Max
     * @param b
     */
    void update(float Ld_Max = 100.0f, float b = 0.95f)
    {
        this->Ld_Max = Ld_Max;
        this->b = b;

    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(Image *imgIn, Image *imgOut)
    {
        updateImage(imgIn);

        //compute luminance and its statistics
        images[0] = flt_lum.Process(Single(imgIn), images[0]);

        float Lw_Max, Lw_a;
        images[0]->getMaxVal(NULL, &Lw_Max);
        images[0]->getMaxVal(NULL, &Lw_a);

        //tone map
        flt_drg.update(Ld_Max, b, Lw_Max, Lw_a);
        imgOut = flt_drg.Process(Double(imgIn, images[0]), imgOut);
        return imgOut;
    }
};
} // end namespace pic

#endif /* PIC_TONE_MAPPING_DRAGO_TMO_HPP */

