/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_GL_IMAGE_STATISTICS
#define PIC_GL_IMAGE_STATISTICS

#include <vector>

#include "gl/image_raw.hpp"
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
    float *getVal(ImageRAWGL *imgIn, float *ret, FilterGLRedux *flt) {
        if(imgIn == NULL) {
            return NULL;
        }

        if(imgIn->stack.empty()) {
            FilterGLRedux::CreateData(imgIn->width, imgIn->height,
                                      imgIn->channels, imgIn->stack, 1);
        }

        ImageRAWGL *out = flt->Redux(imgIn, imgIn->stack);

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
    float *getMaxVal(ImageRAWGL *imgIn, float *ret = NULL) {
        return getVal(imgIn, ret, flt_max);
    }

    /**
     * @brief getMinVal
     * @param imgIn
     * @return
     */
    float *getMinVal(ImageRAWGL *imgIn, float *ret = NULL) {
        return getVal(imgIn, ret, flt_min);
    }

    /**
     * @brief getMeanVal
     * @param imgIn
     * @return
     */
    float *getMeanVal(ImageRAWGL *imgIn, float *ret = NULL) {
        return getVal(imgIn, ret, flt_mean);
    }

};

} // end namespace pic

#endif /* PIC_GL_IMAGE_STATISTICS */

