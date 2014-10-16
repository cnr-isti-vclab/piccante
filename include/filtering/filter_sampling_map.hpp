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

#ifndef PIC_FILTERING_FILTER_SAMPLING_MAP_HPP
#define PIC_FILTERING_FILTER_SAMPLING_MAP_HPP

#include "filtering/filter_npasses.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_gradient.hpp"
#include "filtering/filter_sigmoid_tmo.hpp"
#include "filtering/filter_sampler_2d.hpp"
#include "filtering/filter_gaussian_2d.hpp"

namespace pic {

class FilterSamplingMap: public FilterNPasses
{
protected:
    ImageSamplerBilinear    isb;
    float					scale;

    FilterLuminance			*fltL;
    FilterGradient			*fltG;
    FilterSigmoidTMO		*fltS;
    FilterSampler2D			*fltD;
    FilterGaussian2D		*fltG2D;

    void Setup(float sigma, float scale);

public:
    //Basic constructors
    FilterSamplingMap(float sigma);
    FilterSamplingMap(float sigma, float scale);

    ~FilterSamplingMap();

    //getScale
    float getScale()
    {
        return scale;
    }

    //Filtering
    static Image *Execute(std::string nameIn, std::string nameOut, float sigma,
                             float scale)
    {
        Image imgIn(nameIn);

        FilterSamplingMap filter(sigma, scale);
        Image *imgOut = filter.Process(Single(&imgIn), NULL);

        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Basic constructors
FilterSamplingMap::FilterSamplingMap(float sigma) : FilterNPasses()
{
    float rateScale = 2.0f;
    Setup(rateScale, rateScale / sigma);
}

FilterSamplingMap::FilterSamplingMap(float sigma, float scale) : FilterNPasses()
{
    Setup(sigma * scale, scale);
}

FilterSamplingMap::~FilterSamplingMap()
{
    if(fltL != NULL) {
        delete fltL;
    }

    if(fltD != NULL) {
        delete fltD;
    }

    if(fltS != NULL) {
        delete fltS;
    }

    if(fltG != NULL) {
        delete fltG;
    }

    if(fltG2D != NULL) {
        delete fltG2D;
    }

}

void FilterSamplingMap::Setup(float sigma, float scale)
{
    this->scale = scale;

    //Allocate filters
    fltL = new FilterLuminance(LT_CIE_LUMINANCE);
    fltD = new FilterSampler2D(scale, &isb);
    fltS = new FilterSigmoidTMO();
    fltG = new FilterGradient(0);
    fltG2D = new FilterGaussian2D(sigma);

    InsertFilter(fltL);     //Luminance
    InsertFilter(fltD);     //Downsampling
    InsertFilter(fltS);     //Sigmoid TMO
    InsertFilter(fltG);     //Gradient
    InsertFilter(fltG2D);   //Gaussian
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLING_MAP_HPP */

