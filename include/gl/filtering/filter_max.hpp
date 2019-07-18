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

#ifndef PIC_FILTERING_FILTER_GL_MAX_HPP
#define PIC_FILTERING_FILTER_GL_MAX_HPP

#include "../../util/std_util.hpp"

#include "../../gl/filtering/filter_npasses.hpp"
#include "../../gl/filtering/filter_non_linear_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLMax class
 */
class FilterGLMax: public FilterGLNPasses
{
protected:

protected:
    FilterGLNonLinear1D *filter;
    int kernelSize;

public:

    /**
     * @brief FilterMax
     * @param kernelSize
     */
    FilterGLMax(int kernelSize) : FilterGLNPasses()
    {
        filter = new FilterGLNonLinear1D(kernelSize, "max", GL_TEXTURE_2D);

        insertFilter(filter);
        insertFilter(filter);
    }

    ~FilterGLMax()
    {
        release();
        delete_s(filter);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut, int size)
    {
        FilterGLMax filter(size);
        return filter.Process(SingleGL(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GL_MAX_HPP */

