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

#ifndef PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP
#define PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterAbsoluteDifference class
 */
class FilterAbsoluteDifference: public Filter
{
protected:

    /**
     * @brief f
     * @param data
     */
    void f(FilterFData *data)
    {
        float *dataIn0 = (*data->src[0])(data->x, data->y);
        float *dataIn1 = (*data->src[1])(data->x, data->y);

        for(int k = 0; k < data->dst->channels; k++) {
            data->out[k] = fabsf(dataIn1[k] - dataIn0[k]);
        }
    }

public:

    /**
     * @brief FilterAbsoluteDifference
     */
    FilterAbsoluteDifference() : Filter()
    {

    }

    /**
     * @brief execute
     * @param imgIn1
     * @param imgIn2
     * @return
     */
    static Image *execute(Image *imgIn1, Image *imgIn2)
    {
        FilterAbsoluteDifference filter;
        return filter.Process(Double(imgIn1, imgIn2), NULL);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP */

