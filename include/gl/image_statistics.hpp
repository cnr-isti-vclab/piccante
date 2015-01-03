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

#ifndef PIC_GL_IMAGE_STATISTICS
#define PIC_GL_IMAGE_STATISTICS

#include <vector>

#include "gl/image.hpp"
#include "gl/filtering/filter_redux.hpp"

namespace pic {

/**
 * @brief The ImageStatisticsGL class
 */
class ImageStatisticsGL
{
protected:
    FilterGLRedux *flt_mean, *flt_max, *flt_min;

public:
    /**
     * @brief ImageStatisticsGL
     */
    ImageStatisticsGL()
    {
        flt_mean = FilterGLRedux::CreateMean();
        flt_max = FilterGLRedux::CreateMax();
        flt_min = FilterGLRedux::CreateMin();
    }

    /**
     * @brief getVal
     * @param imgIn
     * @param ret
     * @param flt
     * @return
     */
    float *getVal(ImageGL *imgIn, float *ret, FilterGLRedux *flt)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(imgIn->stack.empty()) {
            FilterGLRedux::CreateData(imgIn->width, imgIn->height,
                                      imgIn->channels, imgIn->stack, 1);
        }

        ImageGL *out = flt->Redux(imgIn, imgIn->stack);

        out->loadToMemory();

        if(ret == NULL) {
            ret = new float [imgIn->channels];
        }

        for(int i = 0; i < imgIn->channels; i++) {
            ret[i] = out->data[i];
        }

        return ret;
    }

    /**
     * @brief getMaxVal
     * @param imgIn
     * @param ret
     * @return
     */
    float *getMaxVal(ImageGL *imgIn, float *ret = NULL)
    {
        return getVal(imgIn, ret, flt_max);
    }

    /**
     * @brief getMinVal
     * @param imgIn
     * @return
     */
    float *getMinVal(ImageGL *imgIn, float *ret = NULL)
    {
        return getVal(imgIn, ret, flt_min);
    }

    /**
     * @brief getMeanVal
     * @param imgIn
     * @return
     */
    float *getMeanVal(ImageGL *imgIn, float *ret = NULL)
    {
        return getVal(imgIn, ret, flt_mean);
    }

};

} // end namespace pic

#endif /* PIC_GL_IMAGE_STATISTICS */

