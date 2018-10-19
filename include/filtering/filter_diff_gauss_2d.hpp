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

#include "../filtering/filter.hpp"
#include "../filtering/filter_gaussian_2d.hpp"

namespace pic {

/**
 * @brief The FilterDiffGauss class
 */
class FilterDiffGauss: public Filter
{
protected:
    FilterGaussian2D *filter_1, *filter_2;
    Image *tmp;

public:
    /**
     * @brief FilterDiffGauss
     * @param sigma_1
     * @param sigma_2
     */
    FilterDiffGauss(float sigma_1, float sigma_2)
    {
        filter_1 = new FilterGaussian2D(sigma_1);
        filter_2 = new FilterGaussian2D(sigma_2);
        tmp = NULL;
    }

    ~FilterDiffGauss()
    {
        if(filter_1 != NULL) {
            delete filter_1;
        }

        if(filter_2 != NULL) {
            delete filter_2;
        }

        if(tmp != NULL) {
            delete tmp;
        }
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        imgOut = filter_1->Process(imgIn, imgOut);

        //MEMORY-LEAK: to check
        tmp = filter_2->Process(imgIn, tmp);
        *imgOut -= *tmp;
        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma_1
     * @param sigma_2
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float sigma_1,
                             float sigma_2)
    {
        FilterDiffGauss filter(sigma_1, sigma_2);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DIFF_GAUSS_2D_HPP */

