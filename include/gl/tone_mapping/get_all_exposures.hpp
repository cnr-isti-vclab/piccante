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

#ifndef PIC_GL_TONE_MAPPING_GET_ALL_EXPOSURES_HPP
#define PIC_GL_TONE_MAPPING_GET_ALL_EXPOSURES_HPP

#include "../../base.hpp"

#include "../../tone_mapping/get_all_exposures.hpp"

#include "../../gl/filtering/filter_simple_tmo.hpp"

namespace pic {

/**
 * @brief getAllExposuresImagesGL converts an HDR image into a stack of LDR images
 * @param imgIn
 * @return
 */
PIC_INLINE ImageGLVec getAllExposuresImagesGL(ImageGL *imgIn, float gamma = 2.2f)
{
    ImageGLVec ret;

    if(imgIn == NULL) {
        return ret;
    }

    std::vector<float> fstops = getAllExposures((Image*) imgIn);

    FilterGLSimpleTMO flt(gamma, 0.0f);

    ImageGLVec input = SingleGL(imgIn);

    for(unsigned int i = 0; i < fstops.size(); i++) {
        flt.update(gamma, fstops[i]);
        ImageGL *expo = flt.Process(input, NULL);

        expo->exposure = powf(2.0f, fstops[i]);
        expo->clamp(0.0f, 1.0f);

        ret.push_back(expo);
    }

    return ret;
}

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_GET_ALL_EXPOSURES_HPP */

