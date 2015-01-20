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

#ifndef PIC_FILTERING_FILTER_DIFF_GAUSS_2D_HPP
#define PIC_FILTERING_FILTER_DIFF_GAUSS_2D_HPP

#include "filtering/filter.hpp"
#include "filtering/filter_conv_1d.hpp"
#include "util/precomputed_difference_of_gaussians.hpp"

namespace pic {

/**
 * @brief The FilterDiffGauss class
 */
class FilterDiffGauss: public FilterNPasses
{
protected:
    FilterConv1D                        *filter;
    PrecomputedDifferenceOfGaussians    *pdg;

public:
    /**
     * @brief FilterDiffGauss
     * @param sigma_1
     * @param sigma_2
     */
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

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param sigma_1
     * @param sigma_2
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, float sigma_1,
                             float sigma_2)
    {
        FilterDiffGauss filter(sigma_1, sigma_2);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma_1
     * @param sigma_2
     * @return
     */
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

