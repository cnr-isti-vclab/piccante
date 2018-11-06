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

#ifndef PIC_FILTERING_FILTER_CONV_2DSP_HPP
#define PIC_FILTERING_FILTER_CONV_2DSP_HPP

#include "../filtering/filter_npasses.hpp"
#include "../filtering/filter_conv_1d.hpp"

namespace pic {

/**
 * @brief The FilterConv2DSP class
 */
class FilterConv2DSP: public FilterNPasses
{
protected:
    FilterConv1D *conv1DFltX, *conv1DFltY;

public:

    /**
     * @brief FilterConv2DSP
     * @param data
     * @param n
     */
    FilterConv2DSP(float *data, int n)
    {
        conv1DFltX = new FilterConv1D(data, n);

        insertFilter(conv1DFltX);
        insertFilter(conv1DFltX);
    }

    /**
     * @brief FilterConv2DSP
     * @param dataX
     * @param nX
     * @param dataY
     * @param nY
     */
    FilterConv2DSP(float *dataX, int nX, float *dataY, int nY) : FilterNPasses()
    {
        conv1DFltX = new FilterConv1D(dataX, nX);
        insertFilter(conv1DFltX);

        conv1DFltY = new FilterConv1D(dataY, nY);
        insertFilter(conv1DFltY);
    }

    ~FilterConv2DSP()
    {
        release();

        if(conv1DFltX != NULL) {
            delete conv1DFltX;
        }

        conv1DFltX = NULL;

        if(conv1DFltY != NULL) {
            delete conv1DFltY;
        }

        conv1DFltY = NULL;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param data
     * @param n
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float *data, int n)
    {
        FilterConv2DSP filter(data, n);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_2DSP_HPP */

