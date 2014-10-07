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

#ifndef PIC_ALGORITHMS_CALCULATE_DIVERGENCE_HPP
#define PIC_ALGORITHMS_CALCULATE_DIVERGENCE_HPP

#include "filtering/filter_conv_1d.hpp"

namespace pic {

/**
 * @brief CalculateDivergence calculates divergence of an image
 * @param f
 * @param div
 * @return
 */
ImageRAW *CalculateDivergence(ImageRAW *f, ImageRAW *div)
{
    if(f == NULL) {
        return div;
    }

    if(div == NULL) {
        div = f->AllocateSimilarOne();
    }

    ImageRAW *f_dx2, *f_dy2;

    float kernelGrad[] = { -0.5f, 0.0f, 0.5f};
    float kernelDiv[] = { -1.0f, 1.0f, 0.0f};

    //Calculate first order gradient
    ImageRAW *f_dx = FilterConv1D::Execute(f, NULL, kernelGrad, 3, true);
    ImageRAW *f_dy = FilterConv1D::Execute(f, NULL, kernelGrad, 3, false);

    //Calculate divergence using backward differences
    f_dx2 = FilterConv1D::Execute(f_dx, div , kernelDiv, 3, true);
    f_dy2 = FilterConv1D::Execute(f_dy, f_dx, kernelDiv, 3, false);

    div->Add(f_dy2);

    delete f_dx;
    delete f_dy;

    return div;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_CALCULATE_DIVERGENCE_HPP */

