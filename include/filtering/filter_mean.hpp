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

#ifndef PIC_FILTERING_FILTER_MEAN_HPP
#define PIC_FILTERING_FILTER_MEAN_HPP

#include "../util/std_util.hpp"
#include "../filtering/filter_npasses.hpp"
#include "../filtering/filter_conv_1d.hpp"

namespace pic {

/**
 * @brief The FilterMean class
 */
class FilterMean: public FilterNPasses
{
protected:

protected:
    FilterConv1D *filter;
    float *data; //NOTE: we own this; so it can be deleted!
    int size;

public:

    /**
     * @brief FilterMean
     * @param size
     */
    FilterMean(int size) : FilterNPasses()
    {
        data = NULL;
        this->size = -1;

        update(size);

        filter = new FilterConv1D(data, size);

        insertFilter(filter);
        insertFilter(filter);
    }

    ~FilterMean()
    {
        release();

        delete_s(filter);
        delete_vec_s(data);
    }

    /**
     * @brief update
     * @param size
     */
    void update(int size)
    {
        size = size > 0 ? size : 3;

        if(this->size != size) {
            this->size = size;

            data = delete_vec_s(data);
            data = FilterConv1D::getKernelMean(size);
        }        
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int size)
    {
        FilterMean filter(size);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MEAN_HPP */

