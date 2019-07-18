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

#include "../../util/std_util.hpp"

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

    /**
     * @brief update
     * @param sigma
     * @param scale
     */
    void update(float sigma, float scale)
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

public:
    /**
     * @brief FilterGLSamplingMap
     * @param sigma
     */
    FilterGLSamplingMap(float sigma) : FilterGLNPasses()
    {
        target = GL_TEXTURE_2D;
        float rateScale = 2.0f;
        update(rateScale, rateScale / sigma);
    }


    /**
     * @brief FilterGLSamplingMap
     * @param sigma
     * @param scale
     */
    FilterGLSamplingMap(float sigma, float scale) : FilterGLNPasses()
    {
        target = GL_TEXTURE_2D;
        update(sigma * scale, scale);
    }

    ~FilterGLSamplingMap()
    {
        release();
    }

    /**
     * @brief releaseAux
     */
    void releaseAux()
    {
        delete_s(filterD);
        delete_s(filterS);
        delete_s(filterG);
        delete_s(filterG2D);
    }

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
    Fbo *getFbo()
    {
        if(filters.empty()) {
            return NULL;
        }

        return filters[filters.size() - 1]->getFbo();
    }

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

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP */

