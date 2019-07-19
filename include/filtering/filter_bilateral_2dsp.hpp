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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DSP_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DSP_HPP

#include "../util/std_util.hpp"
#include "../filtering/filter_bilateral_1d.hpp"
#include "../filtering/filter_npasses.hpp"

namespace pic {

/**
 * @brief The FilterBilateral2DSP class
 */
class FilterBilateral2DSP: public FilterNPasses
{
protected:
    FilterBilateral1D *bilateralFilter;

public:
    /**
     * @brief FilterBilateral2DSP
     * @param sigma_s
     * @param sigma_r
     */
    FilterBilateral2DSP(float sigma_s, float sigma_r) : FilterNPasses()
    {
        bilateralFilter = new FilterBilateral1D(sigma_s, sigma_r);

        insertFilter(bilateralFilter);
        insertFilter(bilateralFilter);
    }

    ~FilterBilateral2DSP()
    {
        release();
        delete_s(bilateralFilter);
    }

    /**
     * @brief update
     * @param sigma_s
     * @param sigma_r
     */
    void update(float sigma_s, float sigma_r)
    {
        bilateralFilter->update(sigma_s, sigma_r);
    }
    
    /**
     * @brief signature
     * @return
     */
    std::string signature()
    {
        return genBilString("SP", bilateralFilter->sigma_s, bilateralFilter->sigma_r);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float sigma_s,
                             float sigma_r)
    {
        FilterBilateral2DSP filter(sigma_s, sigma_r);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DSP_HPP */

