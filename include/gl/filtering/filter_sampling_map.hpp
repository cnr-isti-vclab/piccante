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

#ifndef PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP
#define PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP

#include "../../base.hpp"

#include "../../util/gl/fbo.hpp"

#include "../../gl/filtering/filter_npasses.hpp"
#include "../../gl/filtering/filter_gradient.hpp"
#include "../../gl/filtering/filter_sigmoid_tmo.hpp"
#include "../../gl/filtering/filter_sampler_2d.hpp"
#include "../../gl/filtering/filter_gaussian_2d.hpp"

namespace pic {

/**
 * @brief The FilterGLSamplingMap class
 */
class FilterGLSamplingMap: public FilterGLNPasses
{
protected:
    float scale, sigma;

    FilterGLGradient *filterG;
    FilterGLSigmoidTMO *filterS;
    FilterGLSampler2D *filterD;
    FilterGLGaussian2D *filterG2D;

    void initShaders() {}

    void FragmentShader() {}

    /**
     * @brief update
     * @param sigma
     * @param scale
     */
    void update(float sigma, float scale);

public:
    /**
     * @brief FilterGLSamplingMap
     * @param sigma
     */
    FilterGLSamplingMap(float sigma);

    /**
     * @brief FilterGLSamplingMap
     * @param sigma
     * @param scale
     */
    FilterGLSamplingMap(float sigma, float scale);

    ~FilterGLSamplingMap();

    /**
     * @brief getScale
     * @return
     */
    float getScale()
    {
        return scale;
    }

    /**
     * @brief getFbo
     * @return
     */
    Fbo *getFbo();

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma
     * @return
     */
    static ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut, float sigma)
    {
        FilterGLSamplingMap filter(sigma);

        imgOut = filter.Process(SingleGL(imgIn), imgOut);

        return imgOut;
    }
};

PIC_INLINE FilterGLSamplingMap::FilterGLSamplingMap(float sigma): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;
    float rateScale = 2.0f;
    update(rateScale, rateScale / sigma);
}

PIC_INLINE FilterGLSamplingMap::FilterGLSamplingMap(float sigma,
        float scale): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;
    update(sigma * scale, scale);
}

PIC_INLINE void FilterGLSamplingMap::update(float sigma, float scale)
{
    this->sigma = sigma;
    this->scale = scale;

    filterD = new FilterGLSampler2D(scale);
    filterS = new FilterGLSigmoidTMO();
    filterG = new FilterGLGradient();
    filterG2D = new FilterGLGaussian2D(sigma);

    insertFilter(filterD);
    insertFilter(filterS);
    insertFilter(filterG);
    insertFilter(filterG2D);
}

PIC_INLINE FilterGLSamplingMap::~FilterGLSamplingMap()
{
    release();

    if(filterD != NULL) {
        delete filterD;
    }

    if(filterS != NULL) {
        delete filterS;
    }

    if(filterG != NULL) {
        delete filterG;
    }

    if(filterG2D != NULL) {
        delete filterG2D;
    }
}

PIC_INLINE Fbo *FilterGLSamplingMap::getFbo()
{
    if(filters.empty()) {
        return NULL;
    }

    return filters[filters.size() - 1]->getFbo();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP */

