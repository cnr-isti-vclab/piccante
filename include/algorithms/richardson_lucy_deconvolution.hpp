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

#ifndef PIC_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP
#define PIC_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP

#include "image.hpp"
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
Image *RichardsonLucyDeconvolution(Image *imgIn, Image *psf, int nIterations = 10, Image *imgOut = NULL)
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

    Image *psf_hat = psf->Clone();
    psf_hat->FlipHV();

    *imgOut = 0.5f;

    Image *img_rel_blur = imgIn->AllocateSimilarOne();

    Image *img_est_conv = NULL;
    Image *img_err = NULL;

    FilterConv2D flt_conv;
    ImageVec vec = Double(imgOut, psf);
    ImageVec vec_err = Double(img_rel_blur, psf_hat);

    for(int i = 0; i < nIterations; i++) {

        #ifdef PIC_DEBUG
            printf("%d\n", i);
        #endif

        img_est_conv = flt_conv.ProcessP(vec, img_est_conv);

        img_rel_blur->Assign(imgIn);
        *img_rel_blur /= *img_est_conv;

        img_err = flt_conv.ProcessP(vec_err, img_err);

        *imgOut *= *img_err;
    }

    delete img_est_conv;

    return imgOut;
}

}

#endif //PIC_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP
