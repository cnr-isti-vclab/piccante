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

#ifndef PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP
#define PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP

#include "filtering/filter_conv_1d.hpp"
#include "util/precomputed_gaussian.hpp"

namespace pic {

class FilterGaussian1D: public FilterConv1D
{
protected:
    float				sigma;
    PrecomputedGaussian *pg;
    bool                bPgOwned;
    int					dirs[3];

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

    static Image *Execute(Image *imgIn, Image *imgOut, float sigma,
                             int direction)
    {
        FilterGaussian1D filter(sigma, direction);
        return filter.ProcessP(Single(imgIn), imgOut);
    }
};

PIC_INLINE FilterGaussian1D::FilterGaussian1D()
{
    sigma = 1.0f;
    pg = new PrecomputedGaussian(sigma);

    bPgOwned = true;
    Init(pg->coeff, pg->kernelSize, 0);
}

PIC_INLINE FilterGaussian1D::FilterGaussian1D(float sigma, int direction = 0)
{
    this->sigma = sigma;
    pg = new PrecomputedGaussian(sigma);

    bPgOwned = true;
    Init(pg->coeff, pg->kernelSize, direction);
}

//Reuse constructor
PIC_INLINE FilterGaussian1D::FilterGaussian1D(PrecomputedGaussian *pg, int direction = 0)
{
    if(pg == NULL) {
        #ifdef PICE_DEBUG
            printf("Error no precomputed gaussian values.\n");
        #endif
        return;
    }

    bPgOwned = false;

    Init(pg->coeff, pg->kernelSize, direction);
}

PIC_INLINE FilterGaussian1D::~FilterGaussian1D()
{
    if(pg != NULL && bPgOwned) {
        delete pg;
        pg = NULL;
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GAUSSIAN_1D_HPP */

