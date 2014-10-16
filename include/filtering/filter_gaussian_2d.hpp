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

#ifndef PIC_FILTERING_FILTER_GAUSSIAN_2D_HPP
#define PIC_FILTERING_FILTER_GAUSSIAN_2D_HPP

#include "filtering/filter_npasses.hpp"
#include "filtering/filter_gaussian_1d.hpp"

namespace pic {

class FilterGaussian2D: public FilterNPasses
{
protected:
    FilterGaussian1D *gaussianFilter;

public:
    //Basic constructor
    FilterGaussian2D(float sigma)
    {
        //Gaussian filter
        gaussianFilter = new FilterGaussian1D(sigma);

        InsertFilter(gaussianFilter);
        InsertFilter(gaussianFilter);
    }

    ~FilterGaussian2D()
    {
        Destroy();

        if(gaussianFilter!=NULL) {
            delete gaussianFilter;
        }
    }

    static Image *Execute(Image *imgIn, Image *imgOut, float sigma)
    {
        FilterGaussian2D filter(sigma);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(std::string nameIn, std::string nameOut, float sigma)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, sigma);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GAUSSIAN_2D_HPP */

