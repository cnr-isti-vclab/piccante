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

#ifndef PIC_FILTERING_FILTER_DIFF_GAUSS_2D_OPT_HPP
#define PIC_FILTERING_FILTER_DIFF_GAUSS_2D_OPT_HPP

#include "../filtering/filter_npasses.hpp"
#include "../filtering/filter_diff_gauss_1d.hpp"

namespace pic {

/**
 * @brief The FilterDiffGaussOpt class
 */

class FilterDiffGaussOpt: public FilterNPasses
{
protected:
    FilterDiffGauss1D *flt_dog_1d;

public:
    /**
     * @brief FilterDiffGaussOpt
     * @param sigma_1
     * @param sigma_2
     */
    FilterDiffGaussOpt(float sigma1, float sigma2)
    {
        //Gaussian filter
        flt_dog_1d = new FilterDiffGauss1D(sigma1, sigma2);

        insertFilter(flt_dog_1d);
        insertFilter(flt_dog_1d);
    }

    ~FilterDiffGaussOpt()
    {
        release();

        if(flt_dog_1d != NULL) {
            delete flt_dog_1d;
        }
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DIFF_GAUSS_2D_OPT_HPP */

