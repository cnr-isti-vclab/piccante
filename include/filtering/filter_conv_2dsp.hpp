/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_FILTERING_FILTER_CONV_2DSP_HPP
#define PIC_FILTERING_FILTER_CONV_2DSP_HPP

#include "filtering/filter_npasses.hpp"
#include "filtering/filter_conv_1d.hpp"

namespace pic {

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

        InsertFilter(conv1DFltX);
        InsertFilter(conv1DFltX);
    }

    /**
     * @brief FilterConv2DSP
     * @param dataX
     * @param nX
     * @param dataY
     * @param nY
     */
    FilterConv2DSP(float *dataX, int nX, float *dataY, int nY)
    {
        conv1DFltX = new FilterConv1D(dataX, nX);
        InsertFilter(conv1DFltX);

        conv1DFltY = new FilterConv1D(dataY, nY);
        InsertFilter(conv1DFltY);
    }

    ~FilterConv2DSP()
    {
        Destroy();

        if(conv1DFltX != NULL) {
            delete conv1DFltX;
        }

        conv1DFltX = NULL;

        if(conv1DFltY != NULL) {
            delete conv1DFltY;
        }

        conv1DFltY = NULL;
    }

    static Image *Execute(Image *imgIn, Image *imgOut, float *data, int n)
    {
        FilterConv2DSP filter(data, n);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(std::string nameIn, std::string nameOut, float *data,
                             int n)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, data, n);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_2DSP_HPP */

