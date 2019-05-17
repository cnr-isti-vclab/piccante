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

#ifndef PIC_GL_TONE_MAPPING_DRAGO_TMO_HPP
#define PIC_GL_TONE_MAPPING_DRAGO_TMO_HPP

#include <vector>

#include "../../util/array.hpp"
#include "../../util/math.hpp"
#include "../../gl/filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter_drago_tmo.hpp"

namespace pic {

/**
 * @brief The DragoTMOGL class
 */
class DragoTMOGL
{
protected:
    FilterGLLuminance *flt_lum;
    FilterGLDragoTMO  *flt_tmo;
    std::vector<FilterGL* > filters;

    ImageGL *img_lum;
    float LMax, Lwa, Ld_Max, bias;
    bool bStatisticsRecompute, bAllocate;

    /**
     * @brief allocateFilters
     */
    void allocateFilters()
    {
        bAllocate = true;
        flt_lum = new FilterGLLuminance();
        flt_tmo = new FilterGLDragoTMO();

        filters.push_back(flt_lum);
        filters.push_back(flt_tmo);
    }

public:
    /**
     * @brief DragoTMOGL
     */
    DragoTMOGL(float Ld_Max = 100.0f, float bias = 0.85f, bool bStatisticsRecompute = true)
    {
        update(Ld_Max, bias);

        img_lum = NULL;

        bAllocate = false;

        LMax = -1.0f;
        Lwa = -1.0f;

        this->bStatisticsRecompute = bStatisticsRecompute;
    }

    ~DragoTMOGL()
    {
        stdVectorClear<FilterGL>(filters);
        img_lum = delete_s(img_lum);
    }

    /**
     * @brief update
     * @param Ld_Max
     * @param bias
     */
    void update(float Ld_Max = 100.0f, float bias = 0.95f)
    {
        this->Ld_Max = Ld_Max > 0.0f ? Ld_Max : 100.0f;
        this->bias = CLAMPi(bias, 0.0f, 1.0f);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(!bAllocate) {
            allocateFilters();
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        if(bStatisticsRecompute || (LMax < 0.0f)) {
            img_lum->getMaxVal(&LMax);
            img_lum->getLogMeanVal(&Lwa);
        }

        flt_tmo->update(Ld_Max, bias, LMax, Lwa);
        imgOut = flt_tmo->Process(DoubleGL(imgIn, img_lum), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_DRAGO_TMO_HPP */

