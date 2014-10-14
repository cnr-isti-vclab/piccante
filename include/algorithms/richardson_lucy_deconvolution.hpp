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

#ifndef PIC_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP
#define PIC_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP

#include "image_raw.hpp"
#include "filtering/filter_conv_2d.hpp"

namespace pic {

/**
 * @brief RichardsonLucyDeconvolution
 * @param imgIn
 * @param psf
 * @param nIterations
 * @param imgOut
 * @return
 */
ImageRAW *RichardsonLucyDeconvolution(ImageRAW *imgIn, ImageRAW *psf, int nIterations = 10, ImageRAW *imgOut = NULL)
{
    if(imgIn == NULL) {
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = imgIn->AllocateSimilarOne();
    }

    if(nIterations < 1) {
        nIterations = 10;
    }

    ImageRAW *psf_hat = psf->Clone();
    psf_hat->FlipHV();

    imgOut->Assign(0.5f);

    ImageRAW *img_rel_blur = imgIn->AllocateSimilarOne();

    ImageRAW *img_est_conv = NULL;
    ImageRAW *img_err = NULL;

    for(int i = 0; i < nIterations; i++) {
        printf("%d\n", i);
        img_est_conv = FilterConv2D::Execute(imgOut, psf, img_est_conv);

        img_rel_blur->Assign(imgIn);
        img_rel_blur->Div(img_est_conv);

        img_err = FilterConv2D::Execute(img_rel_blur, psf_hat, img_err);

        imgOut->Mul(img_err);
    }

    delete img_est_conv;

    return imgOut;
}

}

#endif //PIC_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP
