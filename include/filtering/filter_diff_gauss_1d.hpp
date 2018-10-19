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

#ifndef PIC_FILTERING_FILTER_DIFF_GAUSS_1D_HPP
#define PIC_FILTERING_FILTER_DIFF_GAUSS_1D_HPP

#include "../filtering/filter_conv_1d.hpp"
#include "../util/precomputed_diff_of_gaussians.hpp"

namespace pic {

/**
 * @brief The FilterDiffGauss1D class
 */
class FilterDiffGauss1D: public FilterConv1D
{
protected:
    float sigma1, sigma2;
    PrecomputedDiffOfGaussians *pdog;
    bool bpdogOwned;

public:

    /**
     * @brief FilterDiffGauss1D
     * @param sigma
     * @param direction
     */
    FilterDiffGauss1D(float sigma1, float sigma2, int direction);

    /**
     * @brief FilterDiffGauss1D
     * @param pdog
     * @param direction
     */
    FilterDiffGauss1D(PrecomputedDiffOfGaussians *pdog, int direction);

    ~FilterDiffGauss1D();

    static Image *execute(Image *imgIn, Image *imgOut,
                          float sigma1, float sigma2,
                          int direction)
    {
        FilterDiffGauss1D filter(sigma1, sigma2, direction);
        return filter.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterDiffGauss1D::FilterDiffGauss1D(float sigma1, float sigma2, int direction = 0)
{
    this->sigma1 = sigma1;
    this->sigma2 = sigma2;
    pdog = new PrecomputedDiffOfGaussians(sigma1, sigma2);

    bpdogOwned = true;
    update(pdog->coeff, pdog->kernelSize, direction);
}

PIC_INLINE FilterDiffGauss1D::FilterDiffGauss1D(PrecomputedDiffOfGaussians *pdog, int direction = 0)
{
    if(pdog == NULL) {
        #ifdef PICE_DEBUG
            printf("Error no precomputed gaussian values.\n");
        #endif
        return;
    }

    bpdogOwned = false;

    update(pdog->coeff, pdog->kernelSize, direction);
}

PIC_INLINE FilterDiffGauss1D::~FilterDiffGauss1D()
{
    if(pdog != NULL && bpdogOwned) {
        delete pdog;
        pdog = NULL;
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DIFF_GAUSS_1D_HPP */

