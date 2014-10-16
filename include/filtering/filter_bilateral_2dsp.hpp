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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DSP_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DSP_HPP

#include "filtering/filter_bilateral_1d.hpp"
#include "filtering/filter_npasses.hpp"

namespace pic {

class FilterBilateral2DSP: public FilterNPasses
{
protected:
    FilterBilateral1D *bilateralFilter;

public:
    //Standard constructor
    FilterBilateral2DSP(float sigma_s, float sigma_r)
    {
        //Bilatearl 1D Filter
        bilateralFilter = new FilterBilateral1D(sigma_s, sigma_r);

        InsertFilter(bilateralFilter);
        InsertFilter(bilateralFilter);
    }

    ~FilterBilateral2DSP()
    {
        delete bilateralFilter;
    }

    void Update(float sigma_s, float sigma_r)
    {
        bilateralFilter->Update(sigma_s, sigma_r);
    }
    
    std::string Signature()
    {
        return GenBilString("SP", bilateralFilter->sigma_s, bilateralFilter->sigma_r);
    }

    static Image *Execute(Image *imgIn, Image *imgOut, float sigma_s,
                             float sigma_r)
    {
        FilterBilateral2DSP filter(sigma_s, sigma_r);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(std::string nameIn, std::string nameOut, float sigma_s,
                             float sigma_r)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, sigma_s, sigma_r);
        imgOut->Write(nameOut);
        return imgOut;
    }

};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DSP_HPP */

