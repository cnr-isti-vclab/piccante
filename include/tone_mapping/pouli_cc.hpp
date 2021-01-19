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

#ifndef PIC_TONE_MAPPING_DURAND_TMO_HPP
#define PIC_TONE_MAPPING_DURAND_TMO_HPP

#include "../base.hpp"

#include "../util/string.hpp"
#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../algorithms/bilateral_separation.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

/**
 * @brief The DurandTMO class
 */
class CCPOuli: public ToneMappingOperator
{
public:

    FilterLuminance flt_lum;
    float target_contrast;

    /**
     * @brief DurandTMO
     */
    DurandTMO(float target_contrast = 5.0f)
    {
        images.push_back(NULL);
        images.push_back(NULL);
        images.push_back(NULL);
        update(target_contrast);
    }

    ~DurandTMO()
    {
        release();
    }

    /**
     * @brief update
     * @param target_contrast
     */
    void update(float target_contrast = 5.0f)
    {
        if(target_contrast <= 0.0f) {
            target_contrast = 5.0f;
        }

        this->target_contrast = target_contrast;
    }

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessAux(ImageVec imgIn, Image *imgOut)
    {
        updateImage(imgIn[0]);

        //luminance image
        images[2] = flt_lum.Process(imgIn, images[2]);

        //bilateral filter seperation
        bilateralSeparation(images[2], images, -1.0f, 0.4f, true);

        Image *base = images[0];
        Image *detail = images[1];

        float min_log_base, max_log_base;
        base->getMinVal(NULL, &min_log_base);
        base->getMaxVal(NULL, &max_log_base);

        float compression_factor = log10fPlusEpsilon(target_contrast) / (max_log_base - min_log_base);
        float log_absoulte = compression_factor * max_log_base;

        *base *= compression_factor;
        *base += detail;
        *base -= log_absoulte;
        base->applyFunction(powf10fMinusEpsilon);

        *imgOut = *imgIn[0];
        *imgOut *= base;
        *imgOut /= images[2];

        imgOut->removeSpecials();

        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        DurandTMO dtmo(5.0f);
        return dtmo.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_DURAND_TMO_HPP */

