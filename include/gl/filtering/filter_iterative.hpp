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

#ifndef PIC_GL_FILTERING_FILTER_ITERATIVE_HPP
#define PIC_GL_FILTERING_FILTER_ITERATIVE_HPP

#include "../../base.hpp"

#include "../../util/gl/fbo.hpp"

#include "../../gl/filtering/filter_npasses.hpp"

namespace pic {

/**
 * @brief The FilterGLIterative class
 */
class FilterGLIterative: public FilterGLNPasses
{
protected:
    int iterations;

    /**
     * @brief getFilter
     * @param i
     * @return
     */
    FilterGL* getFilter(int i);

    /**
     * @brief getFilter
     * @param i
     * @return
     */
    int getIterations();

public:

    /**
     * @brief FilterGLIterative
     * @param flt
     * @param iterations
     */
    FilterGLIterative(FilterGL *flt, int iterations);

    ~FilterGLIterative();

    /**
     * @brief update
     * @param flt
     * @param iterations
     */
    void update(FilterGL *flt, int iterations);
};

PIC_INLINE FilterGLIterative::FilterGLIterative(FilterGL *flt, int iterations) : FilterGLNPasses()
{
    update(flt, iterations);
}

PIC_INLINE FilterGLIterative::~FilterGLIterative()
{
    release();
}

PIC_INLINE void FilterGLIterative::update(FilterGL *flt, int iterations)
{
    if(iterations > 0) {
        this->iterations = iterations;
    }

    if(flt == NULL) {
        return;
    }

    if(!filters.empty()) {
        filters.clear();
    }

    filters.push_back(flt);
}

PIC_INLINE FilterGL* FilterGLIterative::getFilter(int i)
{
    return filters[0];
}

PIC_INLINE int FilterGLIterative::getIterations()
{
    return iterations;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_ITERATIVE_HPP */

