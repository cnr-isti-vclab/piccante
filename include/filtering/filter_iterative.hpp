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

#ifndef PIC_FILTERING_FILTER_ITERATIVE_HPP
#define PIC_FILTERING_FILTER_ITERATIVE_HPP

#include "../filtering/filter_npasses.hpp"

namespace pic {

/**
 * @brief The FilterIterative class
 */
class FilterIterative: public FilterNPasses
{
protected:
    int iterations;

    /**
     * @brief getFilter
     * @param i
     * @return
     */
    Filter* getFilter(int i);

    /**
     * @brief getFilter
     * @param i
     * @return
     */
    int getIterations();

public:

    /**
     * @brief FilterIterative
     * @param flt
     * @param iterations
     */
    FilterIterative(Filter *flt, int iterations);

    /**
     * @brief update
     * @param flt
     * @param iterations
     */
    void update(Filter *flt, int iterations);

};

PIC_INLINE FilterIterative::FilterIterative(Filter *flt, int iterations) : FilterNPasses()
{
    printf("\n\n%d\n\n", iterations);
    update(flt, iterations);
}

PIC_INLINE void FilterIterative::update(Filter *flt, int iterations)
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

PIC_INLINE Filter* FilterIterative::getFilter(int i)
{
    return filters[0];
}

PIC_INLINE int FilterIterative::getIterations()
{
    return iterations;
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ITERATIVE_HPP */

