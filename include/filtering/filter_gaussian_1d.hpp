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

#ifndef PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP
#define PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP

#include "../filtering/filter_conv_1d.hpp"
#include "../util/precomputed_gaussian.hpp"

namespace pic {

/**
 * @brief The FilterGaussian1D class
 */
class FilterGaussian1D: public FilterConv1D
{
protected:
    float               sigma;
    PrecomputedGaussian *pg;
    bool                bPgOwned;

public:
    /**
     * @brief FilterGaussian1D
     */
    FilterGaussian1D();

    /**
     * @brief FilterGaussian1D
     * @param sigma
     * @param direction
     */
    FilterGaussian1D(float sigma, int direction);

    /**
     * @brief FilterGaussian1D
     * @param pg
     * @param direction
     */
    FilterGaussian1D(PrecomputedGaussian *pg, int direction);

    ~FilterGaussian1D();

    /**
     * @brief update
     * @param sigma
     * @param direction
     */
    void update(float sigma, int direction = 0)
    {
        if(this->sigma != sigma) {
            this->sigma = sigma;

            if(pg != NULL) {
                delete pg;
            }

            pg = new PrecomputedGaussian(sigma);
        }

        bPgOwned = true;
        FilterConv1D::update(pg->coeff, pg->kernelSize, direction);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma
     * @param direction
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float sigma,
                             int direction)
    {
        FilterGaussian1D filter(sigma, direction);
        return filter.Process(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterGaussian1D::FilterGaussian1D()
{
    sigma = 1.0f;
    pg = new PrecomputedGaussian(sigma);

    bPgOwned = true;
    FilterConv1D::update(pg->coeff, pg->kernelSize, 0);
}

PIC_INLINE FilterGaussian1D::FilterGaussian1D(float sigma, int direction = 0)
{
    this->sigma = sigma;
    pg = new PrecomputedGaussian(sigma);

    bPgOwned = true;
    FilterConv1D::update(pg->coeff, pg->kernelSize, direction);
}

PIC_INLINE FilterGaussian1D::FilterGaussian1D(PrecomputedGaussian *pg, int direction = 0)
{
    if(pg == NULL) {
        #ifdef PICE_DEBUG
            printf("Error no precomputed gaussian values.\n");
        #endif
        return;
    }

    bPgOwned = false;

    FilterConv1D::update(pg->coeff, pg->kernelSize, direction);
}

PIC_INLINE FilterGaussian1D::~FilterGaussian1D()
{
    release();

    if(pg != NULL && bPgOwned) {
        delete pg;
        pg = NULL;
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP */

