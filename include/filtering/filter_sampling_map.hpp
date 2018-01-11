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

#ifndef PIC_FILTERING_FILTER_SAMPLING_MAP_HPP
#define PIC_FILTERING_FILTER_SAMPLING_MAP_HPP

#include "filtering/filter_npasses.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_gradient.hpp"
#include "filtering/filter_sigmoid_tmo.hpp"
#include "filtering/filter_sampler_2d.hpp"
#include "filtering/filter_gaussian_2d.hpp"

namespace pic {

/**
 * @brief The FilterSamplingMap class
 */
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

    /**
     * @brief Setup
     * @param sigma
     * @param scale
     */
    void Setup(float sigma, float scale);

public:
    /**
     * @brief FilterSamplingMap
     * @param sigma
     */
    FilterSamplingMap(float sigma);

    /**
     * @brief FilterSamplingMap
     * @param sigma
     * @param scale
     */
    FilterSamplingMap(float sigma, float scale);

    ~FilterSamplingMap();

    /**
     * @brief getScale
     * @return
     */
    float getScale()
    {
        return scale;
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma
     * @param scale
     * @return
     */
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

FilterSamplingMap::FilterSamplingMap(float sigma) : FilterNPasses()
{
    fltL = NULL;
    fltD = NULL;
    fltS = NULL;
    fltG = NULL;
    fltG2D = NULL;

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
    fltG = new FilterGradient();
    fltG2D = new FilterGaussian2D(sigma);

    InsertFilter(fltL);     //Luminance
    InsertFilter(fltD);     //Downsampling
    InsertFilter(fltS);     //Sigmoid TMO
    InsertFilter(fltG);     //Gradient
    InsertFilter(fltG2D);   //Gaussian
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLING_MAP_HPP */

