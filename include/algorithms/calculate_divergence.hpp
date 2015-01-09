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

#ifndef PIC_ALGORITHMS_CALCULATE_DIVERGENCE_HPP
#define PIC_ALGORITHMS_CALCULATE_DIVERGENCE_HPP

#include "filtering/filter_conv_1d.hpp"

namespace pic {

/**
 * @brief CalculateDivergence calculates divergence of the gradient of an image.
 * @param img is an input image.
 * @param div is the output divergence of the gradient of img; i.e. Laplacian.
 * @return
 */
Image *CalculateDivergence(Image *img, Image *div = NULL)
{
    if(img == NULL) {
        return div;
    }

    if(div == NULL) {
        div = img->AllocateSimilarOne();
    }

    Image *img_dx2, *img_dy2;

    float kernelGrad[] = { -0.5f, 0.0f, 0.5f};
    float kernelDiv[] = { -1.0f, 1.0f, 0.0f};

    //calculating the gradient of img
    Image *img_dx = FilterConv1D::Execute(img, NULL, kernelGrad, 3, true);
    Image *img_dy = FilterConv1D::Execute(img, NULL, kernelGrad, 3, false);

    //calculating the divergence using backward differences
    img_dx2 = FilterConv1D::Execute(img_dx, div ,   kernelDiv, 3, true);
    img_dy2 = FilterConv1D::Execute(img_dy, img_dx, kernelDiv, 3, false);

    *div += *img_dy2;

    delete img_dx;
    delete img_dy;

    return div;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_CALCULATE_DIVERGENCE_HPP */

