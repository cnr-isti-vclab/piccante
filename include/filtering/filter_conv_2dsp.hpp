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

class FilterConv2D: public FilterNPasses
{
protected:
    FilterConv1D *conv1DFlt;

public:
    //Basic constructor
    FilterConv2D(float *data, int n)
    {
        conv1DFlt = new FilterConv1D(data, n);

        InsertFilter(conv1DFlt);
        InsertFilter(conv1DFlt);
    }

    ~FilterConv2D()
    {
        Destroy();

        if(conv1DFlt != NULL) {
            delete conv1DFlt;
        }

        conv1DFlt = NULL;
    }

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, float *data, int n)
    {
        FilterConv2D filter(data, n);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static ImageRAW *Execute(std::string nameIn, std::string nameOut, float *data,
                             int n)
    {
        ImageRAW imgIn(nameIn);
        ImageRAW *imgOut = Execute(&imgIn, NULL, data, n);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_2DSP_HPP */

