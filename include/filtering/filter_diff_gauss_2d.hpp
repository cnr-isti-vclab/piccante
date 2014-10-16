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

#ifndef PIC_FILTERING_FILTER_DIFF_GAUSS_2D_HPP
#define PIC_FILTERING_FILTER_DIFF_GAUSS_2D_HPP

#include "filtering/filter.hpp"
#include "filtering/filter_conv_1d.hpp"
#include "util/precomputed_difference_of_gaussians.hpp"

namespace pic {

class FilterDiffGauss: public FilterNPasses
{
protected:
    FilterConv1D                        *filter;
    PrecomputedDifferenceOfGaussians    *pdg;

public:
    //Basic constructor
    FilterDiffGauss(float sigma_1, float sigma_2)
    {
        //Difference of Gaussian filter
        pdg = new PrecomputedDifferenceOfGaussians(sigma_1, sigma_2);

        filter = new FilterConv1D(pdg->coeff, pdg->kernelSize);

        InsertFilter(filter);
        InsertFilter(filter);
    }

    ~FilterDiffGauss()
    {
        Destroy();

        if(filter != NULL) {
            delete filter;
        }

        if(pdg != NULL) {
            delete pdg;
        }
    }

    static Image *Execute(Image *imgIn, Image *imgOut, float sigma_1,
                             float sigma_2)
    {
        FilterDiffGauss filter(sigma_1, sigma_2);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(std::string nameIn, std::string nameOut, float sigma_1,
                             float sigma_2)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, sigma_1, sigma_2);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DIFF_GAUSS_2D_HPP */

