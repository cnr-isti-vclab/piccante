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

#include "../filtering/filter_npasses.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gradient.hpp"
#include "../filtering/filter_sigmoid_tmo.hpp"
#include "../filtering/filter_sampler_2d.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_channel.hpp"

namespace pic {

/**
 * @brief The FilterSamplingMap class
 */
class FilterSamplingMap: public FilterNPasses
{
protected:
    ImageSamplerBilinear isb;
    float scale;

    FilterLuminance *fltL;
    FilterGradient *fltG;
    FilterSigmoidTMO *fltS;
    FilterSampler2D *fltD;
    FilterGaussian2D *fltG2D;
    FilterChannel *fltC;

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
     * @brief update
     * @param sigma
     * @param scale
     */
    void update(float sigma, float scale);

    /**
     * @brief getScale
     * @return
     */
    float getScale()
    {
        return scale;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma
     * @param scale
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float sigma)
    {
        FilterSamplingMap filter(sigma);
        imgOut = filter.Process(Single(imgIn), NULL);

        return imgOut;
    }
};

PIC_INLINE FilterSamplingMap::FilterSamplingMap(float sigma) : FilterNPasses()
{
    fltL = NULL;
    fltD = NULL;
    fltS = NULL;
    fltG = NULL;
    fltG2D = NULL;
    fltC = NULL;

    float rateScale = 2.0f;
    update(rateScale, rateScale / sigma);
}

PIC_INLINE FilterSamplingMap::FilterSamplingMap(float sigma, float scale) : FilterNPasses()
{
    fltL = NULL;
    fltD = NULL;
    fltS = NULL;
    fltG = NULL;
    fltG2D = NULL;

    update(sigma * scale, scale);
}

PIC_INLINE FilterSamplingMap::~FilterSamplingMap()
{
    delete_s(fltL);
    delete_s(fltD);
    delete_s(fltS);
    delete_s(fltC);
    delete_s(fltG);
    delete_s(fltG2D);
}

PIC_INLINE void FilterSamplingMap::update(float sigma, float scale)
{
    this->scale = scale;

    //allocate filters
    fltL = new FilterLuminance(LT_CIE_LUMINANCE);
    fltD = new FilterSampler2D(scale, &isb);
    fltS = new FilterSigmoidTMO();
    fltG = new FilterGradient();
    fltC = new FilterChannel(SingleInt(2));
    fltG2D = new FilterGaussian2D(sigma);

    insertFilter(fltL);     //Luminance
    insertFilter(fltD);     //Downsampling
    insertFilter(fltS);     //Sigmoid TMO
    insertFilter(fltG);     //Gradient
    insertFilter(fltC);     //Gradient
    insertFilter(fltG2D, true);   //Gaussian
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SAMPLING_MAP_HPP */

