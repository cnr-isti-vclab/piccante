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

#ifndef PIC_GL_FILTERING_FILTER_GAUSSIAN_3D_HPP
#define PIC_GL_FILTERING_FILTER_GAUSSIAN_3D_HPP

#include "../../base.hpp"

#include "../../util/std_util.hpp"

#include "../../gl/filtering/filter_npasses.hpp"
#include "../../gl/filtering/filter_gaussian_1d.hpp"

namespace pic {

class FilterGLGaussian3D: public FilterGLNPasses
{
protected:
    FilterGLGaussian1D *filter;

public:
    /**
     * @brief FilterGLGaussian3D
     */
    FilterGLGaussian3D() : FilterGLNPasses()
    {
        target = GL_TEXTURE_3D;
    }

    ~FilterGLGaussian3D()
    {
        release();

        delete_s(filter);
    }

    /**
     * @brief FilterGLGaussian3D
     * @param sigma
     */
    FilterGLGaussian3D(float sigma) : FilterGLNPasses()
    {
        filter = new FilterGLGaussian1D(sigma, 0, GL_TEXTURE_3D);
        target = GL_TEXTURE_3D;

        insertFilter(filter);
        insertFilter(filter);
        insertFilter(filter);
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

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_3D_HPP */

