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

#ifndef PIC_FILTERING_FILTER_GAUSSIAN_2D_HPP
#define PIC_FILTERING_FILTER_GAUSSIAN_2D_HPP

#include "../util/std_util.hpp"

#include "../filtering/filter_npasses.hpp"
#include "../filtering/filter_gaussian_1d.hpp"

namespace pic {

/**
 * @brief The FilterGaussian2D class
 */
class FilterGaussian2D: public FilterNPasses
{
protected:
    FilterGaussian1D *filter;

public:

    /**
     * @brief FilterGaussian2D
     */
    FilterGaussian2D() : FilterNPasses()
    {
        filter = new FilterGaussian1D(1.0f);

        insertFilter(filter);
        insertFilter(filter);
    }

    /**
     * @brief FilterGaussian2D
     * @param sigma
     */
    FilterGaussian2D(float sigma) : FilterNPasses()
    {
        filter = new FilterGaussian1D(sigma);

        insertFilter(filter);
        insertFilter(filter);
    }

    ~FilterGaussian2D()
    {
        release();

        filter = delete_s(filter);
    }

    /**
     * @brief update
     * @param sigma
     */
    void update(float sigma)
    {
        filter->update(sigma, 0);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float sigma)
    {
        FilterGaussian2D filter(sigma);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GAUSSIAN_2D_HPP */

