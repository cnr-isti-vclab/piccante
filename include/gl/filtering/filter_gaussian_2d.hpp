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

#ifndef PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP
#define PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP

#include "../../base.hpp"

#include "../../util/std_util.hpp"

#include "../../gl/filtering/filter_npasses.hpp"
#include "../../gl/filtering/filter_gaussian_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLGaussian2D class
 */
class FilterGLGaussian2D: public FilterGLNPasses
{
protected:
    FilterGLGaussian1D *filter;

public:
    /**
     * @brief FilterGLGaussian2D
     */
    FilterGLGaussian2D() : FilterGLNPasses()
    {
        target = GL_TEXTURE_2D;

        filter = new FilterGLGaussian1D(1.0f, 0, target);
        insertFilter(filter);
        insertFilter(filter);
    }

    /**
     * @brief FilterGLGaussian2D
     * @param sigma
     */
    FilterGLGaussian2D(float sigma)
    {
        target = GL_TEXTURE_2D;

        filter = new FilterGLGaussian1D(sigma, 0, target);
        insertFilter(filter);
        insertFilter(filter);
    }

    ~FilterGLGaussian2D()
    {
        release();

        delete_s(filter);
    }

    /**
     * @brief update
     * @param sigma
     */
    void update(float sigma)
    {
        filter->update(sigma);
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP */

